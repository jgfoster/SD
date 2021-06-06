#pragma once
#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD.h"
#include <cassert>
#include <map>

/**
 * The mock SD has an internal map
 *    key = normalized full path
 *      * path always begins with a forward slash ('/')
 *      * trailing forward slash ('/') indicates a directory
 *    value = contents or name
 *      * binary or text data for a file
 *      * simple name for a directory (easier for name lookup)
 */
namespace SDLib {

// we add a new "mode" so `find()` can remove files and directories
uint8_t const O_REMOVE = 0X80;

class File_CI : public File_Base {
public:
  File_CI() {}                               // not a file (for iteration)
  File_CI(const String &path);               // create a directory
  File_CI(const String &path, uint8_t mode); // create a file

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
  String _path; // full path
  int _pos = 0; // index of _next_ char to read or write
  uint8_t _mode = 0;
  bool _isOpen = false;
  int _writeError = 0;
  String _current; // used by directory iterator
};

class SDClass_CI : public SDClass_Base {
public:
  // hardware-related setup can be ignored for test double
  bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN) { return true; }
  bool begin(uint32_t clock, uint8_t csPin) { return true; }
  void end() {}

  // return a pointer rather than a reference so we can use nullptr for missing
  String *contentsOf(const String &path);

  bool exists(const String &path);
  bool exists(const char *path) { return exists(String(path)); }

  bool mkdir(const String &path);
  bool mkdir(const char *path) { return mkdir(String(path)); }

  const String &nameAfter(const String current, const String start);

  File_CI open(const String &path, uint8_t mode = FILE_READ);
  File_CI open(const char *path, uint8_t mode) {
    return open(String(path), mode);
  }

  bool remove(const String &path);
  bool remove(const char *path) { return remove(String(path)); }

  void removeAll() { fileSystem.clear(); }

  bool rmdir(const String &path);
  bool rmdir(const char *path) { return rmdir(String(path)); }

  String className() const { return "SDClass_CI"; }

private:
  static std::map<String, String> fileSystem;
  String emptyString;
  String normalizePath(const String &inPath, bool isDirectory = false);
};

extern SDClass_CI SD_CI;

} // namespace SDLib

#endif
