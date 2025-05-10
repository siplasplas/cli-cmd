#include "cli-cmd.hpp"

int main(int argc, char** argv) {
    cli::Application app("mycli");
    app.addCommand("build")
        .addFlag("--release", "-r", "Enable release mode")
        .addParameter("--output", "-o", "path", "Set output file path")
        .handler([](const cli::Actual* a) {
            if (a->containsFlag("--release")) {
                std::cout << "Release mode ON\n";
            }
            if (auto out = a->getParamValue("--output")) {
                std::cout << "Output: " << *out << "\n";
            }
        });
    app.run(argc, argv);
    return 0;
}
