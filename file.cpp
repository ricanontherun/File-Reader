#include "file.hpp"

#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <iostream>
#include <memory>

namespace File {

bool StatusOk(STATUS status) {
    return (status & File::STATUS::OK) == File::STATUS::OK;
}
bool StatusError(STATUS status) {
    return (status & File::STATUS::ERROR) == File::STATUS::ERROR;
}

bool StatusAccessError(STATUS status) {
    return (status & File::STATUS::INSUFFICIENT_ACCESS) == File::STATUS::INSUFFICIENT_ACCESS;
}

bool StatusTypeError(STATUS status) {
    return (status & File::STATUS::INVALID_TYPE) == File::STATUS::INVALID_TYPE;
}

Reader::Reader() :
    descriptor(0),
    read_size(0)
{}

File::STATUS Reader::Open(const char * path) {
    if (access(path, F_OK) == -1) {
        return File::STATUS::ERROR | File::STATUS::INSUFFICIENT_ACCESS;
    }

    if ( stat(path, &(file_stat)) == -1 ) {
        return File::STATUS::ERROR;
    }

    // Make sure this is a regular file. For now, that's all that is supported.
    if (!S_ISREG(file_stat.st_mode)) {
        return File::STATUS::ERROR | File::STATUS::INVALID_TYPE;
    }

    if ( (descriptor = open(path, O_RDONLY)) == -1 ) {
        return File::STATUS::ERROR;
    }

    // Set the default read size to the optimum IO blocksize.
    read_size = file_stat.st_blksize;

    // Advise the kernel that we intend to perform sequential reads.
    posix_fadvise(descriptor, 0, 0, POSIX_FADV_SEQUENTIAL);

    return File::STATUS::OK;
}

File::STATUS Reader::Open(const std::string & path) {
    return Open(path.c_str());
}

Reader& Reader::SetReadSize(size_t size) {
    read_size = size;

    return *this;
}

Reader::~Reader() {
  if (descriptor != 0) {
    if (close(descriptor) == -1) {
        std::cerr << "Failed to close file\n";
    }
  }
}

Reader::READ_STATUS Reader::Read(std::string & buffer) {
    char *buf = new (std::nothrow) char[read_size + 1];
    auto mananged_buffer = std::unique_ptr<char[]>(buf);

    ssize_t bytes_read = 0;

    READ_STATUS status = Read(buf, read_size, &bytes_read);

    if ( status != READ_STATUS::ERROR ) {
        buffer.clear();

        // Copy buf into buffer.
        buffer.assign(buf, bytes_read);
    }

    return status;
}

Reader::READ_STATUS Reader::Read(std::function<void(std::string & buffer)> callback) {
    std::string buf;

    READ_STATUS status;

    while (StatusOk(status = Read(buf))) {
        callback(buf);

        // Technically an EOF return status is "ok", so we check and break as needed.
        if (StatusEndOfFile(status)) {
            break;
        }
    }

    return status;
}

Reader::READ_STATUS Reader::ReadAll(std::string & buffer) {
    read_size = file_stat.st_size;

    return Read(buffer);
}

Reader::READ_STATUS Reader::Read(char * buffer, size_t bytes_to_read, ssize_t * bytes_read) {
    *bytes_read = 0;

    if ( flock(descriptor, LOCK_EX | LOCK_NB) == -1 ) {
        return READ_STATUS::ERROR;
    }

    ssize_t num_bytes_read = 0;

    do {
        num_bytes_read = read(descriptor, (void *) buffer, file_stat.st_blksize < bytes_to_read ? file_stat.st_blksize : bytes_to_read);

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
            ret |= READ_STATUS::END_OF_FILE;
            break;
    }

    return ret;
}

bool Reader::StatusOk(READ_STATUS status) {
    return (status & READ_STATUS::OK) == READ_STATUS::OK;
}

bool Reader::StatusError(READ_STATUS status) {
    return (status & READ_STATUS::ERROR) == READ_STATUS::ERROR;
}

bool Reader::StatusEndOfFile(READ_STATUS status) {
    return (status & READ_STATUS::END_OF_FILE) == READ_STATUS::END_OF_FILE;
}

} // End File