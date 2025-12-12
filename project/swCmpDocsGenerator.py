#!/usr/bin/env python3
import os
import re
import shutil
import subprocess
from pathlib import Path
import sys


IMAGE_NAME = "doxygen-plantuml"

# Template names (as requested)
TEMPLATE_DOCKERFILE_PRIMARY = "DoxDockerfile"
TEMPLATE_DOXYFILE_PRIMARY = "Doxygen"

# Fallback names (what you typically have)
TEMPLATE_DOCKERFILE_FALLBACK = "Dockerfile"
TEMPLATE_DOXYFILE_FALLBACK = "Doxyfile"

# Destination names inside each target folder
DEST_DOCKERFILE = "Dockerfile"
DEST_DOXYFILE = "Doxyfile"


def find_targets_with_pltf_or_cfg(root: Path):
    """Yield directories under root that contain a 'pltf' and/or 'cfg' subfolder."""
    for dirpath, dirnames, _ in os.walk(root):
        dirpath = Path(dirpath)

        has_pltf = "pltf" in dirnames and (dirpath / "pltf").is_dir()
        has_cfg = "cfg" in dirnames and (dirpath / "cfg").is_dir()

        if has_pltf or has_cfg:
            yield dirpath


def resolve_template(script_dir: Path, primary: str, fallback: str) -> Path:
    p = script_dir / primary
    if p.is_file():
        return p
    p2 = script_dir / fallback
    if p2.is_file():
        return p2
    raise SystemExit(f"ERROR: Template not found. Tried: {primary} and {fallback} in {script_dir}")


def docker_mount_path(p: Path) -> str:
    """
    Convert to a Docker-friendly absolute path for -v.
    On Windows + Docker Desktop, forward slashes are safer.
    """
    return str(p.resolve()).replace("\\", "/")


def patch_doxyfile(doxy_path: Path, project_name: str, has_pltf: bool, has_cfg: bool) -> None:
    """
    - Set PROJECT_NAME to project_name
    - Set INPUT to include existing ./pltf and/or ./cfg in ONE line
      (Avoids multiple INPUT lines overriding each other)
    """
    content = doxy_path.read_text(encoding="utf-8", errors="replace")

    # Replace PROJECT_NAME (robust: handles spaces around '=')
    if re.search(r"^\s*PROJECT_NAME\s*=", content, flags=re.MULTILINE):
        content = re.sub(
            r"^\s*PROJECT_NAME\s*=.*$",
            f'PROJECT_NAME           = "{project_name}"',
            content,
            flags=re.MULTILINE,
        )
    else:
        # If missing, prepend it
        content = f'PROJECT_NAME           = "{project_name}"\n' + content

    # Build INPUT list based on what exists
    inputs = []
    if has_cfg:
        inputs.append("./cfg")
    if has_pltf:
        inputs.append("./pltf")

    input_line = "INPUT                  = " + " ".join(inputs)

    # Remove all existing INPUT lines, then add the single correct one
    content = re.sub(r"^\s*INPUT\s*=.*$\n?", "", content, flags=re.MULTILINE)
    content = input_line + "\n" + content

    doxy_path.write_text(content, encoding="utf-8")


def main():
    script_dir = Path(__file__).resolve().parent
    codebase_root = script_dir / "code"

    if not codebase_root.is_dir():
        raise SystemExit(f"ERROR: code directory not found at: {codebase_root}")

    template_dockerfile = resolve_template(
        script_dir, TEMPLATE_DOCKERFILE_PRIMARY, TEMPLATE_DOCKERFILE_FALLBACK
    )
    template_doxyfile = resolve_template(
        script_dir, TEMPLATE_DOXYFILE_PRIMARY, TEMPLATE_DOXYFILE_FALLBACK
    )

    print(f"Template Dockerfile : {template_dockerfile}")
    print(f"Template Doxyfile   : {template_doxyfile}")
    print(f"Scanning targets in : {codebase_root}\n")

    targets = list(find_targets_with_pltf_or_cfg(codebase_root))
    if not targets:
        print("No folders found containing 'pltf' or 'cfg'.")
        return

    for target_dir in targets:
        has_pltf = (target_dir / "pltf").is_dir()
        has_cfg = (target_dir / "cfg").is_dir()
        project_name = target_dir.name

        dest_dockerfile = target_dir / DEST_DOCKERFILE
        dest_doxyfile = target_dir / DEST_DOXYFILE

        # Backup any existing files so we can restore them
        docker_backup = None
        doxy_backup = None

        print("------------------------------------------------------------")
        print(f"Target: {target_dir}")
        print(f"  - has cfg : {has_cfg}")
        print(f"  - has pltf: {has_pltf}")
        print(f"  - PROJECT_NAME -> {project_name}")

        try:
            if dest_dockerfile.exists():
                docker_backup = target_dir / (DEST_DOCKERFILE + ".bak")
                shutil.move(str(dest_dockerfile), str(docker_backup))

            if dest_doxyfile.exists():
                doxy_backup = target_dir / (DEST_DOXYFILE + ".bak")
                shutil.move(str(dest_doxyfile), str(doxy_backup))

            # Copy templates in place
            shutil.copy2(str(template_dockerfile), str(dest_dockerfile))
            shutil.copy2(str(template_doxyfile), str(dest_doxyfile))

            # Patch Doxyfile
            patch_doxyfile(dest_doxyfile, project_name, has_pltf=has_pltf, has_cfg=has_cfg)

            # Run docker build + run inside the target directory
            print(f"\n[Docker] Building image in: {target_dir}")
            subprocess.run(
                ["docker", "build", "-t", IMAGE_NAME, "."],
                cwd=str(target_dir),
                check=True,
            )

            mount = docker_mount_path(target_dir)
            print(f"[Docker] Running doxygen with mount: {mount} -> /workspace")
            subprocess.run(
                ["docker", "run", "--rm", "-v", f"{mount}:/workspace", IMAGE_NAME],
                cwd=str(target_dir),
                check=True,
            )

            print("[OK] Documentation generated (check ./docs/html/index.html if OUTPUT_DIRECTORY=./docs).")

        except subprocess.CalledProcessError as e:
            print(f"[ERROR] Command failed in {target_dir}: {e}")
        finally:
            # Remove copied template files and restore previous ones if they existed
            try:
                if dest_dockerfile.exists():
                    dest_dockerfile.unlink()
            except Exception as ex:
                print(f"[WARN] Could not remove {dest_dockerfile}: {ex}")

            try:
                if dest_doxyfile.exists():
                    dest_doxyfile.unlink()
            except Exception as ex:
                print(f"[WARN] Could not remove {dest_doxyfile}: {ex}")

            if docker_backup and docker_backup.exists():
                shutil.move(str(docker_backup), str(dest_dockerfile))

            if doxy_backup and doxy_backup.exists():
                shutil.move(str(doxy_backup), str(dest_doxyfile))

            print("[Cleanup] Done.\n")

def print_help():
    print(
        """
swCmpDocsGenerator.py
=====================

DESCRIPTION
-----------
Generates Doxygen documentation (with PlantUML diagrams) for each software
component under ./code that contains a 'pltf' and/or 'cfg' folder.

For each matching component, the script will:
  1. Copy DoxDockerfile and Doxygen (or Dockerfile / Doxyfile as fallback)
     into the component folder.
  2. Replace PROJECT_NAME in the Doxyfile with the component folder name.
  3. Adjust INPUT in the Doxyfile to include ./pltf and/or ./cfg.
  4. Run:
       docker build -t doxygen-plantuml .
       docker run --rm -v "<component_path>:/workspace" doxygen-plantuml
  5. Remove the copied files and restore any originals.

REQUIREMENTS
------------
- Docker installed and running
- The script must be executed from the project root
- Folder structure:
      ./code/<component>/(pltf|cfg)

USAGE
-----
  python swCmpDocsGenerator.py
  python swCmpDocsGenerator.py -help
  python swCmpDocsGenerator.py --help

OUTPUT
------
Documentation is generated according to the Doxyfile settings
(e.g. ./docs/html/index.html inside each component).

"""
    )


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] in ("-h", "--help", "-help"):
        print_help()
        sys.exit(0)

    main()
