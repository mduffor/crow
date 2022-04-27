

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Anim.hpp"
#include "Util/CalcHash.hpp"

AnimManager *         AnimManager::pInstance = NULL;
TList<AnimClipBase*>  AnimManager::listClipLibrary;

//=============================================================================
// Animation Clip Base
//=============================================================================

//-----------------------------------------------------------------------------
AnimClipBase::AnimClipBase (const char *  szAnimNameIn)
  {
  // used for creating clips stored in library.
  Init ();
  SetName (szAnimNameIn);
  };

//-----------------------------------------------------------------------------
AnimClipBase::AnimClipBase (AnimClipBase *  pLibClipIn)
  {
  // used for LinkedInstance()
  Init ();
  pLibraryClip = pLibClipIn;
  };


//-----------------------------------------------------------------------------
VOID AnimClipBase::Init (VOID)
  {

  uType            = MAKE_FOUR_CODE ("BASE");
  eLoopType        = EAnim::kLoopTypeNone;
  eOutOfRangeLeft  = EAnim::kOutOfRangeClamp;
  eOutOfRangeRight = EAnim::kOutOfRangeClamp;
  fTime            = 0.0f;
  fStartSec        = 0.0f;
  fSpeed           = 1.0f;
  iTimesToLoop     = -1;
  pLibraryClip     = NULL;
  fFadeIn          = 0.0f;
  fFadeOut         = 0.0f;
  uNameHash        = 0;

  pstrExprOnClipDone  = NULL;
  plistExprOnClipDone = NULL;
  };

//-----------------------------------------------------------------------------
AnimClipBase::~AnimClipBase ()
  {
  if (pstrExprOnClipDone  != NULL)
    {
    delete (pstrExprOnClipDone);
    };
  if (plistExprOnClipDone != NULL)
    {
    delete (plistExprOnClipDone);
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimClipBase::SetName  (const char *  szAnimNameIn)
  {
  ASSERT (pLibraryClip == NULL);
  if (szAnimNameIn == NULL)
    {
    strName.Set ("", TRUE);
    uNameHash = 0;
    }
  else
    {
    strName.Set (szAnimNameIn, TRUE);
    uNameHash = CalcHashValue (szAnimNameIn, strlen (szAnimNameIn));
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimClipBase::IncTime (FLOAT            fTimeDeltaIn,
                             ValueRegistry *  pRegIn)
  {
  ASSERT (pLibraryClip != NULL);
  // NOTE:  Keep this fast for both active and inactive clips, since it
  //  is called on every clip, every frame.

  fTime += fTimeDeltaIn * fSpeed;
  };

//-----------------------------------------------------------------------------
FLOAT  AnimClipBase::GetEndSec  (VOID)
  {
  ASSERT (pLibraryClip != NULL);
  if ((eLoopType == EAnim::kLoopTypeRepeat) || (eLoopType == EAnim::kLoopTypeBounce))
    {
    if (iTimesToLoop < 0)
      {
      // invalid for infinite looping types
      return (0.0f);
      };
    };

  INT  iNumLoops = RMax (1, iTimesToLoop);

  return (GetStartSec () + (iNumLoops * GetLength ()));
  };

//-----------------------------------------------------------------------------
FLOAT  AnimClipBase::GetClippedTime  (VOID)
  {
  ASSERT (pLibraryClip != NULL);

  BOOL   bBefore = IsBeforeClip ();
  BOOL   bAfter  = IsAfterClip ();

  if (bBefore)
    {
    return (0.0f);
    }
  else if (bAfter)
    {
    return (GetEndSec ());
    };

  FLOAT  fStart  = GetStartSec ();
  FLOAT  fEnd    = GetEndSec ();
  FLOAT  fLength = GetLength ();

  if (eLoopType == EAnim::kLoopTypeNone)
    {
    // one shot (iLoopCount should be 0 or 1);
    return (RMax (fStart, RMin (fEnd, fTime)));
    };

  if (FLT_APPROX_EQUAL (fLength, 0.0f)) {return (0.0f);};

  INT    iIteration = (INT) floorf ((fTime - fStart) / fLength);
  FLOAT  fClipTime  = (fTime - fStart) - (FLOAT (iIteration) * fLength);

  if (eLoopType == EAnim::kLoopTypeRepeat)
    {
    return (fClipTime);
    }
  else if (eLoopType == EAnim::kLoopTypeBounce)
    {
    return (((iIteration % 2) == 0) ? fClipTime : fEnd - fClipTime);
    };
  // we shouldn't reach here
  DBG_ERROR ("Unknown anim clip loop type");
  return (0.0f);
  };

//-----------------------------------------------------------------------------
BOOL  AnimClipBase::IsTimeInClip (VOID)
  {
  return (!IsBeforeClip () && !IsAfterClip ());
  };

//-----------------------------------------------------------------------------
BOOL  AnimClipBase::IsBeforeClip   (VOID)
  {
  ASSERT (pLibraryClip != NULL);
  return (fTime < fStartSec);
  };

//-----------------------------------------------------------------------------
BOOL  AnimClipBase::IsAfterClip    (VOID)
  {
  ASSERT (pLibraryClip != NULL);
  // NOTE:  Try to keep this fast, since it is called often.

  if (eLoopType == EAnim::kLoopTypeNone)
    {
    return (fTime > GetEndSec ());
    };
  // The looping types
  if (iTimesToLoop < 0) return (FALSE);

  INT  iCount = (INT) floorf ((fTime - fStartSec) / GetLength ());

  return (iCount > iTimesToLoop);
  };

//-----------------------------------------------------------------------------
VOID  AnimClipBase::SetOnDoneExpr  (const char *  pExpIn)
  {
  Ref()->pstrExprOnClipDone = new RStr (pExpIn);
  Ref()->plistExprOnClipDone = Expression::Compile (pExpIn);
  };

//-----------------------------------------------------------------------------
VOID AnimClipBase::OnClipDone (ValueRegistry *  pRegIn)
  {
  if (Ref()->pstrExprOnClipDone != NULL)
    {
    // REFACTOR:  You probably need to set some variables for the expression to
    //   read, such as current channel name.  You can do this once you actually
    //   need to use such functionality.

    // run the on clip done expression
    Expression::Execute (Ref()->plistExprOnClipDone, pRegIn);
    };
  };



//=============================================================================
// Animation Curve
//=============================================================================

//-----------------------------------------------------------------------------
MappedCurve::MappedCurve  (const char *  szMapIn)
  {
  strBaseMapping.Set (szMapIn, TRUE);
  pelemTarget = NULL;
  pRef        = NULL;
  };

//-----------------------------------------------------------------------------
MappedCurve::~MappedCurve  ()
  {
  DetachFromTarget (NULL);
  };

//-----------------------------------------------------------------------------
VOID MappedCurve::SetRef  (MappedCurve *  pcurveIn)
  {
  pRef           = pcurveIn;
  };

//-----------------------------------------------------------------------------
VOID MappedCurve::AttachToTarget (ValueElem *  pelemIn)
  {
  if (pelemTarget != NULL)
    {
    DetachFromTarget (NULL);
    }
  if (pelemIn != NULL)
    {
    pelemTarget = pelemIn;
    pelemTarget->sigOnDelete.Connect (this, &MappedCurve::DetachFromTarget);
    };
  };

//-----------------------------------------------------------------------------
VOID MappedCurve::DetachFromTarget (ValueElem *  pelemIn)
  {
  if (pelemTarget != NULL)
    {
    pelemTarget->sigOnDelete.Disconnect (this, &MappedCurve::DetachFromTarget);
    pelemTarget = NULL;
    };
  };

//-----------------------------------------------------------------------------
VOID  MappedCurve::SetTime  (FLOAT        fTimeIn)
  {
  // NOTE:  Time is not stored, but used immediately to push values out to
  //         mapped targets.
  if (pelemTarget != NULL)
    {
    RVec3  vecSolve = Ref()->GetPointOnCurve_X (fTimeIn);

    //DBG_INFO ("Setting curve %s to %f", pelemTarget->GetName (), vecSolve.fY);
    pelemTarget->SetFloat (vecSolve.fY, TRUE);
    };
  };


//=============================================================================
// Animation Clip Curve
//=============================================================================

//-----------------------------------------------------------------------------
AnimClipCurve::AnimClipCurve (const char *  szAnimNameIn) : AnimClipBase (szAnimNameIn)
  {
  uType = MAKE_FOUR_CODE("CURV");
  };

//-----------------------------------------------------------------------------
AnimClipCurve::AnimClipCurve (AnimClipBase *  pLibClipIn) : AnimClipBase (pLibClipIn)
  {
  };

//-----------------------------------------------------------------------------
AnimClipCurve::~AnimClipCurve ()
  {
  DeleteCurves ();
  };

//-----------------------------------------------------------------------------
AnimClipBase *  AnimClipCurve::LinkedInstance  (VOID)
  {
  ASSERT (pLibraryClip == NULL); // make sure the "this" ptr is to a library clip

  // REFACTOR:  This should eventually pull from a pool, not create a new clip
  //             each time.  Also make sure the clip is returned, not deleted
  //             when you are done with it.

  AnimClipCurve * pNew = new AnimClipCurve (dynamic_cast<AnimClipBase*>(this));

  // set instance-specific variables
  pNew->fStartSec    = fStartSec;
  pNew->iTimesToLoop = iTimesToLoop;
  pNew->fTime        = fTime;
  pNew->fSpeed       = fSpeed;

  for (TListItr<MappedCurve*>  itrCurr = listCurves.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    MappedCurve *  pcurveNew = new MappedCurve ((*itrCurr)->GetMapping ());
    pcurveNew->SetRef (*itrCurr);
    pNew->listCurves.PushBack (pcurveNew);
    }

  return pNew;
  };

//-----------------------------------------------------------------------------
MappedCurve *  AnimClipCurve::FindCurve  (const char *  szMapIn)
  {
  UINT32    uMapHash = CalcHashValue (szMapIn);

  for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
       itrCurve.IsValid ();
       ++itrCurve)
    {
    if ((*itrCurve)->HasMapping ())
      {
      if ((*itrCurve)->GetMappingHash () == uMapHash)
        {
        return (*itrCurve);
        };
      };
    };
  return NULL;
  };

//-----------------------------------------------------------------------------
VOID  AnimClipCurve::PlaybackInit  (const char *     szChannelNameIn,
                                    ValueRegistry *  pRegIn)
  {
  // map the curves

  // start with the channel name, followed by a period separator.
  UINT32  uChanHashIn = CalcHashValue (".", 1, HASH (szChannelNameIn));

  for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
       itrCurve.IsValid ();
       ++itrCurve)
    {
    // calc a hash with the chan name, the period separator, and then the channel mapping
    // if the channel was "bob" and the curve was "arm.x" the final hash would be
    //   off the string "bob.arm.x"
    if ((*itrCurve)->HasMapping ())
      {
      UINT32  uCurveNameHash = CalcHashValue ((*itrCurve)->GetMapping (),
                                              strlen ((*itrCurve)->GetMapping ()),
                                              uChanHashIn);

      ValueElem *  pelemTarget = pRegIn->Find (uCurveNameHash);

      if (pelemTarget != NULL)
        {
        (*itrCurve)->AttachToTarget (pelemTarget);
        // set initial value
        IncTime (0.0f, pRegIn);
        }
      else
        {
        // could not find value elem to map against.
        DBG_ERROR ("Unable to find value elem to map curve to : %s.%s",
                   szChannelNameIn,
                   (*itrCurve)->GetMapping ());
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimClipCurve::UnmapCurves (VOID)
  {
  for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
       itrCurve.IsValid ();
       ++itrCurve)
    {
    ValueElem *  pelemTarget = (*itrCurve)->GetTarget ();

    if (pelemTarget != NULL)
      {
      (*itrCurve)->DetachFromTarget (NULL);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimClipCurve::DeleteCurves  (VOID)
  {
  UnmapCurves ();
  for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
       itrCurve.IsValid ();
       ++itrCurve)
    {
    delete (*itrCurve);
    };
  listCurves.Empty ();
  };


//-----------------------------------------------------------------------------
VOID  AnimClipCurve::IncTime  (FLOAT            fTimeDeltaIn,
                               ValueRegistry *  pRegIn)
  {
  AnimClipBase::IncTime (fTimeDeltaIn, pRegIn);

  // tell each curve in the clip what time it is.
  if (IsTimeInClip ())
    {
    FLOAT  fTime = GetClippedTime ();

    for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
        itrCurve.IsValid ();
        ++itrCurve)
      {
      //DBG_INFO ("SetCurveTime %f", fTime);
      (*itrCurve)->SetTime (fTime);
      };
    };
  };


//-----------------------------------------------------------------------------
FLOAT  AnimClipCurve::GetLength  (VOID)
  {
  // assuming the points in each curve are ordered, find the greatest X value
  //  in all the last points

  FLOAT  fReturn = 0.0f;
  BOOL   bReturnSet = FALSE;

  for (TListItr<MappedCurve*> itrCurve = listCurves.First ();
       itrCurve.IsValid ();
       ++itrCurve)
    {
    INT  iNumKeys = (*itrCurve)->Ref()->GetNumKeys ();

    if (iNumKeys > 0)
      {
      if (!bReturnSet)
        {
        fReturn = (*itrCurve)->Ref()->GetPointTime (iNumKeys - 1);
        }
      else
        {
        fReturn = RMax (fReturn, (*itrCurve)->Ref()->GetPointTime (iNumKeys - 1));
        };
      };
    };
  return (fReturn);
  };


//=============================================================================
// Animation Channel
//=============================================================================

//-----------------------------------------------------------------------------
AnimChan::AnimChan (const char *  szChanNameIn)
  {
  strName.Set (szChanNameIn, TRUE);
  uNameHash = CalcHashValue (szChanNameIn, strlen (szChanNameIn));

  bDiscardPlayedClips = TRUE;
  bPaused             = FALSE;
  fFForwardScale      = 1.0f;
  bFForwardClip       = FALSE;
  fTime               = 0.0f;
  };

//-----------------------------------------------------------------------------
AnimChan::~AnimChan ()
  {
  DeleteAllClips ();
  };

//-----------------------------------------------------------------------------
VOID AnimChan::DeleteAllClips (VOID)
  {
  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       ++itrClip)
    {
    delete (*itrClip);
    };
  listClips.Empty();
  };

/*
//-----------------------------------------------------------------------------
AnimClipBase *  AnimChan::FindClip  (const char *  szAnimNameIn)
  {
  return (FindClip (CalcHashValue (szAnimNameIn, strlen(szAnimNameIn))));
  };

//-----------------------------------------------------------------------------
AnimClipBase *  AnimChan::FindClip  (UINT32  uAnimNameHashIn)
  {
  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       ++itrClip)
    {
    if ((*itrClip)->NameHash () == uAnimNameHashIn)
      {
      // found it
      return (*itrClip);
      };
    };
  return (NULL);
  };
*/

//-----------------------------------------------------------------------------
AnimClipBase *  AnimChan::GetClipByIndex  (INT           iIndexIn)
  {
  TListItr<AnimClipBase*>  itrSearch = listClips.AtIndex (iIndexIn);
  if (itrSearch.IsValid ())
    {
    return (*itrSearch);
    }
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::IncTime (FLOAT            fTimeDeltaIn,
                         ValueRegistry *  pRegIn)
  {
  if (bPaused)
    {
    return;
    };

  fTime += fTimeDeltaIn;

  TListItr<AnimClipBase*>  itrNext;
  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       itrClip = itrNext)
    {
    itrNext = itrClip;
    ++itrNext;

    BOOL  bStartBeforeEnd = ((*itrClip)->IsAfterClip ());

    (*itrClip)->IncTime (fTimeDeltaIn * fFForwardScale, pRegIn);

    // test to see if this increment moved us past the end of the clip
    if (bStartBeforeEnd && (*itrClip)->IsAfterClip ())
      {
      (*itrClip)->OnClipDone (pRegIn);

      if (bDiscardPlayedClips)
        {
        delete (*itrClip);
        listClips.Delete (itrClip);
        }
      else if (bFForwardClip)
        {
        bFForwardClip = FALSE;
        fFForwardScale = 1.0f;
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::SetTime (FLOAT  fTimeIn)
  {
  fTime = fTimeIn;

  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       ++itrClip)
    {
    (*itrClip)->SetTime (fTime);
    };
  };

//-----------------------------------------------------------------------------
FLOAT  AnimChan::GetLength  (VOID)
  {
  // returns the end time of the last clip

  TListItr<AnimClipBase*>  itrClip = listClips.First ();

  //if (itrClip.IsValid ())
  //  {
  //  DBG_INFO ("AnimChan::GetLength first clip is length %f", (*itrClip)->GetLength ());
  //  };

  FLOAT  fEndTime = itrClip.IsValid () ? (*itrClip)->GetLength () + (*itrClip)->GetStartSec () :
                                         0.0f;
  ++itrClip;
  for (;itrClip.IsValid (); ++itrClip)
    {
    fEndTime = RMax (fEndTime, (*itrClip)->GetLength () + (*itrClip)->GetStartSec ());
    };
  return (fEndTime);
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::Pause  (VOID)
  {
  // pause playback of current channel
  bPaused = TRUE;
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::Play  (VOID)
  {
  // start/resume playback of current channel
  bPaused = FALSE;
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::Stop  (VOID)
  {
  // stop current clip, discard it and all future clips
  Pause ();

  if (bDiscardPlayedClips)
    {
    Clear ();
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::Skip  (ValueRegistry *  pRegIn)
  {
  // jump time ahead to the end of the current clip

  // find the current clip

  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       ++itrClip)
    {
    FLOAT  fEndSec   = (*itrClip)->GetEndSec ();

    if (( !(*itrClip)->IsBeforeClip ()) &&
        ( !(*itrClip)->IsAfterClip ()) &&
        (fEndSec > 0.0f))
      {
      // found the clip to skip. If we are in multiple clips, then we just
      //  skip the length of the first one.  We can revisit this logic if
      //  later we find that we overlap clips and skip them in acutal use.
      IncTime (fEndSec - (*itrClip)->GetTime (), pRegIn);
      return;
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::FForward  (FLOAT  fSpeedMultIn)
  {
  // play at a different speed for the remainder of the channel
  fFForwardScale = fSpeedMultIn;
  bFForwardClip  = FALSE;
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::FForwardClip  (FLOAT  fSpeedMultIn)
  {
  // play at a different speed for the duration of the current clip
  fFForwardScale = fSpeedMultIn;
  bFForwardClip  = TRUE;
  };

//-----------------------------------------------------------------------------
VOID  AnimChan::Clear  (VOID)
  {
  // delete all clips from this channel.

  TListItr<AnimClipBase*>  itrNext;
  for (TListItr<AnimClipBase*>  itrClip = listClips.First ();
       itrClip.IsValid ();
       itrClip = itrNext)
    {
    itrNext = itrClip;
    ++itrNext;
    delete (*itrClip);
    listClips.Delete (itrClip);
    };
  };




//=============================================================================
// Animation Manager
//=============================================================================

//-----------------------------------------------------------------------------
AnimManager::AnimManager ()
  {
  pReg = ValueRegistry::Root ();
  };

//-----------------------------------------------------------------------------
AnimManager::~AnimManager ()
  {
  DeleteAllChans ();
  UnloadAllClipLibraries ();
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::IncTime   (FLOAT            fTimeDeltaIn)
  {
  // step through channels and increment time
  for (TListItr<AnimChan*>  itrChan = listChans.First ();
       itrChan.IsValid ();
       ++itrChan)
    {
    //DBG_INFO ("AnimManager::IncTime for channel");
    (*itrChan)->IncTime (fTimeDeltaIn, pReg);
    };
  };

//-----------------------------------------------------------------------------
AnimChan *  AnimManager::FindChan  (const char *  szChanNameIn)
  {
  return (FindChan (CalcHashValue (szChanNameIn, strlen(szChanNameIn))));
  };

//-----------------------------------------------------------------------------
AnimChan *  AnimManager::FindChan  (UINT32        uChanNameHashIn)
  {
  for (TListItr<AnimChan*>  itrChan = listChans.First ();
       itrChan.IsValid ();
       ++itrChan)
    {
    if ((*itrChan)->NameHash () == uChanNameHashIn)
      {
      // found it
      return (*itrChan);
      };
    };
  return (NULL);
  };

/*
//-----------------------------------------------------------------------------
AnimClipBase *  AnimManager::FindClip  (const char *  szAnimNameIn,
                                        const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL)  {return (NULL);};
  return (pChan->FindClip (szAnimNameIn));
  };

//-----------------------------------------------------------------------------
AnimClipBase *  AnimManager::FindClip  (UINT32        uAnimNameHashIn,
                                        UINT32        uChanNameHashIn)
  {
  AnimChan *  pChan = FindChan (uChanNameHashIn);
  if (pChan == NULL)  {return (NULL);};
  return (pChan->FindClip (uAnimNameHashIn));
  };
*/

//-----------------------------------------------------------------------------
AnimClipBase *  AnimManager::FindLibraryClip  (UINT32        uAnimNameHashIn)
  {

  for (TListItr<AnimClipBase*>  itrCurr = listClipLibrary.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->NameHash () == uAnimNameHashIn)
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::PlayClip  (const char *     szAnimNameIn,
                              const char *     szChanNameIn,
                              FLOAT            fDelaySecIn,
                              FLOAT            fSpeedIn,
                              EAnim::LoopType  eLoopingIn,
                              EAnim::QueuePos  eQueueIn,
                              INT              iLoopCountIn,
                              FLOAT            fFadeInSec,
                              FLOAT            fFadeOutSec)
  {
  // XFade : (can scale chans separately, and sum all results)

  AnimChan *  pChan = NULL;

  if ((szChanNameIn == NULL) || (szChanNameIn[0] == '\0'))
    {
    // no channel specified.
    // TODO: Anim with no chan go in default, unnamed chan.
    }
  else
    {
    pChan = FindChan (szChanNameIn);
    };

  // NOTE: For now, we will create the channel if it does not exist for simplicity.
  //  In practice, it may require that channels be explicitly created in which
  //  case we should throw an error if the channel does not exist.

  if (pChan == NULL)
    {
    // Named channel, that doesn't exist.
    pChan = NewChan (szChanNameIn);
    };

  UINT32  uAnimNameHash = CalcHashValue (szAnimNameIn, strlen (szAnimNameIn));
  AnimClipBase *  pLibClip = FindLibraryClip (uAnimNameHash);
  if (pLibClip == NULL) return;

  AnimClipBase *  pNewClip = pLibClip->LinkedInstance ();

  //DBG_INFO ("PlayClip %s LinkedInstance %x", szAnimNameIn, pNewClip);

  if (pNewClip == NULL)
    {
    return;
    };

  pNewClip->SetSpeed     (fSpeedIn);
  pNewClip->SetStartSec  (pChan->GetTime () + fDelaySecIn);
  pNewClip->SetLoopType  (eLoopingIn);
  pNewClip->SetLoopCount (iLoopCountIn);
  pNewClip->SetFadeIn    (fFadeInSec);
  pNewClip->SetFadeOut   (fFadeOutSec);

  switch (eQueueIn)
    {
    case EAnim::kQueuePosBack:
         pNewClip->SetStartSec  (pChan->GetLength () + fDelaySecIn);
         pChan->PushClipBack (pNewClip);
         break;

    case EAnim::kQueuePosXFadeBack:
         // TODO:  Find last clip, set its fade out to this fade in, adjust start of this clip
         break;

    case EAnim::kQueuePosFront:
         pChan->PushClipFront (pNewClip);
         break;

    case EAnim::kQueuePosOverlap:
         pChan->PushClipFront (pNewClip);
         break;

    case EAnim::kQueuePosReplace:
         pChan->Clear ();
         pChan->PushClipBack (pNewClip);
         break;
    };

  // map curves

  // NOTE:  This call can eventually be changed to take a hash instead of a string.
  pNewClip->PlaybackInit (pChan->Name (), pReg);
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::PauseChan  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->Pause ();
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::PlayChan  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->Play ();
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::StopChan  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->Stop ();
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::SkipClip  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->Skip (pReg);
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::FForwardChan  (const char *  szChanNameIn,
                                  FLOAT         fSpeedMultIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->FForward (fSpeedMultIn);
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::FForwardClip  (const char *  szChanNameIn,
                                  FLOAT         fSpeedMultIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->FForwardClip (fSpeedMultIn);
  };

//-----------------------------------------------------------------------------
AnimChan *  AnimManager::NewChan  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);

  // return if chan already exists
  if (pChan != NULL) return (pChan);

  //DBG_INFO ("Creating new Anim Chan %s", szChanNameIn);
  pChan = new AnimChan (szChanNameIn);

  listChans.PushBack (pChan);
  return (pChan);
  };

//-----------------------------------------------------------------------------
INT  AnimManager::NumChans  (VOID)
  {
  return (listChans.Size ());
  };

//-----------------------------------------------------------------------------
INT  AnimManager::NumAnims  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);

  // return if chan doesn't exist
  if (pChan == NULL) return (0);

  return (pChan->NumAnims ());
  };


//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetClipLengthSec  (INT           iClipIndex,
                                       const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  AnimClipBase *  pAnim = pChan->GetClipByIndex (iClipIndex);
  if (pAnim == NULL) {return (0.0f);};

  return (pAnim->GetLength ());
  };


//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetChanLengthSec  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  return (pChan->GetLength ());
  };

//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetClipRemainingSec  (INT           iClipIndex,
                                          const char *  szChanNameIn)
  {
  // NOTE:  This function returns the number of seconds left in the clip,
  //         but NOT factoring in any fast forward speeds.  Also it gives the
  //         number of seconds until the end of the clip is reached, including
  //         any delay until it starts.
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  AnimClipBase *  pAnim = pChan->GetClipByIndex (iClipIndex);
  if (pAnim == NULL) {return (0.0f);};

  return (pAnim->GetEndSec () - pAnim->GetTime ());
  };

//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetChanRemainingSec  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  return (pChan->GetLength () - pChan->GetTime ());
  };

//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetClipTimeSec  (INT           iClipIndex,
                                     const char *  szChanNameIn)
  {
  // gets current time
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  AnimClipBase *  pAnim = pChan->GetClipByIndex (iClipIndex);
  if (pAnim == NULL) {return (0.0f);};

  return (pAnim->GetTime ());
  };

//-----------------------------------------------------------------------------
FLOAT  AnimManager::GetChanTimeSec  (const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) {return (0.0f);};

  return (pChan->GetTime ());
  };

//-----------------------------------------------------------------------------
UINT32  AnimManager::GetClipByIndex  (INT           iIndexIn,
                                      const char *  szChanNameIn)
  {
  AnimChan *  pChan = FindChan (szChanNameIn);

  // return if chan doesn't exist
  if (pChan == NULL) return (0);

  AnimClipBase *  pClip = pChan->GetClipByIndex (iIndexIn);

  return ((pClip == NULL) ? 0 : pClip->NameHash ());
  };

//-----------------------------------------------------------------------------
UINT32  AnimManager::GetChanByIndex  (INT           iIndexIn)
  {
  TListItr<AnimChan*>  itrSearch = listChans.AtIndex (iIndexIn);
  if (itrSearch.IsValid ())
    {
    return ((*itrSearch)->NameHash ());
    }
  return (0);
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::ClearChan (const char *  szChanNameIn)
  {
  // remove all clips from channel
  AnimChan *  pChan = FindChan (szChanNameIn);
  if (pChan == NULL) return;
  pChan->Clear ();
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::DeleteChan (const char *  szChanNameIn)
  {
  UINT32  uChanNameHashIn = CalcHashValue (szChanNameIn, strlen (szChanNameIn));

  // clear chan, and then delete it from list of channels
  for (TListItr<AnimChan*>  itrChan = listChans.First ();
       itrChan.IsValid ();
       ++itrChan)
    {
    if ((*itrChan)->NameHash () == uChanNameHashIn)
      {
      // found it
      delete (*itrChan);
      listChans.Delete (itrChan);
      return;
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::DeleteAllChans  (VOID)
  {
  // delete all channels from this manager.

  TListItr<AnimChan*>  itrNext;
  for (TListItr<AnimChan*>  itrChan = listChans.First ();
       itrChan.IsValid ();
       itrChan = itrNext)
    {
    itrNext = itrChan;
    ++itrNext;
    delete (*itrChan);
    listChans.Delete (itrChan);
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::UnloadClipLibrary (const char *  szLibNameIn)
  {
  UINT32  uLibNameHash = CalcHashValue (szLibNameIn, strlen (szLibNameIn));
  TListItr<AnimClipBase*>  itrNext;

  for (TListItr<AnimClipBase*>  itrCurr = listClipLibrary.First ();
       itrCurr.IsValid ();
       itrCurr = itrNext)
    {
    itrNext = itrCurr;
    ++itrNext;

    if ((*itrCurr)->InLibraryFile (uLibNameHash))
      {
      delete (*itrCurr);
      listClipLibrary.Delete (itrCurr);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  AnimManager::UnloadAllClipLibraries (VOID)
  {
  for (TListItr<AnimClipBase*>  itrCurr = listClipLibrary.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listClipLibrary.Empty ();
  };

//-----------------------------------------------------------------------------
AnimClipBase *  AnimManager::NewLibraryClip (const char *  szTypeIn,
                                             const char *  szNameIn,
                                             const char *  szLibNameIn)
  {
  // REFACTOR:  Create clips from registered templates

  if (strcmp (szTypeIn, "curve") == 0)
    {
    AnimClipBase *  pNewClip = dynamic_cast<AnimClipBase*>(new AnimClipCurve (szNameIn));
    ASSERT (pNewClip != NULL);
    pNewClip->SetLibraryFile (szLibNameIn);

    listClipLibrary.PushBack (pNewClip);

    return (pNewClip);
    };
  DBG_ERROR ("Unknown clip type %s", szTypeIn);
  return NULL;
  };


//-----------------------------------------------------------------------------
VOID  AnimManager::BuildLibraryList  (RStrArray &  arrayLibsOut)
  {
  // build a list of all libraries in the templates

  // NOTE: since clips in the same library will tend to be sequential in
  //  the clip library, use the hash of their name to quickly skip the
  //  sequential ones without doing a full string search.
  UINT32  uLastLibHash = 0;

  for (TListItr<AnimClipBase*>  itrCurr = listClipLibrary.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if (!(*itrCurr)->InLibraryFile (uLastLibHash))
      {
      if (arrayLibsOut.Find ((*itrCurr)->GetLibraryFile ()) == -1)
        {
        // this library hasn't been stored in the list yet.
        arrayLibsOut.Append ((*itrCurr)->GetLibraryFile ());
        uLastLibHash = (*itrCurr)->LibNameHash ();
        };
      };
    };
  };

//-----------------------------------------------------------------------------
AnimClipBase *  AnimManager::GetNextClipInLib  (UINT32          uLibNameHashIn,
                                                AnimClipBase *  pPrevIn)
  {
  // NOTE:  This isn't the fastest routine because it goes through the list
  //  from the start every time.  However, this protects us against the list
  //  changing between calls, and this method isn't currently planned to be
  //  called all that often (only by tools, during saving).

  TListItr<AnimClipBase*>  itrCurr = listClipLibrary.First ();

  if (pPrevIn != NULL)
    {
    for (; itrCurr.IsValid (); ++itrCurr)
      {
      if ((*itrCurr) == pPrevIn)
        {
        ++itrCurr;
        break;
        };
      };
    };

  for (; itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->LibNameHash () == uLibNameHashIn)
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };











    // need event association/setting here.
    // need keyframe commands here



