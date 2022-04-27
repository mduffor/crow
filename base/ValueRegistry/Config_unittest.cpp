#include <gtest/gtest.h>


#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)


#include "ValueRegistry/ValueRegistry.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"
#include "ValueRegistry/ConfigLayer.hpp"
#include "ValueRegistry/ConfigSubset.hpp"
#include "ValueRegistry/ConfigDeck.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
TEST (ConfigLayer, Basic)
  {
  ValueRegistrySimple  registry;

  filePath_localStoragePath.AppendString ("./app/src/main/assets");

  ConfigDeck::Init ();

  ConfigDeck *  pDeck = new ConfigDeck();
  pDeck->pReg = &registry;

  ASSERT_NE (pDeck, nullptr);

  //printf ("Save Path %s\n", ConfigDeck::GetConfigSavePath ());
  //printf ("Cache Path %s\n", ConfigDeck::GetConfigCachePath ());

  ASSERT_EQ (pDeck->GetLayerCount (), 0);

  RStr   strBuffer = R"""(
    [
      {
      "name" : "config_name",
      "enabled" : "",
      "prereq" : "",
      "order" : 20,
      "variables" :
        {
        "stringarray:Var.Array" : ["A", "B", "C"],
        "stringset:Var.StringSet" : ["A", "B"],
        "string:Var.Multiline" : "This line
                                  is spread across multiple lines
                                  and we need to make sure it can be
                                  read in properly",
        "string:Var.One" : "string with 'embedded quotes'.",
        "bool:Var.Bool" : true,
        "int:Var.Two" : 2,
        "int:Var.Four" : 1,
        "int:Var.Six" : 6
        },
      "segments" :
        [
        {
        "name" : "SegA",
        "prereq" : "true",
        "variables" :
          {
          "int:Var.Two" : 3
          }
        },
        {
        "name" : "SegB",
        "prereq" : "false",
        "variables" :
          {
          "int:Var.Two" : 4
          }
        }
        ]
      },
      // single line comment
      {
      "name" : "config_name_2",
      "enabled" : "",
      "prereq" : "",
      "order" : 10,
      "variables" :
        {
        "Var.Three" : "string with 'embedded quotes'.",
        "int:Var.Four" : 2
        }
      },
      {
      "name" : "config_name_3",
      "enabled" : "",
      "prereq" : "",
      "order" : 12,
      "segments" :
        [
        {
        "name" : "SegA",
        "prereq" : "",
        "variables" :
          {
          "int:Var.Five" : 5
          }
        }
        ]
      },
      {
      "name" : "config_name_4",
      "order" : 20,
      "segments" :
        [
        {
        "prereq" : "0",
        "variables" :
          {
          "int:Var.Seven" : 1
          }
        },
        {
        "prereq" : "1",
        "variables" :
          {
          "int:Var.Seven" : 7
          }
        }
        ]
      },

      {
      "name" : "config_name_5",
      "order" : 50,
      "variables" :
        {
        "stringset:Var.StringSet" : ["B", "C"],
        }
      },


    ]
    )""";


  //printf ("%s", strBuffer.AsChar());

  pDeck->ReadBuffer ("", "rcfg", strBuffer.AsChar ());
  ASSERT_EQ (pDeck->GetLayerCount (), 5);

  /*
  for (TListItr<ConfigLayer*> itrCurr = pDeck->listLayers.First();
       itrCurr.IsValid();
       ++itrCurr)
    {
    printf ("found layer \"%s\"\n", (*itrCurr)->GetName());
    };
  */

  ASSERT_EQ (pDeck->pReg, &registry);

  //registry.DebugPrint ();

  // test setting at Layer level
  ASSERT_EQ (registry.GetInt    ("Var.Four"),    1);

  // test setting at single segment level
  ASSERT_EQ (registry.GetInt    ("Var.Five"),    5);

  // test that both base and segment solved
  ASSERT_EQ (registry.GetInt    ("Var.Two"),    3);
  ASSERT_EQ (registry.GetInt    ("Var.Six"),    6);

  // test setting from a choice among multiple segments
  ASSERT_EQ (registry.GetInt    ("Var.Two"),    3);

  ConfigLayer *  pLayer = pDeck->FindLayerByName ("config_name_3");

  ASSERT_FALSE (pLayer == NULL);
  ASSERT_EQ (pLayer->GetOrder(), 12);

  // Test expression-driven segments.
  ASSERT_EQ (registry.GetInt    ("Var.Seven"),    7);

  // Test arrays
  ASSERT_STREQ (registry.GetStringInArray ("Var.Array", 1), "B");

  // Test string sets
  // NOTE:  Technically, ordering after merging is not guaranteed.
  ASSERT_EQ (registry.GetArrayLength ("Var.StringSet"), 3);
  ASSERT_STREQ (registry.GetStringInArray ("Var.StringSet", 0), "A");
  ASSERT_STREQ (registry.GetStringInArray ("Var.StringSet", 1), "B");
  ASSERT_STREQ (registry.GetStringInArray ("Var.StringSet", 2), "C");

  // Test multi-line strings
  ASSERT_STREQ (registry.GetString ("Var.Multiline"),
                "This line\n                                  is spread across multiple lines\n                                  and we need to make sure it can be\n                                  read in properly");

  delete (pDeck);

  };
/*
    ConfigLayer *        FindOrCreateLayer  (const char *  szNameIn,
                                             INT           iOrderIn);

    VOID                 ReadFile           (const char *  szFileIn);

    VOID                 ReadDirectory      (const char *  szRelativePathIn,
                                             const char *  szGlobMatchIn);

VOID  ConfigDeck::DeleteAllLayers (VOID)

*/



//------------------------------------------------------------------------------
TEST (ConfigLayer, Prefix)
  {
  ValueRegistrySimple  registry;

  ConfigDeck::Init ();

  ConfigDeck *  pDeck = new ConfigDeck;
  pDeck->pReg = &registry;

  RStr   strBuffer = R"""(
    [
      {
      "name" : "config_name",
      "prefix" : "Prefix.",
      "variables" :
        {
        "bool:Var.Bool" : true,
        "int:Var.Two" : 2,
        "int:Var.Four" : 4,
        },
      "segments" :
        [
        {
        "name" : "SegA",
        "prereq" : "true",
        "variables" :
          {
          "int:Var.Two" : 3
          }
        },
        {
        "name" : "SegB",
        "prereq" : "false",
        "variables" :
          {
          "int:Var.Two" : 4
          }
        }
        ]
      }
    ]
    )""";


  pDeck->ReadBuffer ("", "rcfg", strBuffer.AsChar ());
  ASSERT_EQ (pDeck->pReg, &registry);

  //registry.DebugPrint ();

  // test prefix at base level
  ASSERT_EQ (registry.GetInt    ("Var.Four"), 0);
  ASSERT_EQ (registry.GetInt    ("Prefix.Var.Four"), 4);
  ASSERT_TRUE (registry.GetBool ("Prefix.Var.Bool"));

  // test prefix at segment level.
  ASSERT_EQ (registry.GetInt    ("Var.Two"), 0);
  ASSERT_EQ (registry.GetInt    ("Prefix.Var.Two"), 3);

  delete (pDeck);
  };

