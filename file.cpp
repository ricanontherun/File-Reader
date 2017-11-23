#include "file.h"
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <iostream>

namespace File {

Reader::Reader() :
    descriptor(0),
    buffer(""),
    advice(ACCESS_ADVICE::SEQUENTIAL),
    mode(O_RDONLY)
{}

Reader::STATUS Reader::Open(const char * path) {
    if (access(path, F_OK) == -1) {
        return Reader::STATUS::NOT_FOUND;
    }

    descriptor = open(path, mode);

    if ( descriptor == -1 ) {
        return Reader::STATUS::ERROR;
    }

    return initialize();
}

Reader::STATUS Reader::Open(const std::string & path) {
    return Open(path.c_str());
}

Reader::STATUS Reader::initialize() {
    int status = fstat(descriptor, &(file_stat));

    if ( status == -1 ) {
        return Reader::STATUS::ERROR;
    }

    // Advise the kernel of how the file will be read.
    posix_fadvise(descriptor, 0, 0, static_cast<int>(advice));

    return Reader::STATUS::OK;
}

Reader::~Reader() {
  if (Ok()) {
    close(descriptor);
  }
}

bool Reader::Ok() const {
  return descriptor > 0;
}

Reader::READ_STATUS Reader::Read(ssize_t bytes) {
    bytes = bytes != 0 ? bytes : file_stat.st_blksize;
    char buf[bytes];
    ssize_t bytes_read = 0;

    READ_STATUS status = ReadIntoBuffer(buf, bytes, &bytes_read);

    if ( status != READ_STATUS::ERROR ) {
        buffer = std::string(buf);
    }

    return status;
}

Reader::READ_STATUS Reader::ReadAll() {
    char buf[file_stat.st_size];
    char *buf_ptr = buf;
    ssize_t bytes_read = 0;

    READ_STATUS status = ReadIntoBuffer(buf, file_stat.st_size, &bytes_read);

    if (status != READ_STATUS::ERROR)  { // EOF
        buffer = std::string(buf);
    }

    return status;
}

const std::string &Reader::Get() const {
  return buffer;
}

Reader& Reader::SetReadAdvice(ACCESS_ADVICE advice) {
    advice = advice;
    
    return *this;
}

Reader& Reader::SetOpenMode(int mode) {
    mode = mode;

    return *this;
}

Reader::READ_STATUS Reader::ReadIntoBuffer(const char * buffer, size_t bytes_to_read, ssize_t * bytes_read) {
    *bytes_read = 0;

    if ( flock(descriptor, LOCK_EX) == -1 ) {
        return READ_STATUS::ERROR;
    }

    lseek(descriptor, 0, SEEK_SET);

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

    if ( flock(descriptor, LOCK_UN) == -1 ) {
        return READ_STATUS::ERROR;
    }

    if ( num_bytes_read == -1 ) {
        return READ_STATUS::ERROR;
    }

    return num_bytes_read == 0 ? READ_STATUS::END_OF_FILE : READ_STATUS::OK;
}

} // End Reader