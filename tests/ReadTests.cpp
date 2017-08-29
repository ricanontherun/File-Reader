#include "test_header.h"
#include <string>
#include "../file.h"

TEST_CASE("File::Read") {
    SECTION("Test that it reads chunks of the most efficient size by default") {
        const char *file_path = "../data/file";
        File f;
        File::STATUS open_status = f.Open(file_path);
        REQUIRE(open_status == File::STATUS::OK);

        struct stat fs;
        stat(file_path, &fs);
        blksize_t optimum_blocksize = fs.st_blksize;

        // Verify the read success.
        File::READ_STATUS status = f.Read();
        REQUIRE(status == File::READ_STATUS::OK);

        // Verify the length of the data.
        std::string data = f.Get();
        REQUIRE(data.length() == optimum_blocksize);
    }

    SECTION("Test that it can read chunks sizes of a specificed type.") {
        const char *file_path = "../data/file";
        File f;
        File::STATUS open_status = f.Open(file_path);
        REQUIRE(open_status == File::STATUS::OK);

        int read_size = 15;

        // Verify the read success.
        File::READ_STATUS status = f.Read(read_size);
        REQUIRE(status == File::READ_STATUS::OK);

        std::string data = f.Get();
        REQUIRE(data.length() == read_size);
    }

    SECTION("Test that it returns the appropriate status when the file has been exhausted") {
        const char *file_path = "../data/empty";
        File f;
        File::STATUS open_status = f.Open(file_path);
        REQUIRE(open_status == File::STATUS::OK);

        File::READ_STATUS status = f.Read(); // Let it read the optimum size.
        REQUIRE(status == File::READ_STATUS::END_OF_FILE);
    }
}
