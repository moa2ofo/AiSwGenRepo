# Project Overview

This project manages unit testing, platform file synchronization, build automation, and static code analysis across multiple C software modules.

It automatically updates platform files, executes unit tests, generates consolidated test result reports, and performs static analysis using **cppcheck** with **MISRA C:2012** rules.

A Docker-based environment is provided to ensure reproducible builds and consistent analysis results across different machines.

The goal of the project is to guarantee consistency, traceability, and quality compliance across all software components.

## Project Structure
---
```
📂 Project Structure
code/                             # Source code for each software component
 ├─ swComponent1/
 │   ├─ build/                    # Build output
 │   ├─ cppcheck_misra_results/   # MISRA rule violations (XML + HTML)
 │   ├─ cfg/                      # Configuration files
 │   ├─ docs/                     # Doxygen documentation
 │   ├─ Doxyfile                  # Doxygen configuration
 │   ├─ pltf/                     # Platform files (auto-updated by scripts)
 │   └─ unitTests/                # Unit test definitions
 ├─ swComponent2/
 │   ├─ build/
 │   ├─ cppcheck_misra_results/
 │   ├─ cfg/
 │   ├─ docs/
 │   ├─ Doxyfile
 │   ├─ pltf/
 │   └─ unitTests/

mixin/                            # Shared or reusable software components

misra/                            # MISRA C:2012 rules headlines file for cppcheck

utExecutionAndResults/
 ├─ utUnderTest/                  # Tests currently under execution
 └─ utResults/
     └─ total_result_report.txt   # Consolidated unit test results

Dockerfile
  Docker environment for build, test, and MISRA analysis

unitTestsLauncher
  Script used to launch all unit tests or a specific one (use help to see functionalities)

project
  Ceedling configuration used to execute unit tests

swCmpBuildCheck.py
  Script used to build and perform static checks on all software components inside the code folder

CMakeLists.txt
  Template CMake configuration used during the build process
```

## UnitTestsLauncher Script Functionality

The `unitTestsLauncher` script automates the execution of unit tests across all software modules and performs the following operations:

### Collect all test folders

The script scans all modules and finds folders named `TEST_*` that contain unit tests.

### Update platform files

The `pltf` folder of each module is updated to ensure test configurations are synchronized.

### Run unit tests

All unit tests located in the `unitTests` directories are executed.

### Update the consolidated report

After execution, the script creates or updates the consolidated report located at:

```text
utExecutionAndResults/utResults/total_result_report.txt
```

This file contains a global summary of all executed unit tests.

## Report Format

The consolidated report contains a table with the following columns:

* **Function Name Under Test**
  The function or module being tested

* **Total**
  Total number of executed tests

* **Passed**
  Number of tests that passed

* **Failed**
  Number of tests that failed

* **Ignored**
  Number of skipped or disabled tests

The table is aligned to ensure easy readability.

## Unit Test Usage Workflow

1. Place or update unit test files inside each module’s `unitTests` folder.
2. From the main project directory, run:

```bash
python parsingUtTest.py all
```

3. The script will:

   * Update the `pltf` files
   * Execute all unit tests
   * Update the consolidated report

4. Review the results in:

```text
utExecutionAndResults/utResults/total_result_report.txt
```

## SwCmpBuildCheck Script Functionality

The `swCmpBuildCheck.py` script automates the build and static analysis of all software components located inside the `code` directory.

Its purpose is to ensure that each software component:

* Builds correctly using **CMake**
* Is compiled with **GCC**
* Is analyzed using **cppcheck** with **MISRA C:2012** rules
* Produces readable and traceable HTML analysis reports

All operations are executed inside a Docker container to guarantee a reproducible environment.

### Component discovery

The script recursively scans the `code` directory and identifies components containing a `pltf` folder and/or a `cfg` folder.

### Temporary CMakeLists generation

If a component does not already contain a `CMakeLists.txt` file, one is generated automatically.
The project name is derived from the component directory name.
Existing `CMakeLists.txt` files are never overwritten.

### Docker-based build and analysis

The Docker image defined by the `Dockerfile` is built.
The project is mounted into `/workspace` inside the container.
For each component, the script:

* Runs CMake configure and build
* Generates `compile_commands.json`
* Executes cppcheck with the MISRA addon

### HTML report generation

`cppcheck_misra_results.xml` files are converted into HTML reports.
MISRA severities are resolved using the MISRA rules file.
Violations are color-coded as **Advisory**, **Required**, or **Mandatory**.
File references are clickable **VS Code links**.
Tester name, date, and time are embedded in the report.

### Cleanup

Only the `CMakeLists.txt` files generated by the script are removed.
The original cppcheck XML files are deleted after HTML generation.

## SwCmpBuildCheck Usage

From the main project directory, run:

```bash
python swCmpBuildCheck.py
```

To display help and exit:

```bash
python swCmpBuildCheck.py -h
```

or:

```bash
python swCmpBuildCheck.py -help
```

## Docker-Based Build System

The Docker environment ensures consistent compiler versions, reproducible builds, and identical static analysis results across machines.

The Docker image includes:

* GCC
* CMake
* cppcheck with MISRA addon
* Required Python tooling

All builds and analyses are executed inside the container, while results are written back to the host filesystem.

## Summary

This project provides automated unit test execution and consolidated reporting, scalable build and static analysis across multiple software components, MISRA-compliant static checks, clean HTML reports with traceable diagnostics, and Docker-based reproducibility.

It is designed for embedded C projects where quality, compliance, and automation are mandatory.

---
## Docker-Based Build System

The Docker environment ensures consistent compiler versions, reproducible builds, and identical static analysis results across machines.

The Docker image includes:

* GCC
* CMake
* cppcheck with MISRA addon
* Required Python tooling

All builds and analyses are executed inside the container, while results are written back to the host filesystem.

---

## SwCmpDocsGenerator Script Functionality

* The swCmpDocsGenerator.py script automates the generation of Doxygen documentation for all software components located inside the code directory.

* Its purpose is to ensure that each software component:

* Is documented using a consistent and reproducible environment

Generates complete HTML documentation using Doxygen

* Includes UML, call, include, and dependency diagrams

* Renders UML diagrams using PlantUML and Graphviz

* Remains fully independent from build and static analysis workflows

* All documentation steps are executed inside a Docker container to guarantee deterministic and repeatable results.

## Component discovery

* The script recursively scans the code directory and identifies software components containing a pltf folder and/or a cfg folder.
 
* Each identified directory is treated as an independent documentation unit.

* For each component, the script executes the documentation workflow directly inside the component directory.

* The following commands are executed:

* docker build -t doxygen-plantuml .

* docker run --rm -v "${PWD}:/workspace" doxygen-plantuml

## Documentation output

* The output directory is defined by the OUTPUT_DIRECTORY parameter in the Doxyfile, for example ./docs/html/index.html.

## Cleanup

* After documentation generation is completed, the script:

* Removes the copied documentation Dockerfile and Doxyfile

## Usage

* From the main project directory, run:

* python swCmpDocsGenerator.py

* To display help and exit:

* python swCmpDocsGenerator.py -h

* python swCmpDocsGenerator.py -help
