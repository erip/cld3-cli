#ifndef CLD3_CLI_H_
#define CLD3_CLI_H_

#include <string>
#include <fstream>
#include <streambuf>

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

std::string get_processing_workflow(const char* arg) {
    if(!arg) {
        throw std::invalid_argument{"No <process-workflow> argument."};
    }
    std::string process{arg};
    std::transform(process.begin(), process.end(), process.begin(), ::tolower);

    if(process != "line-by-line" && process != "whole-text") {
        throw std::invalid_argument{"<process-workflow> should be either \"line-by-line\" or a \"whole-text\"."};
    }
    return process;
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
                 const std::string& process_workflow,
                 const int N=1) :
        input_path{input_path},
        output_format{output_format},
        process_workflow{process_workflow},
        N{N},
        lang_id(0, 1000)
        {
        }

        void work();
        void output() const;
        json get_results() const { return results; }

    private:
        const std::string get_output_format() const { return output_format; }
        json create_lang_entry(const std::string& text,
                               const NNetLanguageIdentifier::Result &result);
        void identify_most_likely(const std::string& text);
        void identify_N_most_likely(const std::string& text);

        // line-by-line processing workflow
        void identify_line_by_line(const std::string& filename);
        void identify_most_likely_lang_per_line(const std::string& filename);
        void identify_most_likely_N_langs_per_line(const std::string& filename);

        // whole-text processing workflow
        void identify_whole_text(const std::string& filename);
        void identify_most_likely_lang_of_file(const std::string& filename);
        void identify_most_likely_N_langs_of_file(const std::string& filename);

        std::string input_path;
        std::string output_format;
        std::string process_workflow;
        int N;
        json results = json::array();
        NNetLanguageIdentifier lang_id;
};

void CLD3_cli::work() {
    if(fs::is_regular_file(this->input_path)) {
        if(process_workflow == "line-by-line") {
            this->identify_line_by_line(this->input_path);
        } else if(process_workflow == "whole-text") {
            this->identify_whole_text(this->input_path);
        }
    }
}

void CLD3_cli::output() const {
    if(this->output_format == "json") {
        std::string outfile;
        std::cout << "Please enter the name of the output file: ";
        std::getline(std::cin, outfile);
        std::ofstream fout{outfile};
        fout << pretty_print(this->results) << '\n';
    } else if(this->output_format == "stdout") {
        std::cout << pretty_print(this->results) << '\n';
    }
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

void CLD3_cli::identify_line_by_line(const std::string& filename) {
    if(this->N == 1) {
        this->identify_most_likely_lang_per_line(filename);
    } else {
        this->identify_most_likely_N_langs_per_line(filename);
    }
}

void CLD3_cli::identify_whole_text(const std::string& filename) {
    if(this->N == 1) {
        this->identify_most_likely_lang_of_file(filename);
    } else {
        this->identify_most_likely_N_langs_of_file(filename);
    }
}

void CLD3_cli::identify_most_likely_lang_per_line(const std::string& filename) {
    std::ifstream fin{filename};
    // Identify the most likely lang for each document in the file.
    // Add them to the results.
    std::string doc;
    while(std::getline(fin, doc) && !doc.empty()) {
        this->identify_most_likely(doc);
    }
}

void CLD3_cli::identify_most_likely_N_langs_per_line(const std::string& filename) {
    std::ifstream fin{filename};
    // Identify the N most likely langs for each document in the file.
    // Add them to the results.
    std::string doc;
    while(std::getline(fin, doc) && !doc.empty()) {
        this->identify_N_most_likely(doc);
    }
}

void CLD3_cli::identify_most_likely_lang_of_file(const std::string& filename) {
    std::ifstream fin{filename};
    // Read file into memory
    std::string doc((std::istreambuf_iterator<char>(fin)),
                     std::istreambuf_iterator<char>());
    fin.close();
    if(!doc.empty()) {
        this->identify_most_likely(doc);
    }
}

void CLD3_cli::identify_most_likely_N_langs_of_file(const std::string& filename) {
    std::ifstream fin{filename};
    // Read file into memory
    std::string doc((std::istreambuf_iterator<char>(fin)),
                     std::istreambuf_iterator<char>());
    fin.close();
    if(!doc.empty()) {
        this->identify_N_most_likely(doc);
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
