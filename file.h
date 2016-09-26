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
#include <memory>

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
    ERROR = -1
  };

  // http://man7.org/linux/man-pages/man2/posix_fadvise.2.html
  enum class ACCESS_ADVICE {
    SEQUENTIAL = POSIX_FADV_SEQUENTIAL,
    RANDOM = POSIX_FADV_RANDOM,
    NOREUSE = POSIX_FADV_NOREUSE,
    WILLNEED = POSIX_FADV_WILLNEED,
    DONTNEED = POSIX_FADV_DONTNEED
  };

  /**
   * Default Construct
   *
   * @param path
   * @return
   */
  File(const char *path);

  /**
   * Construct with flags.
   *
   * @param path
   * @param flags
   * @return
   */
  File(const char *path, int flags);

  /**
   * Construct with flags and advice.
   *
   * @param path
   * @param flags
   * @param advice
   * @return
   */
  File(const char *path, int flags, ACCESS_ADVICE advice);

  ~File();

  /**
   * Was the file opened without error?
   * @return
   */
  bool Ok() const;

  /**
   * Read bytes from the file. When the bytes argument is omitted, the 'optimal'
   * block size is used from a call to fstat.
   *
   * @param bytes
   * @return
   */
  READ_STATUS Read(ssize_t bytes = 0);

  /**
   * Return the bytes that were read in the previous Read() operation.
   * @return
   */
  const std::string & Get() const;

  /**
   * Get the I/O blocksize.
   *
   * @return
   */
  blksize_t BlockSize() const;

private:
  int __fd;
  FILE_STATUS __fstatus;
  std::string __buf;
  READ_STATUS __last_read_status;
  struct stat __fs;

  void init();

  ssize_t ReadIntoBuffer(char *buf, ssize_t bytes);
  bool ReadFailed() const;

  void SetReadStatus(READ_STATUS);
  READ_STATUS GetReadStatus() const;
};

} // Namespace File

#endif // RICANONTHERUN_FILE_H
