#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>
#include <libmath/sum.hpp>

int main(int argc, char **argv) {
    CLI::App app{"wow, it's working!!"};
    double first = 0;
    double second = 0;
    app.add_option<double>("--first", first, "first");
    app.add_option<double>("--second", second, "second");
    CLI11_PARSE(app, argc, argv);
    if ((app.count("--first") != 1) || (app.count("--second") != 1)) {
        fmt::print("wrong command line arguments\ntry this: ./app --first "
                   "[number] --second [number]\n");
        return -1;
    }
    fmt::print("{}\n", math::sum(first, second));
    return 0;
}