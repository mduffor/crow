#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkScript.hpp"
#include "Script/InkParser.hpp"
#include "Script/InkExecute.hpp"
#include "Script/InkScriptManager.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
VOID InkScriptManagerTest_InitDepot (ContentDepot &  depotIn)
  {

  RStr  strScriptA (R"""(
 === knotA ===
 This is line of text A.

 === knotB ===
 This is line of text B.

 === function myFn ===
 This is my Function
 ~return

 )""");

  RStr  strScriptB (R"""(
 === knotA ===
 This is line of text A.

 === knotB ===
 This is line of text B.

 === function myFnTrue ===
 This is my Function
 ~ myVar = true;
 ~ return

 === function myFnFalse ===
 This is my Function
 ~ myVar = false;
 ~ return

 )""");


  depotIn.AddAsset ("ScriptA",
                  "ink",
                  "default",
                  "mem://ScriptA.ink",
                  &strScriptA);
  depotIn.AddAsset ("ScriptB",
                  "ink",
                  "default",
                  "mem://ScriptB.ink",
                  &strScriptB);

  depotIn.SetEnvironment ("dev");

  depotIn.Refresh(TRUE);
  };


//------------------------------------------------------------------------------
TEST (InkScriptManager, LoadUnload)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  // init before attach, so all scripts aren't loaded automatically
  InkScriptManagerTest_InitDepot (depot);

  InkScriptManager *  pManager = new InkScriptManager (&depot, &registry);


  // Test Load & GetScriptList
  pManager->LoadScript ("ScriptA");

  ASSERT_EQ (arrayScripts.Length(), 0);
  pManager->GetScriptList (arrayScripts);

  ASSERT_EQ (arrayScripts.Length(), 1);
  ASSERT_STREQ (arrayScripts[0].AsChar (), "ScriptA");


  // Test Unload
  pManager->UnloadScript ("ScriptA");

  arrayScripts.Clear();
  pManager->GetScriptList (arrayScripts);

  ASSERT_EQ (arrayScripts.Length(), 0);
  delete (pManager);
  };

//------------------------------------------------------------------------------
TEST (InkScriptManager, UpdateScriptList)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;

  // init before attach, so all scripts aren't loaded automatically
  InkScriptManagerTest_InitDepot (depot);

  InkScriptManager *  pManager = new InkScriptManager (&depot, &registry);

  // Test UpdateScriptList
  arrayScripts.Clear();
  arrayScripts.Append ("ScriptA");
  arrayScripts.Append ("ScriptB");

  pManager->UpdateScriptList (arrayScripts);

  arrayScripts.Clear();
  pManager->GetScriptList (arrayScripts);
  ASSERT_EQ (arrayScripts.Length(), 2);

  delete (pManager);
  };

//------------------------------------------------------------------------------
TEST (InkScriptManager, Execution)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  InkScriptManager *  pManager = new InkScriptManager (&depot, &registry);

  // go ahead and load all scripts.
  InkScriptManagerTest_InitDepot (depot);

  // Test Execution

  InkExecute *  pExec = pManager->RequestExecution ("ScriptB", "knotB");
  ASSERT_STREQ (pExec->Continue (), "This is line of text B.");
  pManager->ReleaseExecution (pExec);

  registry.SetBool ("myVar", FALSE);

  pManager->RunFunctionToEnd ("ScriptB", "myFnTrue");

  ASSERT_EQ (registry.GetBool ("myVar"), TRUE);

  delete (pManager);
  };


