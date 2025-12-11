#!/usr/bin/env python3
import os
import sys
import xml.etree.ElementTree as ET
import html
import subprocess
from pathlib import Path

def generate_html_for_cppcheck_xml(xml_path: str) -> str:
    """
    Parse cppcheck_misra_results.xml and generate cppcheck_misra_results.html.
    After generating HTML, remove '\011' and '\342\200\246' from the HTML file.
    Remove also the entire 'file0' and 'verbose' columns.
    Finally, delete the original XML file.
    """
    tree = ET.parse(xml_path)
    root = tree.getroot()

    errors = root.findall(".//error")
    if not errors:
        print(f"[INFO] No <error> elements found in {xml_path}")
        return ""

    # ------------------------------------------------------------------
    # Collect attribute names, then REMOVE 'file0' and 'verbose'
    # ------------------------------------------------------------------
    attr_names = set()
    for err in errors:
        attr_names.update(err.attrib.keys())

    attr_names.discard("cwe")
    attr_names.discard("file0")      # REMOVE ENTIRE COLUMN
    attr_names.discard("verbose")    # REMOVE ENTIRE COLUMN

    # Order wished WITHOUT file0 and verbose
    preferred_order = ["id", "severity", "file1", "msg"]

    ordered_attrs = [a for a in preferred_order if a in attr_names]
    ordered_attrs.extend(sorted(attr_names - set(ordered_attrs)))

    # Add custom column for locations
    columns = ordered_attrs + ["locations"]

    # ------------------------------------------------------------------
    # Build HTML rows
    # ------------------------------------------------------------------
    rows_html = []

    for err in errors:
        cells = []

        # Only include selected columns
        for col in ordered_attrs:
            val = err.attrib.get(col, "")
            cells.append(f"<td>{html.escape(val)}</td>")

        # Build 'locations' cell
        locations = []
        for loc in err.findall("location"):
            file_ = loc.attrib.get("file", "")
            line = loc.attrib.get("line", "")
            col_ = loc.attrib.get("column", "")
            info = loc.attrib.get("info", "")

            parts = [p for p in (file_, line, col_) if p]
            pos = ":".join(parts)
            loc_str = f"{pos} - {info}" if pos and info else pos or info
            locations.append(html.escape(loc_str))

        loc_html = "<br>".join(locations)
        cells.append(f"<td>{loc_html}</td>")

        rows_html.append("<tr>" + "".join(cells) + "</tr>")

    # ------------------------------------------------------------------
    # Build HTML document
    # ------------------------------------------------------------------
    header_html = "<tr>" + "".join(f"<th>{html.escape(col)}</th>" for col in columns) + "</tr>"

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
tr:nth-child(even) td {
    background-color: #fafafa;
}
tbody tr:hover td {
    background-color: #e8f2ff;
}
"""

    title = f"Cppcheck MISRA Results - {html.escape(os.path.basename(xml_path))}"

    html_doc = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>{title}</title>
<style>{css}</style>
</head>
<body>
<h1>{title}</h1>
<p>Source file: <code>{html.escape(xml_path)}</code></p>
<table>
<thead>{header_html}</thead>
<tbody>{''.join(rows_html)}</tbody>
</table>
</body>
</html>
"""

    html_path = os.path.splitext(xml_path)[0] + ".html"

    # Write original HTML
    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html_doc)

    # ------------------------------------------------------------------
    # Clean unwanted escape sequences
    # ------------------------------------------------------------------
    with open(html_path, "r", encoding="utf-8") as f:
        cleaned = (
            f.read()
            .replace("\\011", " ")
            .replace("\\342\\200\\246", "…")
        )

    with open(html_path, "w", encoding="utf-8") as f:
        f.write(cleaned)

    print(f"[OK] Cleaned and generated: {html_path}")

    # ------------------------------------------------------------------
    # Delete the source XML file
    # ------------------------------------------------------------------
    try:
        os.remove(xml_path)
        print(f"[OK] Deleted source XML: {xml_path}")
    except Exception as e:
        print(f"[WARNING] Could not delete {xml_path}: {e}")

    return html_path




def generate_cppcheck_html_reports(root_folder: str) -> None:
    """
    Walk root_folder recursively and convert every cppcheck_misra_results XML file to HTML.
    """
    for dirpath, _, filenames in os.walk(root_folder):
        for filename in filenames:
            if filename in ("cppcheck_misra_results.mxl", "cppcheck_misra_results.xml"):
                xml_path = os.path.join(dirpath, filename)
                try:
                    generate_html_for_cppcheck_xml(xml_path)
                except Exception as e:
                    print(f"[ERROR] Failed to process {xml_path}: {e}")


def find_targets_with_pltf_or_cfg(root: Path):
    """
    Walk 'root' and yield directories that contain a 'pltf' and/or 'cfg' subfolder.
    """
    for dirpath, dirnames, filenames in os.walk(root):
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
        raise SystemExit(f"ERROR: Template CMakeLists.txt not found at: {template_path}")

    template_content = template_path.read_text(encoding="utf-8")

    # Root of the codebase to scan
    codebase_root = script_dir / "code"
    if not codebase_root.is_dir():
        raise SystemExit(f"ERROR: code directory not found at: {codebase_root}")

    print(f"Scanning for components under: {codebase_root}")

    created_cmakelists = []

    # 1) Create CMakeLists.txt for each eligible component dir
    for target_dir in find_targets_with_pltf_or_cfg(codebase_root):
        cmake_path = target_dir / "CMakeLists.txt"

        # Only create if it does NOT already exist
        if cmake_path.exists():
            print(f"Skipping existing CMakeLists.txt in: {target_dir}")
            continue

        project_name = target_dir.name  # e.g. 'swComponent1', 'swComponentXYZ'
        component_content = template_content.replace("projectName", project_name)

        print(f"Creating CMakeLists.txt in: {target_dir} (projectName -> {project_name})")
        cmake_path.write_text(component_content, encoding="utf-8")
        created_cmakelists.append(cmake_path)

    if not created_cmakelists:
        print("No new CMakeLists.txt files were created.")
    else:
        print("\nCMakeLists.txt created in:")
        for p in created_cmakelists:
            print(f"  - {p}")

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
        print(f"[Docker] Running analysis with workspace: {cwd}")
        subprocess.run(
            [
                "docker",
                "run",
                "--rm",
                "-v",
                f"{cwd}:/workspace",
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
                    print(f"  Removed {cmake_path}")
                except FileNotFoundError:
                    print(f"  Already removed: {cmake_path}")
        else:
            print("\nNo generated CMakeLists.txt to clean up.")
    generate_cppcheck_html_reports(codebase_root)
    print("\nDone.")


if __name__ == "__main__":
    main()
