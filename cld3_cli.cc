#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#include "cld3_cli.h"

void print_usage() {
    const std::string usage = "Usage: ./cld3_cli <input-path> <output-format> (<N>=1)";
    const std::string input_path_help = "<input-path>: Path to UTF8 file or directory containing UTF8 files.";
    const std::string output_format_help = "<output-format>: Either \"json\" or \"stdout\".";
    const std::string N_help = "(<N>=1): Top N languages to identify (default to 1).";

    std::cerr << usage << '\n' <<
                 input_path_help << '\n' <<
                 output_format_help << '\n' <<
                 N_help << '\n';
}

int main(int argc, char** argv) {
    if(argc != 3 && argc != 4) {
        print_usage();
        return -1;
    }

    try {
        CLD3_cli cli{get_input_path(argv[1]), get_output_format(argv[2]), get_num_langs(argv[3])};
        cli.work();
    } catch(const std::invalid_argument& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}
