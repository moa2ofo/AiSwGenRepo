#!/usr/bin/env python3

import subprocess
import sys


def print_help():
    print(
        """
launchAll.py
============

DESCRIPTION
-----------
This script executes the complete software verification and documentation
workflow in a predefined order.

It sequentially runs:

* Unit tests for all components
* Build and static analysis checks
* Doxygen documentation generation

Each step is executed as an external Python script.
If any step fails, the execution stops immediately.

EXECUTED COMMANDS
-----------------
1. python unitTestsLauncher.py all
2. python swCmpBuildCheck.py
3. python swCmpDocsGenerator.py

USAGE
-----
python launchAll.py
python launchAll.py -h
python launchAll.py -help

NOTES
-----
* The script must be executed from the main project directory
* Docker must be installed and running
* A non-zero exit code is returned if any step fails
"""
    )


def run(cmd):
    print("\n============================================================")
    print(f"Running: {' '.join(cmd)}")
    print("============================================================\n")

    subprocess.run(cmd, check=True)


def main():
    try:
        run(["python", "unitTestsLauncher.py", "all"])
        run(["python", "swCmpBuildCheck.py"])
        run(["python", "swCmpDocsGenerator.py"])
    except subprocess.CalledProcessError as e:
        print("\nERROR: Command failed:", e)
        sys.exit(e.returncode)

    print("\nAll steps completed successfully.")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] in ("-h", "--help", "-help"):
        print_help()
        sys.exit(0)

    main()
