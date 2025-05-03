# cli-cmd
# CLI Commander - Modern C++ Command Line Parser

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Build Status](https://github.com/siplasplas/cli-cmd/actions/workflows/cmake.yml/badge.svg)](https://github.com/siplasplas/cli-cmd/actions)

A C++ CLI argument parsing library inspired by Git/GCC, featuring:
- Hierarchical commands (categories > subcategories > commands)
- Automatic help generation
- Positional argument validation
- Configurable parsing modes (Git/GCC style) (planning)
- 
## 📦 Installation
Add as submodule to yout repository.
Library has two modes: header-only and link. 
To use header-only, simply #include "cli-cmd.hpp"; 
to use link mode (if we have multipe .cpp to whicj ust inludes, but now 
library code is so small, header-only is enough),
* #include "cli-cmd.h"
* add two *.src files to project
* or ad these files to static library target and add this library

## 🚀 Quick start
```c++
#include "cli-cmd.hpp"

class MyApp: public cli::Application
{
public:
    MyApp():cli::Application("MyApp"){}
    void initHelp()
    {
        addSubcomand(
        [this](cli::Application* app, cli::Command* cmd) { this->help(app, cmd); },
        "help","Display help information about likegit"
    );
    }
};


void mycommand(cli::Application*, cli::Command*)
{
    std::cout << "hello from handler!" << std::endl;
}

void addSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("only common commands");
    auto cmd = sub->addSubcomand(mycommand, "mycmd", "Info about command");
    cmd->setPositionalArgsLimits(0, 1);
}

void addCategory(cli::Application &app)
{
    auto category = app.addCategory("Main category");
    addSubcategory(category);
}

int main(int argc, char** argv) {
    MyApp app;
    app.initHelp();
    addCategory(app);
    app.run(argc, argv);
    return 0;
}
```
Build with
```shell
  g++ -Wall -Wextra -I../../include -std=c++17 -O2 first.cpp -o program
```