#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

#include <filesystem>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>

namespace SDLib {

void File_CI::rewindDirectory(void) {
  // File_Base::rewindDirectory();
}

// =================================================================

bool SDClass_CI::begin(uint8_t csPin) {
  // return SDClass_Base::begin(csPin);
}

bool SDClass_CI::begin(uint32_t clock, uint8_t csPin) {
  // return SDClass_Base::begin(clock, csPin);
}

void SDClass_CI::end() {
  //  SDClass_Base::end();
}

File_CI SDClass_CI::open(const char *filename, uint8_t mode) {
  return File_CI(filename, mode);
}

bool SDClass_CI::exists(const char *filepath) {
  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, filepath);

  return fs::exists(newPath);
}

bool SDClass_CI::mkdir(const char *filepath) {
  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, filepath);

  if (strchr(filepath, '/') == nullptr) {
    // Creating a single directory
    try {
      fs::create_directory(newPath);
    } catch (fs::filesystem_error &excpt) {
      std::cerr << excpt.what() << std::endl;
      return false;
    }
    return true;
  } else {
    // creating multiple directories
    try {
      fs::create_directories(newPath);
    } catch (fs::filesystem_error &excpt) {
      std::cerr << excpt.what() << std::endl;
      return false;
    }
    return true;
  }
}

bool SDClass_CI::remove(const char *filepath) {
  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, filepath);

  return fs::remove(newPath);
}

bool SDClass_CI::rmdir(const char *filepath) {
  // prepend base file path
  char newPath[FILE_PATH_SIZE];
  strcpy(newPath, BASE_PATH);
  strcat(newPath, filepath);

  return fs::remove_all(newPath);
}

SDClass_CI sd_ci;

} // namespace SDLib

#endif
