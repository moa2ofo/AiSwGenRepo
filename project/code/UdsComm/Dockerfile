FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Tool di build + cmake + cppcheck + dipendenze per cppcheck-htmlreport
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

# Directory di lavoro dove monterai il progetto (es. UdsComm)
WORKDIR /workspace

# Directory con config MISRA per cppcheck (NON viene sovrascritta dal volume)
RUN mkdir -p /opt/misra

# File misra.json che dice a cppcheck di usare l'addon MISRA
# e i testi delle regole nel file /workspace/.misra
RUN printf '{\n  "script": "misra.py",\n  "args": [\n    "--rule-texts=/workspace/.misra"\n  ]\n}\n' > /opt/misra/misra.json

# Script comodo per lanciare il check MISRA-like su tutto il progetto
RUN printf '#!/usr/bin/env bash\n'\
'set -e\n\n'\
'cd /workspace\n\n'\
'if [ ! -f "build/compile_commands.json" ]; then\n'\
'  echo "compile_commands.json non trovato. Esegui prima:" >&2\n'\
'  echo "  cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON" >&2\n'\
'  exit 1\n'\
'fi\n\n'\
'echo "Eseguo cppcheck con addon MISRA (MISRA-like)..." >&2\n'\
'cppcheck \\\n'\
'  --project=build/compile_commands.json \\\n'\
'  --enable=style,warning,performance,portability \\\n'\
'  --inconclusive \\\n'\
'  --force \\\n'\
'  --inline-suppr \\\n'\
'  --addon=/opt/misra/misra.json \\\n'\
'  --error-exitcode=1 \\\n'\
'  --xml --xml-version=2 2> cppcheck_misra_results.xml\n\n'\
'echo "Analisi MISRA-like completata."'\
'echo "Risultati XML in: /workspace/cppcheck_misra_results.xml" >&2\n' \
> /usr/local/bin/run-misra-check.sh && \
    chmod +x /usr/local/bin/run-misra-check.sh

CMD ["/bin/bash"]

 