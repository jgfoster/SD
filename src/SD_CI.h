#pragma once
#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD.h"
#include <cassert>
#include <map>
#include <string>

namespace SDLib {

// we add a new "mode" so `find()` can remove files and directories
uint8_t const O_REMOVE = 0X80;

class File_CI : public File_Base {
public:
  File_CI() {}                                    // not a file (for iteration)
  File_CI(const std::string &path);               // create a directory
  File_CI(const std::string &path, uint8_t mode); // create a file

  // file operations
  int available() const { return size() - _pos; }
  int availableForWrite() const { return 4096; }
  void flush() {}
  int peek() const;
  uint32_t position() const { return _pos; }
  int read(char *buf, size_t size);
  bool seek(uint32_t pos);
  uint32_t size() const;
  size_t write(const char *buf, size_t size);
  using Print::write;

  // directory operations
  bool isDirectory(void) const { return _path.back() == '/'; }
  File_CI openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory() { _current = _path; }

  // common operations
  void close() { _isOpen = false; }
  bool isOpen() const { return _isOpen; }
  const char *name() const;
  operator bool() const { return _isOpen; }

  // testing functions
  int getWriteError() const { return _writeError; }
  void setWriteError(int value = 1) { _writeError = value; }
  void clearWriteError() { _writeError = 0; }

private:
  std::string _path; // full path
  int _pos = 0;      // index of _next_ char to read or write
  uint8_t _mode = 0;
  bool _isOpen = false;
  int _writeError = 0;
  std::string _current; // used by directory iterator
};

class SDClass_CI : public SDClass_Base {
public:
  // hardware-related setup can be ignored for test double
  bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN) { return true; }
  bool begin(uint32_t clock, uint8_t csPin) { return true; }
  void end() {}

  // return a pointer rather than a reference so we can use nullptr for missing
  std::string *contentsOf(const std::string &path);

  bool exists(const std::string &path);
  bool exists(const char *path) { return exists(std::string(path)); }
  bool exists(const String &path) { return exists(std::string(path.c_str())); }

  bool mkdir(const std::string &path);
  bool mkdir(const char *path) { return mkdir(std::string(path)); }
  bool mkdir(const String &path) { return mkdir(path.c_str()); }

  const std::string &nameAfter(const std::string current,
                               const std::string start);

  File_CI open(const std::string &path, uint8_t mode = FILE_READ);
  File_CI open(const char *path, uint8_t mode) {
    return open(std::string(path), mode);
  }
  File_CI open(const String &path, uint8_t mode) {
    return open(std::string(path.c_str()), mode);
  }

  bool remove(const std::string &path);
  bool remove(const char *path) { return remove(std::string(path)); }
  bool remove(const String &path) { return remove(std::string(path.c_str())); }

  void removeAll() { fileSystem.clear(); }

  bool rmdir(const std::string &path);
  bool rmdir(const char *path) { return rmdir(std::string(path)); }
  bool rmdir(const String &path) { return rmdir(path.c_str()); }

  String className() const { return "SDClass_Base"; }

private:
  static std::map<std::string, std::string> fileSystem;
  std::string emptyString;
  std::string normalizePath(const std::string &inPath,
                            bool isDirectory = false);
};

extern SDClass_CI SD_CI;

} // namespace SDLib

#endif
