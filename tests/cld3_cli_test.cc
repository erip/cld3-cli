#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../cld3_cli.h"

#include <string>
#include <fstream>

TEST_CASE("Input path does not exist", "[get_input_path]" ) {
    const char* no_input_path = nullptr;
    REQUIRE_THROWS_AS(get_input_path(no_input_path), std::invalid_argument);
}

TEST_CASE("Input path exists, but isn't a file/dir", "[get_input_path]" ) {
    const char* broken_input_path = "hello";
    REQUIRE_THROWS_AS(get_input_path(broken_input_path), std::invalid_argument);
}

TEST_CASE("Input path exists and is a directory", "[get_input_path]" ) {
    fs::create_directories("sandbox");
    const char* input_path = "sandbox";
    REQUIRE(get_input_path(input_path) == "sandbox");
    fs::remove_all("sandbox");
}

TEST_CASE("Input path exists and is a file", "[get_input_path]" ) {
    std::ofstream("file.txt");
    const char* input_path = "file.txt";
    REQUIRE(get_input_path(input_path) == "file.txt");
    fs::remove_all("file.txt");
}

TEST_CASE("Output format does not exist", "[get_output_format]") {
    const char* no_output_format = nullptr;
    REQUIRE_THROWS_AS(get_output_format(no_output_format), std::invalid_argument);
}

TEST_CASE("Output format exists, but isn't json and isn't stdout", "[get_output_format]") {
    const char* broken_output_format = "foo";
    REQUIRE_THROWS_AS(get_output_format(broken_output_format), std::invalid_argument);
}

TEST_CASE("Output format exists and is json (case-insensitive)", "[get_output_format]") {
    const char* cap_json_output_format = "JSON";
    REQUIRE(get_output_format(cap_json_output_format) == "json");
}

TEST_CASE("Output format exists and is stdout (case-insensitive)", "[get_output_format]") {
    const char* mixed_stdout_output_format = "StDoUt";
    REQUIRE(get_output_format(mixed_stdout_output_format) == "stdout");
}

TEST_CASE("Workflow processing does not exist", "[get_processing_workflow]" ) {
    const char* no_workflow = nullptr;
    REQUIRE_THROWS_AS(get_processing_workflow(no_workflow), std::invalid_argument);
}

TEST_CASE("Workflow processing exists, but is invalid", "[get_processing_workflow]" ) {
    const char* broken_workflow = "line-by-file";
    REQUIRE_THROWS_AS(get_processing_workflow(broken_workflow), std::invalid_argument);
}

TEST_CASE("Workflow processing is line-by-line", "[get_processing_workflow]" ) {
    const char* line_by_line_workflow = "line-by-line";
    REQUIRE(get_processing_workflow(line_by_line_workflow) == "line-by-line");
}

TEST_CASE("Workflow processing is whole-text", "[get_processing_workflow]" ) {
    const char* whole_file_workflow = "whole-text";
    REQUIRE(get_processing_workflow(whole_file_workflow) == "whole-text");
}

TEST_CASE("N does not exist", "[get_num_langs]") {
    const char* no_N = nullptr;
    REQUIRE(get_num_langs(no_N) == 1);
}

TEST_CASE("N exists, but is negative", "[get_num_langs]") {
    const char* negative_N = "-1";
    REQUIRE_THROWS_AS(get_num_langs(negative_N), std::invalid_argument);
}

TEST_CASE("N exists, but is not a number", "[get_num_langs]") {
    const char* broken_N = "hello";
    REQUIRE_THROWS_AS(get_num_langs(broken_N), std::invalid_argument);
}

TEST_CASE("N exists, but is 0", "[get_num_langs]") {
    const char* zero_N = "0";
    REQUIRE_THROWS_AS(get_num_langs(zero_N), std::invalid_argument);
}

TEST_CASE("N exists and is positive", "[get_num_langs]") {
    const char* N = "10";
    REQUIRE(get_num_langs(N) == 10);
}

TEST_CASE("Test empty results when no input given", "[CLD3_cli::get_results]") {
    fs::create_directories("sandbox");
    CLD3_cli cli{"sandbox", "json", "line-by-line", 1};
    REQUIRE(cli.get_results().size() == 0);
    fs::remove_all("sandbox");
}

TEST_CASE("Identify English text only line-by-line", "[CLD3_cli::work]") {
    std::ofstream fout{"hello.txt"};
    fout << "This text is written in English.";
    fout.close();
    CLD3_cli cli{"hello.txt", "json", "line-by-line", 1};
    cli.work();
    fs::remove_all("hello.txt");
    REQUIRE(cli.get_results().size() == 1);
    const auto res = cli.get_results()[0];
    REQUIRE(res["language"].as<std::string>() == "en");
    REQUIRE(res["probability"].as<double>() == 0.997049868106842);
    REQUIRE(res["proportion"].as<double>() == 1.0);
    REQUIRE(res["reliable"].as<bool>() == true);
    REQUIRE(res["text"].as<std::string>() == "This text is written in English.");
}

TEST_CASE("Indentifying English text only whole-text", "[CLD3_cli::work]") {
    std::ofstream fout{"hello.txt"};
    std::string text{"This text is written in English.\nCan you read it?\n"};
    fout << text;
    fout.close();
    CLD3_cli cli{"hello.txt", "json", "whole-text", 1};
    cli.work();
    fs::remove_all("hello.txt");
    REQUIRE(cli.get_results().size() == 1);
    const auto res = cli.get_results()[0];
    REQUIRE(res["language"].as<std::string>() == "en");
    REQUIRE(res["probability"].as<double>() == 0.9997177720069885);
    REQUIRE(res["proportion"].as<double>() == 1.0);
    REQUIRE(res["reliable"].as<bool>() == true);
    escape_newlines(text);
    REQUIRE(res["text"].as<std::string>() == text);
}
