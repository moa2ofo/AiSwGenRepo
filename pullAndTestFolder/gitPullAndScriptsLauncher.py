import os
import re
import shutil
import subprocess
import sys
from urllib.parse import urlparse

# Exit codes
EXIT_BAD_GITHUB_URL = 1
EXIT_MISSING_GITHUB_COMMIT_INFO = 2
EXIT_BRANCH_GITHUB_BAD_FORMED = 3
EXIT_GIT_CLONE_FAILED = 4
EXIT_LAUNCH_SCRIPT_NOT_FOUND = 5
EXIT_LAUNCH_SCRIPT_FAILED = 6


def print_help(prog_name: str) -> None:
    print(
        f"""Usage:
  {prog_name} <github_url> [--dir <clone_dir>] [--no-submodules] [--python <python_exe>]

Where <github_url> can be:
  - Repo URL:
      https://github.com/owner/repo
  - Tree URL (branch/tag embedded):
      https://github.com/owner/repo/tree/<branch_or_tag>

Options:
  --dir             Target directory for clone (default: ./<repo_name>)
  --no-submodules   Do not recurse into submodules
  --python          Python executable to run launchAll.py (default: current interpreter)
  -h, --help        Show this help message

Behavior:
  1) Clones the repo (branch/tag inferred from URL, default: main)
  2) Runs: <python_exe> launchAll.py
     from: <clone_dir>/project

Examples:
  {prog_name} https://github.com/moa2ofo/AiSwGenRepo/tree/testAiGeneration
  {prog_name} https://github.com/moa2ofo/AiSwGenRepo --python python
  {prog_name} https://github.com/moa2ofo/AiSwGenRepo/tree/testAiGeneration --dir ./AiSwGenRepo --python "C:\\Python311\\python.exe"
"""
    )


def check_branch(branch: str) -> None:
    if branch is None or branch.strip() == "":
        sys.exit(EXIT_MISSING_GITHUB_COMMIT_INFO)
    # Allow common branch/tag patterns including slashes
    if re.fullmatch(r"[0-9A-Za-z._\-/]+", branch) is None:
        sys.exit(EXIT_BRANCH_GITHUB_BAD_FORMED)


def parse_github_url(url: str):
    """
    Accepts:
      - https://github.com/owner/repo
      - https://github.com/owner/repo/tree/<branch>
    Returns: (repo_url, branch_or_none, repo_name)
    """
    try:
        u = urlparse(url)
    except Exception:
        sys.exit(EXIT_BAD_GITHUB_URL)

    if u.scheme not in ("http", "https") or u.netloc.lower() != "github.com":
        sys.exit(EXIT_BAD_GITHUB_URL)

    parts = [p for p in u.path.split("/") if p]
    if len(parts) < 2:
        sys.exit(EXIT_BAD_GITHUB_URL)

    owner, repo = parts[0], parts[1]
    repo_name = repo[:-4] if repo.endswith(".git") else repo

    repo_url = f"{u.scheme}://{u.netloc}/{owner}/{repo_name}"

    branch = None
    if len(parts) >= 4 and parts[2] == "tree":
        branch = "/".join(parts[3:])
        check_branch(branch)

    return repo_url, branch, repo_name


def force_rmtree(path: str) -> None:
    import stat

    path = os.path.abspath(path)

    def onerror(func, p, exc_info):
        exc = exc_info[1]
        if isinstance(exc, FileNotFoundError):
            return
        try:
            if not os.path.exists(p):
                return
            os.chmod(p, stat.S_IWRITE)
            func(p)
        except FileNotFoundError:
            return

    if os.path.exists(path):
        shutil.rmtree(path, onerror=onerror)


def exec_prj_clone_from_github(repository: str, branch: str, clone_dir: str, recurse_submodules: bool = True):
    if not repository.startswith("http") or "://github.com/" not in repository:
        sys.exit(EXIT_BAD_GITHUB_URL)

    check_branch(branch)

    if os.path.exists(clone_dir):
        force_rmtree(clone_dir)

    cmd = ["git", "clone"]
    if recurse_submodules:
        cmd += ["--recurse-submodules"]
    cmd += ["--branch", branch, repository, clone_dir]

    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError:
        sys.exit(EXIT_GIT_CLONE_FAILED)


def run_launch_all(clone_dir: str, python_exe: str) -> None:
    project_dir = os.path.join(clone_dir, "project")
    script_path = os.path.join(project_dir, "launchAll.py")

    if not os.path.isfile(script_path):
        print(f"ERROR: launch script not found: {script_path}")
        sys.exit(EXIT_LAUNCH_SCRIPT_NOT_FOUND)

    cmd = [python_exe, "launchAll.py"]

    try:
        # Run in <clone_dir>/project so relative paths inside launchAll.py work.
        subprocess.run(cmd, check=True, cwd=project_dir)
    except subprocess.CalledProcessError as e:
        print(f"ERROR: launchAll.py failed with exit code {e.returncode}")
        sys.exit(EXIT_LAUNCH_SCRIPT_FAILED)


def parse_args(argv):
    """
    CLI:
      script.py <github_url> [--dir Y] [--no-submodules] [--python <python_exe>]
    """
    if not argv or argv[0] in ("-h", "--help"):
        print_help(os.path.basename(sys.argv[0]))
        sys.exit(0)

    github_url = argv[0]
    clone_dir = None
    recurse_submodules = True
    python_exe = sys.executable  # default: same python running this script

    i = 1
    while i < len(argv):
        a = argv[i]
        if a in ("-h", "--help"):
            print_help(os.path.basename(sys.argv[0]))
            sys.exit(0)

        if a == "--dir":
            if i + 1 >= len(argv):
                sys.exit(1)
            clone_dir = argv[i + 1]
            i += 2
            continue

        if a == "--no-submodules":
            recurse_submodules = False
            i += 1
            continue

        if a == "--python":
            if i + 1 >= len(argv):
                sys.exit(1)
            python_exe = argv[i + 1]
            i += 2
            continue

        print(f"ERROR: Unknown argument: {a}\n")
        print_help(os.path.basename(sys.argv[0]))
        sys.exit(1)

    repo_url, branch_from_url, repo_name = parse_github_url(github_url)

    branch = branch_from_url or "main"
    check_branch(branch)

    if not clone_dir:
        clone_dir = f"./{repo_name}"

    return repo_url, branch, clone_dir, recurse_submodules, python_exe


if __name__ == "__main__":
    repo_url, branch, clone_dir, recurse_submodules, python_exe = parse_args(sys.argv[1:])

    exec_prj_clone_from_github(
        repository=repo_url,
        branch=branch,
        clone_dir=clone_dir,
        recurse_submodules=recurse_submodules,
    )

    print(f"Repository cloned into {clone_dir} (branch/tag: {branch})")

    run_launch_all(clone_dir=clone_dir, python_exe=python_exe)
    print("launchAll.py executed successfully.")
