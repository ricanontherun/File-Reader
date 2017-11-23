#ifndef  FILE_READER_H
#define  FILE_READER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace File {

enum class STATUS : unsigned {
  OK = 1,
  ERROR = 1 << 1,

  // Specific errors
  NOT_FOUND = 1 << 2,
  LOCK_ERROR = 1 << 3,
  INSUFFICIENT_ACCESS = 1 << 4
};

STATUS operator &(STATUS lhs, STATUS rhs);
STATUS operator ^(STATUS lhs, STATUS rhs);
STATUS operator ~(STATUS rhs);
STATUS& operator |=(STATUS &lhs, STATUS rhs);
STATUS& operator &=(STATUS &lhs, STATUS rhs);
STATUS& operator ^=(STATUS &lhs, STATUS rhs);

class Reader {
public:
  enum class READ_STATUS {
    OK = 0,
    ERROR,
    END_OF_FILE
  };

  // http://man7.org/linux/man-pages/man2/posix_fadvise.2.html
  enum class ACCESS_ADVICE {
    NORMAL = POSIX_FADV_NORMAL,
    SEQUENTIAL = POSIX_FADV_SEQUENTIAL,
    RANDOM = POSIX_FADV_RANDOM,
    NOREUSE = POSIX_FADV_NOREUSE,
    WILLNEED = POSIX_FADV_WILLNEED,
    DONTNEED = POSIX_FADV_DONTNEED
  };

  Reader();
  ~Reader();

  /**
   * Was the file opened without error?
   */
  bool Ok() const;

  /**
   * Read bytes from the file. When the bytes argument is omitted, the optimal block size will be used.
   */
  READ_STATUS Read(ssize_t bytes = 0);

  // Read the entire file into the internal buffer, using the optimal block size.
  READ_STATUS ReadAll();

  /**
   * Return the bytes that were read in the previous Read() operation.
   */
  const std::string &Get() const;

  /**
   * Advise the kernel how you intend to read from this file. See ACCESS_ADVICE.
   */
  Reader& SetReadAdvice(ACCESS_ADVICE advice);
  Reader& SetOpenMode(int mode);

  File::STATUS Open(const char * path);
  File::STATUS Open(const std::string & path);

  // Convenience methods for checking return status.
  bool StatusOk(File::STATUS status);
  bool StatusError(File::STATUS status);
  bool StatusInsufficientAccess(File::STATUS status);
private:
    int descriptor;
    ACCESS_ADVICE advice;
    int mode;
    std::string buffer;
    struct stat file_stat;

    File::STATUS initialize();

    // Read bytes_to_read into buffer, returning *bytes_read as the actual byte count.
    READ_STATUS ReadIntoBuffer(char * buffer, size_t bytes_to_read, ssize_t * bytes_read);
};

} // End File

#endif // FILE_READER_H
