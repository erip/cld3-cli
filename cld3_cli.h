#ifndef CLD3_CLI_H_
#define CLD3_CLI_H_

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "third_party/cld_3/src/src/base.h"
#include "third_party/cld_3/src/src/nnet_language_identifier.h"
using chrome_lang_id::NNetLanguageIdentifier;

#include "jsoncons/json.hpp"
using jsoncons::json;

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
        N{N},
        lang_id(0, 1000)
        {
        }

        json get_results() const { return results; }
        friend std::ostream& operator<<(std::ostream& os, const CLD3_cli& cli);

    private:
        const std::string get_output_format() const { return output_format; }
        json create_lang_entry(const std::string& text,
                               const NNetLanguageIdentifier::Result &result);
        void identify_most_likely(const std::string& text);
        void identify_N_most_likely(const std::string& text);

        std::string input_path;
        std::string output_format;
        int N;
        json results = json::array();
        NNetLanguageIdentifier lang_id;
};

std::ostream& operator<<(std::ostream& os, const CLD3_cli& cli) {
    return os;
}

void CLD3_cli::identify_most_likely(const std::string& text) {
    // Find the most likely language
    const auto result = this->lang_id.FindLanguage(text);
    // JSONify the results. Push into our results.
    const auto jsonified_results = this->create_lang_entry(text, result);
    this->results.add(jsonified_results);
}

void CLD3_cli::identify_N_most_likely(const std::string& text) {
    // Find the N most likely languages
    const auto results = this->lang_id.FindTopNMostLikelyLangs(text, this->N);
    // JSONify the results. Push them into our results.
    for(const auto& result: results) {
        const auto jsonified_results = this->create_lang_entry(text, result);
        this->results.add(jsonified_results);
    }
}

json CLD3_cli::create_lang_entry(const std::string& text,
                                 const NNetLanguageIdentifier::Result &result) {
    json jsonified_result;

    // Add result attributes to JSON object.
    jsonified_result["text"] = text;
    jsonified_result["language"] = result.language;
    jsonified_result["probability"] = result.probability;
    jsonified_result["reliable"] = result.is_reliable;
    jsonified_result["proportion"] = result.proportion;

    return jsonified_result;
}

#endif // CLD3_CLI_H_
