#include <gtest/gtest.h>


#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)


#include "ValueRegistry/ValueRegistry.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"
#include "ValueRegistry/ContentDepot.hpp"


// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md



//------------------------------------------------------------------------------
class ContentDepotUnitTest
  {
  public:
    BOOL  bDepLoadedA;
    BOOL  bDepLoadedB;
    BOOL  bDepLoadedC;

  public:
    VOID OnDataLoad   (const char *        szNameIn,
                       const char *        szTypeIn,
                       const RStrParser&   dataIn);

    VOID OnDataLoadDep (const char *        szNameIn,
                        const char *        szTypeIn,
                        const RStrParser&   dataIn);

    VOID OnMemoryLoad  (const char *        szNameIn,
                        const char *        szTypeIn,
                        const RStrParser&   dataIn);

  };

//------------------------------------------------------------------------------
VOID ContentDepotUnitTest::OnDataLoad (const char *        szNameIn,
                                       const char *        szTypeIn,
                                       const RStrParser&   dataIn)
  {
  //printf ("Got file of type %s with name %s\n", szTypeIn, szNameIn);
  ASSERT_STREQ (szTypeIn, "txt");
  ASSERT_STREQ (dataIn.AsChar (), "Hello World");
  };

//------------------------------------------------------------------------------
VOID ContentDepotUnitTest::OnDataLoadDep (const char *        szNameIn,
                                          const char *        szTypeIn,
                                          const RStrParser&   dataIn)
  {
  //printf ("Dep Load: file of type %s with name %s\n", szTypeIn, szNameIn);
  ASSERT_STREQ (szTypeIn, "txt");

  RStr  strName (szNameIn);

  if (strName.Equals ("AssetA"))  bDepLoadedA = TRUE;
  if (strName.Equals ("AssetB"))
    {
    bDepLoadedB = TRUE;
    // make sure B was loaded before A
    ASSERT_FALSE (bDepLoadedA);
    };
  if (strName.Equals ("AssetC"))
    {
    bDepLoadedC = TRUE;
    // make sure C was loaded before A
    ASSERT_FALSE (bDepLoadedA);
    }
  };

//------------------------------------------------------------------------------
VOID ContentDepotUnitTest::OnMemoryLoad (const char *        szNameIn,
                                         const char *        szTypeIn,
                                         const RStrParser&   dataIn)
  {
  ASSERT_STREQ (szTypeIn, "txt");
  ASSERT_STREQ (szNameIn, "AssetOne");
  ASSERT_STREQ (dataIn.AsChar (), "Hello World");
  };



//------------------------------------------------------------------------------
TEST (ContentDepot, AddRemove)
  {

  ContentDepot   depot;

  ASSERT_EQ (depot.Size (), 0);

  // Test adding assets
  depot.AddAsset ("AssetOne",
                  "rcfg",
                  "dev",
                  "http://www.muckyrobot.com/file.rcfg");

  ASSERT_EQ (depot.Size (), 1);

  // Test multiple assets
  depot.AddAsset ("AssetTwo",
                  "rcfg",
                  "dev",
                  "http://www.muckyrobot.com/file2.rcfg");

  ASSERT_EQ (depot.Size (), 2);

  // Test FindAsset
  ContentLocation * pLoc = depot.FindAsset ("AssetOne");

  ASSERT_NE (pLoc, nullptr);

  ASSERT_STREQ (pLoc->strTag.AsChar(), "AssetOne");

  // Test adding another env+url to existing
  depot.AddAsset ("AssetTwo",
                  "rcfg",
                  "prod",
                  "http://www.muckyrobot.com/file2.rcfg");

  ASSERT_EQ (depot.Size (), 2);

  // Test removing asset
  depot.RemoveAsset ("AssetTwo");
  ASSERT_EQ (depot.Size (), 1);

  // Test removing all assets
  depot.RemoveAllAssets ();

  ASSERT_EQ (depot.Size (), 0);
  };

//------------------------------------------------------------------------------
TEST (ContentDepot, Files)
  {

  ContentDepot   depot;

  // Test registering and loading a file.

  RStrParser  strParser ("Hello World");
  RStr        strPath ("file://UnitTestTemp.txt");

  strParser.WriteToFile (FilePath::ExpandPathURI (strPath.AsChar ()));

  depot.AddAsset ("AssetOne",
                  "txt",
                  "dev",
                  strPath.AsChar ());
  depot.strEnvironment = "dev";

  ContentDepotUnitTest  cdUnitTest;

  depot.sigOnDataLoad.Connect (&cdUnitTest, &ContentDepotUnitTest::OnDataLoad);


  //printf ("Calling Refresh(TRUE)\n");
  // test Refresh with force refresh

  // NOTE:  Behind the scenes, this will call (and thus test) RunCallbacks,
  //  StartAsyncLoad, OnAsyncLoadComplete, RetrieveData

  ASSERT_FALSE (depot.IsEverythingReturned ());

  depot.Refresh(TRUE);

  depot.sigOnDataLoad.Disconnect (&cdUnitTest, &ContentDepotUnitTest::OnDataLoad);

  ASSERT_TRUE (depot.IsEverythingReturned ());
  };

//------------------------------------------------------------------------------
TEST (ContentDepot, Dependencies)
  {

  ContentDepot   depot;

  // create some temp files to work with.
  RStrParser  strParserA ("Hello World");
  RStrParser  strParserB ("Hello Earth");
  RStrParser  strParserC ("Hello Mars");
  RStr        strPathA ("file://UnitTestTempA.txt");
  RStr        strPathB ("file://UnitTestTempB.txt");
  RStr        strPathC ("file://UnitTestTempC.txt");

  strParserA.WriteToFile (FilePath::ExpandPathURI (strPathA.AsChar ()));
  strParserB.WriteToFile (FilePath::ExpandPathURI (strPathB.AsChar ()));
  strParserC.WriteToFile (FilePath::ExpandPathURI (strPathC.AsChar ()));

  ASSERT_EQ (depot.Size (), 0);

  depot.strEnvironment = "dev";
  depot.AddAsset ("AssetA",
                  "txt",
                  depot.strEnvironment.AsChar (),
                  strPathA.AsChar ());

  depot.AddAsset ("AssetB",
                  "txt",
                  depot.strEnvironment.AsChar (),
                  strPathB.AsChar ());

  depot.AddAsset ("AssetC",
                  "txt",
                  depot.strEnvironment.AsChar (),
                  strPathC.AsChar ());

  ASSERT_EQ (depot.Size (), 3);

  RStrArray    arrayDepTags;
  arrayDepTags.Append ("AssetB");


  depot.AddDependency ("AssetA", "AssetC");

  depot.AddDependencies ("AssetA", arrayDepTags);
  depot.RecalcDependents ();

  ContentLocation *  pLocA = depot.FindAsset ("AssetA");
  ContentLocation *  pLocC = depot.FindAsset ("AssetC");

  // make sure assets were created correctly.
  ASSERT_NE (pLocA, nullptr);
  ASSERT_NE (pLocC, nullptr);

  ContentDepotUnitTest  cdUnitTest;

  cdUnitTest.bDepLoadedA = cdUnitTest.bDepLoadedB = cdUnitTest.bDepLoadedC = FALSE;

  depot.sigOnDataLoad.Connect (&cdUnitTest, &ContentDepotUnitTest::OnDataLoadDep);

  // we've registered dependencies, but haven't loaded them yet.
  ASSERT_FALSE (depot.AreDependenciesLoaded (pLocA));
  depot.Refresh(TRUE);
  // because all assets are file types, they should load upon Refresh
  ASSERT_TRUE (depot.AreDependenciesLoaded (pLocA));

  depot.sigOnDataLoad.Disconnect (&cdUnitTest, &ContentDepotUnitTest::OnDataLoadDep);

  // Make sure the dependencies were loaded
  ASSERT_TRUE (cdUnitTest.bDepLoadedA);
  ASSERT_TRUE (cdUnitTest.bDepLoadedB);
  ASSERT_TRUE (cdUnitTest.bDepLoadedC);

  TList<ContentLocation *>   listRoots;

  depot.GetRootDependants (pLocC, listRoots);

  // make sure dependencies are set up correctly.
  ASSERT_EQ (listRoots.Size(), 1);
  ASSERT_EQ ((*listRoots.First()), pLocA);
  };



//------------------------------------------------------------------------------
TEST (ContentDepot, Memory)
  {

  ContentDepot   depot;

  // Test registering and loading a file.

  RStrParser  strParser ("Hello World");
  RStr        strPath ("file://UnitTestTemp.txt");

  strParser.WriteToFile (FilePath::ExpandPathURI (strPath.AsChar ()));

  depot.AddAsset ("AssetOne",
                  "txt",
                  "dev",
                  "mem://AssetOne.txt",
                  &strParser);
  depot.strEnvironment = "dev";


  ContentDepotUnitTest  cdUnitTest;

  depot.sigOnDataLoad.Connect (&cdUnitTest, &ContentDepotUnitTest::OnMemoryLoad);

  depot.Refresh(TRUE);

  depot.sigOnDataLoad.Disconnect (&cdUnitTest, &ContentDepotUnitTest::OnMemoryLoad);

  ASSERT_TRUE (depot.IsEverythingReturned ());
  };


// TODO: Test Memory Assets
