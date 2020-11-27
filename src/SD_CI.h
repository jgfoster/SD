#pragma once
#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include <SD.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace SDLib {

#define BASE_PATH "fileSystem/"
#define FILE_PATH_SIZE 255

class File_CI : public File_Base {
private:
  std::fstream *finOut;
  // int _pos;
  char _path[FILE_PATH_SIZE];
  uint8_t _mode;
  bool _open;
  bool _isDirectory;

public:
  // File_CI(File_Base *baseFile);
  // File_CI(SdFile f, const char *name); // wraps an underlying SdFile
  // File_CI(void);                       // 'empty' constructor
  File_CI(const char *name, uint8_t mode);
  ~File_CI();
  // template <class T> size_t write1(T data);
  virtual size_t write(const char *buf, size_t size);
  virtual int availableForWrite();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(char *buf, size_t size);
  bool seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char *name();
  bool isOpen() { return _open; }

  bool isDirectory(void);
  File_CI openNextFile(uint8_t mode);
  void rewindDirectory(void);

  using Print::write;
  // testing functions
  int getWriteError() { return writeError; }
  void setWriteError(int value = 1) { writeError = value; }
  void clearWriteError() { writeError = 0; }

private:
  int writeError;
};

class SDClass_CI : public SDClass_Base {

private:
public:
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  bool begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
  bool begin(uint32_t clock, uint8_t csPin);

  // call this when a card is removed. It will allow you to insert and
  // initialise a new card.
  void end();

  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  File_CI open(const char *filename, uint8_t mode);
  File_CI open(const String &filename, uint8_t mode = FILE_READ) {
    return open(filename.c_str(), mode);
  }

  // Methods to determine if the requested file path exists.
  bool exists(const char *filepath);
  bool exists(const String &filepath) { return exists(filepath.c_str()); }

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  bool mkdir(const char *filepath);
  bool mkdir(const String &filepath) { return mkdir(filepath.c_str()); }

  // Delete the file.
  bool remove(const char *filepath);
  bool remove(const String &filepath) { return remove(filepath.c_str()); }

  bool rmdir(const char *filepath);
  bool rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

  virtual String className() const { return "SDClass_Base"; }

private:
  friend class File;
};

extern SDClass_CI sd_ci;

} // namespace SDLib

#endif
