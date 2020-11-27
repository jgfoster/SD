/*
bundle config --local path vendor/bundle
bundle install
bundle exec arduino_ci.rb  --skip-examples-compilation
bundle exec arduino_ci.rb  --skip-unittests
 */
#include "Arduino.h"
#include "ArduinoUnitTests.h"
#include "SD_CI.h"
#include <fstream>

unittest(exists_works) {
  assertFalse(sd_ci.exists("noSuchFile.txt"));
  File_CI existingFile = sd_ci.open("existingFile.txt", FILE_WRITE);
  existingFile.close();
  assertTrue(sd_ci.exists("existingFile.txt"));
  sd_ci.remove("existingFile.txt");
}

unittest(mkdir_works) {
  assertTrue(sd_ci.mkdir("test_directory/a/b"));
  assertTrue(sd_ci.mkdir("test_directory/a/c"));
  assertTrue(sd_ci.exists("test_directory"));
  assertTrue(sd_ci.exists("test_directory/a/b"));
  assertTrue(sd_ci.exists("test_directory/a/c"));

  // tear down
  sd_ci.rmdir("test_directory");
}

unittest(open_works) {
  // create file
  File_CI writeFile = sd_ci.open("file.txt", FILE_WRITE);
  writeFile.close();
  // open file for read should exist
  File_CI readFile2 = sd_ci.open("file.txt", FILE_READ);
  assertTrue(readFile2.isOpen());
  readFile2.close();
  // delete test file
  sd_ci.remove("file.txt");
}

unittest(close_works) {
  // close write file
  File_CI file = sd_ci.open("file.txt", FILE_WRITE);
  assertTrue(file.isOpen());
  file.close();
  assertFalse(file.isOpen());

  // close read file
  File_CI readFile = sd_ci.open("file.txt", FILE_READ);
  assertTrue(readFile.isOpen());
  readFile.close();
  assertFalse(readFile.isOpen());
  sd_ci.remove("file.txt");
}

unittest(remove_works) {
  // set up
  File_CI file = sd_ci.open("file.txt", FILE_WRITE);
  file.close();
  assertTrue(sd_ci.exists("file.txt"));

  sd_ci.remove("file.txt");
  assertFalse(sd_ci.exists("file.txt"));
}

unittest(rmdir_works) {
  // set up
  sd_ci.mkdir("test_directory/a/a");
  sd_ci.mkdir("test_directory/a/b");
  sd_ci.mkdir("test_directory/a/c");
  File_CI file = sd_ci.open("test_directory/a/a/file.txt", FILE_WRITE);
  file.close();

  // remove directory
  assertTrue(sd_ci.rmdir("test_directory/a/c"));
  // make sure non-removed dirs still exist
  assertTrue(sd_ci.exists("test_directory"));
  assertTrue(sd_ci.exists("test_directory/a/a"));
  // make sure removed dir no longer exists
  assertFalse(sd_ci.exists("test_directory/a/c"));

  // remove directory with sub directories
  assertTrue(sd_ci.rmdir("test_directory"));
  assertFalse(sd_ci.exists("test_directory"));
}

unittest(name_works) {
  // set up
  File_CI file = sd_ci.open("newFile.txt", FILE_WRITE);

  char expected[] = "newFile.txt";
  assertEqual(expected, file.name());
  file.close();

  // tear down
  sd_ci.remove("newFile.txt");
}

unittest(seek_works) {
  // set up
  File_CI file = sd_ci.open("seek.txt", FILE_WRITE);
  char write[] = "Hide and Seek.";
  file.write(write, sizeof(write) - 1);
  file.close();
  File_CI read = sd_ci.open("seek.txt", FILE_READ);

  // Testing
  char readFrom[4];
  char expected[] = "and";
  read.seek(5);
  read.read(readFrom, 3);
  readFrom[3] = '\0';
  assertEqual(expected, readFrom);

  // tear down
  sd_ci.remove("seek.txt");
}

unittest(read_works) {
  // set up
  File_CI file = sd_ci.open("birthday.txt", FILE_WRITE);
  char toWrite[] = "Happy Birthday to You!";
  file.write(toWrite, sizeof(toWrite) - 1);
  file.close();

  File_CI file2 = sd_ci.open("lines.txt", FILE_WRITE);
  char toWrite2[] = "line 1\nline2";
  file2.write(toWrite2, sizeof(toWrite2) - 1);
  file2.close();

  // testing
  File_CI readFile = sd_ci.open("birthday.txt", FILE_READ);
  size_t size = readFile.size();
  char readFromFile[size + 1];
  readFile.read(readFromFile, size);
  readFromFile[size] = '\0';
  readFile.close();

  // assertEqual(toWrite, readFromFile);

  File_CI readFile2 = sd_ci.open("lines.txt", FILE_READ);
  char readFromFile2[7 + 1];
  char readFromFile3[5 + 1];
  readFile2.read(readFromFile2, 7);
  readFile2.read(readFromFile3, 5);
  readFromFile2[7] = '\0';
  readFromFile3[5] = '\0';

  char expected2[] = "line 1\n";
  char expected3[] = "line2";
  assertEqual(expected2, readFromFile2);
  assertEqual(expected3, readFromFile3);

  // tear down
  sd_ci.remove("birthday.txt");
  sd_ci.remove("lines.txt");
}

unittest(write_works) {
  // open new file for writing
  File_CI writeFile = sd_ci.open("wood.txt", FILE_WRITE);
  char toWrite[] = "How much wood could a wood pecker peck?\n";
  writeFile.write(toWrite, sizeof(toWrite) - 1);

  // read from same write file
  size_t size = writeFile.size();
  char readFromFile[size + 1];
  writeFile.seek(0);
  writeFile.read(readFromFile, size);
  readFromFile[size] = '\0';
  // assertEqual(toWrite, readFromFile);
  writeFile.close();

  // open old writing file to write at end.
  File_CI writeFile2 = sd_ci.open("wood.txt", FILE_WRITE);
  char toWrite2[] = "A lot of wood.\n";
  writeFile2.write(toWrite2, sizeof(toWrite2) - 1);
  writeFile2.close();

  // check old writing file
  File_CI readWrite2 = sd_ci.open("wood.txt", FILE_READ);
  size_t size2 = readWrite2.size();
  char toRead2[size2 + 1];
  readWrite2.read(toRead2, size2);
  toRead2[size] = '\0';
  char expected2[] =
      "How much wood could a wood pecker peck?\nA lot of wood.\n";
  // assertEqual(expected2, toRead2);
  readWrite2.close();

  // teardown
  sd_ci.remove("wood.txt");
}

unittest(size_works) {
  // setup
  File_CI sizeFile = sd_ci.open("size.txt", FILE_WRITE);
  char toWrite[] = "Test text\n";
  sizeFile.write(toWrite, sizeof(toWrite) - 1);
  sizeFile.close();

  // test
  uint32_t fileSize = sizeFile.size();
  assertEqual(10, fileSize);

  // tear down
  sd_ci.remove("size.txt");
}

unittest(peek_works) {
  // set up
  File_CI peekFile = sd_ci.open("peek.txt", FILE_WRITE);
  char toWrite[] = "Peek file content\n";
  peekFile.write(toWrite, sizeof(toWrite) - 1);
  peekFile.close();

  // Test
  File_CI readPeek = sd_ci.open("peek.txt", FILE_READ);
  assertEqual('P', readPeek.peek());
  assertEqual('P', readPeek.peek());
  readPeek.close();

  File_CI readWritePeek = sd_ci.open("peek.txt", FILE_WRITE);
  readWritePeek.seek(0);
  assertEqual('P', readWritePeek.peek());
  assertEqual('P', readWritePeek.peek());
  readWritePeek.close();

  // tear down
  sd_ci.remove("peek.txt");
}

unittest(position_works) {
  // set up
  File_CI posFile = sd_ci.open("pos.txt", FILE_WRITE);
  char toWrite[] = "This is the position file.\n";
  posFile.write(toWrite, sizeof(toWrite) - 1);

  // test
  assertEqual(27, posFile.position());
  posFile.seek(5);
  assertEqual(5, posFile.position());
  posFile.close();

  // tear down
  sd_ci.remove("pos.txt");
}

unittest(isDirectory_works) {
  // set up
  sd_ci.mkdir("test");
  File_CI toRead = sd_ci.open("read.txt", FILE_WRITE);
  toRead.close();
  File_CI testFile = sd_ci.open("test.txt", FILE_WRITE);
  File_CI readFile = sd_ci.open("read.txt", FILE_READ);

  // test
  File_CI myDir = sd_ci.open("test");
  assertTrue(myDir.isDirectory());
  assertFalse(testFile.isDirectory());
  assertFalse(readFile.isDirectory());

  // tear down
  testFile.close();
  readFile.close();
  sd_ci.remove("test.txt");
  sd_ci.remove("read.txt");
  sd_ci.rmdir("test");
}
unittest_main()
