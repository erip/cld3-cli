#ifndef CLD3_CLI_H_
#define CLD3_CLI_H_

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "third_party/cld_3/src/src/nnet_language_identifier.h"

std::string get_input_path(const char* arg) {
    if(!arg) {
        throw std::invalid_argument{"No <input-path> argument."};
    }
    std::string input_path{arg};
    if(!fs::is_directory(input_path) && !fs::is_regular_file(input_path)) {
        throw std::invalid_argument{"<input-path> should be either a directory or a file."};
    }
    return input_path;
}

std::string get_output_format(const char* arg) {
    if(!arg) {
        throw std::invalid_argument{"No <output-format> argument."};
    }

    // Convert the format to lowercase.
    std::string format{arg};
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);

    if(format != "json" && format != "stdout") {
        throw std::invalid_argument{"<output-format> should be either \"json\" or \"stdout\"."};
    }

    return format;
}

int get_num_langs(const char* arg) {
    if(!arg) return 1;
    int N;
    std::stringstream ss{arg};
    ss >> N;
    if(N <= 0) {
        throw std::invalid_argument{"<N> should be a non-negative number."};
    }
    return N;
}

class CLD3_cli {
    public:
        CLD3_cli(const std::string& input_path,
                 const std::string& output_format,
                 const int N=1) :
                    input_path{input_path},
                    output_format{output_format},
                    N{N}
        {
        }

        friend std::ostream& operator<<(std::ostream& os, const CLD3_cli& cli);

    private:
        const std::string get_output_format() const { return output_format; }

        std::string input_path;
        std::string output_format;
        int N;
};

std::ostream& operator<<(std::ostream& os, const CLD3_cli& cli) {
    return os;
}

#endif // CLD3_CLI_H_
