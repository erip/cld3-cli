#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../cld3_cli.h"
#include <string>

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
    CLD3_cli cli{"sandbox", "json", 1};
    REQUIRE(cli.get_results().size() == 0);
    fs::remove_all("sandbox");
}
