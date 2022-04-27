



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "CurrencyRegenTimed.hpp"
#include "Sys/Timer.hpp"
#include "UIHelper.hpp"


CurrencyRegenTimed*  CurrencyRegenTimed::pInstance = NULL;


//=============================================================================

//-----------------------------------------------------------------------------
VOID  CurrencyRegenItem::SetIdentifier (const char *     szIdentifierIn,
                                        ValueRegistry *  pRegIn)
  {
  strIdentifier.Set (szIdentifierIn, TRUE);

  strInProgressKey.Format ("UI.Regen.%s.InProgress", strIdentifier.AsChar ());
  strTimerKey.Format      ("UI.Regen.%s.Timer",      strIdentifier.AsChar ());
  strSoftLimitKey.Format  ("UI.Regen.%s.SoftLimit",  strIdentifier.AsChar ());

  pelemInProgress = pRegIn->SetBool   (strInProgressKey.AsChar (), FALSE);
  pelemTimer      = pRegIn->SetString (strTimerKey.AsChar (), "0:00");
  pelemSoftLimit  = pRegIn->SetInt    (strSoftLimitKey.AsChar (), 0);
  };

//-----------------------------------------------------------------------------
VOID  CurrencyRegenItem::ResetTimer (TimeTracker *    pTimeTrackerIn)
  {
  INT64  iNowMs = pTimeTrackerIn->GetNetworkTimeMs ();
  iTimerStartSec = MS_TO_SEC (iNowMs);
  //DBG_INFO ("Currency Regen timer reset to %d", (INT) iTimerStartSec);
  };

//-----------------------------------------------------------------------------
VOID  CurrencyRegenItem::SetCurrency  (UINT32   uCurrencyTypeIn,
                                       INT      iSoftLimitIn)
  {
  uCurrencyType = uCurrencyTypeIn;
  iSoftLimit    = iSoftLimitIn;

  //DBG_INFO ("Set soft limit to %d", iSoftLimit);

  if (pelemSoftLimit != NULL)
    {
    pelemSoftLimit->SetInt (iSoftLimit);
    };
  };

//-----------------------------------------------------------------------------
BOOL CurrencyRegenItem::CheckForCurrency (TimeTracker *      pTimeTrackerIn,
                                          CurrencyManager *  pCurrencyIn)
  {
  // NOTE:  This will be called every second.  This is separate from the Daily
  //   regen, which gives a certain amount of currency every day.
  BOOL  bChanged = FALSE;

  if ((pelemInProgress == NULL) || (pelemTimer == NULL)) {return (FALSE);};

  INT  iCurrency = pCurrencyIn->Get (uCurrencyType);

  INT64  iNowSec = MS_TO_SEC (pTimeTrackerIn->GetNetworkTimeMs ());
  INT    iElapsedSec = 0;

  if (iCurrency >= iSoftLimit)
    {
    // if we are above the soft limit, we are not regenerating, so we reset the timer.
    iTimerStartSec = iNowSec;
    }
  else
    {
    // regenerating

    iElapsedSec = (INT) (iNowSec - iTimerStartSec);

    //DBG_INFO ("Currency Regen elapsed time %d sec (%d min)  = %d - %d", iElapsedSec, SEC_TO_MINUTE(iElapsedSec), (INT) iNowSec, (INT) iTimerStartSec);

    if (iElapsedSec >= iTimeToRegenSec)
      {

      INT  iToAward = iElapsedSec / iTimeToRegenSec;
      //DBG_INFO ("Currency Regen to award %d because %d / %d", iToAward, iElapsedSec, iTimeToRegenSec);

      iTimerStartSec += (INT64) (iToAward * iTimeToRegenSec);

      // update elapsed sec to the value after any awarding.
      iElapsedSec = (INT) iNowSec - iTimerStartSec;

      INT  iMaxAward = iSoftLimit - iCurrency;
      INT  iMaxAwardClamped = RMax (iMaxAward, 0);
      iToAward = RMin (iToAward, iMaxAwardClamped);

      // apply the award

      if (pCurrencyIn->Escrow (uCurrencyType, iToAward))
        {
        pCurrencyIn->Award (uCurrencyType, "regen", FALSE); // update quickly with no animation
        bChanged = TRUE;
        };
      };
    };

  if (pelemInProgress != NULL)
    {
    pelemInProgress->SetBool (iCurrency < iSoftLimit);
    };

  // update the timer

  if (pelemInProgress->GetBool ())
    {
    // REFACTOR: Format time.  This probably belongs in a separate helper function.  (TimeDelta)
    INT  iRemainingSec = RMin (iTimeToRegenSec - iElapsedSec, 99*MINUTE_TO_SEC(1));



    INT  iRemainingMin = iRemainingSec / MINUTE_TO_SEC(1);
    iRemainingSec -= iRemainingMin * MINUTE_TO_SEC(1);

    if (pelemTimer != NULL)
      {
      static char  szTimeFormat [6];
      szTimeFormat [0] = iRemainingMin > 9 ? '0' + (iRemainingMin / 10) : ' ';
      szTimeFormat [1] = '0' + (iRemainingMin % 10);
      szTimeFormat [2] = ':';
      szTimeFormat [3] = '0' + (iRemainingSec / 10);
      szTimeFormat [4] = '0' + iRemainingSec % 10;
      szTimeFormat [5] = '\0';

      //DBG_INFO ("%s set to %s", pelemTimer->GetName(), szTimeFormat);
      pelemTimer->SetString (szTimeFormat);
      };
    };

  return (bChanged);
  };

//-----------------------------------------------------------------------------
INT  CurrencyRegenItem::TimeTillSoftLimit  (TimeTracker *      pTimeTrackerIn,
                                            CurrencyManager *  pCurrencyIn)
  {
  // return the number of seconds until we regen up to the soft limit
  INT  iCurrency = pCurrencyIn->Get (uCurrencyType);

  if (iCurrency >= iSoftLimit) return (0);

  INT  iCurrencyTillLimit = iSoftLimit - iCurrency;


  INT64  iNowSec = MS_TO_SEC (pTimeTrackerIn->GetNetworkTimeMs ());
  INT    iRemainingSec = (INT) ((iTimerStartSec + iTimeToRegenSec) - iNowSec);

  INT  iSecTillLimit = iCurrencyTillLimit * iTimeToRegenSec;

  if (iRemainingSec > 0)
    {
    iSecTillLimit = iSecTillLimit - iTimeToRegenSec + iRemainingSec;
    };
  return (iSecTillLimit);
  };



//=============================================================================


//-----------------------------------------------------------------------------
CurrencyRegenTimed::CurrencyRegenTimed ()
  {
  // NOTE: This class is for tracking the timed regeneration of currency up to
  //   a certain limit.  The goal is to give the player a drip of currency so they
  //   can keep playing if they are patient, but not enough to keep them fully fed
  //   so they are encouraged to monetize.  This is separate from the Daily regen
  //   which gives a guaranteed amount of currency every day without any hard cap.

  //   This is a manager level class that can handle multiple registered
  //   regen frequencies and rewards.

  pReg = NULL;

  // Initialize references to their singleton versions.  These can be overridden
  //  via dependency injection before use.

  pReg         = ValueRegistry::Root ();
  pCurrency    = CurrencyManager::Instance ();
  pTimeTracker = TimeTracker::Instance ();

  // REFACTOR:  The below timer may need to be recreated if the TimeTracker is overridden

  // register a timer so this instance is polled every second.
  pTimerInSeconds = (TimerSignal *) TimerManager::Instance ()->NewTimer (TimerSignal::TypeID ());
  pTimerInSeconds->Pause ();
  pTimerInSeconds->sigOnPost.Connect (this, &CurrencyRegenTimed::OnTimerTick);

  pTimerInSeconds->Start (SEC_TO_MS(1)); // fire off every second

  // REFACTOR: Hard-coding for now.  Set this up more elegantly for re-use later.

 // CurrencyRegenTimed::Instance->SetRegen ("Premium", 90, 50, CURRENCY_PREMIUM);

  };

//-----------------------------------------------------------------------------
CurrencyRegenTimed::~CurrencyRegenTimed ()
  {
  TimerManager::Instance ()->DeleteTimer (pTimerInSeconds);
  pTimerInSeconds = NULL;
  ClearAll ();
  };

//-----------------------------------------------------------------------------
VOID  CurrencyRegenTimed::ClearAll (VOID)
  {
  for (TListItr<CurrencyRegenItem*> itrCurr = listItems.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listItems.Empty ();
  };

//-----------------------------------------------------------------------------
CurrencyRegenItem *  CurrencyRegenTimed::Find  (const char *  szIdentifierIn)
  {
  RStr  strIdentifierIn (szIdentifierIn, TRUE);

  for (TListItr<CurrencyRegenItem*> itrCurr = listItems.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->strIdentifier.Equals (strIdentifierIn))
      {
      // exists
      return (*itrCurr);
      };
    };
  return (NULL);
  };


//-----------------------------------------------------------------------------
VOID  CurrencyRegenTimed::SetRegen (const char *  szIdentifierIn,
                                    INT           iTimeToRegenSecIn,
                                    INT           iSoftLimitIn,
                                    UINT32        uCurrencyTypeIn)
  {
  ASSERT (pReg != NULL);

  CurrencyRegenItem *  pItem = Find (szIdentifierIn);

  if (pItem == NULL)
    {
    pItem = new CurrencyRegenItem;
    listItems.PushBack (pItem);
    };

  pItem->SetIdentifier (szIdentifierIn, pReg);
  pItem->ResetTimer (pTimeTracker);
  pItem->SetCurrency (uCurrencyTypeIn, iSoftLimitIn);
  pItem->iTimeToRegenSec = iTimeToRegenSecIn;
  };

//-----------------------------------------------------------------------------
VOID  CurrencyRegenTimed::OnTimerTick (VOID)
  {
  //DBG_INFO ("Currency Regen Tick");
  BOOL  bChanged = FALSE;
  for (TListItr<CurrencyRegenItem*> itrCurr = listItems.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->CheckForCurrency (pTimeTracker, pCurrency))
      {
      bChanged = TRUE;
      };
    };

  if (bChanged)
    {
    // currency has been awarded.  Need to save the game.
    CurrencyManager::Instance()->SetSaveFlag (TRUE);
    };
  };

//-----------------------------------------------------------------------------
VOID CurrencyRegenTimed::Serialize (RStrParser &  parserOut)
  {
  parserOut.SetU4_LEnd (0x01); // version, just in case

  INT  iCount = listItems.Size ();
  parserOut.SetU4_LEnd (iCount);

  for (TListItr<CurrencyRegenItem*> itrCurr = listItems.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    parserOut.SetDataStr ((*itrCurr)->strIdentifier);
    parserOut.SetU8_LEnd ((UINT64)(*itrCurr)->iTimerStartSec);
    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->iTimeToRegenSec);

    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->iSoftLimit);
    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->uCurrencyType);
    };
  };

//-----------------------------------------------------------------------------
VOID CurrencyRegenTimed::Deserialize (RStrParser &  parserIn)
  {
  if (parserIn.IsEmpty ()) return;

  parserIn.ResetCursor ();

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  INT  iCount = parserIn.GetU4_LEnd ();

  for (INT  iIndex = 0; iIndex < iCount; ++iIndex)
    {
    RStr  strIdentifier;

    parserIn.GetDataStr (&strIdentifier);
    UINT64 iTimerStartSec  = (UINT64) parserIn.GetU8_LEnd ();
    INT    iTimeToRegenSec = (INT)    parserIn.GetU4_LEnd ();
    INT    iSoftLimit      = (INT)    parserIn.GetU4_LEnd ();
    UINT32 uCurrencyType   = (UINT32) parserIn.GetU4_LEnd ();

    SetRegen (strIdentifier.AsChar (),
              iTimeToRegenSec,
              iSoftLimit,
              uCurrencyType);
    CurrencyRegenItem * pItem = Find (strIdentifier.AsChar ());
    if (pItem)  {pItem->iTimerStartSec = iTimerStartSec;};
    };
  OnTimerTick ();
  };

//-----------------------------------------------------------------------------
INT  CurrencyRegenTimed::TimeTillSoftLimit  (const char *  szIdentifierIn)
  {
  CurrencyRegenItem *  pItem = Find (szIdentifierIn);

  if (pItem == NULL) return (0);

  return (pItem->TimeTillSoftLimit (pTimeTracker, pCurrency));
  };


