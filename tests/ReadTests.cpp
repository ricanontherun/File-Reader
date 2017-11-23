#include "test_header.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../file.h"

TEST_CASE("Reader::Read(size)", "[reader] [size]") {
    using File::Reader;

    SECTION("Test that it can read chunks sizes of a specificed type.") {
        const char *file_path = "../data/file";

        Reader f;
        File::STATUS open_status = f.Open(file_path);
        
        REQUIRE(open_status == File::STATUS::OK);

        int read_size = 15;

        // Verify the read success.
        Reader::READ_STATUS status = f.Read(read_size);
        
        REQUIRE(status == Reader::READ_STATUS::OK);
        REQUIRE(f.Get().length() == read_size);
    }
}

TEST_CASE("Reader::Read", "[heck]") {
    using File::Reader;

    SECTION("Test that it reads chunks of the most efficient size by default") {
        const char *file_path = "../data/file";
        Reader f;
        File::STATUS open_status = f.Open(file_path);
        REQUIRE(open_status == File::STATUS::OK);

        struct stat fs;
        stat(file_path, &fs);
        blksize_t optimum_blocksize = fs.st_blksize;

        // Read a single chunk, letting it default to the optimum block size.
        Reader::READ_STATUS status = f.Read();

        REQUIRE(status == Reader::READ_STATUS::OK);

        // Verify the length of the buffer is equal to the optimum block size.
        REQUIRE(f.Get().length() == optimum_blocksize);
    }

    SECTION("Test that it returns the appropriate status when the file has been exhausted") {
        const char *file_path = "../data/empty";
        Reader f;
        File::STATUS open_status = f.Open(file_path);
        REQUIRE(open_status == File::STATUS::OK);

        Reader::READ_STATUS status = f.Read(); // Let it read the optimum size.
        REQUIRE(status == Reader::READ_STATUS::END_OF_FILE);
    }

    SECTION("It can read an entire file") {
        Reader f;
        f.Open("../data/file");

        REQUIRE(f.Ok());

        f.ReadAll();

        std::ifstream t("../data/file");
        std::stringstream buffer;
        buffer << t.rdbuf();

        REQUIRE(buffer.str() == f.Get());
        REQUIRE(buffer.str().length() == f.Get().length());
    }
}
