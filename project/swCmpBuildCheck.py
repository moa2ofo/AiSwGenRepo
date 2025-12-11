#!/usr/bin/env python3
import os
import subprocess
from pathlib import Path


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

    print("\nDone.")


if __name__ == "__main__":
    main()
