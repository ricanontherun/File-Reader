#define CATCH_CONFIG_MAIN

#include "test_header.h"
#include <string>
#include "../file.h"

TEST_CASE("Test the file interface") {
SECTION("Test that it can read a file from a c-style string") {
const char *file_path = "../data/file";
ricanontherun::File f(file_path);

REQUIRE(f
.
Ok()
);
}

SECTION("Test that it can read a file from a c++ string path") {
std::string file_path = "../data/file";
ricanontherun::File f(file_path);

REQUIRE(f
.
Ok()
);
}

SECTION("Test that it fails when given a bad file path") {
std::string file_path = "../data/i_dont_exist";
ricanontherun::File f(file_path);

REQUIRE_FALSE(f
.
Ok()
);
}

SECTION("Test that it reads chunks of the most efficient size by default") {
const char *file_path = "../data/file";
ricanontherun::File f(file_path);

REQUIRE(f
.
Ok()
);

struct stat fs;
stat(file_path, &fs
);
blksize_t optimum_blocksize = fs.st_blksize;

// Verify the read success.
ricanontherun::File::READ_STATUS status = f.Read();
REQUIRE(status
== ricanontherun::File::READ_STATUS::OK);

// Verify the length of the data.
std::string data = f.Get();
REQUIRE(data
.
length()
== optimum_blocksize);
}

SECTION("Test that it can read chunks sizes of a specificed type.") {
const char *file_path = "../data/file";
ricanontherun::File f(file_path);

REQUIRE(f
.
Ok()
);

int read_size = 15;

// Verify the read success.
ricanontherun::File::READ_STATUS status = f.Read(read_size);
REQUIRE(status
== ricanontherun::File::READ_STATUS::OK);

std::string data = f.Get();
REQUIRE(data
.
length()
== read_size);
}

SECTION("Test that it returns the appropriate status when the file has been exhausted") {
const char *file_path = "../data/empty";
ricanontherun::File f(file_path);

REQUIRE(f
.
Ok()
);

ricanontherun::File::READ_STATUS status = f.Read(); // Let it read the optimum size.
REQUIRE(status
== ricanontherun::File::READ_STATUS::EXHAUSTED);
}
}