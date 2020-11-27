#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

#include <cassert>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
// #include <vector>

namespace SDLib {

// File_CI::File_CI(File_Base *baseFile) {
//   // this->baseFile = baseFile;
// }

// File_CI::File_CI(SdFile f, const char *name) : File_Base(f, name) {}

// File_CI::File_CI(void) : File_Base() {}

File_CI::File_CI(const char *name, uint8_t mode = FILE_READ) {
  _mode = mode;
  strcpy(_path, name);

  // prepend base path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, name);

  // does path exist?
  if (fs::exists(newPath)) {
    // if so, is it a directory?
    if (fs::is_directory(newPath)) {
      // if so, return a new object with the _isDirectory flag set
      _isDirectory = true;
    } else {
      // path is an existing file
      _isDirectory = false;
      // if file is read, open for read
      if (_mode == FILE_READ) {
        finOut = new std::fstream;
        finOut->open(newPath, std::fstream::binary | std::fstream::in);
      } else if (_mode == FILE_WRITE) {
        // else if mode is FILE_WRITE open for read and write
        finOut = new std::fstream;
        finOut->open(newPath, std::fstream::binary | std::fstream::in |
                                  std::fstream::out | std::fstream::app);
      } else {
        // else invalid mode, assert false
        assert(false);
      }
      _open = true;
    }
  } else {
    // path does not exist
    _isDirectory = false;
    // if mode is write, open new file
    if (_mode == FILE_WRITE) {
      finOut = new std::fstream;
      finOut->open(newPath, std::fstream::binary | std::fstream::in |
                                std::fstream::out | std::fstream::app);
      _open = true;
    } else if (_mode == FILE_READ) {
      // else assert false for file doesn't exist
      assert(false);
    } else {
      // else invalid mode. assert false
      assert(false);
    }
  }
}

File_CI::~File_CI() {
  if (!_isDirectory) {
    delete finOut;
  }
}

// size_t File_CI::write(uint8_t value) {
//   // TODO
//   // return File_Base::write(value);
// }

size_t File_CI::write(const char *buf, size_t size) {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, _path);

  assert(_mode == FILE_WRITE);
  finOut->write(buf, size);
  return size;
}

int File_CI::availableForWrite() {
  // TODO
  // return File_Base::availableForWrite();
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
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, _path);

  int remaining = fs::file_size(newPath) - finOut->tellg();
  // if size is greater than remaining, use remaining for read
  if (size > remaining) {
    finOut->read(buf, remaining);
  } else {
    // else use size
    finOut->read(buf, size);
  }
  return 0;
}

int File_CI::peek() {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  char byte[2];
  // save old pointer position
  fpos_t oldPos = finOut->tellg();
  // read in first char
  finOut->read(byte, 1);
  byte[1] = '\0';
  // put file pointer back
  finOut->seekg(oldPos);

  // if file pointer isn't back, assert false
  if (finOut->tellg() != oldPos) {
    assert(false);
  }
  // return first char in buffer
  return byte[0];
}

int File_CI::available() {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, _path);

  return fs::file_size(newPath);
}

void File_CI::flush() {
  // return File_Base::flush();
}

bool File_CI::seek(uint32_t pos) {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, _path);

  // if pos is greater than size - 1, return false
  if (pos > fs::file_size(newPath) - 1) {
    // trying to seek outside of file scope
    return false;
  } else {
    finOut->seekg(pos);
  }
  // check for success
  if (finOut->tellg() == pos) {
    return true;
  }
  return false;
}

uint32_t File_CI::position() {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  return finOut->tellg();
}

uint32_t File_CI::size() {
  // halt if file is directory
  if (_isDirectory) {
    assert(false);
  }

  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, _path);
  return fs::file_size(newPath);
}

void File_CI::close() {
  // close if not directory
  if (!_isDirectory) {
    finOut->close();
  }
  _open = false;
}

File_CI File_CI::openNextFile(uint8_t mode = FILE_READ) {
  // TODO
}

char *File_CI::name() { return _path; }

File_CI::operator bool() { return File_Base::operator bool(); }

bool File_CI::isDirectory(void) { return _isDirectory; }

} // namespace SDLib

#endif
