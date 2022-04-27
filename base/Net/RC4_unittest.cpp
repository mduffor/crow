#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Net/RC4.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

const char szDecA [] = "f";
const char szEncA [] = "6E";

const char szDecB [] = "fo";
const char szEncB [] = "6EBC";

const char szDecC [] = "foo";
const char szEncC [] = "6EBC88";

const char szDecD [] = "foob";
const char szEncD [] = "6EBC88C6";

const char szDecE [] = "fooba";
const char szEncE [] = "6EBC88C6D3";

const char szDecF [] = "foobar";
const char szEncF [] = "6EBC88C6D3C2";

//------------------------------------------------------------------------------
TEST (RC4, EncodeDecode)
  {
  RStr  strKey ("This is a test key for encoding with RC4.  It can be any string, even binary, up to 255 characters long.");

  RStr  strEncoded;
  RStr  strDecoded;
  RStr  strHex;

  RC4   rc4;

  rc4.InitWithKey (strKey);
  rc4.Encode (szDecA, strlen (szDecA), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncA);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecA);


  rc4.InitWithKey (strKey);
  rc4.Encode (szDecB, strlen (szDecB), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncB);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecB);


  rc4.InitWithKey (strKey);
  rc4.Encode (szDecC, strlen (szDecC), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncC);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecC);


  rc4.InitWithKey (strKey);
  rc4.Encode (szDecD, strlen (szDecD), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncD);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecD);


  rc4.InitWithKey (strKey);
  rc4.Encode (szDecE, strlen (szDecE), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncE);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecE);


  rc4.InitWithKey (strKey);
  rc4.Encode (szDecF, strlen (szDecF), strEncoded);
  strEncoded.ToHex (strHex);
  ASSERT_STREQ (strHex.AsChar (), szEncF);

  rc4.InitWithKey (strKey);
  rc4.Decode (strEncoded, strlen (strEncoded), strDecoded);
  ASSERT_STREQ (strDecoded.AsChar (), szDecF);
  };
