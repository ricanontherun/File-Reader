#include "test_header.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

#include "../file.hpp"

TEST_CASE("Reader::Read(size)", "[reader] [size]") {
    using File::Reader;

    SECTION("Test that it can read chunks sizes of a specificed type.") {
        const char *file_path = "../data/file";

        Reader reader;
        File::STATUS open_status = reader.Open(file_path);

        REQUIRE(File::StatusOk(open_status));

        int read_size = 15;

        // Verify the read success.
        std::string buffer;
        Reader::READ_STATUS status = reader.SetReadSize(read_size).Read(buffer);
        
        REQUIRE(reader.StatusOk(status));
        REQUIRE(buffer.length() == read_size);
    }
}

TEST_CASE("Reader::Read") {
    using File::Reader;

    SECTION("Test that it reads chunks of the most efficient size by default") {
        const char *file_path = "../data/file";
        Reader reader;
        File::STATUS open_status = reader.Open(file_path);
        REQUIRE(File::StatusOk(open_status));

        struct stat fs;
        stat(file_path, &fs);
        blksize_t optimum_blocksize = fs.st_blksize;

        // Read a single chunk, letting it default to the optimum block size.
        std::string buffer;
        Reader::READ_STATUS status = reader.Read(buffer);

        REQUIRE(reader.StatusOk(status));
        REQUIRE(buffer.length() == optimum_blocksize);
    }

    SECTION("Test that it returns the appropriate status when the file has been exhausted") {
        const char *file_path = "../data/empty";
        Reader reader;
        File::STATUS open_status = reader.Open(file_path);
        REQUIRE(File::StatusOk(open_status));

        std::string buffer;
        Reader::READ_STATUS status = reader.Read(buffer);
        REQUIRE(reader.StatusEndOfFile(status));
    }

    SECTION("It can read an entire file") {
        Reader reader;
        File::STATUS status = reader.Open("../data/file");

        REQUIRE(File::StatusOk(status));

        std::string buffer;
        File::Reader::READ_STATUS read_status = reader.ReadAll(buffer);

        REQUIRE(reader.StatusOk(read_status));

        std::ifstream t("../data/file");
        REQUIRE(t.good());
        
        std::stringstream stream_buffer;
        stream_buffer << t.rdbuf();

        REQUIRE(stream_buffer.str() == buffer);
    }

    SECTION("It can perform consecutive reads") {
        std::ifstream stream("../data/file");
        REQUIRE(stream.good());

        std::stringstream expected_stream, actual_stream;

        // Get the entire file contents.
        expected_stream << stream.rdbuf();

        Reader reader;
        File::STATUS status = reader.Open("../data/file");
        REQUIRE(File::StatusOk(status));

        std::string buffer;
        reader.SetReadSize(10);

        // Read the entire file in chunks, streaming into actual_stream.
        Reader::READ_STATUS r_status = reader.Read([&actual_stream](std::string & buffer) {
            actual_stream << buffer;
        });

        REQUIRE(reader.StatusEndOfFile(r_status));

        // They should be identical.
        REQUIRE(actual_stream.str() == expected_stream.str());
    }
}
