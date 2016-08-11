#ifndef CLD3_CLI_H_
#define CLD3_CLI_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <utility>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "third_party/cld_3/src/src/base.h"
#include "third_party/cld_3/src/src/nnet_language_identifier.h"
using chrome_lang_id::NNetLanguageIdentifier;

#include "jsoncons/json.hpp"
using jsoncons::json;

#include "tclap/CmdLine.h"
using namespace TCLAP;

void escape_newlines(std::string& text) {
    std::size_t start_pos = 0;
    std::string from{"\n"};
    std::string to{"\\n"};
    while((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string check_input_path(const std::string& input_path) {
    if(!fs::is_directory(input_path) && !fs::is_regular_file(input_path)) {
        throw std::invalid_argument{"<input-path> should be either a directory or a file."};
    }
    return input_path;
}

std::string lowercase(const std::string& s) {
    std::string lowercase_s;
    lowercase_s.resize(s.size());
    std::transform(s.begin(), s.end(), lowercase_s.begin(), ::tolower);
    return lowercase_s;
}

std::string check_output_format(const std::string& format) {
    std::string lowercase_format = lowercase(format);
    if(lowercase_format != "json" && lowercase_format != "stdout") {
        throw std::invalid_argument{"<output-format> should be either \"json\" or \"stdout\"."};
    }

    return lowercase_format;
}

std::string check_workflow(const std::string& workflow) {
    std::string lowercase_workflow = lowercase(workflow);
    if(lowercase_workflow != "line-by-line" && lowercase_workflow != "whole-text") {
        throw std::invalid_argument{"<workflow> should be either \"line-by-line\" or a \"whole-text\", not " + lowercase_workflow};
    }
    return lowercase_workflow;
}

int check_num_langs(const int N) {
    if(N <= 0) {
        throw std::invalid_argument{"<N> should be a non-negative number."};
    }
    return N;
}

std::tuple<std::string, std::string, std::string, int> get_command_line_args(int argc, char** argv) {
  CmdLine cmd("CLD3-cli leverages Google's CLD3 model to identify languages.", ' ', "0.1.0");

  // Add handler for number of langs
  std::string short_flag = "N";
  std::string long_flag = "";
  std::string help = "Top N languages to identify (default to 1).";
  bool required = false;
  int default_n = 1;
  std::string type = "N";
  ValueArg<int> N_flag(short_flag, long_flag, help, required,
                       default_n, type);
  cmd.add(N_flag);

  // Add workflow arg handler
  short_flag = "w";
  long_flag = "workflow";
  help = "Either \"line-by-line\" or \"whole-text\".";
  required = true;
  std::string default_value = "line-by-line";
  type = "workflow";
  ValueArg<std::string> workflow_flag(short_flag, long_flag, help, required,
                                      default_value, type);
  cmd.add(workflow_flag);

  // Add output format arg handler
  short_flag = "o";
  long_flag = "output-format";
  help = "Either \"json\" or \"stdout\".";
  required = true;
  default_value = "stdout";
  type = "output-format";
  ValueArg<std::string> output_format_flag(short_flag, long_flag, help, required,
                                           default_value, type);
  cmd.add(output_format_flag);

  // Add input path arg handler
  short_flag = "i";
  long_flag = "input-path";
  help = "Path to UTF8 file or directory containing UTF8 files.";
  required = true;
  default_value = "/dev/null";
  type = "input-path";
  ValueArg<std::string> input_path_flag(short_flag, long_flag, help, required,
                                        default_value, type);
  cmd.add(input_path_flag);

  cmd.parse(argc, argv);

  int N = check_num_langs(N_flag.getValue());
  std::string workflow = check_workflow(workflow_flag.getValue());
  std::string output_format = check_output_format(output_format_flag.getValue());
  std::string input_path = check_input_path(input_path_flag.getValue());

  std::tuple<std::string, std::string, std::string, int> t{input_path, output_format, workflow, N};
  return t;
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
        void identify_most_likely(const std::string& text,
                                  const std::string& source,
                                  const unsigned long line_number=0);
        void identify_N_most_likely(const std::string& text,
                                    const std::string& source,
                                    const unsigned long line_number=0);

        // line-by-line processing workflow
        void identify_directory_line_by_line(const std::string& dirname);
        void identify_line_by_line(const std::string& filename);
        void identify_most_likely_lang_per_line(const std::string& filename);
        void identify_most_likely_N_langs_per_line(const std::string& filename);

        // whole-text processing workflow
        void identify_directory_whole_text(const std::string& dirname);
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
    if(fs::is_regular_file(input_path)) {
        if(process_workflow == "line-by-line") {
            identify_line_by_line(input_path);
        } else if(process_workflow == "whole-text") {
            identify_whole_text(input_path);
        }
    } else if(fs::is_directory(input_path)) {
        if(process_workflow == "line-by-line") {
            identify_directory_line_by_line(input_path);
        } else if(process_workflow == "whole-text") {
            identify_directory_whole_text(input_path);
        }
    }
}

void CLD3_cli::output() const {
    if(output_format == "json") {
        std::string outfile;
        std::cout << "Please enter the name of the output file: ";
        std::getline(std::cin, outfile);
        std::ofstream fout{outfile};
        fout << pretty_print(results) << '\n';
    } else if(output_format == "stdout") {
        for(const auto& result: results.elements()) {
            std::cout << std::fixed;
            std::cout << result["source"].as<std::string>() << '\t' <<
            result["language"].as<std::string>() << '\t' << std::setprecision(9) <<
            result["probability"].as<double>() << '\t' << std::setprecision(9) <<
            result["proportion"].as<double>() << '\t' <<
            result["text"].as<std::string>() << '\n';
        }
    }
}

void CLD3_cli::identify_most_likely(const std::string& text,
                                    const std::string& source,
                                    const unsigned long line_number) {
    // Find the most likely language
    const auto result = lang_id.FindLanguage(text);
    // JSONify the results. Push into our results.
    auto jsonified_results = create_lang_entry(text, result);
    jsonified_results["source"] = source;
    if(process_workflow == "line-by-line") {
        jsonified_results["line_number"] = line_number;
    }
    results.add(jsonified_results);
}

void CLD3_cli::identify_N_most_likely(const std::string& text,
                                    const std::string& source,
                                    const unsigned long line_number) {
    // Find the N most likely languages
    const auto lang_results = lang_id.FindTopNMostLikelyLangs(text, N);
    // JSONify the results. Push them into our results.
    for(const auto& result: lang_results) {
        auto jsonified_results = create_lang_entry(text, result);
        jsonified_results["source"] = source;
        if(process_workflow == "line-by-line") {
            jsonified_results["line_number"] = line_number;
        }
        results.add(jsonified_results);
    }
}

void CLD3_cli::identify_line_by_line(const std::string& filename) {
    if(N == 1) {
        identify_most_likely_lang_per_line(filename);
    } else {
        identify_most_likely_N_langs_per_line(filename);
    }
}

void CLD3_cli::identify_directory_line_by_line(const std::string& dirname) {
    for(const auto& file: fs::directory_iterator(dirname)) {
        if(fs::is_regular_file(file)) {
            identify_line_by_line(file.path());
        }
    }
}


void CLD3_cli::identify_directory_whole_text(const std::string& dirname) {
    for(const auto& file: fs::directory_iterator(dirname)) {
        if(fs::is_regular_file(file)) {
            identify_whole_text(file.path());
        }
    }
}

void CLD3_cli::identify_whole_text(const std::string& filename) {
    if(N == 1) {
        identify_most_likely_lang_of_file(filename);
    } else {
        identify_most_likely_N_langs_of_file(filename);
    }
}

void CLD3_cli::identify_most_likely_lang_per_line(const std::string& filename) {
    std::ifstream fin{filename};
    // Identify the most likely lang for each document in the file.
    // Add them to the results.
    std::string doc;
    unsigned long line_no = 1;
    while(std::getline(fin, doc)) {
        if(!doc.empty()) {
            identify_most_likely(doc, filename, line_no);
        }
        ++line_no;
    }
}

void CLD3_cli::identify_most_likely_N_langs_per_line(const std::string& filename) {
    std::ifstream fin{filename};
    // Identify the N most likely langs for each document in the file.
    // Add them to the results.
    std::string doc;
    unsigned long line_no = 1;
    while(std::getline(fin, doc)) {
        if(!doc.empty()) {
            identify_N_most_likely(doc, filename, line_no);
        }
        ++line_no;
    }
}

void CLD3_cli::identify_most_likely_lang_of_file(const std::string& filename) {
    std::ifstream fin{filename};
    // Read file into memory
    std::string doc((std::istreambuf_iterator<char>(fin)),
                     std::istreambuf_iterator<char>());
    fin.close();
    escape_newlines(doc);
    if(!doc.empty()) {
        identify_most_likely(doc, filename);
    }
}

void CLD3_cli::identify_most_likely_N_langs_of_file(const std::string& filename) {
    std::ifstream fin{filename};
    // Read file into memory
    std::string doc((std::istreambuf_iterator<char>(fin)),
                     std::istreambuf_iterator<char>());
    fin.close();
    escape_newlines(doc);
    if(!doc.empty()) {
        identify_N_most_likely(doc, filename);
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
