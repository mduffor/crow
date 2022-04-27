#include <gtest/gtest.h>

#include "Util/CalcHash.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (CalcHash, Basic)
  {
  ASSERT_EQ (CalcHashValue (""), 0x0);
  ASSERT_EQ (CalcHashValue ("Test"), 0x2ba2d4);
  ASSERT_EQ (CalcHashValue ("The quick brown fox"), 0x7e4854);
  ASSERT_EQ (CalcHashValue ("The quick brown fox "), 0x1c90bb1);
  ASSERT_EQ (CalcHashValue ("The quick brown fox jumped over the two lazy dogs"), 0xa19959);
  ASSERT_EQ (CalcHashValue (NULL), 0x0);
  ASSERT_EQ (CalcHashValue ("The quick brown fox$^(*%!&%#*@#!"), 0x14819b8);
  };
