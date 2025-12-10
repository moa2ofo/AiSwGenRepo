# -*- coding: utf-8 -*-
"""
Created on Thu Feb 20 18:37:13 2025

Unit test launcher for Ceedling + Docker.

Rewritten to use pathlib.Path for filesystem operations.
"""

import re
import sys
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from datetime import datetime
import getpass

UNIT_TEST_PREFIX = "TEST_"

# -----------------------------------------
# Paths & Docker configuration
# -----------------------------------------
SCRIPT_PATH = Path(__file__).resolve()
PROJECT_ROOT = SCRIPT_PATH.parent  # root of the project (directory of this script)

UNIT_EXECUTION_FOLDER = PROJECT_ROOT / "utExecutionAndResults" / "utUnderTest"
UNIT_EXECUTION_FOLDER_BUILD = UNIT_EXECUTION_FOLDER / "build"
UNIT_RESULT_FOLDER = PROJECT_ROOT / "utExecutionAndResults" / "utResults"

# Windows-specific normalization for Docker volume mount: /c/<path>
# Example:
#   PROJECT_ROOT = "C:\\repo\\proj"
#   RELATIVE_PATH = "repo\\proj"
#   NORMALIZED_PATH = "repo/proj"
RELATIVE_PATH = str(PROJECT_ROOT).split(":", 1)[-1].lstrip("\\/")
NORMALIZED_PATH = RELATIVE_PATH.replace("\\", "/")

DOCKER_BASE = [
    "docker",
    "run",
    "-it",
    "--rm",
    "-v",
    "/c/" + NORMALIZED_PATH + ":/home/dev/project",
    # ToDo use a fixed version and not the last one to avoid unexpected issues
    "throwtheswitch/madsciencelab-plugins:latest",
]
CEEDLING_CLEAN = ["ceedling", "clobber"]

DOCKER_CLEAN = DOCKER_BASE + CEEDLING_CLEAN


# ==============================
# DATA CLASS FOR MODULE METADATA
# ==============================
@dataclass
class TestResultRow:
    function_name: str
    total: str
    passed: str
    failed: str
    ignored: str
    date_time: str
    tester: str

    def to_csv_line(self) -> str:
        return (
            f"{self.function_name},"
            f"{self.total},"
            f"{self.passed},"
            f"{self.failed},"
            f"{self.ignored},"
            f"{self.date_time},"
            f"{self.tester}"
        )

@dataclass
class UnitModule:
    """
    Metadata for a unit under test:

    - module_name   : C source file name (e.g. 'diagnostic.c')
    - function_name : C function name (e.g. 'ApplLinDiagReadDataById')
    - source_dir    : directory (absolute Path) where the C file is
    - test_root     : directory (absolute Path) of the unitTests root
                      (the TEST_<function> folder will be inside this)
    """
    module_name: str
    function_name: str
    source_dir: Path
    test_root: Path

    @property
    def test_case_folder(self) -> Path:
        """
        Folder that contains the TEST_<function_name> unit test:
        <test_root>/TEST_<function_name>
        """
        return self.test_root / f"{UNIT_TEST_PREFIX}{self.function_name}"

    @property
    def test_c_path(self) -> Path:
        """
        Path to the C test file:
        <test_root>/TEST_<function_name>/src/<function_name>.c
        """
        return self.test_case_folder / "src" / f"{self.function_name}.c"


# ==============================
# FUNCTION SEARCHING
# ==============================
def find_function_definition(root: Path, func_name: str):
    """
    Search for the definition of a C function in .c files
    located under 'root' and its subdirectories.

    Returns a list of tuples: (file_path, line_number, line_content)
    """
    results = []
    pattern = re.compile(rf"\b{func_name}\s*\([^)]*\)", re.IGNORECASE)

    for c_file in root.rglob("*.c"):
        try:
            with c_file.open("r", encoding="utf-8", errors="ignore") as f:
                for i, line in enumerate(f, start=1):
                    if pattern.search(line):
                        results.append((c_file, i, line.strip()))
        except Exception as e:
            print(f"❌ Error reading '{c_file}': {e}")
    return results


def build_modules(root: Path):
    """
    Search for all folders starting with TEST_XXXX and build
    a list of UnitModule objects.

    Example structure:

    UnitModule(
        module_name="diagnostic.c",
        function_name="ApplLinDiagReadDataById",
        source_dir=PROJECT_ROOT / "code" / "UdsComm" / "pltf",
        test_root=PROJECT_ROOT / "code" / "UdsComm" / "unitTests"
    )
    """
    modules = []

    for test_dir in root.rglob("*"):
        if not test_dir.is_dir():
            continue
        if not test_dir.name.startswith(UNIT_TEST_PREFIX):
            continue

        func_name = test_dir.name.replace(UNIT_TEST_PREFIX, "", 1)

        # Root directory from which to search the source function
        # (one level up from TEST_xxx, then its parent)
        # <...>/unitTests/TEST_func  -> search_root = <...>
        test_root = test_dir.parent
        search_root = test_root.parent

        matches = find_function_definition(search_root, func_name)

        if matches:
            file_path, line_number, line_content = matches[0]
            module_name = file_path.name
            source_dir = file_path.parent
        else:
            print(f"⚠️ No source file found for test folder '{test_dir}'")
            continue

        modules.append(
            UnitModule(
                module_name=module_name,
                function_name=func_name,
                source_dir=source_dir,
                test_root=test_root,
            )
        )

    return modules


# ==============================
# FUNCTION EXTRACTION
# ==============================
def find_and_extract_function(file_name: str, function_name: str, unit_path: Path):
    """
    Search for a file within a directory and its subdirectories, extract
    a C function, and return a "normalized" version of the function
    containing only:

        <return_type> <function_name>(<params>) { ... }

    It removes qualifiers/attributes such as:
    static, inline, extern, volatile, register, constexpr, __inline__,
    __forceinline, __attribute__((...)), etc.
    """
    print(f"unit_path    : {unit_path}")
    print(f"file_name    : {file_name}")
    print(f"function_name: {function_name}")

    file_path: Path | None = None

    # Search the file within the given directory and subdirectories
    for candidate in unit_path.rglob(file_name):
        if candidate.is_file():
            file_path = candidate
            break

    if file_path is None:
        print(f"❌ Error: File '{file_name}' not found in directory '{unit_path}'.")
        return None

    try:
        content = file_path.read_text(encoding="utf-8")

        # Regex:
        # - anchors at the beginning of the line (^)
        # - captures everything before the function name in "before"
        #   (return type + qualifiers + attributes)
        # - then the function name
        # - parameters
        # - optional __attribute__ after the parameters
        function_pattern = re.compile(
            rf"""
            (?P<header>
                ^[ \t]*
                (?P<before>[^\n]*?)          # return type + qualifiers + attributes (all on the line)
                \b{function_name}\b          # function name
                \s*
                (?P<params>\([^)]*\))        # parameter list
                (?P<post_attr>               # optional __attribute__ after parameters
                    (?:\s*__attribute__\s*\(\([^)]*\)\))*
                )
            )
            \s*\{{                           # opening brace of the body
            """,
            re.MULTILINE | re.VERBOSE
        )

        match = function_pattern.search(content)

        if not match:
            print(f"⚠️ Function '{function_name}' not found in '{file_name}'.")
            return None

        # Position of the first '{' after the header
        brace_index = content.find("{", match.end("header"))
        if brace_index == -1:
            print(f"⚠️ Opening brace for function '{function_name}' not found in '{file_name}'.")
            return None

        # Find the end of the function by counting braces
        open_braces = 0
        end_index = None
        for i in range(brace_index, len(content)):
            char = content[i]
            if char == "{":
                open_braces += 1
            elif char == "}":
                open_braces -= 1
                if open_braces == 0:
                    end_index = i
                    break

        if end_index is None:
            print(f"⚠️ Closing brace for function '{function_name}' not found in '{file_name}'.")
            return None

        # ----- Header normalization -----
        before = match.group("before") or ""
        params = match.group("params")

        # 1) Remove all __attribute__((...))
        before_clean = re.sub(r'__attribute__\s*\(\([^)]*\)\)\s*', ' ', before)

        # 2) Remove qualifiers like static, inline, extern, etc.
        before_clean = re.sub(
            r'\b(static|inline|INLINE|extern|constexpr|volatile|register|__inline__|__forceinline)\b',
            ' ',
            before_clean
        )

        # 3) Normalize whitespace
        before_clean = before_clean.replace('\t', ' ')
        return_type = ' '.join(before_clean.split())

        # If, for some reason, it's empty, fall back to "void"
        if not return_type:
            return_type = "void"  # conservative fallback

        # Clean header
        clean_header = f"{return_type} {function_name}{params}"

        # Original body from '{' up to the matching '}' (inclusive)
        body_part = content[brace_index:end_index + 1]

        # Final result: clean header + original body
        normalized_function = clean_header + " " + body_part
        print("✅ Function body found.\n")
        return f"\n\n{normalized_function}"

    except Exception as e:
        print(f"❌ Error reading file '{file_name}': {e}")
        return None


# ==============================
# MODIFY FILE AFTER MARKER
# ==============================
def modify_file_after_marker(file_path: Path, new_content: str):
    """
    Replace the content in the file after a specific marker.
    """
    marker = "/* FUNCTION TO TEST */"
    try:
        content = file_path.read_text(encoding="utf-8")

        marker_index = content.find(marker)
        if marker_index == -1:
            print("⚠️ Marker not found in file.")
            sys.exit(1)

        modified_content = content[:marker_index + len(marker)] + "\n" + new_content + "\n"

        file_path.write_text(modified_content, encoding="utf-8")
        print(f"✅ File '{file_path}' successfully updated.")
    except FileNotFoundError:
        print(f"❌ Error: File '{file_path}' not found.")
    except Exception as e:
        print(f"❌ Error modifying file '{file_path}': {e}")


def extract_function_name(path_str: str) -> str:
    """
    Extract the function name from a path or filename.
    Examples:
      'TEST_myFunc.c'  -> 'myFunc'
      'myFunc.c'       -> 'myFunc'
      'myFunc'         -> 'myFunc'
    """
    filename = Path(path_str).name
    name_no_ext = Path(filename).stem
    if name_no_ext.startswith(UNIT_TEST_PREFIX):
        return name_no_ext[len(UNIT_TEST_PREFIX):]
    return name_no_ext


def copy_folder_contents(src_folder: Path, dest_folder: Path):
    """
    Copy all files and subdirectories from src_folder into dest_folder.
    Creates dest_folder if it does not exist.
    """
    print(f"▶ Copying data from '{src_folder}' to '{dest_folder}'")
    if not src_folder.exists():
        print(f"❌ Error: Source folder '{src_folder}' does not exist. Nothing to copy.")
        return

    dest_folder.mkdir(parents=True, exist_ok=True)

    for item in src_folder.iterdir():
        dest_path = dest_folder / item.name
        if item.is_dir():
            # Copy subdirectory recursively
            shutil.copytree(item, dest_path, dirs_exist_ok=True)
        else:
            # Copy single file
            shutil.copy2(item, dest_path)


def clear_folder(folder_path: Path):
    """
    Delete all files and subdirectories inside the given folder.
    The folder itself is preserved.
    """
    if not folder_path.exists():
        print(f"⚠️ Folder '{folder_path}' does not exist.")
        return

    for item in folder_path.iterdir():
        try:
            if item.is_file() or item.is_symlink():
                item.unlink()   # delete file or symbolic link
            elif item.is_dir():
                shutil.rmtree(item)  # delete subdirectory recursively
        except Exception as e:
            print(f"❌ Error deleting '{item}': {e}")

    print(f"✅ Folder '{folder_path}' cleared successfully.")


def update_unit_under_test(module: UnitModule, unit_name: str):
    """
    Update the unit test for a single module.
    """
    if not module:
        print(f"❌ No entry found for function = {unit_name}")
        sys.exit(1)

    module_name = module.module_name
    function_name = module.function_name
    source_dir = module.source_dir

    extracted_body = find_and_extract_function(module_name, function_name, source_dir)

    if extracted_body is None:
        print(f"❌ Cannot extract body for function '{function_name}' in module '{module_name}'")
        sys.exit(1)

    # Update the corresponding unit test file with the extracted function body
    test_c_path = module.test_c_path
    modify_file_after_marker(test_c_path, extracted_body)

    # Prepare the execution folder
    clear_folder(UNIT_EXECUTION_FOLDER)
    copy_folder_contents(module.test_case_folder, UNIT_EXECUTION_FOLDER)

def load_result_rows(summary_file: Path) -> dict[str, TestResultRow]:
    """
    Load existing results from 'total_result_report.txt'.

    Supports:
      - CSV format:
          function_name,total,passed,failed,ignored,Date and time,Tester
          foo,10,10,0,0,10/12/25 14:13,USER
      - Pretty table format (pipe-separated), which is parsed and converted.

    Returns:
      dict[function_name, TestResultRow]
    """
    rows: dict[str, TestResultRow] = {}

    if not summary_file.exists() or summary_file.stat().st_size == 0:
        return rows

    text = summary_file.read_text(encoding="utf-8")
    lines = text.splitlines()
    if not lines:
        return rows

    # Detect format: pretty table if first non-empty line starts with '|'
    first_non_empty = next((ln for ln in lines if ln.strip()), "")
    header_csv = "function_name,total,passed,failed,ignored,Date and time,Tester"

    # ---------- Pretty table format ----------
    if first_non_empty.startswith("|"):
        header_cells: list[str] = []
        data_rows_cells: list[list[str]] = []

        for line in lines:
            stripped = line.strip()
            if not stripped:
                continue
            if stripped.startswith("|-"):
                # separator
                continue
            if not stripped.startswith("|"):
                continue

            cells = [c.strip() for c in stripped.split("|")[1:-1]]
            if not header_cells:
                header_cells = cells
            else:
                data_rows_cells.append(cells)

        def idx(col_name: str, default: int = -1) -> int:
            try:
                return header_cells.index(col_name)
            except ValueError:
                return default

        idx_name   = idx("function_name", 0)
        idx_total  = idx("total")
        idx_passed = idx("passed")
        idx_failed = idx("failed")
        idx_ignored = idx("ignored")
        idx_date   = idx("Date and time")
        idx_tester = idx("Tester")

        def get_cell(row: list[str], i: int) -> str:
            if i is None or i < 0 or i >= len(row):
                return ""
            return row[i]

        for row in data_rows_cells:
            fn = get_cell(row, idx_name)
            if not fn:
                continue
            t   = get_cell(row, idx_total)
            p   = get_cell(row, idx_passed)
            f   = get_cell(row, idx_failed)
            ig  = get_cell(row, idx_ignored)
            dt  = get_cell(row, idx_date)
            tst = get_cell(row, idx_tester)

            rows[fn] = TestResultRow(
                function_name=fn,
                total=t,
                passed=p,
                failed=f,
                ignored=ig,
                date_time=dt,
                tester=tst,
            )

        return rows

    # ---------- CSV format ----------
    # first line is header, rest are data
    header_line = lines[0].strip()
    if "function_name" not in header_line:
        # unexpected header: replace with our canonical header
        header_line = header_csv

    for line in lines[1:]:
        stripped = line.strip()
        if not stripped:
            continue
        parts = [p.strip() for p in stripped.split(",")]
        if len(parts) < 5:
            continue

        # handle old or short rows: pad to 7 columns
        while len(parts) < 7:
            parts.append("")

        fn, t, p, f, ig, dt, tst = parts[:7]
        if not fn:
            continue

        rows[fn] = TestResultRow(
            function_name=fn,
            total=t,
            passed=p,
            failed=f,
            ignored=ig,
            date_time=dt,
            tester=tst,
        )

    return rows

def update_total_result_report(build_folder: Path, function_name: str, report_folder: Path):
    """
    Read test result values from the Ceedling report file and update a summary file
    with raw data (CSV-style, no formatting).

    Behaviour:
      - If the file does not exist or is empty -> create it (header + row).
      - If the file exists and function_name is NEW      -> keep all rows and add a new one.
      - If the file exists and function_name already in -> overwrite that row with new values.

    Raw CSV format (internal representation):
      function_name,total,passed,failed,ignored,Date and time,Tester
    """
    report_file = build_folder / "test" / "results" / f"test_{function_name}.pass"
    if not report_file.exists():
        print(f"⚠️ Report file '{report_file}' does not exist.")
        return

    total = passed = failed = ignored = None

    try:
        # ---- 1. Extract values from Ceedling .pass file ----
        for line in report_file.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if line.startswith(":total:"):
                total = line.split(":", 2)[2].strip()
            elif line.startswith(":passed:"):
                passed = line.split(":", 2)[2].strip()
            elif line.startswith(":failed:"):
                failed = line.split(":", 2)[2].strip()
            elif line.startswith(":ignored:"):
                ignored = line.split(":", 2)[2].strip()

        if None in (total, passed, failed, ignored):
            print(f"⚠️ Missing values in report file '{report_file}'.")
            return

        # New metadata: date/time and tester (PC owner)
        now_str = datetime.now().strftime("%d/%m/%y %H:%M")
        tester = getpass.getuser()

        report_folder.mkdir(parents=True, exist_ok=True)
        summary_file = report_folder / "total_result_report.txt"

        # ---- 2. Load existing rows into a dict ----
        rows = load_result_rows(summary_file)

        # ---- 3. Update / insert current function row ----
        rows[function_name] = TestResultRow(
            function_name=function_name,
            total=total,
            passed=passed,
            failed=failed,
            ignored=ignored,
            date_time=now_str,
            tester=tester,
        )

        # ---- 4. Rewrite CSV file from the dictionary ----
        header_csv = "function_name,total,passed,failed,ignored,Date and time,Tester"
        lines_out = [header_csv]

        # Keep insertion order of dict (Python 3.7+ preserves it),
        # or sort by name if you prefer: for fn in sorted(rows)
        for fn, row in rows.items():
            lines_out.append(row.to_csv_line())

        summary_file.write_text("\n".join(lines_out) + "\n", encoding="utf-8")
        print(f"✅ Updated raw summary data for '{function_name}' in: {summary_file}")

    except Exception as e:
        print(f"❌ Error updating raw report data: {e}")


def format_total_result_report(report_folder: Path):
    """
    Read the raw 'total_result_report.txt' (CSV style) and rewrite it
    as a pretty aligned table (pipe format) with dynamic column widths.

    Final table example:

    | function_name           | total | passed | failed | ignored | Date and time  | Tester |
    |-------------------------|-------|--------|--------|---------|----------------|--------|
    | ApplLinDiagReadDataById | 5     | 5      | 0      | 0       | 10/12/25 13:25 | owner  |
    """
    summary_file = report_folder / "total_result_report.txt"
    if not summary_file.exists():
        print(f"⚠️ Summary file '{summary_file}' does not exist. Nothing to format.")
        return

    lines = summary_file.read_text(encoding="utf-8").splitlines()
    if not lines:
        print(f"⚠️ Summary file '{summary_file}' is empty. Nothing to format.")
        return

    # First line is the header in CSV form
    header_parts = [h.strip() for h in lines[0].split(",") if h.strip()]
    if not header_parts:
        print(f"⚠️ Invalid header in summary file '{summary_file}'. Cannot format.")
        return

    # Read data rows
    data_rows: list[list[str]] = []
    for line in lines[1:]:
        if not line.strip():
            continue
        parts = [p.strip() for p in line.split(",")]
        # Pad shorter rows with empty strings to match header length
        if len(parts) < len(header_parts):
            parts.extend([""] * (len(header_parts) - len(parts)))
        data_rows.append(parts[:len(header_parts)])

    if not data_rows:
        print(f"⚠️ No data rows to format in '{summary_file}'.")
        return

    # Compute column widths based on header + data
    col_widths: list[int] = []
    for col_idx, header in enumerate(header_parts):
        max_data_len = max(len(row[col_idx]) for row in data_rows)
        col_widths.append(max(len(header), max_data_len))

    # Build header line
    header_line = "| " + " | ".join(
        header_parts[i].ljust(col_widths[i]) for i in range(len(header_parts))
    ) + " |\n"

    # Build separator line
    separator_line = "|" + "|".join(
        "-" * (col_widths[i] + 2) for i in range(len(col_widths))
    ) + "|\n"

    # Build data lines
    row_lines = ""
    for row in data_rows:
        line = "| " + " | ".join(
            row[i].ljust(col_widths[i]) for i in range(len(row))
        ) + " |\n"
        row_lines += line

    table_str = header_line + separator_line + row_lines

    # Overwrite file with pretty table
    summary_file.write_text(table_str, encoding="utf-8")
    print(f"✅ Formatted summary report: {summary_file}")



def run_bash_cmd(cmd: list[str]):
    """
    Execute a system command (list of arguments).
    On error, print a detailed message and TERMINATE the script.
    """
    try:
        print(f"▶ Running command: {' '.join(cmd)}")
        subprocess.run(cmd, check=True)

    except FileNotFoundError as e:
        print(f"\n❌ FATAL ERROR: Command not found: '{cmd[0]}'")
        print(f"   Full command : {' '.join(cmd)}")
        print(f"   Details      : {e}\n")
        sys.exit(1)

    except PermissionError as e:
        print(f"\n❌ FATAL ERROR: Permission denied while executing command:")
        print(f"   {' '.join(cmd)}")
        print(f"   Details      : {e}\n")
        sys.exit(1)

    except subprocess.CalledProcessError as e:
        print(f"\n❌ FATAL ERROR: Command executed but FAILED.")
        print(f"   Command    : {' '.join(cmd)}")
        print(f"   Exit code  : {e.returncode}")
        print(f"   Details    : {e}\n")
        sys.exit(1)

    except Exception as e:
        print(f"\n❌ FATAL ERROR: Unexpected error while executing command:")
        print(f"   Command    : {' '.join(cmd)}")
        print(f"   Details    : {e}\n")
        sys.exit(1)

    print("✔️  Command executed successfully.\n")


def run_and_collect_results(module: UnitModule):
    """
    Orchestrate: update unit under test, run Ceedling in Docker,
    update the aggregated report and copy build results.
    """
    function_name = module.function_name
    update_unit_under_test(module, function_name)
    run_bash_cmd(DOCKER_CLEAN)
    run_bash_cmd(DOCKER_BASE + ["ceedling", f"test:{function_name}"])
    update_total_result_report(UNIT_EXECUTION_FOLDER_BUILD, function_name, UNIT_RESULT_FOLDER)
    copy_folder_contents(
        UNIT_EXECUTION_FOLDER_BUILD,
        UNIT_RESULT_FOLDER / f"{function_name}Results",
    )


def print_help():
    script_name = Path(sys.argv[0]).name
    help_text = f"""
Usage:
  python {script_name} <function_name|all>
  python {script_name} -h | --help | help

Description:
  This script:
    - Extracts the specified C function from the code base
    - Injects it into the corresponding TEST_<function_name> unit file
      after the marker: /* FUNCTION TO TEST */
    - Copies the unit test project into:
        {UNIT_EXECUTION_FOLDER}
    - Runs Ceedling inside the Docker container
    - Collects build and coverage results into:
        {UNIT_RESULT_FOLDER}

Arguments:
  function_name        Name of the function to test.
                       It must match 'function_name' in the discovered modules.
  all                  Run the process for all modules discovered under the project root.

Options:
  -h, --help, help     Show this help message and exit.

Examples:
  python {script_name} monitoring
  python {script_name} all
"""
    print(help_text.strip())


if __name__ == "__main__":
    # ---- handle missing args / help ----
    if len(sys.argv) < 2:
        print_help()
        sys.exit(1)

    if sys.argv[1] in ("-h", "--help", "help"):
        print_help()
        sys.exit(0)

    unit_to_test = extract_function_name(sys.argv[1])
    print(f"▶ Selected argument (function to test): {unit_to_test}")

    modules = build_modules(PROJECT_ROOT)

    if unit_to_test == "all":
        clear_folder(UNIT_RESULT_FOLDER)
        # Run for all modules in the list
        for module in modules:
            print(f"▶ Processing unit: {module.function_name}")
            run_and_collect_results(module)
    else:
        # Run only for the selected unit
        unit_metadata = [m for m in modules if m.function_name == unit_to_test]
        if not unit_metadata:
            print(f"❌ No module found for function '{unit_to_test}'")
            sys.exit(1)

        module = unit_metadata[0]   # single module
        run_and_collect_results(module)

    format_total_result_report(UNIT_RESULT_FOLDER)
    clear_folder(UNIT_EXECUTION_FOLDER)
