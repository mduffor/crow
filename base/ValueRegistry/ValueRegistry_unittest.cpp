#include <gtest/gtest.h>


#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)


#include "ValueRegistry/ValueRegistry.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"


// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

// TODO: Test AltName setting and searching
// TODO: Test ValueElemIntArray, ValueElemStringArray, ValueElemFloatArray, ValueElemDoubleArray
// TODO: Test deletion, overwriting.
// TODO: Test Vec type

//------------------------------------------------------------------------------
TEST (ValueRegistry, Basic)
  {
  ValueRegistrySimple  registry;
  const char   szString [] = "This is my string";

  registry.SetInt    ("myInt",   1);
  registry.SetFloat  ("myFloat", 2.0f);
  registry.SetDouble ("myDouble", 3.0);
  registry.SetBool   ("myBool",  TRUE);
  registry.SetString ("myString", szString);
  registry.SetPtr    ("myPtr",    (PVOID) &szString[0]);

  ASSERT_EQ (registry.GetInt    ("myInt"),    1);
  ASSERT_EQ (registry.GetFloat  ("myFloat"),  2.0f);
  ASSERT_EQ (registry.GetDouble ("myDouble"),  3.0);
  ASSERT_EQ (registry.GetBool   ("myBool"),   TRUE);
  ASSERT_STREQ (registry.GetString ("myString"), szString);
  ASSERT_EQ (registry.GetPtr    ("myPtr"),    (PVOID) &szString[0]);

  // test saving and restoring
  registry.MarkForSave ("myInt");
  registry.MarkForSave ("myFloat");
  registry.MarkForSave ("myDouble");
  registry.MarkForSave ("myBool");
  registry.MarkForSave ("myString");

  RStrParser  parserSaved;
  registry.ToParser (parserSaved);

  registry.Clear ();

  parserSaved.ResetCursor ();
  registry.FromParser (parserSaved);

  ASSERT_EQ (registry.GetInt    ("myInt"),    1);
  ASSERT_EQ (registry.GetFloat  ("myFloat"),  2.0f);
  ASSERT_EQ (registry.GetDouble ("myDouble"),  3.0);
  ASSERT_EQ (registry.GetBool   ("myBool"),   TRUE);
  ASSERT_STREQ (registry.GetString ("myString"), szString);


  //ASSERT_STREQ (strOut.AsChar (), szEncF);
  };

//------------------------------------------------------------------------------
TEST (ValueRegistry, Hash)
  {
  ValueRegistrySimple  registry;
  const char   szString [] = "This is my string";

  registry.SetInt    ("myInt",   1);
  registry.SetFloat  ("myFloat", 2.0f);
  registry.SetDouble ("myDouble", 3.0);
  registry.SetBool   ("myBool",  TRUE);
  registry.SetString ("myString", szString);
  registry.SetPtr    ("myPtr",    (PVOID) &szString[0]);

  ASSERT_EQ (registry.GetInt    ("myInt"),    1);
  ASSERT_EQ (registry.GetFloat  ("myFloat"),  2.0f);

  ASSERT_TRUE (DBL_APPROX_EQUAL (registry.GetDouble  ("myDouble"), 3.0));
  ASSERT_EQ (registry.GetBool   ("myBool"),   TRUE);
  ASSERT_STREQ (registry.GetString ("myString"), szString);
  ASSERT_EQ (registry.GetPtr    ("myPtr"),    (PVOID) &szString[0]);

  // lookup by hash, when created with string name
  ASSERT_EQ (registry.GetInt    (CalcHashValue ("myInt")),    1);
  ASSERT_EQ (registry.GetFloat  (CalcHashValue ("myFloat")),  2.0f);
  ASSERT_EQ (registry.GetDouble (CalcHashValue ("myDouble")),  3.0);
  ASSERT_EQ (registry.GetBool   (CalcHashValue ("myBool")),   TRUE);
  ASSERT_STREQ (registry.GetString (CalcHashValue ("myString")), szString);
  ASSERT_EQ (registry.GetPtr    (CalcHashValue ("myPtr")),    (PVOID) &szString[0]);

  registry.Clear ();

  registry.SetInt    (HASH ("myInt"),   1);
  registry.SetFloat  (HASH ("myFloat"), 2.0f);
  registry.SetDouble (HASH ("myDouble"), 3.0);
  registry.SetBool   (HASH ("myBool"),  TRUE);
  registry.SetString (HASH ("myString"), szString);
  registry.SetPtr    (HASH ("myPtr"),    (PVOID) &szString[0]);

  // lookup by hash, when created with hash name
  ASSERT_EQ (registry.GetInt    (CalcHashValue ("myInt")),    1);
  ASSERT_EQ (registry.GetFloat  (CalcHashValue ("myFloat")),  2.0f);
  ASSERT_EQ (registry.GetDouble (CalcHashValue ("myDouble")),  3.0);
  ASSERT_EQ (registry.GetBool   (CalcHashValue ("myBool")),   TRUE);
  ASSERT_STREQ (registry.GetString (CalcHashValue ("myString")), szString);
  ASSERT_EQ (registry.GetPtr    (CalcHashValue ("myPtr")),    (PVOID) &szString[0]);
  };


//------------------------------------------------------------------------------
TEST (ValueRegistry, Links)
  {
  ValueRegistrySimple  registry;


  // list One
  registry.SetLink (HASH("IAP.Offers.List"), HASH("IAP.Offers.UniqueOfferID_ABC"));

  registry.SetString ("IAP.Offers.UniqueOfferID_ABC.Name", "ABC");
  registry.SetString ("IAP.Offers.UniqueOfferID_ABC.Type", "Coins");
  registry.SetInt    ("IAP.Offers.UniqueOfferID_ABC.Amount", 100);
  registry.SetLink   ("IAP.Offers.UniqueOfferID_ABC.Next", HASH("IAP.Offers.UniqueOfferID_DEF"));

  registry.SetString ("IAP.Offers.UniqueOfferID_DEF.Name", "DEF");
  registry.SetString ("IAP.Offers.UniqueOfferID_DEF.Type", "Gems");
  registry.SetInt    ("IAP.Offers.UniqueOfferID_DEF.Amount", 200);
  registry.SetLink   ("IAP.Offers.UniqueOfferID_DEF.Next", HASH(""));


  // list Two
  registry.SetLink (HASH("IAP.Offers.ListTwo"), HASH("IAP.Offers.UniqueOfferID_GHI"));

  registry.SetString ("IAP.Offers.UniqueOfferID_GHI.Name", "GHI");
  registry.SetString ("IAP.Offers.UniqueOfferID_GHI.Type", "Coins");
  registry.SetInt    ("IAP.Offers.UniqueOfferID_GHI.Amount", 300);
  registry.SetLink   ("IAP.Offers.UniqueOfferID_GHI.Next", HASH("IAP.Offers.UniqueOfferID_JKL"));

  registry.SetString ("IAP.Offers.UniqueOfferID_JKL.Name", "JKL");
  registry.SetString ("IAP.Offers.UniqueOfferID_JKL.Type", "Gems");
  registry.SetInt    ("IAP.Offers.UniqueOfferID_JKL.Amount", 400);
  registry.SetLink   ("IAP.Offers.UniqueOfferID_JKL.Next", HASH(""));


  // count items in list

  INT  iCount = registry.LinkCount (HASH("IAP.Offers.List"), ".Next");
  ASSERT_EQ (iCount, 2);


  // test getting last element

  HASH_T  uEndHash = registry.LinkGetLast (HASH("IAP.Offers.UniqueOfferID_ABC"), ".Next");

  ASSERT_EQ (uEndHash, HASH("IAP.Offers.UniqueOfferID_DEF"));

  uEndHash = registry.LinkGetLast (HASH("IAP.Offers.UniqueOfferID_GHI"), ".Next");

  ASSERT_EQ (uEndHash, HASH("IAP.Offers.UniqueOfferID_JKL"));

  uEndHash = registry.LinkGetLast (HASH("IAP.Offers.UniqueOfferID_JKL"), ".Next");

  ASSERT_EQ (uEndHash, HASH("IAP.Offers.UniqueOfferID_JKL"));

  // unlink list, and make sure it was unlinked

  registry.Unlink (HASH("IAP.Offers.List"), ".Next");

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 0);

  // relink (with link appending)

  // push back
  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_ABC"));
  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_DEF"));

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 2);
  registry.Unlink (HASH("IAP.Offers.List"), ".Next");


  // push front
  registry.LinkPushFront (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_ABC"));
  registry.LinkPushFront (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_DEF"));

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 2);


  // setup multiple lists, and test list merging

  // test appending
  registry.Unlink (HASH("IAP.Offers.List"), ".Next");
  registry.Unlink (HASH("IAP.Offers.ListTwo"), ".Next");

  registry.SetLink (HASH("IAP.Offers.List"), HASH("IAP.Offers.UniqueOfferID_ABC"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_ABC.Next", HASH("IAP.Offers.UniqueOfferID_DEF"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_DEF.Next", HASH(""));

  registry.SetLink (HASH("IAP.Offers.ListTwo"), HASH("IAP.Offers.UniqueOfferID_GHI"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_GHI.Next", HASH("IAP.Offers.UniqueOfferID_JKL"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_JKL.Next", HASH(""));


  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", registry.GetLink (HASH("IAP.Offers.ListTwo")));
  registry.SetLink (HASH("IAP.Offers.ListTwo"), 0);

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 4);

  uEndHash = registry.LinkGetLast (registry.GetLink (HASH("IAP.Offers.List")), ".Next");

  ASSERT_EQ (uEndHash, HASH("IAP.Offers.UniqueOfferID_JKL"));


  // test prepending
  registry.Unlink (HASH("IAP.Offers.List"), ".Next");
  registry.Unlink (HASH("IAP.Offers.ListTwo"), ".Next");

  registry.SetLink (HASH("IAP.Offers.List"), HASH("IAP.Offers.UniqueOfferID_ABC"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_ABC.Next", HASH("IAP.Offers.UniqueOfferID_DEF"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_DEF.Next", HASH(""));

  registry.SetLink (HASH("IAP.Offers.ListTwo"), HASH("IAP.Offers.UniqueOfferID_GHI"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_GHI.Next", HASH("IAP.Offers.UniqueOfferID_JKL"));
  registry.SetLink   ("IAP.Offers.UniqueOfferID_JKL.Next", HASH(""));


  registry.LinkPushFront (HASH("IAP.Offers.List"), ".Next", registry.GetLink (HASH("IAP.Offers.ListTwo")));
  registry.SetLink (HASH("IAP.Offers.ListTwo"), 0);

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 4);

  uEndHash = registry.LinkGetLast (registry.GetLink (HASH("IAP.Offers.List")), ".Next");

  ASSERT_EQ (uEndHash, HASH("IAP.Offers.UniqueOfferID_DEF"));


  // append null and make sure it maintains list.

  registry.Unlink (HASH("IAP.Offers.List"), ".Next");

  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_ABC"));
  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", HASH("IAP.Offers.UniqueOfferID_DEF"));

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 2);

  registry.LinkPushBack (HASH("IAP.Offers.List"), ".Next", 0);

  ASSERT_EQ (registry.LinkCount (HASH("IAP.Offers.List"), ".Next"), 2);

  };


//------------------------------------------------------------------------------
TEST (ValueRegistry, AccessShortcuts)
  {
  ValueRegistrySimple  registry;


  // list One
  registry.SetLink (HASH("IAP.Offers.List"), HASH("IAP.Offers.UniqueOfferID_ABC"));

  registry.SetString ("IAP.Offers.UniqueOfferID_ABC.Name", "ABC");

  RStr  strPrefix  ("IAP.Offers.");
  RStr  strMiddle  ("UniqueOfferID_ABC");
  RStr  strPostfix (".Name");


  ASSERT_STREQ (registry.GetString (strPrefix + "UniqueOfferID_ABC" + strPostfix), "ABC");
  ASSERT_STREQ (registry.GetString ("IAP.Offers." + strMiddle + strPostfix), "ABC");
  ASSERT_STREQ (registry.GetString ("IAP.Offers." + strMiddle + ".Name"), "ABC");
  //ASSERT_STREQ (registry.GetString (), "ABC");


  };



//------------------------------------------------------------------------------
TEST (ValueRegistry, ExpandVars)
  {
  ValueRegistrySimple  registry;

  registry.SetInt ("One", 1);
  registry.SetInt ("Two", 2);

  RStrParser  parserSource;

  parserSource = "${One}/${Two}";

  ASSERT_STREQ (registry.ExpandVars (parserSource), "1/2");
  };

