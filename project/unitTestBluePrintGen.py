#!/usr/bin/env python3
"""
PATCHED per latest rule change:

- Copy headers (flat) from:
    code/<ModuleName>/pltf/**
    code/<ModuleName>/cfg/**
  into:
    code/<ModuleName>/unitTests/TEST_<namefunction>/src
  (NO subfolders)

- Headers cleanup (NEW):
  In TEST_<namefunction>/src/, from every copied .h file EXCEPT <namefunction>.h,
  REMOVE the function *declaration/prototype* of <namefunction> (if present).

- Create:
    <namefunction>.h  -> contains the (cleaned) declaration/prototype
    <namefunction>.c  -> includes <namefunction>.h + includes ALL copied headers + definition

- Remove "static", "static inline", "static_inline" (and "inline") from generated decl/def.

Usage:
  python3 gen_ut_function_files.py
  python3 gen_ut_function_files.py /path/to/repo
"""

import re
import sys
import shutil
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, List, Tuple, Dict

C_EXTS = {".c", ".h"}
FORTRAN_EXTS = {".f", ".F", ".f90", ".F90", ".for", ".ftn"}

# Remove these keywords in generated decl/def
STATIC_WORDS_RE = re.compile(r"\bstatic_inline\b|\bstatic\b|\binline\b", re.IGNORECASE)


@dataclass
class Extracted:
    lang: str
    source_path: Path
    definition_text: str
    prototype: Optional[str]


def delete_all_inside(dir_path: Path) -> None:
    dir_path.mkdir(parents=True, exist_ok=True)
    for p in dir_path.iterdir():
        if p.is_file():
            p.unlink()
        else:
            shutil.rmtree(p)


def read_text_file(p: Path) -> Optional[str]:
    try:
        return p.read_text(encoding="utf-8", errors="ignore")
    except Exception:
        return None


def write_text_file(p: Path, txt: str) -> None:
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(txt, encoding="utf-8")


def normalize_ws(s: str) -> str:
    return re.sub(r"\s+", " ", s).strip()


def strip_static_words(s: str) -> str:
    out = STATIC_WORDS_RE.sub("", s)
    out = re.sub(r"\s+", " ", out).strip()
    out = out.replace(" (", "(")
    return out


def brace_match_c(text: str, open_brace_index: int) -> Optional[int]:
    """Return index of matching '}' for '{' at open_brace_index. Skips strings/comments."""
    depth = 0
    i = open_brace_index
    n = len(text)

    in_str = False
    in_chr = False
    escape = False
    in_line_comment = False
    in_block_comment = False

    while i < n:
        c = text[i]

        if in_line_comment:
            if c == "\n":
                in_line_comment = False
            i += 1
            continue

        if in_block_comment:
            if c == "*" and i + 1 < n and text[i + 1] == "/":
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue

        if in_str:
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == '"':
                in_str = False
            i += 1
            continue

        if in_chr:
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == "'":
                in_chr = False
            i += 1
            continue

        if c == "/" and i + 1 < n:
            nxt = text[i + 1]
            if nxt == "/":
                in_line_comment = True
                i += 2
                continue
            if nxt == "*":
                in_block_comment = True
                i += 2
                continue

        if c == '"':
            in_str = True
            i += 1
            continue
        if c == "'":
            in_chr = True
            i += 1
            continue

        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i

        i += 1

    return None


def _strip_c_comments_and_strings_keep_len(s: str) -> str:
    """
    Replace comments/strings/chars with spaces (same length) to preserve indices.
    Helps scanning without being confused by ' ; { } ' inside strings/comments.
    """
    out = list(s)
    n = len(s)
    i = 0
    in_str = False
    in_chr = False
    in_line_comment = False
    in_block_comment = False
    escape = False

    while i < n:
        c = s[i]

        if in_line_comment:
            out[i] = " "
            if c == "\n":
                in_line_comment = False
            i += 1
            continue

        if in_block_comment:
            out[i] = " "
            if c == "*" and i + 1 < n and s[i + 1] == "/":
                out[i + 1] = " "
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue

        if in_str:
            out[i] = " "
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == '"':
                in_str = False
            i += 1
            continue

        if in_chr:
            out[i] = " "
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == "'":
                in_chr = False
            i += 1
            continue

        if c == "/" and i + 1 < n:
            nxt = s[i + 1]
            if nxt == "/":
                out[i] = out[i + 1] = " "
                in_line_comment = True
                i += 2
                continue
            if nxt == "*":
                out[i] = out[i + 1] = " "
                in_block_comment = True
                i += 2
                continue

        if c == '"':
            out[i] = " "
            in_str = True
            i += 1
            continue

        if c == "'":
            out[i] = " "
            in_chr = True
            i += 1
            continue

        i += 1

    return "".join(out)


def extract_file_scope_variable_definitions_c(full_c_text: str) -> List[str]:
    """
    Extract *file-scope* variable definition statements from a .c file.

    Heuristics (best-effort):
      - Only considers statements ending with ';' at brace depth 0.
      - Skips preprocessor, typedef, extern declarations and likely function prototypes.
      - Keeps globals/static/const vars and initialized objects (arrays/structs).
    """
    txt = full_c_text
    masked = _strip_c_comments_and_strings_keep_len(txt)

    vars_out: List[str] = []

    brace_depth = 0
    paren_depth = 0
    bracket_depth = 0

    stmt_start = 0
    i = 0
    n = len(masked)

    def looks_like_function_prototype(stmt: str) -> bool:
        s = stmt.strip()
        if not s.endswith(';'):
            return True
        if '{' in s:
            return True
        # likely prototype: has ( ) and no '=' and not function-pointer var
        if '(' in s and ')' in s and '=' not in s and '(*' not in s:
            return True
        return False

    while i < n:
        c = masked[i]

        if c == '(':
            paren_depth += 1
        elif c == ')':
            paren_depth = max(0, paren_depth - 1)
        elif c == '[':
            bracket_depth += 1
        elif c == ']':
            bracket_depth = max(0, bracket_depth - 1)

        if c == '{' and paren_depth == 0 and bracket_depth == 0:
            brace_depth += 1
        elif c == '}' and paren_depth == 0 and bracket_depth == 0:
            brace_depth = max(0, brace_depth - 1)

        if c == ';' and brace_depth == 0 and paren_depth == 0 and bracket_depth == 0:
            stmt = txt[stmt_start:i + 1]
            stmt_start = i + 1

            s = stmt.strip()
            if not s:
                i += 1
                continue

            if s.startswith('#'):
                i += 1
                continue
            if re.match(r'^\s*typedef\b', s):
                i += 1
                continue
            if re.match(r'^\s*extern\b', s):
                i += 1
                continue
            if looks_like_function_prototype(s):
                i += 1
                continue

            vars_out.append(s.rstrip() + "\n")

        i += 1

    # de-dup preserving order
    seen = set()
    uniq: List[str] = []
    for v in vars_out:
        key = normalize_ws(v)
        if key not in seen:
            seen.add(key)
            uniq.append(v)

    return uniq


def find_c_definition_spans(text: str, func_name: str) -> List[Tuple[int, int, str]]:
    """
    Find all C function definition blocks for func_name.
    Returns (start_idx, end_idx_inclusive, prototype_str).
    """
    name = re.escape(func_name)
    pattern = (
        r"^[ \t]*(?!#)"
        r"(?:[\w\*\(\)\[\]\s]+?\s+)?"
        r"\b" + name + r"\b"
        r"[ \t]*\([^;]*?\)"
        r"[ \t\r\n]*\{"
    )
    pat = re.compile(pattern, re.MULTILINE)

    spans: List[Tuple[int, int, str]] = []
    for m in pat.finditer(text):
        open_brace = text.find("{", m.end() - 1)
        if open_brace < 0:
            continue
        close_brace = brace_match_c(text, open_brace)
        if close_brace is None:
            continue

        signature = text[m.start():open_brace].strip()
        signature = normalize_ws(signature)
        rp = signature.rfind(")")
        if rp != -1:
            signature = signature[: rp + 1]
        signature = strip_static_words(signature)
        prototype = signature + ";"

        spans.append((m.start(), close_brace, prototype))

    return spans


def extract_best_c_definition(text: str, func_name: str) -> Optional[Tuple[str, str]]:
    spans = find_c_definition_spans(text, func_name)
    if not spans:
        return None
    start, end, proto = spans[0]
    definition = text[start:end + 1].rstrip() + "\n"

    open_brace = definition.find("{")
    if open_brace != -1:
        sig = definition[:open_brace]
        rest = definition[open_brace:]
        sig2 = strip_static_words(sig)
        definition = sig2.rstrip() + "\n" + rest.lstrip("\n")

    return definition, proto


def extract_fortran_routine(text: str, func_name: str) -> Optional[str]:
    start_pat = re.compile(
        rf"^[ \t]*(?:recursive[ \t]+)?(?:pure[ \t]+)?(?:elemental[ \t]+)?(function|subroutine)[ \t]+{re.escape(func_name)}\b.*$",
        re.IGNORECASE | re.MULTILINE,
    )
    m = start_pat.search(text)
    if not m:
        return None

    kind = m.group(1).lower()
    start = m.start()

    end_pat = re.compile(rf"^[ \t]*end[ \t]+{kind}\b.*$", re.IGNORECASE | re.MULTILINE)
    m_end = end_pat.search(text, pos=m.end())
    if not m_end:
        end_generic = re.compile(r"^[ \t]*end\b.*$", re.IGNORECASE | re.MULTILINE)
        m_end = end_generic.search(text, pos=m.end())
        if not m_end:
            return None

    end_line_end = text.find("\n", m_end.end())
    if end_line_end == -1:
        end_line_end = len(text)

    return text[start:end_line_end].rstrip() + "\n"


def search_definition_in_module(module_root: Path, func_name: str) -> Optional[Extracted]:
    roots = [module_root / "pltf", module_root / "cfg"]
    candidates: List[Tuple[int, Extracted]] = []

    for root in roots:
        if not root.exists():
            continue
        for p in root.rglob("*"):
            if not p.is_file():
                continue
            if p.suffix not in C_EXTS and p.suffix not in FORTRAN_EXTS:
                continue

            txt = read_text_file(p)
            if txt is None:
                continue

            if p.suffix in C_EXTS:
                res = extract_best_c_definition(txt, func_name)
                if res:
                    definition, proto = res
                    score = 100 + (60 if p.suffix == ".c" else 0)
                    candidates.append((score, Extracted("c", p, definition, proto)))
            else:
                block = extract_fortran_routine(txt, func_name)
                if block:
                    candidates.append((10, Extracted("fortran", p, block, None)))

    if not candidates:
        return None

    candidates.sort(key=lambda x: x[0], reverse=True)
    return candidates[0][1]


def make_guard(name: str) -> str:
    return re.sub(r"[^A-Za-z0-9_]", "_", name).upper() + "_"


def unique_flat_name(dst_dir: Path, base_name: str, used: Dict[str, int]) -> str:
    """Avoid collisions when flattening copies."""
    if base_name not in used:
        used[base_name] = 1
        if not (dst_dir / base_name).exists():
            return base_name

    used[base_name] += 1
    stem = Path(base_name).stem
    suf = Path(base_name).suffix
    candidate = f"{stem}__{used[base_name]}{suf}"
    while (dst_dir / candidate).exists():
        used[base_name] += 1
        candidate = f"{stem}__{used[base_name]}{suf}"
    return candidate


def copy_headers_flat(module_root: Path, dst_src_dir: Path) -> List[Path]:
    """Copy all .h from module_root/pltf and module_root/cfg into dst_src_dir (flat)."""
    copied: List[Path] = []
    used_names: Dict[str, int] = {}

    for sub in ("pltf", "cfg"):
        root = module_root / sub
        if not root.exists():
            continue
        for h in root.rglob("*.h"):
            txt = read_text_file(h)
            if txt is None:
                continue

            dst_name = unique_flat_name(dst_src_dir, h.name, used_names)
            dst = dst_src_dir / dst_name
            write_text_file(dst, txt)
            copied.append(dst)

    return sorted(copied)


def remove_func_declaration_from_header_text(header_text: str, func_name: str) -> str:
    """
    Remove C *declarations/prototypes* of func_name from a header.
    We remove statements like:
      ret func(args);
      extern ret func(args);
      static inline ret func(args);
    We do NOT remove definitions:
      ret func(args) { ... }

    Strategy:
      - find occurrences of func_name followed by '('
      - look backward to start of statement (after previous ';' or beginning of file)
      - look forward to next ';'
      - if there's a '{' before that ';' => it's a definition, skip
      - otherwise delete that whole statement (best-effort)
    """
    name = func_name
    text = header_text
    i = 0
    n = len(text)

    def is_ident_char(ch: str) -> bool:
        return ch.isalnum() or ch == "_"

    def find_next_semicolon(s: str, start: int) -> int:
        # simple scan to ';' ignoring strings/comments (basic)
        in_str = in_chr = False
        escape = False
        in_line_comment = in_block_comment = False
        j = start
        while j < len(s):
            c = s[j]

            if in_line_comment:
                if c == "\n":
                    in_line_comment = False
                j += 1
                continue

            if in_block_comment:
                if c == "*" and j + 1 < len(s) and s[j + 1] == "/":
                    in_block_comment = False
                    j += 2
                    continue
                j += 1
                continue

            if in_str:
                if escape:
                    escape = False
                elif c == "\\":
                    escape = True
                elif c == '"':
                    in_str = False
                j += 1
                continue

            if in_chr:
                if escape:
                    escape = False
                elif c == "\\":
                    escape = True
                elif c == "'":
                    in_chr = False
                j += 1
                continue

            if c == "/" and j + 1 < len(s):
                nxt = s[j + 1]
                if nxt == "/":
                    in_line_comment = True
                    j += 2
                    continue
                if nxt == "*":
                    in_block_comment = True
                    j += 2
                    continue

            if c == '"':
                in_str = True
                j += 1
                continue
            if c == "'":
                in_chr = True
                j += 1
                continue

            if c == ";":
                return j

            j += 1

        return -1

    while i < n:
        idx = text.find(name, i)
        if idx == -1:
            break

        # word boundary check
        before_ok = (idx == 0) or (not is_ident_char(text[idx - 1]))
        after_idx = idx + len(name)
        after_ok = (after_idx >= len(text)) or (not is_ident_char(text[after_idx]))
        if not (before_ok and after_ok):
            i = idx + len(name)
            continue

        # must be followed by optional whitespace then '('
        j = after_idx
        while j < len(text) and text[j].isspace():
            j += 1
        if j >= len(text) or text[j] != "(":
            i = idx + len(name)
            continue

        # find the end of the statement (next ';')
        semi = find_next_semicolon(text, j)
        if semi == -1:
            # no semicolon => not a prototype, move on
            i = idx + len(name)
            continue

        # if there is a '{' before ';', it's likely a definition -> skip
        brace_pos = text.find("{", j, semi)
        if brace_pos != -1:
            i = idx + len(name)
            continue

        # find statement start: after previous ';' or beginning
        stmt_start = text.rfind(";", 0, idx)
        stmt_start = 0 if stmt_start == -1 else stmt_start + 1

        # delete statement [stmt_start .. semi+1)
        text = text[:stmt_start] + text[semi + 1:]
        n = len(text)
        i = stmt_start  # continue scanning from where we deleted

    return text



def strip_func_decl_from_all_headers_in_src(dst_src_dir: Path, func_name: str) -> None:
    """
    Remove the function *declaration* from all .h files in dst_src_dir,
    EXCEPT the file <namefunction>.h (generated later and must never be modified).
    """
    exclude = f"{func_name}.h"
    for hp in dst_src_dir.glob("*.h"):
        if hp.name == exclude:
            continue
        txt = read_text_file(hp)
        if txt is None:
            continue
        new_txt = remove_func_declaration_from_header_text(txt, func_name)
        if new_txt != txt:
            write_text_file(hp, new_txt)


def convert_static_vars_to_extern_in_header(header_text: str) -> str:
    """
    In a header file:
    - Replace 'static <type> <var>;' with 'extern <type> <var>;' (variables only)
    - Add 'extern' to bare variable declarations (type var;)
    - Remove 'static' keyword from function declarations (keep them as prototypes without extern)
    """
    text = header_text
    
    # First pass: Replace 'static ' with 'extern ' for variables, remove for functions
    lines = text.split('\n')
    pass1_lines = []
    
    for line in lines:
        stripped = line.strip()
        
        # If line has 'static', check if it's a variable or function
        if 'static ' in line and ';' in line:
            # Check if it's a function (has parentheses with matching closing before semicolon)
            paren_start = stripped.find('(')
            paren_end = stripped.find(')')
            
            if paren_start >= 0 and paren_end > paren_start and paren_end < len(stripped) - 1:
                # Has parentheses before the end - likely a function
                after_paren = stripped[paren_end+1:].strip()
                if after_paren == ';':
                    # It's a function prototype - remove static but don't add extern
                    line = STATIC_WORDS_RE.sub('', line)
                    line = re.sub(r'\s+', ' ', line).rstrip()
                else:
                    # It's a variable with pointer - convert static to extern
                    line = line.replace('static ', 'extern ', 1)
            else:
                # No parentheses or after semicolon - it's a variable
                line = line.replace('static ', 'extern ', 1)
        elif 'static' in line and ';' not in line:
            # Multi-line declaration starting with static - just remove static
            line = STATIC_WORDS_RE.sub('', line)
            line = re.sub(r'\s+', ' ', line).rstrip()
        
        pass1_lines.append(line)
    
    text = '\n'.join(pass1_lines)
    
    # Second pass: Add 'extern' to bare variable declarations (not functions)
    type_keywords = {'int', 'char', 'float', 'double', 'void', 'uint8_t', 'uint16_t', 'uint32_t', 
                    'uint64_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t', 'bool', 'size_t',
                    'struct', 'union', 'enum'}
    
    lines = text.split('\n')
    result_lines = []
    
    for line in lines:
        stripped = line.strip()
        
        # Skip empty lines, comments, preprocessor directives
        if not stripped or stripped.startswith('/*') or stripped.startswith('*') or \
           stripped.startswith('//') or stripped.startswith('#') or stripped == '}':
            result_lines.append(line)
            continue
        
        # Skip lines that already have extern or contain { or =
        if 'extern' in line or '{' in line or '=' in line:
            result_lines.append(line)
            continue
        
        # Check if line ends with semicolon
        if stripped.endswith(';'):
            # Get first word
            words = stripped.split()
            if words:
                first_word = words[0]
                
                # Check if it's a known C type (not 'void' which is usually for functions)
                if first_word in type_keywords and first_word != 'void':
                    # Check if it's not a function (functions have ( and ))
                    has_paren = '(' in stripped and ')' in stripped and stripped.find('(') < stripped.find(';')
                    
                    if not has_paren:
                        # It's a variable declaration, ensure it starts with 'extern '
                        if not stripped.startswith('extern'):
                            indent = len(line) - len(line.lstrip())
                            line = ' ' * indent + 'extern ' + stripped
                    else:
                        # Has parentheses - it might be a function pointer variable
                        # Only add extern if it has * (function pointer)
                        if '*' in stripped and '(*' in stripped:
                            if not stripped.startswith('extern'):
                                indent = len(line) - len(line.lstrip())
                                line = ' ' * indent + 'extern ' + stripped
        
        result_lines.append(line)
    
    return '\n'.join(result_lines)


def convert_static_to_extern_in_all_headers(dst_src_dir: Path) -> None:
    """
    Process all .h files in dst_src_dir to convert static variables to extern.
    """
    for hp in dst_src_dir.glob("*.h"):
        txt = read_text_file(hp)
        if txt is None:
            continue
        new_txt = convert_static_vars_to_extern_in_header(txt)
        if new_txt != txt:
            write_text_file(hp, new_txt)



def _find_next_semicolon_c_like(s: str, start: int) -> int:
    """Scan to the next ';' while skipping strings/comments (basic C-like)."""
    in_str = in_chr = False
    escape = False
    in_line_comment = in_block_comment = False
    j = start
    while j < len(s):
        c = s[j]

        if in_line_comment:
            if c == "\n":
                in_line_comment = False
            j += 1
            continue

        if in_block_comment:
            if c == "*" and j + 1 < len(s) and s[j + 1] == "/":
                in_block_comment = False
                j += 2
                continue
            j += 1
            continue

        if in_str:
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == '"':
                in_str = False
            j += 1
            continue

        if in_chr:
            if escape:
                escape = False
            elif c == "\\":
                escape = True
            elif c == "'":
                in_chr = False
            j += 1
            continue

        if c == "/" and j + 1 < len(s):
            nxt = s[j + 1]
            if nxt == "/":
                in_line_comment = True
                j += 2
                continue
            if nxt == "*":
                in_block_comment = True
                j += 2
                continue

        if c == '"':
            in_str = True
            j += 1
            continue
        if c == "'":
            in_chr = True
            j += 1
            continue

        if c == ";":
            return j

        j += 1

    return -1


def find_declaration_and_doxygen_in_module(module_root: Path, func_name: str) -> Tuple[Optional[str], Optional[str], Optional[Path]]:
    """
    Find the declaration/prototype of func_name in module headers (pltf/cfg) and
    also capture the doxygen comment immediately above it.

    Returns (doxygen_comment_or_None, prototype_or_None, header_path_or_None)
    Prototype is cleaned from static/static inline/static_inline.
    """

    def is_ident_char(ch: str) -> bool:
        return ch.isalnum() or ch == "_"

    def find_decl_span(txt: str, masked: str) -> Optional[Tuple[int, int]]:
        i = 0
        name = func_name
        while True:
            idx = txt.find(name, i)
            if idx == -1:
                return None

            # word boundaries
            before_ok = (idx == 0) or (not is_ident_char(txt[idx - 1]))
            after_idx = idx + len(name)
            after_ok = (after_idx >= len(txt)) or (not is_ident_char(txt[after_idx]))
            if not (before_ok and after_ok):
                i = idx + len(name)
                continue

            # Check if this occurrence is NOT in a comment/string (masked will have spaces there)
            if masked[idx] != name[0]:
                # This match is inside a comment or string
                i = idx + len(name)
                continue

            # Reject if this is inside a #define (look backward for # before any semicolon or newline)
            line_start = txt.rfind('\n', 0, idx)
            if line_start == -1:
                line_start = 0
            else:
                line_start += 1
            line_before_func = txt[line_start:idx].strip()
            if line_before_func.startswith('#define '):
                i = idx + len(name)
                continue

            # followed by '(' (after optional whitespace)
            j = after_idx
            while j < len(txt) and txt[j].isspace():
                j += 1
            if j >= len(txt) or txt[j] != "(":
                i = idx + len(name)
                continue

            semi = _find_next_semicolon_c_like(txt, j)
            if semi == -1:
                i = idx + len(name)
                continue

            # reject if '{' appears before ';' (definition)
            if txt.find("{", j, semi) != -1:
                i = idx + len(name)
                continue

            # Find statement start robustly: ignore ';' inside comments/strings
            # (masked has comments/strings replaced by spaces, indices preserved)
            stmt_start = masked.rfind(";", 0, idx)
            stmt_start = 0 if stmt_start == -1 else stmt_start + 1
            # Skip leading whitespace, comments, and preprocessor directives between the previous statement and the decl
            while stmt_start < len(txt):
                # whitespace
                while stmt_start < len(txt) and txt[stmt_start] in " \t\r\n":
                    stmt_start += 1
                if stmt_start >= len(txt):
                    break

                # preprocessor line (skip entire line)
                if txt[stmt_start] == '#':
                    nl = txt.find("\n", stmt_start)
                    stmt_start = len(txt) if nl == -1 else nl + 1
                    continue

                # line comment
                if txt.startswith("//", stmt_start):
                    nl = txt.find("\n", stmt_start)
                    stmt_start = len(txt) if nl == -1 else nl + 1
                    continue

                # block comment
                if txt.startswith("/*", stmt_start):
                    endc = txt.find("*/", stmt_start + 2)
                    if endc == -1:
                        break
                    stmt_start = endc + 2
                    continue

                break

            return (stmt_start, semi + 1)

    def extract_doxygen_comment_above(text: str, decl_start: int) -> Optional[str]:
        """Extract the doxygen *block* comment immediately above a declaration.

        Supported starters:
          - /** ... */
          - /*! ... */

        Rules:
          - The comment must end with '*/'.
          - Between the end of the comment and decl_start there must be only whitespace.
          - Returns the full block preserving formatting + a trailing newline.

        Notes:
          - This is intentionally "best effort" and avoids removing blocks that contain nested
            '*/' sequences (rare, but can appear inside examples).
        """
        # Move backward over whitespace to land on the character right before decl_start
        k = decl_start - 1
        while k >= 0 and text[k].isspace():
            k -= 1
        if k < 1:
            return None

        # Must end with '*/'
        if not (text[k] == '/' and text[k - 1] == '*'):
            return None
        end = k + 1  # include final '/'

        # Ensure only whitespace between comment end and decl_start
        if text[end:decl_start].strip() != "":
            return None

        # Find the nearest preceding doxygen block start (/** or /*!)
        start_candidates = []
        s1 = text.rfind('/**', 0, end)
        if s1 != -1:
            start_candidates.append(s1)
        s2 = text.rfind('/*!', 0, end)
        if s2 != -1:
            start_candidates.append(s2)
        if not start_candidates:
            return None

        start = max(start_candidates)

        # Sanity: ensure there's no earlier close '*/' between start and the final close (end)
        # (i.e., avoid matching an opening that actually closes before this end).
        inner_close = text.find('*/', start + 3, end - 1)
        if inner_close != -1:
            return None

        block = text[start:end]
        return block.rstrip() + "\n"

    for root in (module_root / "pltf", module_root / "cfg"):
        if not root.exists():
            continue
        for hp in root.rglob("*.h"):
            txt = read_text_file(hp)
            if txt is None:
                continue
            masked = _strip_c_comments_and_strings_keep_len(txt)
            span = find_decl_span(txt, masked)
            if not span:
                continue
            start, end = span
            decl = txt[start:end].strip()
            # normalize + strip static-ish
            decl = normalize_ws(decl)
            decl = strip_static_words(decl)
            if not decl.endswith(";"):
                decl += ";"
            doxy = extract_doxygen_comment_above(txt, start)
            return (doxy, decl, hp)

    return (None, None, None)


def extract_extern_variables_from_headers(src_dir: Path) -> List[str]:
    """
    Extract all 'extern <type> <var>;' declarations from all .h files in src_dir.
    Returns a list of extern variable declarations (strings).
    """
    extern_vars: List[str] = []
    seen = set()
    
    for hp in sorted(src_dir.glob("*.h")):
        txt = read_text_file(hp)
        if txt is None:
            continue
        
        # Find all lines with 'extern' that end with ';'
        for line in txt.split('\n'):
            stripped = line.strip()
            
            # Check if it's an extern variable declaration (not a function)
            if stripped.startswith('extern ') and stripped.endswith(';'):
                # Make sure it's not a function (no '(' and ')' together before ';')
                has_func_parens = '(' in stripped and ')' in stripped and stripped.find('(') < stripped.rfind(';')
                
                if not has_func_parens:
                    # It's a variable declaration
                    key = normalize_ws(stripped)
                    if key not in seen:
                        seen.add(key)
                        extern_vars.append(stripped)
    
    return extern_vars


def write_wrapper_files(dst_src_dir: Path, func_name: str, extracted: Extracted, decl_comment: Optional[str], decl_proto: Optional[str]) -> None:
    """
    Create <func_name>.h and <func_name>.c in dst_src_dir.

    <func_name>.h contains the declaration/prototype (NO Doxygen comment).

    <func_name>.c includes <func_name>.h and ALL other .h files in dst_src_dir.

    NEW:
      - If the function definition was extracted from a .c file, also extract file-scope variable
        definitions from that same .c file and inject them into <func_name>.c (before the function).
    """
    dst_src_dir.mkdir(parents=True, exist_ok=True)

    h_path = dst_src_dir / f"{func_name}.h"
    c_path = dst_src_dir / f"{func_name}.c"

    guard = make_guard(f"{func_name}_h")

    # Header prototype: prefer declaration from header, fallback to extracted prototype.
    if extracted.lang == "c":
        proto = decl_proto or extracted.prototype or f"/* TODO: prototype for {func_name} */"
        proto = strip_static_words(proto)
        h_txt = (
            f"#ifndef {guard}\n"
            f"#define {guard}\n\n"
            f"{proto}\n\n"
            f"#endif /* {guard} */\n"
        )
    else:
        h_txt = (
            f"#ifndef {guard}\n"
            f"#define {guard}\n\n"
            f"/* TODO: {func_name} extracted from Fortran; create a C-compatible declaration if needed. */\n\n"
            f"#endif /* {guard} */\n"
        )

    write_text_file(h_path, h_txt)

    # Includes: wrapper header + ALL other headers in src/
    include_lines: List[str] = [f'#include "{func_name}.h"']
    for hp in sorted(dst_src_dir.glob("*.h")):
        if hp.name == f"{func_name}.h":
            continue
        include_lines.append(f'#include "{hp.name}"')

    # de-dup
    seen = set()
    inc_unique: List[str] = []
    for line in include_lines:
        if line not in seen:
            seen.add(line)
            inc_unique.append(line)
    includes_block = "\n".join(inc_unique)

    # NEW: extract file-scope variable definitions from the same .c file where the function was found
    var_block = ""
    if extracted.lang == "c" and extracted.source_path.suffix.lower() == ".c":
        src_txt = read_text_file(extracted.source_path) or ""
        var_defs = extract_file_scope_variable_definitions_c(src_txt)
        if var_defs:
            # Convert extracted variable definitions: remove static, add extern for declarations
            converted_defs = []
            for var_def in var_defs:
                # If it's a definition with initialization, keep it as is (but remove static)
                # If it's just a declaration, convert to extern
                cleaned = STATIC_WORDS_RE.sub("", var_def).strip()
                # If it has '=', it's a definition with init value
                if '=' in cleaned:
                    converted_defs.append(cleaned + "\n")
                else:
                    # It's a declaration, convert to extern
                    if not cleaned.startswith('extern'):
                        cleaned = 'extern ' + cleaned
                    converted_defs.append(cleaned + "\n")
            
            var_block = (
                "/* ---- extracted file-scope variables from original source ---- */\n"
                + "".join(converted_defs)
                + "\n"
            )
    
    # NEW: extract extern variable declarations from all copied headers
    extern_vars_block = ""
    extern_vars = extract_extern_variables_from_headers(dst_src_dir)
    if extern_vars:
        extern_vars_block = (
            "/* ---- extern variables from module headers ---- */\n"
            + "\n".join(extern_vars)
            + "\n\n"
        )

    if extracted.lang == "c":
        defn = extracted.definition_text
        open_brace = defn.find("{")
        if open_brace != -1:
            sig = defn[:open_brace]
            rest = defn[open_brace:]
            sig2 = strip_static_words(sig)
            defn = sig2.rstrip() + "\n" + rest.lstrip("\n")

        c_txt = f"{includes_block}\n\n{extern_vars_block}{var_block}{defn}"
    else:
        c_txt = (
            f"{includes_block}\n\n"
            f"/* Extracted Fortran routine from: {extracted.source_path} */\n"
            f"#if 0\n{extracted.definition_text}#endif\n"
        )

    write_text_file(c_path, c_txt)
def main(argv: List[str]) -> int:
    repo_root = Path(argv[1]) if len(argv) > 1 else Path(".")
    code_root = repo_root / "code"

    if not code_root.exists():
        print(f"[ERROR] Not found: {code_root}")
        return 2

    total = ok = miss = 0

    for module_root in sorted([p for p in code_root.iterdir() if p.is_dir()]):
        unit_root = module_root / "unitTests"  # required
        if not unit_root.exists():
            continue

        test_dirs = sorted([p for p in unit_root.iterdir() if p.is_dir() and p.name.startswith("TEST_")])
        for test_dir in test_dirs:
            func_name = test_dir.name[len("TEST_"):]
            if not func_name:
                continue

            total += 1
            src_dir = test_dir / "src"

            # 1) delete all in src
            delete_all_inside(src_dir)

            # 2) copy all headers flat
            copy_headers_flat(module_root, src_dir)

            # 3) convert static variables to extern in all copied headers
            convert_static_to_extern_in_all_headers(src_dir)

            # 4) strip function *declaration* from all copied headers except <namefunction>.h
            # (it doesn't exist yet, but rule will still be enforced)
            strip_func_decl_from_all_headers_in_src(src_dir, func_name)

            # 4) find function definition in module pltf/cfg
            extracted = search_definition_in_module(module_root, func_name)
            if not extracted:
                miss += 1
                print(f"[MISS] {module_root.name}: {func_name} (definition not found)")
                continue
            # 5) write wrapper c/h (generated <namefunction>.h is never modified)
            decl_comment, decl_proto, decl_path = find_declaration_and_doxygen_in_module(module_root, func_name)
            write_wrapper_files(src_dir, func_name, extracted, decl_comment, decl_proto)

            # Safety: enforce rule again AFTER generation (do not touch <namefunction>.h)
            convert_static_to_extern_in_all_headers(src_dir)
            strip_func_decl_from_all_headers_in_src(src_dir, func_name)

            ok += 1
            print(f"[OK] {module_root.name}: {func_name} <- {extracted.source_path}")

    print(f"\nDone. TEST folders: {total}, generated: {ok}, missing: {miss}")
    return 0 if miss == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
