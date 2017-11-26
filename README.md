# File Reader [![Build Status](https://travis-ci.org/ricanontherun/File-Reader.svg?branch=master)](https://travis-ci.org/ricanontherun/File-Reader)

A file reader with a focus on fast sequential reads. Requires at least C++11 support.


### Opening files
```cpp
File reader;

const char * c_style_string = "file.txt";
File::STATUS open_status = reader.Open(c_style_string);

// -- or

std::string cpp_string = "file.txt";
File::STATUS open_status = reader.Open(cpp_string);

// Error handling.
if (File::StatusOk(open_status)) {
    // Use reader
} else if (File::StatusError(open_status)) {

    // Extract some more information from the return status, if you wish.
    if (File::StatusAccessError(open_status)) {
        // You don't have access to this file.
    } else if (File::StatusTypeError(open_status)) {
        // A non-regular file was provided.
    }
}
```

### Options
```cpp
// Assuming an opened file reader.

// By default, the reader will use the optimum IO blocksize for reads, as determined
// by a call to stat().

// Set the read size to 1000 bytes.
reader.SetReadSize(1000);
```

### Read a chunk
```cpp
std::string buffer;

// read a chunk from the file.
File::Reader::READ_STATUS read_status = reader.Read(buffer);

// Error handling
if (reader.StatusError(read_status)) {
    // Handle the error.
} else {
    if (reader.StatusEndOfFile(status)) {
        // We've hit EOF
    }
    // Use buffer...
}
```

### Read the entire file into memory
```cpp
std::string buffer;
File::Reader::READ_STATUS read_status = reader.ReadAll(buffer);
```

### Read the entire file in chunks
```cpp

// Optionally set the read/chunk size
reader.SetReadSize(1000);

// Will return on error or eof.
Reader::READ_STATUS r_status = reader.Read([](std::string & chunk) {
    // Do something with chunk
});

if (reader.StatusEndOfFile(status)) {
    // Expected.
} else if (reader.StatusError(status)) {
    // ruh roh
}
```