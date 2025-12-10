# -*- coding: utf-8 -*-
"""
Created on Thu Feb 20 18:37:13 2025

@author: moa2ofo
"""

import os
import re
import sys
import shutil
import subprocess
from dataclasses import dataclass

UNIT_TEST_PREFIX = "TEST_"
UNIT_EXECUTION_FOLDER = "utExecutionAndResults/utUnderTest"
UNIT_EXECUTION_FOLDER_BUILD = "utExecutionAndResults/utUnderTest/build"
UNIT_RESULT_FOLDER = "utExecutionAndResults/utResults"

# Determine the script directory, remove the file name and drive letter,
# and normalize the path for Docker (Cygwin/WSL style).
SCRIPT_PATH = os.path.abspath(__file__)
SCRIPT_DIRECTORY_PATH = os.path.dirname(SCRIPT_PATH)
RELATIVE_PATH = SCRIPT_DIRECTORY_PATH.split(":", 1)[-1].lstrip("\\/")
NORMALIZED_PATH = RELATIVE_PATH.replace("\\", "/")

DOCKER_BASE = [
    "docker",
    "run",
    "-it",
    "--rm",
    "-v",
    "/c/" + NORMALIZED_PATH + ":/home/dev/project",
    "throwtheswitch/madsciencelab-plugins:latest",
]
CEEDLING_CLEAN = ["ceedling", "clobber"]

DOCKER_CLEAN = DOCKER_BASE + CEEDLING_CLEAN


# ==============================
# DATA CLASS FOR MODULE METADATA
# ==============================
@dataclass
class UnitModule:
    """
    Metadata for a unit under test:

    - module_name   : C source file name (e.g. 'diagnostic.c')
    - function_name : C function name (e.g. 'ApplLinDiagReadDataById')
    - source_dir    : directory (relative to project root) where the C file is
    - test_root     : directory (relative to project root) of the unitTests root
                      (the TEST_<function> folder will be inside this)
    """
    module_name: str
    function_name: str
    source_dir: str
    test_root: str

    @property
    def test_case_folder(self) -> str:
        """
        Folder that contains the TEST_<function_name> unit test:
        <test_root>/TEST_<function_name>
        """
        return os.path.join(self.test_root, f"{UNIT_TEST_PREFIX}{self.function_name}")

    @property
    def test_c_path(self) -> str:
        """
        Path to the C test file:
        <test_root>/TEST_<function_name>/src/<function_name>.c
        """
        return os.path.join(
            self.test_case_folder,
            "src",
            f"{self.function_name}.c",
        )


def find_function_definition(path, func_name):
    """
    Search for the definition of a C function in .c files
    located in 'path' and its subdirectories.
    """
    results = []
    pattern = re.compile(rf"\b{func_name}\s*\([^)]*\)", re.IGNORECASE)

    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".c"):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                        for i, line in enumerate(f, start=1):
                            if pattern.search(line):
                                results.append((file_path, i, line.strip()))
                except Exception as e:
                    print(f"❌ Error reading '{file_path}': {e}")
    return results


def build_modules(path):
    """
    Search for all folders starting with TEST_XXXX and build
    a list of UnitModule objects.

    Example structure:

    UnitModule(
        module_name="diagnostic.c",
        function_name="ApplLinDiagReadDataById",
        source_dir="code/UdsComm/pltf",
        test_root="code/UdsComm/unitTests"
    )
    """
    modules = []

    for root, dirs, files in os.walk(path):
        for d in dirs:
            if d.startswith("TEST_"):
                func_name = d.replace("TEST_", "")
                folder_path = os.path.join(root, d)

                # Root directory from which to search the source function
                search_root = os.path.dirname(os.path.dirname(folder_path))

                matches = find_function_definition(search_root, func_name)

                if matches:
                    file_path, line_number, line_content = matches[0]

                    # Relative path of the .c file with respect to 'path'
                    file_rel = os.path.relpath(file_path, path)
                    module_name = os.path.basename(file_rel)
                    source_dir = os.path.dirname(file_rel).replace(os.sep, "/")
                else:
                    print(f"⚠️ No source file found for test folder '{folder_path}'")
                    continue

                # unitTests folder relative to the root
                test_root_rel = os.path.relpath(os.path.dirname(folder_path), path)
                test_root_rel = test_root_rel.replace(os.sep, "/")

                modules.append(
                    UnitModule(
                        module_name=module_name,
                        function_name=func_name,
                        source_dir=source_dir,
                        test_root=test_root_rel,
                    )
                )

    return modules


# ==============================
# FUNCTION EXTRACTION
# ==============================
def find_and_extract_function(file_name, function_name, unit_path):
    """
    Search for a file within a directory and its subdirectories, extract
    a C function, and return a "normalized" version of the function
    containing only:

        <return_type> <function_name>(<params>) { ... }

    It removes qualifiers/attributes such as:
    static, inline, extern, volatile, register, constexpr, __inline__,
    __forceinline, __attribute__((...)), etc.
    """
    file_path = None
    print(f"unit_path    : {unit_path}")
    print(f"file_name    : {file_name}")
    print(f"function_name: {function_name}")

    # Search the file within the given directory and subdirectories
    for root, _, files in os.walk(unit_path):
        if file_name in files:
            file_path = os.path.join(root, file_name)
            break

    if not file_path:
        print(f"❌ Error: File '{file_name}' not found in directory '{unit_path}'.")
        return None

    try:
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

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
def modify_file_after_marker(file_path, new_content):
    """
    Replace the content in the file after a specific marker.
    """
    marker = "/* FUNCTION TO TEST */"
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

        marker_index = content.find(marker)
        if marker_index == -1:
            print("⚠️ Marker not found in file.")
            sys.exit(1)

        modified_content = content[:marker_index + len(marker)] + "\n" + new_content + "\n"

        with open(file_path, "w", encoding="utf-8") as file:
            file.write(modified_content)
        print(f"✅ File '{file_path}' successfully updated.")
    except FileNotFoundError:
        print(f"❌ Error: File '{file_path}' not found.")
    except Exception as e:
        print(f"❌ Error modifying file '{file_path}': {e}")


def extract_function_name(path: str) -> str:
    """
    Extract the function name from a path or filename.
    Examples:
      'TEST_myFunc.c'  -> 'myFunc'
      'myFunc.c'       -> 'myFunc'
      'myFunc'         -> 'myFunc'
    """
    filename = os.path.basename(path)
    name_no_ext = os.path.splitext(filename)[0]
    if name_no_ext.startswith("TEST_"):
        return name_no_ext[len("TEST_"):]
    return name_no_ext


def copy_folder_contents(src_folder: str, dest_folder: str):
    """
    Copy all files and subdirectories from src_folder into dest_folder.
    Creates dest_folder if it does not exist.
    """
    print(f"▶ Copying data from '{src_folder}' to '{dest_folder}'")
    if not os.path.exists(src_folder):
        print(f"❌ Error: Source folder '{src_folder}' does not exist. Nothing to copy.")
        return

    if not os.path.exists(dest_folder):
        os.makedirs(dest_folder)

    for item in os.listdir(src_folder):
        src_path = os.path.join(src_folder, item)
        dest_path = os.path.join(dest_folder, item)

        if os.path.isdir(src_path):
            # Copy subdirectory recursively
            shutil.copytree(src_path, dest_path, dirs_exist_ok=True)
        else:
            # Copy single file
            shutil.copy2(src_path, dest_path)


def clear_folder(folder_path: str):
    """
    Delete all files and subdirectories inside the given folder.
    The folder itself is preserved.
    """
    if not os.path.exists(folder_path):
        print(f"⚠️ Folder '{folder_path}' does not exist.")
        return

    for item in os.listdir(folder_path):
        item_path = os.path.join(folder_path, item)
        try:
            if os.path.isfile(item_path) or os.path.islink(item_path):
                os.remove(item_path)   # delete file or symbolic link
            elif os.path.isdir(item_path):
                shutil.rmtree(item_path)  # delete subdirectory recursively
        except Exception as e:
            print(f"❌ Error deleting '{item_path}': {e}")

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
    unit_test_root = module.test_root

    extracted_body = find_and_extract_function(module_name, function_name, source_dir)

    if extracted_body is None:
        print(f"❌ Cannot extract body for function '{function_name}' in module '{module_name}'")
        sys.exit(1)

    # Update the corresponding unit test file with the extracted function body
    test_c_path = module.test_c_path
    modify_file_after_marker(test_c_path, extracted_body)

    # Prepare the execution folder
    clear_folder(UNIT_EXECUTION_FOLDER)
    copy_folder_contents(
        os.path.join(unit_test_root, UNIT_TEST_PREFIX + unit_name),
        UNIT_EXECUTION_FOLDER,
    )


def update_total_result_report(build_folder, function_name, report_folder):
    """
    Read test result values from the Ceedling report file and update a summary report.
    The summary report will contain rows with columns:
    | Function Name Under Test | Total | Passed | Failed | Ignored |

    If the function_name already exists in the summary report, its row is updated
    instead of adding a duplicate row.
    """
    report_file = os.path.join(build_folder, "test", "results", f"test_{function_name}.pass")
    if not os.path.exists(report_file):
        print(f"⚠️ Report file '{report_file}' does not exist.")
        return

    total = passed = failed = ignored = None

    try:
        # Read the report file and extract values
        with open(report_file, "r", encoding="utf-8") as file:
            for line in file:
                line = line.strip()
                if line.startswith(":total:"):
                    total = line.split(":")[2].strip()
                elif line.startswith(":passed:"):
                    passed = line.split(":")[2].strip()
                elif line.startswith(":failed:"):
                    failed = line.split(":")[2].strip()
                elif line.startswith(":ignored:"):
                    ignored = line.split(":")[2].strip()

        # Ensure all values were found
        if None in (total, passed, failed, ignored):
            print(f"⚠️ Missing values in report file '{report_file}'.")
            return

        summary_file = os.path.join(report_folder, "total_result_report.txt")
        os.makedirs(report_folder, exist_ok=True)

        # Column widths for alignment
        col_widths = [30, 10, 10, 10, 10]

        header = (
            f"| {'Function Name Under Test'.ljust(col_widths[0])}"
            f"| {'Total'.ljust(col_widths[1])}"
            f"| {'Passed'.ljust(col_widths[2])}"
            f"| {'Failed'.ljust(col_widths[3])}"
            f"| {'Ignored'.ljust(col_widths[4])}|\n"
        )
        separator = (
            f"|{'-'*(col_widths[0]+1)}|{'-'*(col_widths[1]+1)}|{'-'*(col_widths[2]+1)}|"
            f"{'-'*(col_widths[3]+1)}|{'-'*(col_widths[4]+1)}|\n"
        )
        row = (
            f"| {function_name.ljust(col_widths[0])}"
            f"| {total.ljust(col_widths[1])}"
            f"| {passed.ljust(col_widths[2])}"
            f"| {failed.ljust(col_widths[3])}"
            f"| {ignored.ljust(col_widths[4])}|\n"
        )

        # If the summary file does not exist, create it with header and first row
        if not os.path.exists(summary_file):
            with open(summary_file, "w", encoding="utf-8") as summary:
                summary.write(header)
                summary.write(separator)
                summary.write(row)
            print(f"✅ Created summary report: {summary_file}")
            return

        # If the summary file exists, update the row if function_name is already present
        with open(summary_file, "r", encoding="utf-8") as summary:
            lines = summary.readlines()

        updated = False
        for i, line in enumerate(lines):
            # Skip separator lines
            if line.startswith("|-"):
                continue
            # Consider only table rows (start with '|' and not the header separator)
            if line.startswith("|"):
                parts = line.split("|")
                if len(parts) > 2:
                    name_col = parts[1].strip()
                    if name_col == function_name:
                        lines[i] = row
                        updated = True
                        break

        # If not updated, append the new row at the end
        if not updated:
            if lines and not lines[-1].endswith("\n"):
                lines[-1] = lines[-1] + "\n"
            lines.append(row)

        with open(summary_file, "w", encoding="utf-8") as summary:
            summary.writelines(lines)

        if updated:
            print(f"✅ Updated existing entry for '{function_name}' in summary report: {summary_file}")
        else:
            print(f"✅ Added new entry for '{function_name}' to summary report: {summary_file}")

    except Exception as e:
        print(f"❌ Error updating report: {e}")


def run_bash_cmd(cmd):
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
    run_bash_cmd(DOCKER_BASE + ["ceedling", "test:" + function_name])
    update_total_result_report(UNIT_EXECUTION_FOLDER_BUILD, function_name, UNIT_RESULT_FOLDER)
    copy_folder_contents(
        UNIT_EXECUTION_FOLDER_BUILD,
        os.path.join(UNIT_RESULT_FOLDER, function_name + "Results")
    )


def print_help():
    script_name = os.path.basename(sys.argv[0])
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

    base_path = os.path.dirname(os.path.abspath(__file__))  # current folder
    modules = build_modules(base_path)

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

    clear_folder(UNIT_EXECUTION_FOLDER)
