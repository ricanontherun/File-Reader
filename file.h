#ifndef FILE_READER_H
#define FILE_READER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <functional>

#include "enums.h"

namespace File
{

enum class STATUS : unsigned
{
  OK = 1,
  ERROR = 1 << 1,

  // Specific errors
  COULD_NOT_LOCK = 1 << 2,
  INSUFFICIENT_ACCESS = 1 << 3,
  INVALID_TYPE = 1 << 4
};

bool StatusOk(STATUS status);
bool StatusError(STATUS status);
bool StatusAccessError(STATUS status);
bool StatusTypeError(STATUS status);

class Reader
{
public:
  enum class READ_STATUS : unsigned
  {
    OK = 1,
    ERROR = 1 << 2,

    // Specific errors.
    END_OF_FILE = 1 << 3,
    COULD_NOT_LOCK = 1 << 4
  };

  Reader();
  ~Reader();

  // Read bytes into a buffer.
  READ_STATUS Read(std::string &buffer);

  // Read chunks from the file, using a callback.
  READ_STATUS Read(std::function<void(std::string &)> callback);

  // Read the entire file into the internal buffer, using the optimal block size.
  READ_STATUS ReadAll(std::string &buffer);

  Reader &SetReadSize(size_t size);

  File::STATUS Open(const char *path);
  File::STATUS Open(const std::string &path);

  bool StatusOk(READ_STATUS status);
  bool StatusEndOfFile(READ_STATUS status);
  bool StatusError(READ_STATUS status);
private:
  int descriptor;
  struct stat file_stat;
  size_t read_size;

  File::STATUS initialize();

  // Read bytes_to_read into buffer, returning *bytes_read as the actual byte count.
  READ_STATUS Read(char *buffer, size_t bytes_to_read, ssize_t *bytes_read);
};

} // End File

#endif // FILE_READER_H
