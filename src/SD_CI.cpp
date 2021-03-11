#include <Arduino.h>
#ifdef MOCK_PINS_COUNT
#include "SD_CI.h"

namespace SDLib {

String *SDClass_CI::contentsOf(const String &path) {
  String fullPath = normalizePath(path);
  if (fileSystem.count(fullPath)) {
    return &fileSystem[fullPath];
  }
  return nullptr;
}

bool SDClass_CI::exists(const String &path) {
  String fullPath = normalizePath(path);
  return contentsOf(fullPath) != nullptr;
}

bool SDClass_CI::mkdir(const String &path) {
  String fullPath = normalizePath(path, true);
  if (exists(fullPath)) {
    return false;
  }
  size_t index = fullPath.find_last_of('/', fullPath.size() - 2);
  assert(index == 0 || exists(fullPath.substr(0, index + 1)));
  // save directory base name as "file contents" to simplify name() lookup
  String dirName = fullPath.substr(index + 1, fullPath.size() - index - 2);
  fileSystem.emplace(fullPath, dirName);
  return true;
}

const String &SDClass_CI::nameAfter(const String fullPath, const String start) {
  std::map<String, String>::iterator iter;
  iter = fileSystem.find(fullPath);
  assert(iter != fileSystem.end());
  if (iter->first != start &&
      fullPath.back() == '/') { // current entry is a directory
    // advance to next entry after directory
    while ((++iter)->first.substr(0, fullPath.size()) == fullPath) {
    }
  } else { // advance to next entry
    ++iter;
  }
  // are we at the end of the file system?
  if (iter == fileSystem.end()) {
    return emptyString;
  }
  // have we finished the current directory?
  if (iter->first.substr(0, start.size()) != start) {
    return emptyString;
  }
  return iter->first;
}

File_CI SDClass_CI::open(const String &path, uint8_t mode) {
  // ensure we have a root directory
  fileSystem.emplace(String("/"), String(""));
  String fullPath = normalizePath(path);
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

bool SDClass_CI::remove(const String &path) {
  String fullPath = normalizePath(path);
  assert(fullPath.back() != '/');
  if (exists(fullPath)) {
    fileSystem.erase(fullPath);
    return true;
  }
  return false;
}

bool SDClass_CI::rmdir(const String &path) {
  String fullPath = normalizePath(path, true);
  if (exists(fullPath)) {
    // check for children of this directory
    String next = nameAfter(fullPath, fullPath);
    assert(next == "" || next.substr(0, fullPath.size()) != fullPath);
    fileSystem.erase(fullPath);
    return true;
  }
  return false;
}

// private
String SDClass_CI::normalizePath(const String &inPath, bool isDirectory) {
  String fullPath = inPath;
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
std::map<String, String> SDClass::fileSystem;

} // namespace SDLib

#endif
