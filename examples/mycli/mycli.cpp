#include "cli-cmd.hpp"

int main(int, char**) {
    cli::Application app("mycli",1,1,1);
    app.addCommand("build")
        .addFlag("--release", "-r", "Enable release mode")
        .addParameter("--output", "-o", "Set output file path", "path")
        .handler([](const cli::Actual* a)->int {
            if (a->containsFlag("--release")) {
                std::cout << "Release mode ON\n";
            }
            if (auto out = a->getValue("--output")) {
                std::cout << "Output: " << *out << "\n";
            }
            return 0;
        });
    return 0;
}