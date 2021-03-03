#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"
#include <iostream>

namespace SDLib {

std::string *
SDClass_CI::contentsOfNewOrExistingFileWithName(const char *filepath) {
  if (!fileExists(filepath)) {
    files.emplace(std::string(filepath), new std::string);
  }
  return files[filepath];
}

bool SDClass_CI::mkdir(const char *filepath) {
  assert(!fileExists(filepath));
  if (dirExists(filepath)) {
    return false;
  }
  char *path = new char[strlen(filepath) + 1];
  strcpy(path, filepath);
  char *ptr = strchr(path, '/');
  while (ptr) {
    *ptr = '\0';
    directories.insert(path);
    *ptr = '/';
    ptr = strchr(ptr + 1, '/');
  }
  directories.insert(filepath);
  delete[] path;
  return true;
}

bool SDClass_CI::remove(const char *filepath) {
  if (files.count(filepath)) {
    delete files[filepath];
    files.erase(filepath);
    return true;
  }
  return false;
}

void SDClass_CI::removeAll() {
  for (const auto &[key, value] : files) {
    delete value;
  }
  directories.clear();
  files.clear();
}

bool SDClass_CI::rmdir(const char *filepath) {
  if (directories.count(filepath)) {
    directories.erase(filepath);
    return true;
  }
  return false;
}

SDClass_CI SD_CI;
std::set<std::string> SDClass_CI::directories;
std::map<std::string, std::string *> SDClass_CI::files;

} // namespace SDLib

#endif
