#include "cli-cmd.hpp"

void test(int cmdDepth, int helpAvailability) {
    std::cout << cli::fmt("==================cmdDepth=%d, helpAvailability=%d\n",cmdDepth,helpAvailability);
    cli::Application app("test", cmdDepth,1,helpAvailability);
    auto category = app.addCategory("Category");
    category->addCommand("extendedCmd").desc("visible with --all");
    category->addCommand("categoryCmd").desc("visible with simple help");
    app.addHelpCategory("Commons")
        .ref("categoryCmd");
    app.addCommand("globcmd").desc("glob command")
        .addFlag("--release", "-r", "Enable release mode")
        .addParameter("--output", "-o", "Set output file path", "general-path")
        .handler([](const cli::Actual* a) {
            if (a->containsFlag("--release")) {
                std::cout << "Release mode ON\n";
            }
            if (auto out = a->getParamValue("--output")) {
                std::cout << "Output: " << *out << "\n";
            }
        });

    app.parse("test");
    app.execute();

    app.parse("test help");
    app.execute();

    app.parse("test help build");
    app.execute();

    app.parse("test --help");
    app.execute();
    app.parse("test help --all");
    app.execute();
    app.parse("test --help --all");
    app.execute();
    app.parse("test build --help");
    app.execute();
}

int main(int, char**) {
    for (int cmDepth = 0; cmDepth <= 3; cmDepth++)
        for (int ha = 0; ha <= 2; ha++)
            test(cmDepth, ha);
    return 0;
}
