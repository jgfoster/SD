#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

#include <cassert>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

namespace SDLib {

File_CI::File_CI(const char *name, uint8_t mode) {
  _path = name;
  _mode = mode;
  _isOpen = true;
  _pos = 0;
  if (SD_CI.dirExists(name)) {
    _isDir = true;
  } else {
    _isDir = false;
    assert(mode & O_CREAT || SD_CI.fileExists(name));
    pContents = SD_CI.contentsOfNewOrExistingFileWithName(name);
    if (mode & O_WRITE) {
      if (mode & O_APPEND) {
        _pos = pContents->size();
      } else {
        *pContents = "";
      }
    }
  }
}

int File_CI::peek() {
  if (_pos < pContents->size()) {
    return pContents->at(_pos);
  } else {
    return EOF;
  }
}

size_t File_CI::write(const char *buf, size_t size) {
  assert(_mode == FILE_WRITE);
  for (int i = 0; i < size; ++i) {
    *pContents += buf[i];
  }
  _pos += size;
  return size;
}

/* Note for read(): the user of this function needs to append
   their own null character to the end of the char array.
   This function will attempt to insert the number of bytes
   specified by size into the buf. So the user should pass a
   size that is at most one size less than the declared size
   of the buffer so the user can append a null character after
   this function is used.
*/
int File_CI::read(char *buf, size_t size) {
  size = size <= available() ? size : available();
  memcpy(buf, pContents->c_str() + _pos, size);
  _pos += size;
  return size;
}

} // namespace SDLib

#endif
