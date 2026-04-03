# C++ Style Guide

This project follows an LLVM-inspired C++ style, tuned for emulator and
systems programming.

Consistency and clarity are prioritized over cleverness.

---

## 1. Git

### 1.1 Commit Header
```<type>(<scope>): <description>```
- `type` = feat/fix/chore/bug/test
- `scope` = subsystem like "cpu", "core", ... 

*try to match commit header with branch name*

example: 
- in branch : ```cpu/add-instr```
- commit : ```fix(cpu): ...```


Type examples:

```
feat     → new emulator behavior
fix      → bug fix
refactor → code restructure
perf     → performance improvement
test     → tests

ci       → CI/CD
build    → build system
docs     → documentation
tooling  → dev tools (hooks, scripts)
config   → config files

chore    → fallback (rare)
```

### 1.2 Issue Number
must use Github Issue # --> reference in header with at the end : `(#<number>)`
its okay to have multiple commits per issue number.
- commits are atomic (one implementation per commit; not feature)

### 1.3 Body (optional)
add optional body for nontrivial commits. If you can't understand the why? or the how? (or maybe even the what lol) with the commit header ... make a **body**

### 1.4 Final commit message examples:

```
fix(timer): reload tima on overflow edge (#17)
  
The previous implementation reloaded too early, which broke  
timing-sensitive ROM tests. Match hardware behavior by delaying  
the reload until the next machine cycle.  
```
```
feat(cpu): implement adc instruction (#22) <-- no body is ok
``` 
```
test(cpu): add opcode regression tests (#22)
```

---

## 2. Formatting

Formatting is enforced via `clang-format`.

- Base style: LLVM
- Indentation: **2 spaces**
- Tabs: **never**
- Brace style: K&R (opening brace on same line)
- Column limit: ~110 characters
- Access specifiers (`public`, `private`) are indented for readability,
  especially when multiple classes exist in one file.

### 2.1 Formatting commands

Format a single file:
```bash
clang-format -i path/to/file.cpp
````

Format multiple files:

```bash
clang-format -i src/**/*.cpp include/**/*.h
```

Format only changed lines:

```bash
git clang-format
```

---

## 3. Naming Conventions

### 3.1 Namespaces

* **PascalCase**

```cpp
namespace GameBoy
namespace Cartridge
namespace CPU
```

---

### 3.2 Types (classes, structs, enums)

* **PascalCase**

```cpp
class MBC1;
struct RomHeader;
enum class CartridgeType;
```

---

### 3.3 Functions

* **snake_case**

```cpp
validate_rom_file(...)
clamp_rom_bank_(...)
```

---

### 3.4 Variables

* **snake_case**

```cpp
rom_size_code
ram_enabled
```

---

### 3.5 Member Variables

* **snake_case with trailing underscore**

```cpp
rom_bank_low5_
ram_enabled_
```

---

### 3.6 Constants

* **ALL_CAPS with underscores**
* Prefer `constexpr` over macros

```cpp
constexpr uint16_t ROM_BEGIN = 0x0100;
constexpr uint16_t HEADER_CHECKSUM_OFFSET = 0x014D;
```

---

### 3.7 Enums

* Always use `enum class`
* Specify underlying type when relevant

```cpp
enum class MBCType : uint8_t {
  ROM_ONLY = 0x00,
  MBC1     = 0x01,
};
```

---

## C++ Language Rules

### Constructors

* Use `explicit` for all domain objects

```cpp
explicit MBC1(const std::vector<uint8_t>& rom,
              std::vector<uint8_t>& ram);
```

---

### Polymorphism

* Use `override` for **every** overridden virtual function
* Use `final` for leaf classes
* Base destructors must be virtual

---

### Copy / Move Semantics

* Hardware-like identity objects (CPU, MBC, MMU, PPU) must not be copied or moved

```cpp
MBC(const MBC&) = delete;
MBC& operator=(const MBC&) = delete;
MBC(MBC&&) = delete;
MBC& operator=(MBC&&) = delete;
```

---

### Ownership

* Use `std::unique_ptr` for ownership
* Avoid raw `new` / `delete`
* Use references or raw pointers **only** for non-owning access

---

## Comments

### Philosophy

* Code explains **what**
* Comments explain **why**

---

### Hardware Behavior

Hardware-specific logic **must be commented** and should reference
authoritative sources (e.g., Pan Docs).

```cpp
// Pan Docs §Cartridge Header:
// Bytes 0x0134–0x014C are used for header checksum computation.
```

---

### Address Ranges

Always document address ranges explicitly.

```cpp
// ROM bank select register (0x2000–0x3FFF).
```

---

### Algorithms

Use short block comments for non-trivial logic.

```cpp
// Header checksum algorithm:
// 1. Iterate bytes 0x0134–0x014C
// 2. Subtract each byte and 1 from accumulator
// 3. Result must equal byte at 0x014D
```

---

### File Headers

Each `.cpp` file should start with a short header:

```cpp
// rom-validation.cpp
// Cartridge ROM header validation.
// Created by William Kiem Lafond on 2025-09-17.
```

Keep file headers factual and brief.

---

### Avoid

* Commenting obvious code
* Large prose blocks
* Stale comments (e.g., "last modified by")

---

## Includes

* Headers must be self-contained
* Do not use `using namespace` in headers
* Prefer forward declarations where reasonable

---

## General Principles

* Make illegal states unrepresentable
* Prefer early returns
* Prefer table-driven logic for hot paths
* Optimize for readability before micro-optimizations
