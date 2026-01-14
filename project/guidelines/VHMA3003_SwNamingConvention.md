# VHMA 3003 — SW Naming Convention (3rd issue, 08/2023)

# Content

## BSW — C language

---

### Files

Full file name:

`<modulename><_optional>`

Applies to both `.h` and `.c` (extension excluded).

#### Description of `<modulename>`

Identifies the module. All files in same module share the same `<modulename>`. It is composed by words/keywords in **PascalCasing**.

If a word appears in Table 5, its keyword must be used.

#### Description of `<_optional>`

Optional role descriptor. If used:

- single word/keyword
- first letter capital, remainder lower case
- keyword substitution rules as above

---

### Variables

Full variable name:

`<scope_><variablename><_datatype>`

#### Description of `<scope_>`

Indicates accessibility region, depending on:

- static storage class usage
- declaration region

Five valid categories are listed in Table 6 with corresponding prefixes.

#### Description of `<variablename>`

Meaning‑specific name; duplicates allowed if other parts differ.

Casing depends on storage class (Table 6):

- **PascalCasing** → static variables
- **camelCasing** → non‑static variables

Keywords substitution follows Table 2 & Table 5.

#### Description of `<_datatype>`

Indicates data type + dynamic behavior.

Structure:

`_<pointerqualifier><pointer><typequalifier><type>`

Exception: void / pointer‑to‑void → N/A.

- `<pointerqualifier>`: `v` volatile pointer, `c` const pointer, `r` restrict pointer, else N/A
- `<pointer>`: `p` if pointer, else N/A
- `<typequalifier>`: `v` volatile, `c` const, else N/A
- `<type>`:
  - `b`, `u8`, `u16`, `u32`, `s8`, `s16`, `s32`
  - N/A for user‑defined types

---

### Functions

Full function name:

`<scope_><functionname><_returntype>`

#### Description of `<scope_>`

Indicates scope + defining file.

Two categories (Table 7):

- Global (extern or hfile‑static)
- Local (cfile‑static)

Global prefix is `<modulename>_`.

#### Description of `<functionname>`

Meaning‑specific, PascalCasing.

Keyword substitution as usual.

#### Description of `<_returntype>`

Structure:

`_<pointer><type>`

Exception: void / pointer‑to‑void → N/A.

- `<pointer>`: `p` if pointer return, else N/A
- `<type>`: same set as variables

---

### Constructs

Full construct name:

`<scope_><constructname>_<constructtype>`

#### Description of `<scope_>`

Two categories (Table 8):

- Global (hfile‑declared) → `<modulename>_`
- Local (cfile‑declared) → N/A

#### Description of `<constructname>`

camelCasing, with usual keyword substitution.

#### Description of `<constructtype>`

Entity kind (Table 9).

---

### Construct fields

Full field name:

`<fieldname><_datatype>`

#### Description of `<fieldname>`

camelCasing, with usual keyword substitution.

#### Description of `<_datatype>`

Same structure and rules as variables:

`_<pointerqualifier><pointer><typequalifier><type>`

---

### Defines and enumerative cases

Full name:

`<scope_><macroname>`

Exception: function‑like macros may follow this or function naming rules.

#### Description of `<scope_>`

Two categories (Table 10):

- Global (hfile‑defined) → `<MODULENAME>_` (capital letters)
- Local (cfile‑defined) → N/A

#### Description of `<macroname>`

SNAKE_CASING, with usual keyword substitution.

---

### Tables (BSW)

#### Table 5 — Keywords for C language

| Keyword | Full word |
|---|---|
| Alv | Alive |
| Appl | Application |
| Buf | Buffer |
| Cfg | Configure, Configuration |
| Clr | Clear |
| Diag | Diagnostic |
| Err | Error |
| Flg | Flag |
| Get | Get |
| If | Interface |
| Man | Manager |
| Mdl | Module |
| Mon | Monitor, Monitoring |
| Rd | Read |
| Run | Run |
| Rx | Receive, Reception |
| Stop | Stop |
| Tst | Test |
| Tx | Transmit, Transmission |
| Sch | Schedule |
| Set | Set |
| Warn | Warning |
| Word | Word |
| Wr | Write |

#### Table 6 — Variable scope categories

| Variable scope category | `<scope_>` | `<variablename>` casing |
|---|---|---|
| Global (extern) | `g_` | camelCasing |
| Global static (file‑static) | N/A | PascalCasing |
| Local | `l_` | camelCasing |
| Local static (function‑static) | `l_` | PascalCasing |
| Function argument | N/A | camelCasing |

#### Table 7 — Function scope categories

| Function scope category | `<scope_>` | `<functionname>` casing |
|---|---|---|
| Global (extern or hfile‑static) | `<modulename>_` | PascalCasing |
| Local (cfile‑static) | N/A | PascalCasing |

#### Table 8 — Construct scope categories

| Construct scope category | `<scope_>` | `<constructname>` casing |
|---|---|---|
| Global (hfile‑declared) | `<modulename>_` | camelCasing |
| Local (cfile‑declared) | N/A | camelCasing |

#### Table 9 — Construct entities

| Construct entity | `<constructtype>` |
|---|---|
| Type definition (typedef) | `t` |
| Struct (struct) | `s` |
| Union (union) | `u` |
| Enumerative (enum) | `e` |

#### Table 10 — Macro scope categories

| Macro scope category | `<scope_>` | `<macroname>` casing |
|---|---|---|
| Global (hfile‑defined) | `<MODULENAME>_` | SNAKE_CASING |
| Local (cfile‑defined) | N/A | SNAKE_CASING |

---
