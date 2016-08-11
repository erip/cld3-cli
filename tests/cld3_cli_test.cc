#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../cld3_cli.h"

#include <string>
#include <fstream>

TEST_CASE("Input path does not exist", "[check_input_path]" ) {
    const std::string no_input_path = "";
    REQUIRE_THROWS_AS(check_input_path(no_input_path), std::invalid_argument);
}

TEST_CASE("Input path exists, but isn't a file/dir", "[get_input_path]" ) {
    const std::string broken_input_path = "hello";
    REQUIRE_THROWS_AS(check_input_path(broken_input_path), std::invalid_argument);
}

TEST_CASE("Input path exists and is a directory", "[check_input_path]" ) {
    fs::create_directories("sandbox");
    const std::string input_path = "sandbox";
    REQUIRE(check_input_path(input_path) == "sandbox");
    fs::remove_all("sandbox");
}

TEST_CASE("Input path exists and is a file", "[check_input_path]" ) {
    std::ofstream("file.txt");
    const std::string input_path = "file.txt";
    REQUIRE(check_input_path(input_path) == "file.txt");
    fs::remove_all("file.txt");
}

TEST_CASE("Output format does not exist", "[check_output_format]") {
    const std::string no_output_format = "";
    REQUIRE_THROWS_AS(check_output_format(no_output_format), std::invalid_argument);
}

TEST_CASE("Output format exists, but isn't json and isn't stdout", "[check_output_format]") {
    const std::string broken_output_format = "foo";
    REQUIRE_THROWS_AS(check_output_format(broken_output_format), std::invalid_argument);
}

TEST_CASE("Output format exists and is json (case-insensitive)", "[check_output_format]") {
    const std::string cap_json_output_format = "JSON";
    REQUIRE(check_output_format(cap_json_output_format) == "json");
}

TEST_CASE("Output format exists and is stdout (case-insensitive)", "[check_output_format]") {
    const std::string mixed_stdout_output_format = "StDoUt";
    REQUIRE(check_output_format(mixed_stdout_output_format) == "stdout");
}

TEST_CASE("Workflow processing does not exist", "[check_workflow]" ) {
    const std::string no_workflow = "";
    REQUIRE_THROWS_AS(check_workflow(no_workflow), std::invalid_argument);
}

TEST_CASE("Workflow processing exists, but is invalid", "[check_workflow]" ) {
    const std::string broken_workflow = "line-by-file";
    REQUIRE_THROWS_AS(check_workflow(broken_workflow), std::invalid_argument);
}

TEST_CASE("Workflow processing is line-by-line", "[check_workflow]" ) {
    const std::string line_by_line_workflow = "line-by-line";
    REQUIRE(check_workflow(line_by_line_workflow) == "line-by-line");
}

TEST_CASE("Workflow processing is whole-text", "[check_workflow]" ) {
    const std::string whole_file_workflow = "whole-text";
    REQUIRE(check_workflow(whole_file_workflow) == "whole-text");
}

TEST_CASE("N exists, but is negative", "[check_num_langs]") {
    const int negative_N = -1;
    REQUIRE_THROWS_AS(check_num_langs(negative_N), std::invalid_argument);
}

TEST_CASE("N exists, but is 0", "[check_num_langs]") {
    const int zero_N = 0;
    REQUIRE_THROWS_AS(check_num_langs(zero_N), std::invalid_argument);
}

TEST_CASE("N exists and is positive", "[check_num_langs]") {
    const int N = 10;
    REQUIRE(check_num_langs(N) == 10);
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

TEST_CASE("Indentifying English text with two lines line-by-line", "[CLD3_cli::work]") {
    std::ofstream fout{"hello.txt"};
    std::string text{"This text is written in English.\nCan you read it?\n"};
    fout << text;
    fout.close();
    CLD3_cli cli{"hello.txt", "json", "line-by-line", 1};
    cli.work();
    fs::remove_all("hello.txt");
    REQUIRE(cli.get_results().size() == 2);
}

TEST_CASE("Indentifying English text with two lines line-by-line, top 2 languages", "[CLD3_cli::work]") {
    std::ofstream fout{"hello.txt"};
    std::string text{"This text is written in English.\nCan you read it?\n"};
    fout << text;
    fout.close();
    CLD3_cli cli{"hello.txt", "json", "line-by-line", 2};
    cli.work();
    fs::remove_all("hello.txt");
    REQUIRE(cli.get_results().size() == 4);
    REQUIRE(cli.get_results()[0]["language"].as<std::string>() == "en");
    REQUIRE(cli.get_results()[1]["language"].as<std::string>() == "<unknown>");
    REQUIRE(cli.get_results()[2]["language"].as<std::string>() == "en");
    REQUIRE(cli.get_results()[3]["language"].as<std::string>() == "<unknown>");
}

TEST_CASE("Identifying Chinese (non-ASCII) text line-by-line", "[get_input_path]") {
  std::ofstream fout{"hello.txt"};
  std::string text{"这是中文的。\n看得懂吗？\n"};
  fout << text;
  fout.close();
  CLD3_cli cli{"hello.txt", "stdout", "line-by-line", 1};
  cli.work();
  fs::remove_all("hello.txt");
  REQUIRE(cli.get_results().size() == 2);
  REQUIRE(cli.get_results()[0]["language"].as<std::string>() == "zh");
  REQUIRE(cli.get_results()[1]["language"].as<std::string>() == "zh");
}

TEST_CASE("Identifying Chinese (non-ASCII) directory whole-text", "[get_input_path]") {
  fs::create_directories("sandbox");
  std::ofstream fout1{"sandbox/hello1.txt"};
  std::ofstream fout2{"sandbox/hello2.txt"};
  std::string text1{"这是中文的。"}, text2{"看得懂吗？"};
  fout1 << text1; fout2 << text2;
  fout1.close(); fout2.close();
  CLD3_cli cli{"sandbox", "stdout", "whole-text", 1};
  cli.work();
  fs::remove_all("sandbox");
  REQUIRE(cli.get_results().size() == 2);
  REQUIRE(cli.get_results()[0]["language"].as<std::string>() == "zh");
  REQUIRE(cli.get_results()[0]["text"].as<std::string>() == "这是中文的。");
  REQUIRE(cli.get_results()[1]["language"].as<std::string>() == "zh");
  REQUIRE(cli.get_results()[1]["text"].as<std::string>() == "看得懂吗？");
}
