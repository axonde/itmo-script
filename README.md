# ITMO-Script: An AST-based Interpreter

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Test Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen)]()
[![C++](https://img.shields.io/badge/C%2B%2B-93.9%25-blue)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()

> An elegant interpreter built on context-free grammar foundations with comprehensive AST parsing capabilities.

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Architecture Overview](#architecture-overview)
- [Tokenizer](#tokenizer)
- [AST Generation](#ast-generation)
- [Holder Pack System](#holder-pack-system)
- [Operator Management](#operator-management)
- [Standart Library](#standart-library)
- [Features](#features)
- [Testing](#testing)
- [Contributing](#contributing)

## 🚀 Installation

Clone the repository and build using CMake:

```bash
# Clone the repository
git clone https://github.com/axonde/itmo-script.git
cd itmo-script

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)
```

### Dependencies

- C++23 compatible compiler
- CMake 3.12+
- Python 3.6+ (for AST visualization)

## 🏃‍♂️ Quick Start

After building, run the interpreter:

```bash
# Run with a script file
./itmo-script path/to/your/script.is

# Or use interactive mode
./itmo-script

# You can take around with the help command
./itmo-script --help
```

Example script:

```
// Simple factorial calculation
factorial = function(n)
    if n <= 1 then
        return 1
    end if
    return n * factorial(n - 1)
end function

print(factorial(5))  // Outputs: 120
```

## 🏗️ Architecture Overview

ITMO-Script is built with a clean separation of concerns:

1. **Lexical Analysis** - Tokenization of source code
2. **Syntactic Analysis** - Parsing tokens into an Abstract Syntax Tree (AST)
4. **Semantic Analysis/ Interpretation** - Validating the AST and Executing the validated AST

The architecture follows modern compiler design principles while maintaining flexibility for easy extension.

## 🔍 Tokenizer

The tokenizer converts source text into a stream of tokens using a deterministic finite automaton (DFA) approach:

```
Source Text → Character Stream → Token Classification → Token Stream
```

The tokenizer recognizes:
- Keywords (if, else, while, for, function, return, etc.)
- Identifiers (variable and function names)
- Literals (numbers, strings, booleans)
- Operators (+, -, *, /, ==, !=, etc.)
- Delimiters ((, ), [, ], //, etc.)

Each token maintains position information for precise error reporting.

## 🌲 AST Generation

The parser constructs an Abstract Syntax Tree (AST) following the context-free grammar defined for ITMO-Script. The AST provides a hierarchical representation of the program structure.

### AST Visualization

The repository includes a Python script for visualizing the AST:

```bash
# Generate AST visualization for a script
python src/parser/serializer/visualizer.py <generated .json by serializer binary>
```

This generates an image representation of the AST, useful for debugging and understanding program structure.

## 📦 Holder Pack System

The Holder Pack system manages memory and object references efficiently:

```
    +----------------+
    |  Holder Pack   |
    +----------------+
           |
           v
    +--------------+     +--------------+     +--------------+
    |  Type Info   |---->| Object Data  |---->| Reference    |
    +--------------+     +--------------+     | Management   |
           |                   |              +--------------+
           v                   v                    |
    +--------------+     +--------------+          |
    | Memory все надо тут передалть!!!!!|<---------+
    | Management   |     | Semantics    |
    +--------------+     +--------------+
```

This system allows for:
- Type-safe operations
- Efficient memory management
- Clean garbage collection
- Intuitive reference handling

## ⚙️ Operator Management

Operators are isolated in a dedicated module, making the interpreter easily extensible:

- **Arithmetic operators**: +, -, *, /, %, ^
- **Comparison operators**: ==, !=, <, >, <=, >=
- **Logical operators**: and, or, not
- **Assignment operators**: =, +=, -=, *=, /=, %=, ^=

Adding new operators requires minimal changes, thanks to the modular design.

## 🌟 Features

- **Strong Type System** - Dynamic typing with strong type checking
- **First-class Functions** - Functions as values and closures
- **Comprehensive Control Flow** - If/else, loops
- **Rich Standard Library** - Common utility functions
- **Detailed Error Reporting** - Clear error messages with line and column information
- **Memory Safety** - Automatic memory management

## 🧪 Testing

The project is completely covered by unit and integration tests, ensuring robustness and reliability. Run the tests with:

```bash
# Run tests
cd build
ctest -V
```

Test coverage includes:
- Lexer and parser edge cases
- AST construction and validation
- Execution of complex programs
- Performance benchmarks

## 🤝 Contributing

Contributions are welcome! Please check the issues page for open tasks or create a new issue to discuss proposed changes.

---

Built with 💻 and ☕ by [axonde](https://github.com/axonde)
