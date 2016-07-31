# CLD3-cli

CLD is a language identification model created by Google. Version 3 of CLD can
be found [here](https://github.com/google/cld3).

CLD3-cli is a command-line interface for this model to be used with input files
of corpora containing UTF8-encoded text to be identified.

CLD3-cli is written in C++ using experimental features from C++17.

### Installation

It is expected that your C++ compiler supports C++14 features and can use
experimental filesystem features. It was developed using
`gcc (Homebrew gcc 5.3.0) 5.3.0`.

### CLD3 assumptions

It is assumed that the path to `third_party/` can be found at `${THIRD_PARTY}`,
and it is also assumed that all generated headers (from Google Protocol Buffers)
have already been generated at compile-time of the CLI.

### Testing

CLD3-cli uses [Catch](https://github.com/philsquared/Catch) for testing. It is
assumed that the path to Catch's `include/` directory can be found at `${CATCH_DIR}`
to successfully run the tests.

To run the tests for CLD3-cli, one can execute `make test` in the root directory.

### Building

To build CLD3-cli, one can simply execute `make` in the root directory.

If CLD3-cli fails to build or any tests fail, please ensure your C++ compiler
supports the aforementioned features before raising an issue.
