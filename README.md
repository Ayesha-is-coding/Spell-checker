# Spell Checker

A robust, high-performance command-line spell checker utility built from scratch in C++. This application analyzes structured text documents, flags typographical discrepancies against region-specific lexical databases, and recommends optimized string corrections. 

Every architectural layer—from token parsing to state generation and collision management—is implemented completely using core language paradigms, eliminating any reliance on third-party frameworks, natural language parsing libraries, or cloud infrastructure APIs.

## Co-Authors (
This project was co-developed equally from scratch by:
* **Ayesha Ahmad**
* **Tabeen Bokhat** 

## Core Architecture & Custom Math Algorithms
Unlike standard systems leveraging modern dictionary arrays, this engine contains custom algorithmic mechanics optimized for quick operations:

* **Custom DJB2 Hash Table:** Built with a localized `HashTable` structure utilizing an adaptive **DJB2 string hashing algorithm** (`hash = ((hash << 5) + hash) + c`). It handles collisions natively using bucket-chaining elements (`std::list`) and triggers automatic **dynamic rehashing** when element frequency exceeds thresholds to preserve $O(1)$ lookups.
* **Algorithmic Edit Distance Candidate Generation:** Typographical corrections generate target string recommendations utilizing four strict distance transformations:
  * *Insertions:* Emplaces letters `a-z` at every index character break.
  * *Deletions:* Truncates a singular target offset character across the word index.
  * *Transpositions:* Swaps adjacent parallel characters down the array chain.
  * *Substitutions:* Iteratively swaps a character value across a static dictionary key range.
* **Linguistic Rule-Based Suffix Stemming:** Employs a custom preprocessing system that strips grammatical variations (`-ing`, `-ies`, `-ment`, `-tion`, etc.) to find the base root word before querying the dictionary.
* **Regional Localization:** Dynamically supports targeted text indexing swaps between **UK English** (e.g., *colour*, *realise*) and **US English** text processing requirements on initial boot.

## Prerequisites & Local Setup

### 1. Project Requirements
* A modern C++ compiler supporting the standard `C++11` library or higher (`g++`, `clang++`, or MSVC Compiler).
* Two text files containing target verification dictionaries named **`cleaned_dict_uk.txt`** and **`cleaned_dict_us.txt`** placed inside your project directory folder path.

### 2. Compilation
Compile the source file using standard optimization settings via terminal/command prompt:
```bash
g++ -O3 -std=c++11 main.cpp -o spell_checker
```

### 3. Execution
Launch the executable application interface directly:
```bash
./spell_checker
```

## System Pipeline Layout
1. **Dictionary Hydration:** System loads the selected local language library directly into memory using the dynamic DJB2 hash-map.
2. **Text Parsing:** Raw input text documents are indexed line-by-line, isolating valid alphabetical tokens from punctuation arrays.
3. **Candidate Validation:** Flagged misspelled terms undergo edit distance processing, sorting suggestions based on usage weight frequency metrics before printing selection screens.

