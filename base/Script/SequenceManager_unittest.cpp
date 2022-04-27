#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkScript.hpp"
#include "Script/InkParser.hpp"
#include "Script/InkExecute.hpp"
#include "Script/InkScriptManager.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"
#include "Script/SequenceManager.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
VOID SequenceManagerTest_InitDepot (ContentDepot &  depotIn)
  {

  RStr  strScriptA (R"""(
 === knotA ===
 This is line of ((tag myTag)) text A.

 === knotB ===
 This is line of text B.

 === function myFn ===
 This is my Function
 ~return

 )""");

  RStr  strScriptB (R"""(
 === knotC ===
 This is line of text C.

 === knotD ===
 This is line of text D.

 === knotE ===
 ~ myVar = true;
 This is my Function
 )""");

  RStr  strScriptC (R"""(
  === knotF ===
  Line One
  * Choice [One]is one.
  * Choice Two[] is chosen.
    Line after Choice Two

  === knotG ===
  Line Four
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
  depotIn.AddAsset ("ScriptC",
                    "ink",
                    "default",
                    "mem://ScriptC.ink",
                    &strScriptC);

  depotIn.SetEnvironment ("dev");
  depotIn.Refresh(TRUE);
  };


static RStr               strLastName;
static RStr               strLastLine;
static RStrArray          astrLastChoices;
static SequenceManager *  pSeqStatic;
static RStr               strLastTag;

//------------------------------------------------------------------------------
VOID SequenceManagerTest_Say (Token *          ptokParamsIn,
                              ValueRegistry *  pRegistryIn,
                              Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens < 2) return;

  strLastName.Set (ptokParamsIn->AsString (pRegistryIn));
  strLastLine.Set (ptokParamsIn->Next()->AsString (pRegistryIn));

  astrLastChoices.Clear ();
  Token *  pParam = ptokParamsIn->Next()->Next();
  for (INT  iIndex = 2; iIndex < iNumTokens; ++iIndex)
    {
    astrLastChoices.Append (pParam->AsString (pRegistryIn));
    pParam = pParam->Next();
    };

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set (0);

  if (iNumTokens == 2)
    {
    // Text without choices.  Use "Continue" mechanic to advance.
    if (pSeqStatic->NeedsContinue ())
      {
      // called once already.  React to the "Continue" status
      if (pSeqStatic->CanContinue())
        {
        ptokResultOut->Set (1);
        pSeqStatic->ClearContinue();
        }
      }
    else
      {
      // first time through.  Enable the "Continue" mechanic
      pSeqStatic->SetNeedsContinue (TRUE);
      return;
      };
    }
  else
    {
    // Text with choices.  Use "Choice" mechanic to advance.

    if (pSeqStatic->NeedsChoice ())
      {
      // called once already.  React to the "Choice" status
      if (pSeqStatic->HasMadeChoice())
        {
        ptokResultOut->Set (1);
        pSeqStatic->SetNeedsChoice (FALSE);
        }
      }
    else
      {
      // first time through.  Enable the "Choice" mechanic
      pSeqStatic->SetNeedsChoice (TRUE);
      return;
      };
    };
  };

//------------------------------------------------------------------------------
VOID SequenceManagerTest_Tag (Token *          ptokParamsIn,
                              ValueRegistry *  pRegistryIn,
                              Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  strLastTag.Set (ptokParamsIn->AsString (pRegistryIn));

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set (1);
  };

//------------------------------------------------------------------------------
TEST (SequenceManager, RunAndContinue)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  InkScriptManager * pManager = new InkScriptManager (&depot, &registry);
  SequenceManager *  pSeq     = new SequenceManager  (pManager);

  pSeqStatic = pSeq;

  // load all scripts
  SequenceManagerTest_InitDepot (depot);

  pManager->GetScriptList (arrayScripts);
  ASSERT_EQ (arrayScripts.Length(), 3);

  pSeq->RegisterFunction ("ScriptSay", &SequenceManagerTest_Say);


  // Test RunScript
  registry.SetBool ("myVar", FALSE);

  pSeq->RunScript ("ScriptB", "knotD");

  ASSERT_EQ (registry.GetBool ("myVar"), FALSE);

  // Sub-test : make sure the script executes the "ScriptSay" command
  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "This is line of text D.");

  // Sub-test : make sure we block on "Continue"
  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "This is line of text D.");


  // Sub-test : make sure we un-block on "Continue"
  pSeq->TriggerContinue();
  pSeq->Update ();
  ASSERT_STREQ  (strLastLine.AsChar (), "This is my Function");
  ASSERT_EQ     (registry.GetBool ("myVar"), TRUE);

  pSeq->TriggerContinue();
  pSeq->Update ();

  ASSERT_EQ (pSeq->GetNumExecutions (), 0);
  ASSERT_EQ (pSeq->GetNumCommands   (), 0);

  delete pSeq;
  delete pManager;
  pSeqStatic = NULL;
  };

//------------------------------------------------------------------------------
TEST (SequenceManager, RunAndChoose)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  InkScriptManager * pManager = new InkScriptManager (&depot, &registry);
  SequenceManager *  pSeq     = new SequenceManager  (pManager);

  pSeqStatic = pSeq;

  // load all scripts
  SequenceManagerTest_InitDepot (depot);

  pManager->GetScriptList (arrayScripts);
  ASSERT_EQ (arrayScripts.Length(), 3);

  pSeq->RegisterFunction ("ScriptSay", &SequenceManagerTest_Say);


  // Test RunScript
  pSeq->RunScript ("ScriptC", "knotF");

  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "Line One");

  // Sub-test : make sure we block on "Choice"
  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "Line One");

  // Sub-test : make sure we have our choices.

  ASSERT_EQ (astrLastChoices.Length (), 2);
  ASSERT_STREQ (astrLastChoices[0].AsChar(), "Choice One");
  ASSERT_STREQ (astrLastChoices[1].AsChar(), "Choice Two");


  // Sub-test : make sure we un-block on "Choice"
  pSeq->MakeChoice(0);
  pSeq->Update ();
  //ASSERT_STREQ  (strLastLine.AsChar (), "Choice is one.");

  //pSeq->TriggerContinue();
  //pSeq->Update ();
  ASSERT_STREQ  (strLastLine.AsChar (), "Line Four");

  pSeq->TriggerContinue();
  pSeq->Update ();

  ASSERT_EQ (pSeq->GetNumExecutions (), 0);
  ASSERT_EQ (pSeq->GetNumCommands   (), 0);

  // Test Choice 2
  pSeq->RunScript ("ScriptC", "knotF");

  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "Line One");

  // Sub-test : make sure we have our choices.
  ASSERT_EQ (astrLastChoices.Length (), 2);
  ASSERT_STREQ (astrLastChoices[0].AsChar(), "Choice One");
  ASSERT_STREQ (astrLastChoices[1].AsChar(), "Choice Two");


  // Sub-test : make sure we un-block on "Choice"
  pSeq->MakeChoice(1);
  pSeq->Update ();
  //ASSERT_STREQ  (strLastLine.AsChar (), "Choice Two is chosen.");

  //pSeq->TriggerContinue();
  //pSeq->Update ();
  ASSERT_STREQ  (strLastLine.AsChar (), "Line after Choice Two");

  pSeq->TriggerContinue();
  pSeq->Update ();
  ASSERT_STREQ  (strLastLine.AsChar (), "Line Four");

  pSeq->TriggerContinue();
  pSeq->Update ();

  ASSERT_EQ (pSeq->GetNumExecutions (), 0);
  ASSERT_EQ (pSeq->GetNumCommands   (), 0);

  delete pSeq;
  delete pManager;
  pSeqStatic = NULL;
  };



//------------------------------------------------------------------------------
TEST (SequenceManager, ExpressionFn)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;

  InkScriptManager * pManager = new InkScriptManager (&depot, &registry);

  // Test: Instance, CreateInstance, DestroyInstance
  SequenceManager::CreateInstance (pManager);
  SequenceManager *  pSeq     = SequenceManager::Instance();
  pSeqStatic = pSeq;

  // load all scripts
  SequenceManagerTest_InitDepot (depot);

  pSeq->RegisterFunction ("ScriptSay", &SequenceManagerTest_Say);

  // Test RunScript
  pSeq->RunScript ("ScriptC", "knotF");

  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "Line One");

  // Make sure we block on "Choice"
  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "Line One");

  // Make sure we have our choices.
  ASSERT_EQ (astrLastChoices.Length (), 2);
  ASSERT_STREQ (astrLastChoices[0].AsChar(), "Choice One");
  ASSERT_STREQ (astrLastChoices[1].AsChar(), "Choice Two");


  // Sub-test : make sure we un-block on "Choice"
  Expression::Execute ("ScriptChoose(0);");
  pSeq->Update ();
  //ASSERT_STREQ  (strLastLine.AsChar (), "Choice is one.");

  //Expression::Execute ("ScriptContinue();");
  //pSeq->Update ();
  ASSERT_STREQ  (strLastLine.AsChar (), "Line Four");

  Expression::Execute ("ScriptContinue();");
  pSeq->Update ();

  ASSERT_EQ (pSeq->GetNumExecutions (), 0);
  ASSERT_EQ (pSeq->GetNumCommands   (), 0);

  SequenceManager::DestroyInstance ();
  delete pManager;
  pSeqStatic = NULL;
  };


//------------------------------------------------------------------------------
TEST (SequenceManager, Tags)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  InkScriptManager * pManager = new InkScriptManager (&depot, &registry);
  SequenceManager *  pSeq     = new SequenceManager  (pManager);

  pSeqStatic = pSeq;

  // load all scripts
  SequenceManagerTest_InitDepot (depot);

  pSeq->RegisterFunction ("ScriptSay", &SequenceManagerTest_Say);
  pSeq->RegisterFunction ("tag", &SequenceManagerTest_Tag);

  pSeq->RunScript ("ScriptA", "knotA");

  pSeq->Update ();
  ASSERT_STREQ (strLastLine.AsChar (), "This is line of text A.");
  ASSERT_STREQ (strLastTag.AsChar (), "myTag");

  pSeq->TriggerContinue();
  pSeq->Update ();

  delete pSeq;
  delete pManager;
  pSeqStatic = NULL;
  };

//------------------------------------------------------------------------------
TEST (SequenceManager, Cleanup)
  {
  ValueRegistrySimple  registry;
  ContentDepot         depot;
  RStrArray            arrayScripts;


  InkScriptManager * pManager = new InkScriptManager (&depot, &registry);
  SequenceManager *  pSeq     = new SequenceManager  (pManager);

  pSeqStatic = pSeq;

  // load all scripts
  SequenceManagerTest_InitDepot (depot);

  pManager->GetScriptList (arrayScripts);
  ASSERT_EQ (arrayScripts.Length(), 3);

  pSeq->RegisterFunction ("ScriptSay", &SequenceManagerTest_Say);


  pSeq->RunScript ("ScriptB", "knotD");

  // Test releasing resources

  ASSERT_EQ (pSeq->GetNumExecutions (), 1);
  ASSERT_EQ (pSeq->GetNumCommands   (), 0);

  pSeq->Update ();
  ASSERT_EQ (pSeq->GetNumCommands   (), 1);

  pSeq->ReleaseExecutions ();
  ASSERT_EQ (pSeq->GetNumExecutions (), 0);


  pSeq->DeleteCommands ();
  ASSERT_EQ (pSeq->GetNumCommands (), 0);

  delete pSeq;
  delete pManager;
  pSeqStatic = NULL;
  };
