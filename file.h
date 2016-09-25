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

#ifndef  RICANONTHERUN_FILE_H
#define  RICANONTHERUN_FILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace ricanontherun {

#define IS_READ(flags) ( ((flags) == O_RDONLY) || ((flags) & O_RDWR) )

class File {
public:
  // Possible return statuses for a read operation.
  // These generally line up with POSIX interface return types.
  enum class READ_STATUS {
    ERROR = -1,
    EXHAUSTED = 0, // No more bytes to read.
    OK = 1
  };

  enum class FILE_STATUS {
    ERROR = -1,
    OK = 0
  };

  enum class ACCESS_ADVICE {
    SEQUENTIAL = POSIX_FADV_SEQUENTIAL,
    RANDOM = POSIX_FADV_RANDOM,
    NOREUSE = POSIX_FADV_NOREUSE,
    WILLNEED = POSIX_FADV_WILLNEED,
    DONTNEED = POSIX_FADV_DONTNEED
  };

  File();
  File(const char *path);
  File(const char *path, int flags);
  File(const char *path, int flags, ACCESS_ADVICE advice);

  ~File();

  bool Ok();
  READ_STATUS Read(ssize_t bytes = BUFSIZ);
  const std::string & Get() const;

private:
  int __fd;
  std::string __buf;
  READ_STATUS __last_read_status;

  ssize_t ReadIntoBuffer(char *buf, ssize_t bytes);
  bool ReadFailed() const;

  void SetReadStatus(READ_STATUS);
  READ_STATUS GetReadStatus() const;
};

} // Namespace File

#endif // RICANONTHERUN_FILE_H
