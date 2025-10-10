/*!
\mainpage MyProject — Command Line Library

# 🧩 Overview
**MyProject** is a modern, modular C++ command-line parsing library.  
It provides a simple interface for building rich CLI tools with argument parsing,
documentation generation, and validation.

# 🚀 Features
- Simple command/argument registration API
- Automatic documentation generation
- Colorized logging system
- Modular and extensible architecture

# 📚 Getting Started
To begin, include the library and define your root command:

```cpp
cli::CliApp app("mytool");
app.addCommand("run", "Execute a process", []() { /* ... */ });