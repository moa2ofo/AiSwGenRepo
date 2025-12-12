#!/usr/bin/env python3
import os
import getpass
from datetime import datetime
import xml.etree.ElementTree as ET
import html
import subprocess
import re
from pathlib import Path
from typing import Union, Dict

# Default MISRA rules file path (adjust if needed)
MISRA_RULES_PATH = Path(__file__).resolve().parent / "misra" / "misra_c_2012_headlines.txt"


def load_misra_rules(misra_rules_path: Union[str, Path]) -> Dict[str, str]:
    """
    Load MISRA rules from a text file and build a map:
        "21.5" -> "Required"

    The file is expected to contain lines like:
        Rule 21.5\tRequired
    (the next line can be the description, which we ignore here).
    """
    rules = {}  # type: Dict[str, str]
    path = Path(misra_rules_path)

    if not path.is_file():
        print(
            "[WARNING] MISRA rules file not found: %s. "
            "cppcheck severities will be used instead." % path
        )
        return rules

    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if not line.startswith("Rule "):
                continue

            # Remove leading "Rule "
            rest = line[5:].strip()  # e.g. "21.5\tRequired"
            # Prefer tab as separator between rule id and severity
            parts = rest.split("\t")
            if len(parts) < 2:
                # Fallback: split on whitespace if no tab
                parts = rest.split(None, 1)
                if len(parts) < 2:
                    continue

            rule_id = parts[0].strip()   # "21.5"
            severity = parts[1].strip()  # "Required"

            if rule_id:
                rules[rule_id] = severity

    print("[INFO] Loaded %d MISRA rules from %s" % (len(rules), path))
    return rules


# Match things like "misra-c2012-21.5" and extract "21.5"
MISRA_ID_REGEX = re.compile(r"misra-c2012-(\d+\.\d+)")

def generate_html_for_cppcheck_xml(
    xml_path: Union[str, Path], misra_rules_path: Union[str, Path]
) -> str:
    """
    Parse cppcheck_misra_results.xml and generate cppcheck_misra_results.html.

    - Severity for MISRA violations is taken from 'misra_rules_path'
      instead of cppcheck's severity (style/warning/etc).
    - Remove '\\011' and '\\342\\200\\246' from the HTML file.
    - Remove the entire 'file0', 'verbose', and 'cwe' columns.
    - Color the row background based on final Severity:
        style/unknown -> white (default)
        Advisory      -> yellow
        Required      -> orange
        Mandatory     -> red
    - Locations column entries become clickable VS Code links:
        pltf/diagnostic.c:45:27  -->  vscode://file/<abs-path>/pltf/diagnostic.c:45:27
    - Add metadata under the title:
        Tester: <pc-user>  Date: DD/MM/YY  Time: HH:mm:ss
    - Finally, delete the original XML file.
    """

    xml_path = str(xml_path)
    xml_dir = os.path.dirname(xml_path)

    tree = ET.parse(xml_path)
    root = tree.getroot()

    errors = root.findall(".//error")
    if not errors:
        print("[INFO] No <error> elements found in %s" % xml_path)
        return ""

    # Load MISRA rules mapping (e.g. "21.5" -> "Required")
    misra_rules = load_misra_rules(misra_rules_path)

    # ------------------------------------------------------------------
    # Collect attribute names, then REMOVE 'file0', 'verbose', and 'cwe'
    # ------------------------------------------------------------------
    attr_names = set()
    for err in errors:
        attr_names.update(err.attrib.keys())

    # Remove unused / unwanted columns completely
    attr_names.discard("cwe")
    attr_names.discard("file0")
    attr_names.discard("verbose")

    # Column order (we will override 'severity' values using the MISRA file)
    preferred_order = ["id", "severity", "file1", "msg"]

    ordered_attrs = [a for a in preferred_order if a in attr_names]
    # Add remaining attributes sorted alphabetically
    ordered_attrs.extend(sorted(attr_names - set(ordered_attrs)))

    # Add custom column for locations
    columns = ordered_attrs + ["locations"]

    # ------------------------------------------------------------------
    # Build HTML rows
    # ------------------------------------------------------------------
    rows_html = []

    for err in errors:
        cells = []

        # ---- Determine final severity for this error (for both column + row color)
        err_id = err.attrib.get("id", "")
        severity_val = ""

        m = MISRA_ID_REGEX.search(err_id)
        if m:
            rule_number = m.group(1)  # e.g. "21.5"
            severity_val = misra_rules.get(rule_number, "")

        # Fallback to cppcheck 'severity' if we didn't find a MISRA mapping
        if not severity_val:
            severity_val = err.attrib.get("severity", "")

        severity_for_row = severity_val or ""

        # Normalize and use substring matching (robust to extra text)
        sev_norm = severity_for_row.strip().lower()

        # Map severity to row background color
        # style / unknown -> default (no inline style)
        row_style = ""
        if "advisory" in sev_norm:
            row_style = ' style="background-color: #ffff99;"'   # yellow
        elif "required" in sev_norm:
            row_style = ' style="background-color: #ffcc80;"'   # orange
        elif "mandatory" in sev_norm:
            row_style = ' style="background-color: #ff9999;"'   # red

        # ---- Build <td> cells
        for col in ordered_attrs:
            if col == "severity":
                val = severity_for_row
            else:
                val = err.attrib.get(col, "")

            cells.append("<td>%s</td>" % html.escape(val))

        # ------------------------------------------------------------------
        # Build 'locations' cell with VS Code links
        # ------------------------------------------------------------------
        locations_html = []

        for loc in err.findall("location"):
            file_ = loc.attrib.get("file", "")
            line = loc.attrib.get("line", "")
            col_ = loc.attrib.get("column", "")
            info = loc.attrib.get("info", "")

            # Build relative display like "pltf/diagnostic.c:45:27"
            display_parts = []
            if file_:
                display_parts.append(file_)
            if line:
                display_parts.append(line)
            if col_:
                display_parts.append(col_)
            display_text = ":".join(display_parts)

            # Absolute path for VS Code link (resolved from XML directory)
            abs_path = os.path.abspath(os.path.join(xml_dir, file_)) if file_ else ""

            # VS Code URI: vscode://file/<absolute-path>:line[:col]
            if line:
                if col_:
                    vscode_target = "vscode://file/%s:%s:%s" % (
                        abs_path.replace("\\", "/"),
                        line,
                        col_,
                    )
                else:
                    vscode_target = "vscode://file/%s:%s" % (
                        abs_path.replace("\\", "/"),
                        line,
                    )
            else:
                # No line -> open file at start
                vscode_target = "vscode://file/%s" % abs_path.replace("\\", "/")

            # HTML link text (safe)
            link_text = html.escape(display_text) if display_text else html.escape(file_)

            # Info text
            info_html = ""
            if info:
                info_html = " - %s" % html.escape(info)

            if abs_path:
                link_html = '<a href="%s">%s</a>%s' % (
                    html.escape(vscode_target),
                    link_text,
                    info_html,
                )
            else:
                # Fallback: just plain text if path is missing
                link_html = "%s%s" % (link_text, info_html)

            locations_html.append(link_html)

        loc_html = "<br>".join(locations_html)
        cells.append("<td>%s</td>" % loc_html)

        # Append full row with optional inline style for background color
        rows_html.append("<tr%s>%s</tr>" % (row_style, "".join(cells)))

    # ------------------------------------------------------------------
    # Build HTML document
    # ------------------------------------------------------------------
    header_cells = "".join("<th>%s</th>" % html.escape(col) for col in columns)
    header_html = "<tr>%s</tr>" % header_cells

    # NOTE: removed zebra striping, to not override severity colors
    css = """
table {
    border-collapse: collapse;
    width: 100%;
    font-family: Arial, sans-serif;
    font-size: 14px;
}
th, td {
    border: 1px solid #ccc;
    padding: 4px 8px;
}
th {
    background-color: #f2f2f2;
}
tbody tr:hover td {
    background-color: #e8f2ff;
}
.meta {
    margin: 6px 0 10px 0;
}
"""

    title = "Cppcheck MISRA Results"

    # Metadata line (PC user + current date/time)
    tester = getpass.getuser()
    now = datetime.now()
    date_str = now.strftime("%d/%m/%y")
    time_str = now.strftime("%H:%M:%S")
    meta_line = "Tester: %s&nbsp;&nbsp;Date: %s&nbsp;&nbsp;Time: %s" % (
        html.escape(tester),
        html.escape(date_str),
        html.escape(time_str),
    )

    html_doc = """<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>%s</title>
<style>%s</style>
</head>
<body>
<h1>%s</h1>
<p class="meta"><strong>%s</strong></p>
<p>Source file: <code>%s</code></p>
<table>
<thead>%s</thead>
<tbody>%s</tbody>
</table>
</body>
</html>
""" % (
        title,
        css,
        title,
        meta_line,
        html.escape(xml_path),
        header_html,
        "".join(rows_html),
    )

    html_path = os.path.splitext(xml_path)[0] + ".html"

    # Write original HTML
    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html_doc)

    # ------------------------------------------------------------------
    # Clean unwanted escape sequences in the HTML
    # ------------------------------------------------------------------
    with open(html_path, "r", encoding="utf-8") as f:
        cleaned = (
            f.read()
            .replace("\\011", " ")
            .replace("\\342\\200\\246", "…")
        )

    with open(html_path, "w", encoding="utf-8") as f:
        f.write(cleaned)

    print("[OK] Cleaned and generated: %s" % html_path)

    # ------------------------------------------------------------------
    # Delete the source XML file
    # ------------------------------------------------------------------
    try:
        os.remove(xml_path)
        print("[OK] Deleted source XML: %s" % xml_path)
    except Exception as e:
        print("[WARNING] Could not delete %s: %s" % (xml_path, e))

    return html_path


def generate_cppcheck_html_reports(
    root_folder: Union[str, Path], misra_rules_path: Union[str, Path]
) -> None:
    """
    Walk root_folder recursively and convert every cppcheck_misra_results XML file to HTML.
    """
    root_folder = str(root_folder)
    for dirpath, _, filenames in os.walk(root_folder):
        for filename in filenames:
            if filename in ("cppcheck_misra_results.mxl", "cppcheck_misra_results.xml"):
                xml_path = os.path.join(dirpath, filename)
                try:
                    generate_html_for_cppcheck_xml(xml_path, misra_rules_path)
                except Exception as e:
                    print("[ERROR] Failed to process %s: %s" % (xml_path, e))


def find_targets_with_pltf_or_cfg(root: Path):
    """
    Walk 'root' and yield directories that contain a 'pltf' and/or 'cfg' subfolder.
    """
    for dirpath, dirnames, _ in os.walk(root):
        dirpath = Path(dirpath)

        has_pltf = "pltf" in dirnames and (dirpath / "pltf").is_dir()
        has_cfg = "cfg" in dirnames and (dirpath / "cfg").is_dir()

        if has_pltf or has_cfg:
            yield dirpath


def main():
    # Directory where script.py lives (your "main" directory)
    script_dir = Path(__file__).resolve().parent
    os.chdir(script_dir)  # ensure docker build uses this directory as context

    # Template CMakeLists.txt in main/
    template_path = script_dir / "CMakeLists.txt"
    if not template_path.is_file():
        raise SystemExit("ERROR: Template CMakeLists.txt not found at: %s" % template_path)

    template_content = template_path.read_text(encoding="utf-8")

    # Root of the codebase to scan
    codebase_root = script_dir / "code"
    if not codebase_root.is_dir():
        raise SystemExit("ERROR: code directory not found at: %s" % codebase_root)

    # MISRA rules file path (fixed default)
    misra_rules_path = MISRA_RULES_PATH

    print("Scanning for components under: %s" % codebase_root)
    print("Using MISRA rules file: %s" % misra_rules_path)

    created_cmakelists = []

    # 1) Create CMakeLists.txt for each eligible component dir
    for target_dir in find_targets_with_pltf_or_cfg(codebase_root):
        cmake_path = target_dir / "CMakeLists.txt"

        # Only create if it does NOT already exist
        if cmake_path.exists():
            print("Skipping existing CMakeLists.txt in: %s" % target_dir)
            continue

        project_name = target_dir.name  # e.g. 'swComponent1', 'swComponentXYZ'
        component_content = template_content.replace("projectName", project_name)

        print("Creating CMakeLists.txt in: %s (projectName -> %s)" % (target_dir, project_name))
        cmake_path.write_text(component_content, encoding="utf-8")
        created_cmakelists.append(cmake_path)

    if not created_cmakelists:
        print("No new CMakeLists.txt files were created.")
    else:
        print("\nCMakeLists.txt created in:")
        for p in created_cmakelists:
            print("  - %s" % p)

    # 2) Run docker build + docker run, then clean up generated CMakeLists
    try:
        # Build the Docker image
        print("\n[Docker] Building image: cmake-misra-multi")
        subprocess.run(
            ["docker", "build", "-t", "cmake-misra-multi", "."],
            check=True,
        )

        # Run the container with the current project mounted to /workspace
        cwd = str(script_dir.resolve())
        print("[Docker] Running analysis with workspace: %s" % cwd)
        subprocess.run(
            [
                "docker",
                "run",
                "--rm",
                "-v",
                "%s:/workspace" % cwd,
                "cmake-misra-multi",
                "build-and-check-all.sh",
            ],
            check=True,
        )

    finally:
        # 3) Delete only the CMakeLists.txt files created by this script
        if created_cmakelists:
            print("\nCleaning up generated CMakeLists.txt files...")
            for cmake_path in created_cmakelists:
                try:
                    cmake_path.unlink()
                    print("  Removed %s" % cmake_path)
                except FileNotFoundError:
                    print("  Already removed: %s" % cmake_path)
        else:
            print("\nNo generated CMakeLists.txt to clean up.")

    # 4) Generate HTML reports from cppcheck MISRA XML
    generate_cppcheck_html_reports(codebase_root, misra_rules_path)
    print("\nDone.")


if __name__ == "__main__":
    main()
