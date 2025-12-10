FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Build tools + CMake + cppcheck + python deps
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        cppcheck \
        python3 \
        python3-pip \
        python3-lxml \
        python3-pygments \
        git \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# Workspace where the project will be mounted
WORKDIR /workspace

# misra.json configuration for cppcheck addon
RUN mkdir -p /opt/misra && \
    printf '%s\n' \
'{' \
'    "script": "misra.py",' \
'    "args": [' \
'        "--rule-texts=/workspace/misra/misra_c_2012_headlines.txt"' \
'    ],' \
'    "ctu": true' \
'}' > /opt/misra/misra.json

# Script to run MISRA-like cppcheck on a single project
# Usage:
#   run-misra-check.sh              # use current working dir
#   run-misra-check.sh /path/proj   # use given directory
RUN printf '%s\n' \
'#!/usr/bin/env bash' \
'set -euo pipefail' \
'' \
'PROJ_DIR="${1:-$(pwd)}"' \
'' \
'cd "${PROJ_DIR}"' \
'' \
'# Find compile_commands.json (in build/ or project root)' \
'if [ -f "build/compile_commands.json" ]; then' \
'  CC_JSON="build/compile_commands.json"' \
'elif [ -f "compile_commands.json" ]; then' \
'  CC_JSON="compile_commands.json"' \
'else' \
'  echo "compile_commands.json not found in ${PROJ_DIR} (nor in build/)." >&2' \
'  echo "Run CMake first, e.g.:" >&2' \
'  echo "  cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON" >&2' \
'  exit 1' \
'fi' \
'' \
'echo "Running cppcheck with MISRA addon on ${CC_JSON}..." >&2' \
'cppcheck \' \
'  --project="${CC_JSON}" \' \
'  --enable=style,warning,performance,portability \' \
'  --inconclusive \' \
'  --force \' \
'  --inline-suppr \' \
'  --addon=/opt/misra/misra.json \' \
'  --error-exitcode=1 \' \
'  --xml --xml-version=2 2> cppcheck_misra_results.xml' \
'' \
'echo "MISRA-like analysis completed." >&2' \
'echo "XML results in: ${PROJ_DIR}/cppcheck_misra_results.xml" >&2' \
> /usr/local/bin/run-misra-check.sh && \
    chmod +x /usr/local/bin/run-misra-check.sh

# Script to:
#  - find all dirs under /workspace/code with a CMakeLists.txt (e.g. UdsComm, VoltMon)
#  - run cmake + build
#  - optionally run ctest
#  - run MISRA/cppcheck for each project
RUN printf '%s\n' \
'#!/usr/bin/env bash' \
'set -euo pipefail' \
'' \
'ROOT_DIR="/workspace/code"' \
'' \
'echo "Searching for CMakeLists.txt under ${ROOT_DIR}..." >&2' \
'' \
'# Find all directories that contain a CMakeLists.txt' \
'mapfile -t PROJECT_DIRS < <(find "${ROOT_DIR}" -mindepth 2 -maxdepth 2 -type f -name "CMakeLists.txt" -printf "%h\n" | sort -u)' \
'' \
'if [ ${#PROJECT_DIRS[@]} -eq 0 ]; then' \
'  echo "No CMakeLists.txt found under ${ROOT_DIR}" >&2' \
'  exit 1' \
'fi' \
'' \
'for PROJ in "${PROJECT_DIRS[@]}"; do' \
'  echo "==============================="' \
'  echo " Project: ${PROJ}"' \
'  echo "==============================="' \
'' \
'  cd "${PROJ}"' \
'' \
'  # Clean and create build directory' \
'  rm -rf build' \
'  mkdir -p build' \
'  cd build' \
'' \
'  echo "Configuring CMake in ${PROJ}/build..." >&2' \
'  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..' \
'' \
'  echo "Building ${PROJ}..." >&2' \
'  cmake --build .' \
'' \
'  # If CTest config exists, run tests (don'\''t stop others if they fail)' \
'  if [ -f "CTestTestfile.cmake" ]; then' \
'    echo "Running CTest in ${PROJ}/build..." >&2' \
'    if ! ctest; then' \
'      echo "⚠️ CTest FAILED in ${PROJ} (continuing with other projects)" >&2' \
'    fi' \
'  fi' \
'' \
'  # Back to project root for MISRA' \
'  cd "${PROJ}"' \
'' \
'  echo "Running MISRA/cppcheck for ${PROJ}..." >&2' \
'  if ! run-misra-check.sh "${PROJ}"; then' \
'    echo "⚠️ MISRA/cppcheck FAILED for ${PROJ} (continuing with other projects)" >&2' \
'  fi' \
'' \
'  echo "Done with ${PROJ}" >&2' \
'  echo' \
'done' \
'' \
'echo "All CMake projects under ${ROOT_DIR} have been processed." >&2' \
> /usr/local/bin/build-and-check-all.sh && \
    chmod +x /usr/local/bin/build-and-check-all.sh

# Default command: interactive shell
# If you prefer automatic build+check, change to:
# CMD ["build-and-check-all.sh"]

CMD ["build-and-check-all.sh"]
