#include <gtest/gtest.h>

#include "Net/Base64.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

const char szDecA [] = "f";
const char szEncA [] = "Zg==";

const char szDecB [] = "fo";
const char szEncB [] = "Zm8=";

const char szDecC [] = "foo";
const char szEncC [] = "Zm9v";

const char szDecD [] = "foob";
const char szEncD [] = "Zm9vYg==";

const char szDecE [] = "fooba";
const char szEncE [] = "Zm9vYmE=";

const char szDecF [] = "foobar";
const char szEncF [] = "Zm9vYmFy";

//------------------------------------------------------------------------------
TEST (Base64, Encode)
  {
  RStr  strOut;
  Base64::Encode (szDecA, strlen (szDecA), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncA);

  strOut.Reset ();
  Base64::Encode (szDecB, strlen (szDecB), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncB);

  strOut.Reset ();
  Base64::Encode (szDecC, strlen (szDecC), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncC);

  strOut.Reset ();
  Base64::Encode (szDecD, strlen (szDecD), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncD);

  strOut.Reset ();
  Base64::Encode (szDecE, strlen (szDecE), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncE);

  strOut.Reset ();
  Base64::Encode (szDecF, strlen (szDecF), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncF);
  };

//------------------------------------------------------------------------------
TEST (Base64, Decode)
  {
  RStr  strOut;
  Base64::Decode (szEncA, strlen (szEncA), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecA);

  strOut.Reset ();
  Base64::Decode (szEncB, strlen (szEncB), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecB);

  strOut.Reset ();
  Base64::Decode (szEncC, strlen (szEncC), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecC);

  strOut.Reset ();
  Base64::Decode (szEncD, strlen (szEncD), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecD);

  strOut.Reset ();
  Base64::Decode (szEncE, strlen (szEncE), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecE);

  strOut.Reset ();
  Base64::Decode (szEncF, strlen (szEncF), strOut);
  ASSERT_STREQ (strOut.AsChar (), szDecF);
  };
