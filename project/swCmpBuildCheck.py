#!/usr/bin/env python3
import os
import subprocess
from pathlib import Path


def find_targets_with_pltf_or_cfg(root: Path):
    """
    Walk 'root' and yield directories that contain
    a 'pltf' and/or 'cfg' subfolder.
    """
    for dirpath, dirnames, filenames in os.walk(root):
        dirpath = Path(dirpath)

        has_pltf = "pltf" in dirnames and (dirpath / "pltf").is_dir()
        has_cfg = "cfg" in dirnames and (dirpath / "cfg").is_dir()

        if has_pltf or has_cfg:
            yield dirpath, has_pltf, has_cfg


def generate_cmakelists_content(project_name: str, has_pltf: bool, has_cfg: bool) -> str:
    """
    Generate the content of CMakeLists.txt for a given project.
    project_name: will be used in 'project(<name> C)' and 'add_library(<name> ...)'.
    has_pltf / has_cfg control which include_directories and GLOB entries are added.
    """
    lines = []
    lines.append("cmake_minimum_required(VERSION 3.16)")
    lines.append(f"project({project_name} C)")
    lines.append("")
    lines.append("set(CMAKE_EXPORT_COMPILE_COMMANDS ON)")
    lines.append("")
    lines.append("# Include directories")
    lines.append("include_directories(")
    if has_pltf:
        lines.append("    ${CMAKE_CURRENT_SOURCE_DIR}/pltf")
    if has_cfg:
        lines.append("    ${CMAKE_CURRENT_SOURCE_DIR}/cfg")
    lines.append(")")
    lines.append("")
    lines.append("# Automatically collect sources")
    lines.append("file(GLOB SOURCES")
    if has_pltf:
        lines.append('    "${CMAKE_CURRENT_SOURCE_DIR}/pltf/*.c"')
    if has_cfg:
        lines.append('    "${CMAKE_CURRENT_SOURCE_DIR}/cfg/*.c"')
    lines.append(")")
    lines.append("")
    lines.append(f"add_library({project_name} STATIC ${{SOURCES}})")
    lines.append("")

    return "\n".join(lines)


def main():
    # Directory where script.py lives (your "main" directory)
    script_dir = Path(__file__).resolve().parent
    os.chdir(script_dir)  # ensure docker build uses this as context

    # Root of the codebase to scan
    codebase_root = script_dir / "code"

    if not codebase_root.is_dir():
        raise SystemExit(f"ERROR: code directory not found at: {codebase_root}")

    print(f"Scanning for components under: {codebase_root}")

    created_cmakelists = []

    # 1) Create CMakeLists.txt where needed
    for target_dir, has_pltf, has_cfg in find_targets_with_pltf_or_cfg(codebase_root):
        cmake_path = target_dir / "CMakeLists.txt"

        # Only create if it does NOT already exist
        if cmake_path.exists():
            print(f"Skipping existing CMakeLists.txt in: {target_dir}")
            continue

        project_name = target_dir.name  # e.g. 'swComponent', 'swComponentXYZ', etc.
        content = generate_cmakelists_content(project_name, has_pltf, has_cfg)

        print(f"Creating CMakeLists.txt in: {target_dir} (project: {project_name})")
        cmake_path.write_text(content, encoding="utf-8")
        created_cmakelists.append(cmake_path)

    if not created_cmakelists:
        print("No new CMakeLists.txt files were created (nothing to do).")
    else:
        print("\nCMakeLists.txt created in:")
        for p in created_cmakelists:
            print(f"  - {p}")

    # 2) Run docker build + docker run, then clean up
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
                    # In case someone manually removed/changed it in the meantime
                    print(f"  Already removed: {cmake_path}")
        else:
            print("\nNo generated CMakeLists.txt to clean up.")

    print("\nDone.")


if __name__ == "__main__":
    main()