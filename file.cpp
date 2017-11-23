#include "file.h"
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <iostream>

namespace File {

STATUS operator &(STATUS lhs, STATUS rhs)  
{
    return static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) &
        static_cast<std::underlying_type<STATUS>::type>(rhs)
    );
}

STATUS operator |(STATUS lhs, STATUS rhs)  
{
    return static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) |
        static_cast<std::underlying_type<STATUS>::type>(rhs)
    );
}

STATUS operator ^(STATUS lhs, STATUS rhs)  
{
    return static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) ^
        static_cast<std::underlying_type<STATUS>::type>(rhs)
    );
}

STATUS operator ~(STATUS rhs)  
{
    return static_cast<STATUS> (
        ~static_cast<std::underlying_type<STATUS>::type>(rhs)
    );
}

STATUS& operator |=(STATUS &lhs, STATUS rhs)  
{
    lhs = static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) |
        static_cast<std::underlying_type<STATUS>::type>(rhs)           
    );

    return lhs;
}

STATUS& operator &=(STATUS &lhs, STATUS rhs)  
{
    lhs = static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) &
        static_cast<std::underlying_type<STATUS>::type>(rhs)           
    );

    return lhs;
}

STATUS& operator ^=(STATUS &lhs, STATUS rhs)  
{
    lhs = static_cast<STATUS> (
        static_cast<std::underlying_type<STATUS>::type>(lhs) ^
        static_cast<std::underlying_type<STATUS>::type>(rhs)           
    );

    return lhs;
}

Reader::Reader() :
    descriptor(0),
    buffer(""),
    advice(ACCESS_ADVICE::SEQUENTIAL)
{}

File::STATUS Reader::Open(const char * path) {
    if (access(path, F_OK) == -1) {
        return File::STATUS::ERROR | File::STATUS::INSUFFICIENT_ACCESS;
    }

    descriptor = open(path, O_RDONLY);

    if ( descriptor == -1 ) {
        // What kinds of error causes can we extract from -1? errno?
        return File::STATUS::ERROR;
    }

    return initialize();
}

File::STATUS Reader::Open(const std::string & path) {
    return Open(path.c_str());
}

File::STATUS Reader::initialize() {
    int status = fstat(descriptor, &(file_stat));

    if ( status == -1 ) {
        return File::STATUS::ERROR;
    }

    // Advise the kernel of how the file will be read.
    posix_fadvise(descriptor, 0, 0, static_cast<int>(advice));

    return File::STATUS::OK;
}

Reader::~Reader() {
  if (Ok()) {
    if (close(descriptor) == -1) {
        std::cerr << "Failed to close file\n";
    }
  }
}

bool Reader::Ok() const {
    return descriptor > 0;
}

Reader::READ_STATUS Reader::Read(ssize_t bytes) {
    bytes = bytes != 0 ? bytes : file_stat.st_blksize;

    char buf[bytes + 1];

    ssize_t bytes_read = 0;
    READ_STATUS status = ReadIntoBuffer(buf, bytes, &bytes_read);
    buf[bytes_read] = '\0';

    if ( status != READ_STATUS::ERROR ) {
        buffer = std::string(buf);
    }

    return status;
}

Reader::READ_STATUS Reader::ReadAll() {
    char buf[file_stat.st_size + 1];
    char *buf_ptr = buf;

    ssize_t bytes_read = 0;
    READ_STATUS status = ReadIntoBuffer(buf, file_stat.st_size, &bytes_read);
    buf[bytes_read] = '\0';

    if (status != READ_STATUS::ERROR)  { // EOF
        buffer = std::string(buf);
    }

    return status;
}

const std::string &Reader::Get() const {
  return buffer;
}

Reader& Reader::SetReadAdvice(ACCESS_ADVICE advice) {
    this->advice = advice;
    
    return *this;
}

Reader& Reader::SetOpenMode(int mode) {
    this->mode = mode;

    return *this;
}

Reader::READ_STATUS Reader::ReadIntoBuffer(char * buffer, size_t bytes_to_read, ssize_t * bytes_read) {
    *bytes_read = 0;

    if ( flock(descriptor, LOCK_EX | LOCK_NB) == -1 ) {
        return READ_STATUS::ERROR;
    }

    ssize_t num_bytes_read = 0;

    do {
        ssize_t read_size = file_stat.st_blksize < bytes_to_read ? file_stat.st_blksize : bytes_to_read;

        num_bytes_read = read(descriptor, (void *) buffer, read_size);

        if (num_bytes_read <= 0) {
            break;
        }

        // Add the output variable.
        *bytes_read += num_bytes_read;

        // Move the buffer forward for the next read.
        buffer += num_bytes_read;

        // Deduct what we've just read from the target byte count.
        bytes_to_read -= num_bytes_read;
    } while (bytes_to_read > 0);

    if ( flock(descriptor, LOCK_UN | LOCK_NB) == -1 ) {
        return READ_STATUS::ERROR;
    }

    READ_STATUS ret = READ_STATUS::OK;

    switch (num_bytes_read) {
        case -1:
            ret = READ_STATUS::ERROR;
            break;
        case 0:
            ret = READ_STATUS::END_OF_FILE;
            break;
    }

    return ret;
}

// TODO: This can get verbose...is there a way to clean this up?

bool Reader::StatusOk(File::STATUS status) {
    return (status & File::STATUS::OK) == File::STATUS::OK;
}

bool Reader::StatusError(File::STATUS status) {
    return (status & File::STATUS::ERROR) == File::STATUS::ERROR;
}

bool Reader::StatusInsufficientAccess(File::STATUS status) {
    return (status & File::STATUS::INSUFFICIENT_ACCESS) == File::STATUS::INSUFFICIENT_ACCESS;
}

} // End File