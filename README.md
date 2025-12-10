# Project Overview

This project is designed to manage unit testing across multiple modules, update platform files automatically, execute tests, and generate consolidated reports of the results.

---

## 📂 Project Structure


- **code/** → Contains source code for each module.  
  - **cfg/** → Configuration files.  
  - **pltf/** → Platform files updated by the script.  
  - **unitTests/** → Unit test definitions.  
- **mixin/** → Shared or reusable components.  
- **utExecutionAndResults/** → Handles execution and results of unit tests.  
  - **utUnderTest/** → Tests currently being executed.  
  - **utResults/** → Stores results of executed tests.  
    - **total_result_report.txt** → Consolidated report of all test results.

---

## 🛠️ Script Functionality

The script performs three main tasks:
1. **Collect all the test folder**  
   Search all the forders TEST_ that contain unit tests.

1. **Update platform files**  
   It updates the files contained in the `pltf` folder of each module to ensure the test configuration is refreshed.

2. **Run unit tests**  
   After updating, the script automatically launches the unit tests defined in the `unitTests` folders.

3. **Update the report**  
   Once all/one tests are executed, the script creates or updates the file:

This file contains a summary of all executed tests.

---

## 📊 Report Format

The report file is structured as a table with aligned columns:

- **Function Name Under Test** → Name of the function/module tested.  
- **Total** → Total number of tests executed.  
- **Passed** → Number of tests that passed.  
- **Failed** → Number of tests that failed.  
- **Ignored** → Number of tests ignored/skipped.

---

## 🚀 Usage Workflow

1. Place or update unit test files in the appropriate `unitTests` folder.  
2. Run the script parsingUtTest.py in the folder main/project passing the augument all:
   - Update the `pltf` files.  
   - Execute all unit tests.  
   - Generate/update the consolidated report.  
3. Review results in utExecutionAndResults/utResults/total_result_report.txt



