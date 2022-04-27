#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Gfx/Anim.hpp"
#include "Util/CalcHash.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md




//------------------------------------------------------------------------------
TEST (AnimManager, AnimClipBase)
  {

  AnimClipBase *  pBase = new AnimClipBase ("Test.Anim");

  ASSERT_TRUE (pBase != NULL);

  // AnimClipBase   (AnimClipBase *  pLibClipIn);
  // AnimClipBase * Ref            (VOID)                          {return ((pLibraryClip == NULL) ? this : pLibraryClip);};

  ASSERT_STREQ (pBase->Name (), "Test.Anim");

  pBase->SetName ("Test.Anim.2");
  ASSERT_STREQ (pBase->Name (), "Test.Anim.2");
  ASSERT_EQ (pBase->Type (),    MAKE_FOUR_CODE ("BASE"));
  ASSERT_EQ (pBase->NameHash (), CalcHashValue ("Test.Anim.2"));

  const char *  szTestLibrary = "res://gfx/test.anim";

  pBase->SetLibraryFile (szTestLibrary);
  ASSERT_STREQ (pBase->GetLibraryFile (), szTestLibrary);
  ASSERT_EQ    (pBase->LibNameHash (), CalcHashValue (szTestLibrary));
  ASSERT_TRUE  (pBase->InLibraryFile (CalcHashValue (szTestLibrary)));

  // NOTE: Test time related stuff on instances of AnimClipBase, because the
  //  base doesn't have enough time related functionality.

  delete (pBase);
  };

//------------------------------------------------------------------------------
AnimClipCurve *  CreateTestCurveClip (const char *  szClipNameIn,
                                      const char *  szAttrNameIn)
  {
  RVec3  vecPnt;

  AnimClipCurve *  pCurveClip = new AnimClipCurve (szClipNameIn);

  MappedCurve *   pCurve = pCurveClip->NewCurve (szAttrNameIn);

  pCurve->AddPoint (vecPnt.Set (1.0f, 1.0f));
  pCurve->AddPoint (vecPnt.Set (2.0f, 2.0f));

  return (pCurveClip);
  };

//------------------------------------------------------------------------------
TEST (AnimManager, AnimChan)
  {
  const char *  szChanName = "TestChan";
  const char *  szClipName = "CurveA";
  const char *  szAttrName = "TestAttr";


  ValueRegistry *  pReg = new ValueRegistrySimple;

  AnimChan *  pChan = new AnimChan (szChanName);

  ASSERT_TRUE (pChan != NULL);

  AnimManager::Instance()->SetValueRegistry (pReg);
  ASSERT_STREQ (pChan->Name (), szChanName);
  ASSERT_EQ    (pChan->NameHash (), CalcHashValue (szChanName));

  // test time without curves
  pChan->SetTime (2.0f);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetTime (), 2.0f));

  pChan->IncTime (0.2f, pReg);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetTime (), 2.2f));

  pChan->IncTime (20.f, pReg);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetTime (), 22.2f));

  pChan->Pause ();
  pChan->IncTime (20.f, pReg);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetTime (), 22.2f));

  pChan->Play ();
  pChan->IncTime (20.f, pReg);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetTime (), 42.2f));


  // test time with curves

  // directly add a curve clip to the channel
  AnimClipCurve *  pCurveClip = CreateTestCurveClip (szClipName, szAttrName);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pCurveClip->GetLength (), 2.0f));


  pChan->PushClipBack (pCurveClip);

  ASSERT_EQ (pChan->GetClipByIndex (0), pCurveClip);
  ASSERT_EQ (pChan->NumAnims (), 1);


  pChan->SetTime (0.0f);

  // check how curves affect channel
  ASSERT_TRUE (FLT_APPROX_EQUAL (pChan->GetLength (), 2.0f));

  pChan->Clear ();
  ASSERT_EQ (pChan->NumAnims (), 0);

  pCurveClip = CreateTestCurveClip (szClipName, szAttrName);
  pChan->PushClipFront (pCurveClip);

  ASSERT_EQ (pChan->GetClipByIndex (0), pCurveClip);
  ASSERT_EQ (pChan->NumAnims (), 1);

  pChan->Stop ();
  ASSERT_EQ (pChan->NumAnims (), 0);


  pChan->Clear ();
  ASSERT_EQ (pChan->NumAnims (), 0);


  delete (pChan);
  delete (pReg);
  /*
    AnimClipBase *   GetAnimByIndex  (INT     iIndexIn);

    FLOAT            GetLength       (VOID); // returns the end time of the last clip

    VOID             Skip            (ValueRegistry *  pRegIn);  // jump time ahead to the end of the current clip

    VOID             FForward        (FLOAT  fSpeedMultIn);  // play at a different speed for the remainder of the channel

    VOID             FForwardClip    (FLOAT  fSpeedMultIn);  // play at a different speed for the duration of the current clip
  */

  };

//------------------------------------------------------------------------------
TEST (AnimManager, AnimManager)
  {
  RVec3            vecPnt;
  ValueRegistry *  pReg = new ValueRegistrySimple;


  AnimManager::Instance()->SetValueRegistry (pReg);

  // Test library clip creation
  AnimClipBase *  pclipNew = AnimManager::NewLibraryClip ("curve",
                                                          "TestClip",
                                                          "res://gfx/clips/test.anim");

  ASSERT_FALSE (pclipNew == NULL);

  AnimClipBase *  pclipSearch = AnimManager::FindLibraryClip (CalcHashValue ("TestClip"));

  ASSERT_TRUE (pclipSearch == pclipNew);

  AnimClipCurve *  pclipCurve = dynamic_cast<AnimClipCurve*>(pclipSearch);
  MappedCurve *    pCurve     = pclipCurve->NewCurve ("TestAttr");
  pCurve->AddPoint (vecPnt.Set (1.0f, 1.0f));
  pCurve->AddPoint (vecPnt.Set (2.0f, 2.0f));
  pCurve->AddPoint (vecPnt.Set (3.0f, 3.0f));


  // test

  ASSERT_EQ (AnimManager::Instance()->NumChans (), 0);
  //AnimChan *  pChanUI =
  AnimManager::Instance()->NewChan ("UI");

  ASSERT_EQ (AnimManager::Instance()->NumChans (), 1);
  //AnimChan *  pChanAnim =
  AnimManager::Instance()->NewChan ("Character");

  ASSERT_EQ (AnimManager::Instance()->NumChans (), 2);

  ASSERT_EQ (AnimManager::Instance()->NumAnims ("UI"), 0);

  pReg->SetFloat ("UI.TestAttr", 1.0f);

  AnimManager::Instance()->PlayClip ("TestClip", // szAnimNameIn,
                                     "UI",       // szChanNameIn,
                                     1.0f        // fDelaySecIn
                                     // fSpeedIn     = 1.0f,
                                     // eLoopingIn   = EAnim::kLoopTypeNone,
                                     // eQueueIn     = EAnim::kQueuePosBack,
                                     // iLoopCountIn = -1,
                                     // fFadeInSec   = 0,
                                     // fFadeOutSec  = 0
                                    );
  ASSERT_EQ (AnimManager::Instance()->NumAnims ("UI"), 1);
  ASSERT_EQ (AnimManager::Instance()->NumAnims ("Character"), 0);

  ASSERT_EQ (AnimManager::Instance()->GetChanLengthSec     ("UI"),    4.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipLengthSec     (0, "UI"), 3.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipRemainingSec  (0, "UI"), 4.0f);
  ASSERT_EQ (AnimManager::Instance()->GetChanRemainingSec  ("UI"),    4.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipTimeSec       (0, "UI"), 0.0f);
  ASSERT_EQ (AnimManager::Instance()->GetChanTimeSec       ("UI"),    0.0f);

  AnimManager::Instance()->IncTime (1.0f);

  ASSERT_EQ (AnimManager::Instance()->GetChanLengthSec     ("UI"),    4.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipLengthSec     (0, "UI"), 3.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipRemainingSec  (0, "UI"), 3.0f);
  ASSERT_EQ (AnimManager::Instance()->GetChanRemainingSec  ("UI"),    3.0f);
  ASSERT_EQ (AnimManager::Instance()->GetClipTimeSec       (0, "UI"), 1.0f);
  ASSERT_EQ (AnimManager::Instance()->GetChanTimeSec       ("UI"),    1.0f);

  ASSERT_TRUE (FLT_APPROX_EQUAL (pReg->GetFloat ("UI.TestAttr"), 1.0f));

  AnimManager::Instance()->IncTime (0.5f);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pReg->GetFloat ("UI.TestAttr"), 1.5f));
  AnimManager::Instance()->IncTime (0.5f);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pReg->GetFloat ("UI.TestAttr"), 2.0f));

  AnimManager::Instance()->PauseChan            ("UI");
  AnimManager::Instance()->IncTime (0.5f);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pReg->GetFloat ("UI.TestAttr"), 2.0f));

  AnimManager::Instance()->PlayChan             ("UI");
  AnimManager::Instance()->IncTime (0.5f);
  ASSERT_TRUE (FLT_APPROX_EQUAL (pReg->GetFloat ("UI.TestAttr"), 2.5f));



/*
  DBG_INFO ("%f %f %f %f %f %f",
    AnimManager::Instance()->GetClipLengthSec     (0, "UI"),
    AnimManager::Instance()->GetChanLengthSec     ("UI"),
    AnimManager::Instance()->GetClipRemainingSec  (0, "UI"),
    AnimManager::Instance()->GetChanRemainingSec  ("UI"),
    AnimManager::Instance()->GetClipTimeSec       (0, "UI"),
    AnimManager::Instance()->GetChanTimeSec       ("UI"));
*/




  AnimManager::DestroyInstance();

  // only delete registry after AnimManager has been cleaned up, so that attachments can
  //   be cleaned up.
  delete (pReg);
  };

  /*
    // AnimManager
    // XFade : (can scale chans separeately, and sum all results)

    VOID                   StopChan             (const char *  szChanNameIn);

    VOID                   SkipClip             (const char *  szChanNameIn);  // skip current clip to next one

    VOID                   FForwardChan         (const char *  szChanNameIn,
                                                 FLOAT         fSpeedMultIn); // possibly also as time, rather than speed mult.

    VOID                   FForwardClip         (const char *  szChanNameIn,
                                                 FLOAT         fSpeedMultIn); // possibly also as time, rather than speed mult.

    UINT32                 GetAnimByIndex       (INT           iIndexIn,
                                                 const char *  szChanNameIn);

    UINT32                 GetChanByIndex       (INT           iIndexIn);

    VOID                   ClearChan            (const char *  szChanNameIn);  // remove all clips from channel

    VOID                   DeleteChan           (const char *  szChanNameIn);  // clear chan, and then delete it from list of channels

    VOID                   DeleteAllChans       (VOID);

    static VOID            UnloadClipLibrary    (const char *  szLibNameIn);

    VOID                   BuildLibraryList     (RStrArray &     arrayLibsOut);

    AnimClipBase *         GetNextClipInLib     (UINT32          uLibNameHashIn,
                                                 AnimClipBase *  pPrevIn);
  */


  /*
  AnimClipBase calls
  public:
    // These calls are made on clips in the library

                   AnimClipBase   (const char *    szAnimNameIn);

                   AnimClipBase   (AnimClipBase *  pLibClipIn);

    AnimClipBase * Ref            (VOID)                          {return ((pLibraryClip == NULL) ? this : pLibraryClip);};

    VOID           SetLoopType    (EAnim::LoopType    eTypeIn)    {eLoopType = eTypeIn;};

    VOID           SetLoopCount   (INT                iCountIn)   {iTimesToLoop = iCountIn;};

    VOID           SetRangeLeft   (EAnim::OutOfRange  eOutIn)     {ASSERT (pLibraryClip == NULL);
                                                                   eOutOfRangeLeft  = eOutIn;};

    VOID           SetRangeRight  (EAnim::OutOfRange  eOutIn)     {ASSERT (pLibraryClip == NULL);
                                                                   eOutOfRangeRight = eOutIn;};

    virtual AnimClipBase *  LinkedInstance        (VOID) const = 0;

    VOID           SetSpeed       (FLOAT  fSpeedIn)          {fSpeed = fSpeedIn;};

    virtual FLOAT  GetLength      (VOID)                     {return 0.0f;};  // returns the length of a single loop

    VOID           SetStartSec    (FLOAT  fStartIn)          {fStartSec = fStartIn;};

    FLOAT          GetStartSec    (VOID)                     {return fStartSec;};

    FLOAT          GetEndSec      (VOID);

    FLOAT          GetTime        (VOID)                     {return fTime;};

    VOID           SetTime        (FLOAT  fTimeIn)           {fTime = fTimeIn;};

    virtual VOID   IncTime        (FLOAT            fTimeDeltaIn,
                                   ValueRegistry *  pRegIn);

    FLOAT          GetClippedTime (VOID);  // clipped and transformed to 0.0f - GetEndSec () range.

    BOOL           IsTimeInClip   (VOID);

    BOOL           IsBeforeClip   (VOID);

    BOOL           IsAfterClip    (VOID);

    VOID           SetFadeIn      (FLOAT  fSecIn)   {fFadeIn  = fSecIn;};

    VOID           SetFadeOut     (FLOAT  fSecIn)   {fFadeOut = fSecIn;};

    FLOAT          GetFadeIn      (VOID)            {return (fFadeIn);};

    FLOAT          GetFadeOut     (VOID)            {return (fFadeOut);};

    virtual VOID   PlaybackInit   (const char *     szChannelNameIn,
                                   ValueRegistry *  pRegIn)              {};

    VOID           SetOnDoneExpr  (const char *  pExpIn);

    VOID           OnClipDone     (ValueRegistry *  pRegIn);
  };
  */
