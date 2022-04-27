#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Util/RStr.hpp"

ASSERTFILE (__FILE__);


#include "Containers/KVPArray.hpp"
#include "Util/RStrParser.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

// TODO: Need to test all the container types.

//------------------------------------------------------------------------------
TEST (KVPArray, BasicTest)
  {
  KVPArray  array;

  // TODO: Need to test creation and setting by array index assignment. i.e.  a[b]=c

  array.Append ("ONE", "1");
  array.Append ("TWO", "2");
  array.Append ("THREE", "3");
  array.Append ("FOUR", "This is a test.");

  ASSERT_STREQ (array["ONE"], "1");
  ASSERT_STREQ (array["TWO"], "2");
  ASSERT_STREQ (array["THREE"], "3");
  ASSERT_STREQ (array["FOUR"], "This is a test.");

  array.SetAt ("THREE", "333");
  ASSERT_STREQ (array["THREE"], "333");

  ASSERT_EQ (array.FindKey ("THREE"), 2);

  ASSERT_STREQ ((array.KeyAtIndex (2)).AsChar (), "THREE");
  ASSERT_STREQ ((array.ValueAtIndex (2)).AsChar (), "333");

  };

//------------------------------------------------------------------------------
TEST (KVPArray, ExpandVars)
  {
  KVPArray  array;

  array.Append ("ONE", "1");
  array.Append ("TWO", "2");
  array.Append ("THREE", "3");
  array.Append ("FOUR", "This is a test.");

  // Test environment variable replacement
  RStrParser  parserReplace ("$ONE ${TWO} $(THREE) ${ONE}also and $FOUR");
  parserReplace.ResetCursor();

  array.ExpandVars (parserReplace);

  ASSERT_STREQ (parserReplace.AsChar (), "1 2 3 1also and This is a test.");
  }

//------------------------------------------------------------------------------
TEST (KVPArray, ExpandMacros)
  {
  KVPArray  array;

  array.Append ("ONE", "1");
  array.Append ("TWO", "2");
  array.Append ("THREE", "3");
  array.Append ("FOUR", "This is a test.");

  // Test environment variable replacement
  RStrParser  parserSource ("ONE TWO THREE ONE$also and $FOUR");
  parserSource.ResetCursor();

  RStrParser  parserTarget;
  const char *  pszResult = array.ExpandMacros (parserSource.AsChar(), parserTarget);

  ASSERT_STREQ (pszResult, "1 2 3 1$also and $This is a test.");
  }


/*
                 KVPArray      ();
                 KVPArray      (const KVPArray &  arrayIn);
                 KVPArray      (INT   iInitialSize);
                 ~KVPArray     ();

    VOID *       AllocArray     (INT      iSizeIn);
    VOID         DeleteArray    (PVOID *  pvArrayIn);
    VOID         InitValues     (INT      iStartOffset,
                                 INT      iNumToInit);
    VOID         CopyValues     (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy);
    VOID         CopyValuesRev  (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy);
    VOID         AverageValues  (INT      iIndexAverage,
                                 INT      iIndexValueOne,
                                 INT      iIndexValueTwo,
                                 FLOAT    fBias = 0.5f);
    VOID         SwapIndexes    (INT  iIndexOne,
                                 INT  iIndexTwo);
    INT          CompareIndexes (INT  iIndexOne,
                                 INT  iIndexTwo);

    EStatus      Copy           (const KVPArray &  arraySource);

    KVPArray &   operator=      (const  KVPArray & arrayIn);
    BOOL         operator==     (const  KVPArray & arrayIn);
    BOOL         operator!=     (const  KVPArray & arrayIn)   {return (!(*this == arrayIn));};

    VOID               DebugPrint     (VOID);

*/
