#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

namespace SDLib {

std::string *SDClass_CI::contentsOf(const std::string &path) {
  std::string fullPath = normalizePath(path);
  if (fileSystem.count(fullPath)) {
    return &fileSystem[fullPath];
  }
  return nullptr;
}

bool SDClass_CI::exists(const std::string &path) {
  std::string fullPath = normalizePath(path);
  return contentsOf(fullPath) != nullptr;
}

bool SDClass_CI::mkdir(const std::string &path) {
  std::string fullPath = normalizePath(path, true);
  if (exists(fullPath)) {
    return false;
  }
  size_t index = fullPath.find_last_of('/', fullPath.size() - 2);
  assert(index == 0 || exists(fullPath.substr(0, index + 1)));
  fileSystem.emplace(fullPath, "");
  return true;
}

const std::string &SDClass_CI::nameAfter(const std::string fullPath,
                                         const std::string start) {
  std::map<std::string, std::string>::iterator iter;
  iter = fileSystem.find(fullPath);
  assert(iter != fileSystem.end());
  if (iter->first != start &&
      fullPath.back() == '/') { // current entry is a directory
    // advance to next entry after directory
    while ((++iter)->first.substr(0, fullPath.size()) == fullPath) {
    }
  } else {
    ++iter;
  }
  if (iter == fileSystem.end()) {
    return emptyString;
  }
  return iter->first;
}

File_CI SDClass_CI::open(const std::string &path, uint8_t mode) {
  // ensure we have a root directory
  fileSystem.emplace(std::string("/"), std::string(""));
  std::string fullPath = normalizePath(path);
  if (exists(fullPath)) {
    if (fullPath.back() == '/') {
      return File_CI(fullPath);
    }
    return File_CI(fullPath, mode);
  }
  assert(fullPath.back() != '/' && mode & O_WRITE);
  size_t index = fullPath.find_last_of('/');
  assert(index == 0 || exists(fullPath.substr(0, index + 1)));
  fileSystem.emplace(fullPath, "");
  return File_CI(fullPath, mode);
}

bool SDClass_CI::remove(const std::string &path) {
  std::string fullPath = normalizePath(path);
  assert(fullPath.back() != '/');
  if (exists(fullPath)) {
    fileSystem.erase(fullPath);
    return true;
  }
  return false;
}

bool SDClass_CI::rmdir(const std::string &path) {
  std::string fullPath = normalizePath(path, true);
  if (exists(fullPath)) {
    // check for children of this directory
    std::string next = nameAfter(fullPath, fullPath);
    assert(next == "" || next.substr(0, fullPath.size()) != fullPath);
    fileSystem.erase(fullPath);
    return true;
  }
  return false;
}

// private
std::string SDClass_CI::normalizePath(const std::string &inPath,
                                      bool isDirectory) {
  std::string fullPath = inPath;
  // add leading '/' for root
  if (fullPath.front() != '/') {
    fullPath.insert(0, 1, '/');
  }
  if (fullPath.back() == '/') { // if already a directory, then done
    return fullPath;
  } else if (isDirectory) { // if should be a directory, make it one
    fullPath += '/';
    return fullPath;
  }
  fullPath += '/';
  if (exists(fullPath)) { // look for a directory
    return fullPath;
  }
  fullPath = fullPath.substr(0, fullPath.size() - 1);
  return fullPath;
}

// static variables
SDClass_CI SD_CI;
std::map<std::string, std::string> SDClass::fileSystem;

} // namespace SDLib

#endif
