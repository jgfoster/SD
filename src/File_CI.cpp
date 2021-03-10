#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

#include <cassert>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

namespace SDLib {

// create a directory reference
File_CI::File_CI(const std::string &path) {
  assert(path.size());
  assert(path.back() == '/');
  assert(SD_CI.exists(path));
  _isOpen = true;
  _path = path;
  _current = path;
}

// create a file reference
File_CI::File_CI(const std::string &path, uint8_t mode) {
  assert(path.size());
  assert(path.back() != '/');
  assert(SD_CI.exists(path));
  _isOpen = true;
  _mode = mode;
  _path = path;
}

// find the file name in the full path
const char *File_CI::name() const {
  size_t index;
  if (isDirectory()) {
    index = _path.find_last_of('/', _path.size() - 2);
  } else {
    index = _path.find_last_of('/');
  }
  if (index == std::string::npos) {
    index = -1;
  }
  return _path.c_str() + index + 1;
}

// get next char without advancing position
int File_CI::peek() const {
  assert(!isDirectory());
  std::string *pContents = SD_CI.contentsOf(_path);
  assert(pContents);
  if (_pos < pContents->size()) {
    return pContents->at(_pos);
  }
  return EOF;
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
  assert(!isDirectory());
  assert(_mode & O_READ);
  std::string *pContents = SD_CI.contentsOf(_path);
  size = size <= available() ? size : available();
  memcpy(buf, pContents->c_str() + _pos, size);
  _pos += size;
  return size;
}

// set position for next read
bool File_CI::seek(uint32_t pos) {
  uint32_t sz = size();
  if (pos < sz) {
    _pos = pos;
    return true;
  }
  _pos = sz;
  return false;
}

uint32_t File_CI::size() const {
  assert(!isDirectory());
  return SD_CI.contentsOf(_path)->size();
}

size_t File_CI::write(const char *buf, size_t size) {
  assert(!isDirectory());
  assert(_mode & O_WRITE);
  std::string *pContents = SD_CI.contentsOf(_path);
  for (int i = 0; i < size; ++i) {
    *pContents += buf[i];
  }
  _pos += size;
  return size;
}

File_CI File_CI::openNextFile(uint8_t mode) {
  assert(isDirectory());
  const std::string &name = SD_CI.nameAfter(_current, _path);
  if (name.empty()) {
    _current = _path;
    return File_CI();
  }
  _current = name;
  if (name.back() == '/') {
    return File_CI(name);
  }
  return File_CI(name, mode);
}

} // namespace SDLib

#endif
