/*

Copyright (C) 2016 Christian Roman

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "file.h"
#include <iostream>
#include <string.h>
#include <errno.h>

namespace ricanontherun {

bool File::debug = false;

/**
 * C style string constructor
 *
 * @param path
 * @param advice
 * @return
 */
File::File(const char *path, ACCESS_ADVICE advice) : __file_path(path), __fd(open(path, O_RDONLY)) {
  this->init();
  this->TakeAdvice(advice);
}

/**
 * C++ style string constructor
 *
 * @param path
 * @param advice
 * @return
 */
File::File(const std::string & path, ACCESS_ADVICE advice) : File(path.c_str(), advice) {}

File::~File() {
  if (this->Ok()) {
    close(this->__fd);
  }
}

void File::TakeAdvice(ACCESS_ADVICE advice) const {
  if (advice != ACCESS_ADVICE::NORMAL) {
    posix_fadvise(this->__fd, 0, 0, static_cast<int>(advice));
  }
}

/**
 * Was the file opened without error?
 * @return
 */
bool File::Ok() const {
  return this->__fd != static_cast<int>(FILE_STATUS::ERROR);
}

/**
 * Read the underlying file's stat info.
 *
 * @return  Did the operation succeed?
 */
bool File::ReadFileInfo() {
  return fstat(this->__fd, &(this->__fs)) != -1;
}

/**
 * Read bytes from the file
 *
 * @param bytes Optional, leave blank for st_blksize bytes.
 * @return
 */
File::READ_STATUS File::Read(ssize_t bytes) {
  bytes = bytes != 0 ? bytes : this->BlockSize();

  char buf[bytes + 1];

  ssize_t bytes_read = this->ReadIntoBuffer(buf, bytes);

  if (this->ReadOk()) {
    // Top off the string at however many bytes were actually read.
    buf[bytes_read] = '\0';

    // Set the internal buffer.
    this->__buf = std::string(buf);
  }

  return this->GetReadStatus();
}

/**
 * Return the bytes that were read in the previous Read() operation.
 *
 * @return
 */
const std::string &File::Get() const {
  return this->__buf;
}

/**
 * Get the optimum file block size.
 *
 * @return
 */
off_t File::BlockSize() const {
  return this->Ok() ? this->__fs.st_blksize : 0;
}

void File::init() {
  if (!this->Ok() || !this->ReadFileInfo()) {
    this->__fstatus = FILE_STATUS::ERROR;

    if (File::debug) {
      std::cerr << this->__file_path << ": " << strerror(errno) << "\n";
    }
  }
}

ssize_t File::ReadIntoBuffer(char *buf, ssize_t bytes) {
  ssize_t bytes_read = read(this->__fd, buf, bytes);

  // Set the read status.
  switch (bytes_read) {
  case -1:this->__last_read_status = READ_STATUS::ERROR;
    break;
  case 0:this->__last_read_status = READ_STATUS::EXHAUSTED;
    break;
  default:this->__last_read_status = READ_STATUS::OK;
    break;
  }

  return bytes_read;
}

void File::SetReadStatus(READ_STATUS status) {
  this->__last_read_status = status;
}

/**
 * Get the status from the last call to Read()
 *
 * @return
 */
File::READ_STATUS File::GetReadStatus() const {
  return this->__last_read_status;
}

/**
 * Did the last call to Read() succeed?
 *
 * @return
 */
bool File::ReadOk() const {
  return this->__last_read_status == READ_STATUS::OK;
}

void File::SetDebug(bool debug) {
  File::debug = debug;
}

} // Namespace ricanontherun
