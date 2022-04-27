#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/ParseTools.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md



//------------------------------------------------------------------------------
TEST (ParseTools, KVP)
  {
  RStr        strKey;
  RStrParser  parserValue;
  RStrParser  parserSource;


  parserSource.Set ("myKey:myValue");
  ASSERT_TRUE (ParseTools::IsKVPColon (parserSource, strKey, parserValue));
  ASSERT_STREQ (strKey.AsChar(), "myKey");
  ASSERT_STREQ (parserValue.AsChar(), "myValue");

  strKey.Empty(); parserValue.Empty();
  parserSource.Set ("myKey:");
  ASSERT_TRUE (ParseTools::IsKVPColon (parserSource, strKey, parserValue));
  ASSERT_STREQ (strKey.AsChar(), "myKey");
  ASSERT_STREQ (parserValue.AsChar(), "");

  strKey.Empty(); parserValue.Empty();
  parserSource.Set ("myKey");
  ASSERT_FALSE (ParseTools::IsKVPColon (parserSource, strKey, parserValue));


  parserSource.Set ("myKey [myValue]");
  ASSERT_TRUE (ParseTools::IsKVPBracket(parserSource, strKey, parserValue));
  ASSERT_STREQ (strKey.AsChar(), "myKey");
  ASSERT_STREQ (parserValue.AsChar(), "myValue");

  }


//------------------------------------------------------------------------------
TEST (ParseTools, GetArray)
  {
  RStrParser  parserSource;
  IntArray    aiOut;
  RStrArray   astrOut;

  // Test GetIntArray
  parserSource.Set ("[1,2,3]");
  ParseTools::GetIntArray (parserSource, '[', ']', aiOut);
  ASSERT_EQ (aiOut.Length(), 3);
  ASSERT_EQ (aiOut[1], 2);
  ASSERT_EQ (aiOut[2], 3);

  aiOut.Clear();
  ASSERT_EQ (aiOut.Length(), 0);

  parserSource.Set ("[   1  , 2, 3  ]");
  ParseTools::GetIntArray (parserSource, '[', ']', aiOut);
  ASSERT_EQ (aiOut.Length(), 3);
  ASSERT_EQ (aiOut[1], 2);
  ASSERT_EQ (aiOut[2], 3);

  aiOut.Clear();
  ASSERT_EQ (aiOut.Length(), 0);

  parserSource.Set ("[]");
  ParseTools::GetIntArray (parserSource, '[', ']', aiOut);
  ASSERT_EQ (aiOut.Length(), 0);

  aiOut.Clear();

  // Test GetStringArray

  parserSource.Set (R"""(["A","B","C"])""");
  ParseTools::GetStringArray (parserSource, '[', ']', astrOut);
  ASSERT_EQ (astrOut.Length(), 3);
  ASSERT_STREQ (astrOut[1], "B");
  ASSERT_STREQ (astrOut[2], "C");

  astrOut.Clear();
  ASSERT_EQ (astrOut.Length(), 0);

  parserSource.Set (R"""([ "A" ,   "B", "C"   ])""");
  ParseTools::GetStringArray (parserSource, '[', ']', astrOut);
  ASSERT_EQ (astrOut.Length(), 3);
  ASSERT_STREQ (astrOut[1], "B");
  ASSERT_STREQ (astrOut[2], "C");

  astrOut.Clear();

  parserSource.Set (R"""([])""");
  ParseTools::GetStringArray (parserSource, '[', ']', astrOut);
  ASSERT_EQ (astrOut.Length(), 0);

  astrOut.Clear();

  parserSource.Set (R"""([  ])""");
  ParseTools::GetStringArray (parserSource, '[', ']', astrOut);
  ASSERT_EQ (astrOut.Length(), 0);

/*
     static VOID     GetIntArray    (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    IntArray &    arrayOut);

    static VOID     GetStringArray (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    RStrArray &   arrayOut);
*/

  }

/*


    static BOOL     IsKVPColon     (RStrParser &    parserBufferIn,
                                    RStr &          strKeyWordOut,
                                    RStrParser &    parserValueOut);

    static BOOL     IsKVPBracket   (RStrParser &    parserBufferIn,
                                    RStr &          strKeyOut,
                                    RStrParser &    parserValueOut);

    static BOOL     IsKVPEquals    (RStrParser &    parserBufferIn,
                                    RStr &          strKeyOut,
                                    RStrParser &    parserValueOut);

    static EStatus  GetKey         (RStrParser &  parserIn,
                                    UINT32        uSeparatorIn,
                                    RStr &        strKeyOut);

    static VOID     GetIntArray    (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    IntArray &    arrayOut);

    static VOID     GetFloatArray  (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    FloatArray &  arrayOut);

    static VOID     GetDoubleArray (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    DoubleArray & arrayOut);

    static VOID     GetStringArray (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    RStrArray &   arrayOut);



*/
