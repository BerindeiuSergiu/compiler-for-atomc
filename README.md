# Simple AtomC Compiler -> work in progress

## Project Overview

This project is a simple parser and lexer written in C. It is designed to tokenize and parse a given input file, providing error messages for syntax errors. The project is aimed at students and developers who are learning about compilers and interpreters.

### Key Features

- Tokenizes input files into a list of tokens.
- Parses the tokens according to a defined grammar.
- Provides detailed error messages for syntax errors.
- Supports basic data types and control structures.

### Target Audience

- Students learning about compilers and interpreters.
- Developers interested in building a simple parser and lexer.
- Educators looking for a teaching tool for compiler design.

## Installation Guide

### Prerequisites

- GCC (GNU Compiler Collection)
- Make (optional, for convenience)

### Step-by-Step Instructions

1. **Clone the repository:**
    ```sh
    git clone https://github.com/yourusername/your-repo-name.git
    cd your-repo-name
    ```

2. **Compile the project:**
    ```sh
    gcc -o parser main.c lexer/lexer.c utils/utils.c parser/parser.c
    ```

3. **(Optional) Use Makefile:**
    ```sh
    (make) || (mingw-32 make)
    ```

## Usage Instructions

### Running the Project

1. **Run the parser with an input file:**
    ```sh
    ./parser path/to/your/inputfile.c
    ```

### Example

```sh
./parser examples/test.c
```

### Environment Variables or Configurations
No specific environment variables or configurations are needed for this project.

###Project Structure

compiler-for-atomc/
├── lexer/
│   └── lexer.c
├── parser/
│   └── [parser.c](http://_vscodecontentref_/0)
├── utils/
│   └── utils.c
├── examples/
│   └── test.c
├── [main.c](http://_vscodecontentref_/1)
├── Makefile
└── README.md


### Configuration & Customization
Modifying Settings
Lexer and Parser Settings: Modify the lexer.c and parser.c files to change the tokenization and parsing rules.
Error Messages: Customize error messages in the tkerr function in parser.c.


### Acknowledgments
GCC for the compiler.
GNU Make for build automation.
Inspiration from various compiler design courses and textbooks.
