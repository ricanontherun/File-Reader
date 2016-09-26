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

namespace ricanontherun {

/**
 * Default Construct
 *
 * @param path
 * @return
 */
File::File(const char *path) : __fd(open(path, O_RDONLY)) {}

/**
 * Construct with flags.
 *
 * @param path
 * @param flags
 * @return
 */
File::File(const char *path, int flags) : __fd(open(path, flags)) {}

/**
 * Construct with flags and advice.
 *
 * @param path
 * @param flags
 * @param advice
 * @return
 */
File::File(const char *path, int flags, ACCESS_ADVICE advice) : __fd(open(path, flags)) {
  if (IS_READ(flags)) {
    posix_fadvise(this->__fd, 0, 0, static_cast<int>(advice));
  }
}

File::~File() {
  if (this->Ok()) {
    close(this->__fd);
  }
}

/**
 * Was the file opened without error?
 * @return
 */
bool File::Ok() {
  return this->__fd != static_cast<int>(FILE_STATUS::ERROR);
}

/**
 * Read bytes from the file.
 *
 * @param bytes
 * @return
 */
File::READ_STATUS File::Read(ssize_t bytes) {
  char buf[bytes + 1];

  ssize_t bytes_read = this->ReadIntoBuffer(buf, bytes);

  if (!this->ReadFailed()) {
    // Top off the string at however many bytes were actually read.
    buf[bytes_read] = '\0';

    // Set the internal buffer.
    this->__buf = std::string(buf);
  }

  return this->GetReadStatus();
}

/**
 * Return the bytes that were read in the previous Read() operation.
 * @return
 */
const std::string &File::Get() const {
  return this->__buf;
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

File::READ_STATUS File::GetReadStatus() const {
  return this->__last_read_status;
}

bool File::ReadFailed() const {
  return this->__last_read_status == READ_STATUS::ERROR;
}

} // Namespace ricanontherun
