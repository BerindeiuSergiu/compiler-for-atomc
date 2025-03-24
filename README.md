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
    git clone https://github.com/BerindeiuSergiu/compiler-for-atomc.git
    cd compiler-for-atomc
    ```

2. ** Use Makefile: **
    ```sh
    make or mingw-32 make
    ```
   You can also use:
   ```
   make clean or mingw-32 make clean
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

### Project Structure

![image](https://github.com/user-attachments/assets/e4c9a01c-232c-41a3-bffd-4b5e69999701)

For the sake of testing: diff.py -> used to spot differnces in various output files, just change the variables file1 and file2 to your preferences

### Configuration & Customization
Modifying Settings
Lexer and Parser Settings: Modify the lexer.c and parser.c files to change the tokenization and parsing rules.
Error Messages: Customize error messages in the tkerr function in parser.c.


### Acknowledgments
GCC for the compiler.
GNU Make for build automation.
Inspiration from various compiler design courses and textbooks.
