#pragma once
#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD.h"
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>

namespace SDLib {

class File_CI : public File_Base {
public:
  File_CI(const char *name, uint8_t mode);
  size_t write(const char *buf, size_t size);
  int availableForWrite() { return 4096; }
  int peek();
  int available() { return pContents->size() - _pos; }
  void flush() {}
  int read(char *buf, size_t size);
  bool seek(uint32_t pos) { return _pos = pos <= size() ? pos : size(); }
  uint32_t position() { return _pos; }
  uint32_t size() { return pContents->size(); }
  void close() { _isOpen = false; }
  operator bool() { return _isOpen; }
  const char *name() { return _path.c_str(); }
  bool isOpen() { return _isOpen; }

  bool isDirectory(void) { return _isDir; }
  File_CI openNextFile(uint8_t mode) { assert(false); }
  void rewindDirectory(void) { assert(false); }

  using Print::write;
  // testing functions
  int getWriteError() { return writeError; }
  void setWriteError(int value = 1) { writeError = value; }
  void clearWriteError() { writeError = 0; }

private:
  std::string *pContents;
  int _pos; // index of _next_ char to read or write
  std::string _path;
  uint8_t _mode;
  bool _isOpen;
  bool _isDir;
  int writeError;
};

class SDClass_CI : public SDClass_Base {
public:
  // Ignore hardware-related setup
  bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN) { return true; }
  bool begin(uint32_t clock, uint8_t csPin) { return true; }
  void end() {}

  std::string *contentsOfNewOrExistingFileWithName(const char *filepath);
  bool dirExists(const char *filepath) {
    std::cout << "dirExists(\"" << filepath
              << "\") == " << directories.count(filepath) << std::endl;
    return directories.count(filepath) == 1;
  }
  bool exists(const char *filepath) {
    // This doesn't check directories for intermediate paths
    return files.count(filepath) || dirExists(filepath);
  }
  bool exists(const String &filepath) { return exists(filepath.c_str()); }
  bool fileExists(const char *filepath) { return files.count(filepath); }

  bool mkdir(const char *filepath);
  bool mkdir(const String &filepath) { return mkdir(filepath.c_str()); }

  File_CI open(const char *filename, uint8_t mode) {
    return File_CI(filename, mode);
  }
  File_CI open(const String &filename, uint8_t mode = FILE_READ) {
    return open(filename.c_str(), mode);
  }

  bool remove(const char *filepath);
  bool remove(const String &filepath) { return remove(filepath.c_str()); }

  void removeAll();

  bool rmdir(const char *filepath);
  bool rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

  virtual String className() const { return "SDClass_Base"; }

private:
  static std::set<std::string> directories;
  static std::map<std::string, std::string *> files;
};

extern SDClass_CI SD_CI;

} // namespace SDLib

#endif
