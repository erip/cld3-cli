# CLD3-cli

CLD is a language identification model created by Google. Version 3 of CLD can
be found [here](https://github.com/google/cld3).

CLD3-cli is a command-line interface for this model to be used with input files
of corpora containing UTF8-encoded text to be identified.

CLD3-cli is written in C++ using experimental features from C++17.

CLD3-cli relies on [jsoncons](https://github.com/danielaparker/jsoncons) for
JSON serialization. This header-only library's `src/` directory is expected to
be found at `${JSONCONS}` for CLD3-cli to work.

### Installation

It is expected that your C++ compiler supports C++14 features and can use
experimental filesystem features. It was developed using
`gcc (Homebrew gcc 5.3.0) 5.3.0`.

Because CLD3's header convention follows Chromium's `<third_party/<lib_name>/src/src/X.h>`, it is expected that
the files in the [CLD3 `src/` directory](https://github.com/google/cld3/tree/master/src) live in
`${CLD3}/cld_3/third_party/src/src/`, so:

    06:51 $ tree $CLD3/third_party/
    $CLD3/third_party/
    └── cld_3
        └── src
            └── src
                ├── BUILD.gn
                ├── *.cc
                ├── *.h
                ├── *.pb.h
                ├── cld_3.gyp
                ├── *.proto
                ├── script_span/
    ...

It is also assumed that all headers have been generated. This will be outlined later.

It is important that `libprotobuf` is built with the same compiler that will
be used to build CLD3-cli. I assume that the `libprotobuf.dylib` can be found
at `${PROTOBUF_LIBS}` and the headers can be found at `${PROTOBUF}`.

Generate `cld3_full.so`:

    cd ${CLD3}/third_party/cld_3/src/src/script_span
    # Generate C++14 `libcld2_full.so`
    g++ -shared -fPIC -I${CLD3} *.cc -std=c++14 -o libcld2_full.so $LDFLAGS -Wl,-install_name,libcld2_full.so
    cd ${CLD3}/third_party/cld_3/src/src
    # Generate protobuf headers
    protoc --cpp_out=. *.proto
    # Generate C++14 `libcld3_full.so`
    g++ -L${PROTOBUF_LIBS} -Lscript_span -I${PROTOBUF} -shared -fPIC *.cc -std=c++14 -o libcld3_full.so $LDFLAGS -Wl,-install_name,libcld3_full.so -I${CLD3} -lcld2_full -lprotobuf
    # Move libs
    cp script_span/libcld2_full.so /usr/local/lib
    cp libcld3_full.so /usr/local/lib


### Testing

CLD3-cli uses [Catch](https://github.com/philsquared/Catch) for testing. It is
assumed that the path to Catch's `include/` directory can be found at `${CATCH_DIR}`
to successfully run the tests.

To run the tests for CLD3-cli, one can execute `make test` in the root directory.

### Building

To build CLD3-cli, one can simply execute `make` in the root directory.

If CLD3-cli fails to build or any tests fail, please ensure your C++ compiler
supports the aforementioned features before raising an issue.

### Authors

- [Elijah Rippeth](mailto:elijah.rippeth@gmail.com)
