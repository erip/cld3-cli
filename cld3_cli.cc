#include <iostream>
#include <string>
#include <algorithm>
#include <utility>

#include "cld3_cli.h"

int main(int argc, char** argv) {
    try {
      auto args = get_command_line_args(argc, argv);
      CLD3_cli cli{std::get<0>(args), std::get<1>(args),
                   std::get<2>(args), std::get<3>(args)};
      cli.work();
      cli.output();
    } catch(const std::invalid_argument& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}
