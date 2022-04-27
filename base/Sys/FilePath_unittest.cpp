#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/FilePath.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

// TODO:  Currently does not check any file operations.  May want to try to check them.

//------------------------------------------------------------------------------
TEST (FilePath, Basic)
  {
  RStr  strPath = "/home/user/file.txt";

  FilePath  path;

  ASSERT_STREQ ((const char *) path, "");

  RStrArray  arrayParts;

  FilePath::SplitPath (strPath.AsChar (), '/', arrayParts);

  ASSERT_STREQ ((const char *) arrayParts[0], "home");
  ASSERT_STREQ ((const char *) arrayParts[2], "file.txt");

  ASSERT_STREQ (FilePath::GetFilenameFromPath (strPath.AsChar ()), "file.txt");
  ASSERT_STREQ (FilePath::GetFilenameNoExtFromPath (strPath.AsChar ()), "file");
  ASSERT_STREQ (FilePath::GetPathNoExt (strPath.AsChar ()), "/home/user/file");
  ASSERT_STREQ (FilePath::GetPathNoFilename (strPath.AsChar ()), "/home/user/");
  ASSERT_STREQ (FilePath::GetExtension (strPath.AsChar ()), "txt");
  };
