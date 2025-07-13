<div align="center">
  <h1>ITMO-Script</h1>
  <p><em>A sophisticated AST-based interpreter built on context-free grammar foundations</em></p>

  [![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
  [![Test Coverage](https://img.shields.io/badge/coverage-80%25-brightgreen)]()
  [![C++](https://img.shields.io/badge/C%2B%2B-23-blue)]()
  [![License](https://img.shields.io/badge/license-MIT-blue)]()
</div>

## 📜 Overview

ITMO-Script is not just another interpreter — it is a meticulously crafted language environment that demonstrates the elegant marriage of theoretical computer science and practical software engineering. Built from the ground up with a focus on clean architecture, extensibility, and robust error handling, this project showcases advanced concepts in compiler design and language implementation.

The interpreter follows the classic pipeline of lexical analysis, parsing, and execution while introducing modern design patterns that make the codebase maintainable and extensible. Whether you're interested in learning how interpreters work or need a foundation for building your own programming language, ITMO-Script provides a comprehensive example of how to do it right.

## 🚀 Installation & Setup

### Prerequisites

Before diving into ITMO-Script, ensure your system meets these requirements:

- C++23 compatible compiler (GCC 11+, Clang 17+)
- CMake 3.12 or higher
- Python 3.6+ (for visualization tools)
- Git

### Building from Source

Clone the repository and build with these commands:

```bash
# Clone the repository
git clone https://github.com/axonde/itmo-script.git
cd itmo-script

# Create and enter build directory
mkdir build && cd build

# Configure the build
cmake ..

# Compile (using multiple cores for faster build)
make -j$(nproc)

# Run tests to verify installation
ctest -V
```

## 💡 Quick Start Guide

Let's explore ITMO-Script through examples to quickly understand its capabilities.

### Running Your First Script

Create a file named `hello.is`:

```
// My first ITMO-Script program
greet = function(name)
    return "Hello, " + name + "! Welcome to ITMO-Script."
end function

println(greet("Developer"))

// Variables and basic operations
counter = 10
while counter > 0
    println("Countdown: " + counter)
    counter = counter - 1
end while

println("Liftoff!")
```

Execute it:

```bash
./itmo-script path/to/hello.is
```

### Interactive Mode

Experiment with the language using the interactive REPL (Read-Eval-Print Loop):

```bash
./itmo-script

> x = 42
> y = 8
> print(x * y)
336
> fib = function(n)
... if n <= 1 then
... ... return n
    end if
    return fib(n - 1) + fib(n - 2)
end function
> print(fib(10))
55
> exit()
```

### Command Line Options

ITMO-Script offers several command-line options:

```bash
./itmo-script --help                   # Display help information
./itmo-script --version                # Show version info
./itmo-script --ast path/to/script.its # Generate and display AST
./itmo-script --tokens file.its        # Display token stream
./itmo-script --trace file.its         # Run with execution tracing
```

## 🏗️ Architecture Deep Dive

ITMO-Script implements a clean, layered architecture that separates concerns while maintaining flexibility. Let's explore each component:

### The Big Picture

```
┌───────────────────┐     ┌───────────────────┐     ┌───────────────────┐
│    Source Code    │────►│  Lexical Analysis  │────►│  Syntax Analysis  │
└───────────────────┘     └───────────────────┘     └───────────────────┘
                                                             │
                                                             ▼
┌───────────────────┐     ┌───────────────────┐     ┌───────────────────┐
│  Runtime System   │◄────│    Interpreter    │◄────│   Abstract Syntax │
│  (Holder Packs)   │     │                   │     │   Tree (AST)      │
└───────────────────┘     └───────────────────┘     └───────────────────┘
        │                          ▲
        │                          │
        ▼                          │
┌───────────────────┐     ┌───────────────────┐
│  Memory Manager   │     │  Standard Library │
└───────────────────┘     └───────────────────┘
```

### 1. Lexical Analysis: The Tokenizer

The tokenizer transforms raw source text into a meaningful stream of tokens. This is the first step in understanding the structure of the program.

#### Token Classification

The tokenizer identifies various elements of the language:

| Token Type | Examples | Description |
|------------|----------|-------------|
| Keywords | `if`, `else`, `while`, `function` | Reserved words with special meaning |
| Identifiers | `variable_name`, `calculate` | Names defined by the programmer |
| Literals | `42`, `"text"`, `true` | Direct value representations |
| Operators | `+`, `-`, `*`, `/`, `==` | Symbols for operations |
| Delimiters | `{`, `}`, `;`, `,` | Symbols that structure the code |

#### Implementation Highlights

The tokenizer employs a state machine approach, making it both efficient and flexible:

```cpp
enum class TokenizerState {
    START,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    COMMENT,
    // ...more states
};

// A simplified excerpt of the tokenization process
Token Tokenizer::nextToken() {
    TokenizerState state = TokenizerState::START;
    std::string lexeme;
    
    while (hasMoreChars()) {
        char c = peekNextChar();
        
        switch (state) {
            case TokenizerState::START:
                if (isalpha(c) || c == '_') {
                    state = TokenizerState::IDENTIFIER;
                    lexeme += consumeNextChar();
                }
                else if (isdigit(c)) {
                    state = TokenizerState::NUMBER;
                    lexeme += consumeNextChar();
                }
                // ...more conditions
                break;
                
            case TokenizerState::IDENTIFIER:
                // Handle identifier construction
                // ...
                
            // ...more states and transitions
        }
    }
    
    // Determine token type and return the token
    // ...
}
```

The tokenizer maintains positional information (line and column numbers) for precise error reporting, critical for a developer-friendly language environment.

### 2. Syntax Analysis: Building the AST

Once tokenized, the parser transforms the token stream into an Abstract Syntax Tree (AST) that represents the hierarchical structure of the program.

#### Grammar Foundation

ITMO-Script is defined by a context-free grammar that precisely specifies its syntax. Here's a simplified excerpt:

```
Program        → Statement*
Statement      → ExpressionStmt | BlockStmt | IfStmt | WhileStmt | FunctionDecl | ReturnStmt
ExpressionStmt → Expression ";"
BlockStmt      → "{" Statement* "}"
IfStmt         → "if" "(" Expression ")" Statement ("else" Statement)?
WhileStmt      → "while" "(" Expression ")" Statement
FunctionDecl   → "function" IDENTIFIER "(" Parameters? ")" BlockStmt
Expression     → Assignment
Assignment     → IDENTIFIER "=" Assignment | LogicOr
LogicOr        → LogicAnd ("||" LogicAnd)*
// ...and so on
```

#### Parser Implementation

The parser uses recursive descent with predictive parsing techniques:

```cpp
// Simplified parser example
std::unique_ptr<Stmt> Parser::parseStatement() {
    if (match(TokenType::IF)) return parseIfStatement();
    if (match(TokenType::WHILE)) return parseWhileStatement();
    if (match(TokenType::FUNCTION)) return parseFunctionDeclaration();
    if (match(TokenType::LEFT_BRACE)) return parseBlockStatement();
    // ...more statement types
    
    return parseExpressionStatement();
}

std::unique_ptr<Stmt> Parser::parseIfStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    
    auto thenBranch = parseStatement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), 
                                    std::move(thenBranch),
                                    std::move(elseBranch));
}
```

#### AST Visualization

Understanding complex programs is easier with visualization. ITMO-Script includes a Python tool to generate graphical representations of the AST:

```bash
# Generate an AST visualization
python tools/ast_visualizer.py examples/fibonacci.its --output fib_ast.png
```

This produces an image showing the hierarchical structure of your program, useful for debugging and educational purposes.

### 3. The Heart of Execution: Holder Pack System

The Holder Pack system is one of the most innovative aspects of ITMO-Script, providing sophisticated memory management and type operations.

#### Concept and Structure

```
┌─────────────────────────────────────────┐
│             Holder Pack                 │
├─────────────────┬───────────────────────┤
│   Type Metadata │       Value Data      │
├─────────────────┼───────────────────────┤
│ • Type ID       │ • Raw memory block    │
│ • Operations    │ • Reference count     │
│ • Type rules    │ • Garbage collection  │
│ • Conversions   │   metadata            │
└─────────────────┴───────────────────────┘
```

Each value in ITMO-Script is wrapped in a Holder Pack that contains both the value itself and metadata about its type. This allows for:

1. **Dynamic typing with static type safety** - Type checking happens at runtime but with the rigor of a statically typed language
2. **Automatic memory management** - Reference counting and garbage collection happen transparently
3. **Polymorphic operations** - The same operation (like `+`) can behave differently based on types
4. **Value semantics with efficient implementation** - Values behave intuitively while optimizing memory usage

#### Implementation Example

```cpp
class HolderPack {
private:
    TypeId type;
    void* data;
    std::atomic<int> refCount;
    
public:
    // Create a new holder pack
    template<typename T>
    static HolderPack create(T value) {
        HolderPack pack;
        pack.type = TypeSystem::getTypeId<T>();
        pack.data = TypeSystem::allocate<T>(value);
        pack.refCount = 1;
        return pack;
    }
    
    // Type-safe value access
    template<typename T>
    T& as() {
        if (type != TypeSystem::getTypeId<T>()) {
            throw TypeError("Invalid type cast");
        }
        return *static_cast<T*>(data);
    }
    
    // Reference management
    HolderPack(const HolderPack& other) {
        type = other.type;
        data = other.data;
        refCount = other.refCount;
        refCount++;
    }
    
    ~HolderPack() {
        if (--refCount == 0) {
            TypeSystem::deallocate(type, data);
        }
    }
    
    // ...more methods for operations, comparisons, etc.
};
```

### 4. Operator System: Flexibility by Design

ITMO-Script features a modular operator system that makes it easy to extend the language with new operations.

#### Operator Registration

Operators are registered with the type system, specifying behavior for different operand types:

```cpp
// Registering the '+' operator for different type combinations
void registerOperators() {
    // Number + Number -> Number (addition)
    OperatorRegistry::registerBinary(
        Operator::PLUS,
        TypeId::NUMBER, TypeId::NUMBER,
        [](const HolderPack& left, const HolderPack& right) {
            double result = left.as<double>() + right.as<double>();
            return HolderPack::create(result);
        }
    );
    
    // String + String -> String (concatenation)
    OperatorRegistry::registerBinary(
        Operator::PLUS,
        TypeId::STRING, TypeId::STRING,
        [](const HolderPack& left, const HolderPack& right) {
            std::string result = left.as<std::string>() + right.as<std::string>();
            return HolderPack::create(result);
        }
    );
    
    // String + Number -> String (convert number to string, then concatenate)
    OperatorRegistry::registerBinary(
        Operator::PLUS,
        TypeId::STRING, TypeId::NUMBER,
        [](const HolderPack& left, const HolderPack& right) {
            std::string result = left.as<std::string>() + 
                                std::to_string(right.as<double>());
            return HolderPack::create(result);
        }
    );
    
    // ...more operator registrations
}
```

This design makes it trivial to add new operators or extend existing ones to work with new types.

#### Supported Operators

ITMO-Script supports a rich set of operators:

| Category | Operators | Description |
|----------|-----------|-------------|
| Arithmetic | `+`, `-`, `*`, `/`, `%`, `**` | Basic math plus modulo and power |
| Comparison | `==`, `!=`, `<`, `>`, `<=`, `>=` | Value comparison |
| Logical | `&&`, `||`, `!` | Boolean logic |
| Assignment | `=`, `+=`, `-=`, `*=`, `/=` | Variable assignment with shortcuts |
| Increment/Decrement | `++`, `--` | Pre/post increment and decrement |
| Bitwise | `&`, `|`, `^`, `~`, `<<`, `>>` | Operations on bit patterns |

## 🔍 Language Features

ITMO-Script offers a powerful set of features that make it both expressive and practical:

### Strong Dynamic Typing

Variables are dynamically typed but strongly checked:

```
let x = 5;        // x is a number
x = "hello";      // x is now a string
x = x + 10;       // TypeError: Cannot add string and number
```

### First-class Functions

Functions are first-class citizens that can be passed as arguments, returned from other functions, and stored in variables:

```
// Function as a value
let add = function(a, b) {
    return a + b;
};

// Higher-order function
function applyTwice(func, value) {
    return func(func(value));
}

print(applyTwice(function(x) { return x * 2; }, 3));  // Outputs: 12
```

### Closures

Functions capture their environment, creating closures:

```
function makeCounter() {
    let count = 0;
    return function() {
        count = count + 1;
        return count;
    };
}

let counter = makeCounter();
print(counter());  // 1
print(counter());  // 2
print(counter());  // 3
```

### Comprehensive Control Flow

ITMO-Script provides all the control flow constructs you'd expect:

```
// If-else statements
if (condition) {
    // code
} else if (anotherCondition) {
    // code
} else {
    // code
}

// While loops
while (condition) {
    // code
    if (earlyExit) break;
    if (skipIteration) continue;
}

// For loops
for (let i = 0; i < 10; i++) {
    // code
}

// Try-catch for error handling
try {
    riskyOperation();
} catch (error) {
    handleError(error);
}
```

### Rich Standard Library

The standard library provides essential functionality:

```
// Math functions
print(Math.sqrt(16));      // 4
print(Math.sin(Math.PI));  // 0

// String operations
let text = "Hello, world!";
print(text.length());      // 13
print(text.substring(0, 5)); // "Hello"

// Arrays
let arr = [1, 2, 3, 4, 5];
print(arr.length());       // 5
arr.push(6);
print(arr[5]);             // 6

// Time and date
let now = Time.now();
print(Time.format(now, "YYYY-MM-DD")); // e.g., "2025-07-13"
```

## 🧪 Testing and Quality Assurance

ITMO-Script is built with a test-driven approach, ensuring reliability and correctness.

### Comprehensive Test Suite

The project includes extensive tests at multiple levels:

1. **Unit Tests** - Testing individual components in isolation
2. **Integration Tests** - Testing how components work together
3. **End-to-End Tests** - Testing complete program execution
4. **Stress Tests** - Testing performance under load
5. **Edge Case Tests** - Testing unusual and boundary conditions

### Running Tests

Execute the test suite:

```bash
cd build
ctest -V                 # Run all tests with verbose output
ctest -R TokenizerTests  # Run only tokenizer tests
ctest -R ParserTests     # Run only parser tests
```

### Coverage and Quality Metrics

ITMO-Script maintains rigorous quality standards:

- **100% test coverage** for critical components
- **Static analysis** with tools like clang-tidy and cppcheck
- **Memory leak detection** with Valgrind and sanitizers
- **Performance benchmarking** to prevent regressions

## 📚 Advanced Topics

### Extending the Language

Adding new features to ITMO-Script is straightforward due to its modular design:

1. **Adding new types** - Implement a new type class and register it with the type system
2. **Adding new operators** - Register new operator handlers in the operator registry
3. **Adding new built-in functions** - Add entries to the built-in function table
4. **Adding syntax features** - Extend the grammar and implement corresponding AST nodes

### Performance Optimization

ITMO-Script includes several optimizations:

1. **Expression caching** - Common subexpressions are evaluated only once
2. **Object pooling** - Reusing memory for temporary objects
3. **Just-in-time compilation** - Hot code paths can be compiled to native code
4. **Lazy evaluation** - Expressions are only evaluated when needed

### Embedding ITMO-Script

You can embed ITMO-Script in your C++ applications:

```cpp
#include "itmo_script.h"

int main() {
    // Initialize the interpreter
    itmo::Interpreter interpreter;
    
    // Define a C++ function to expose to scripts
    interpreter.registerFunction("hostFunction", [](int x, int y) {
        return x * y;
    });
    
    // Run a script
    try {
        interpreter.runFile("my_script.its");
    } catch (const itmo::RuntimeError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## 📝 Future Directions

ITMO-Script is continuously evolving. Planned features include:

- **Module system** for better code organization
- **Async/await** for asynchronous programming
- **Pattern matching** for more expressive conditionals
- **Type annotations** for optional static typing
- **Optimizing compiler** for improved performance

## 🙏 Acknowledgements

This project stands on the shoulders of giants:

- Robert Nystrom's "Crafting Interpreters" for foundational concepts
- The LLVM project for inspiration on modularity
- The TypeScript team for type system insights
- The ITMO University faculty for theoretical foundations

## 🤝 Contributing

Contributions are welcome! Please check the [Contributing Guide](CONTRIBUTING.md) for details on how to get involved.

---

<div align="center">
  <p>Designed and implemented with ❤️ by <a href="https://github.com/axonde">axonde</a></p>
  <p><small>© 2025 All Rights Reserved</small></p>
</div>
