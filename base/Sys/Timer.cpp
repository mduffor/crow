/* -----------------------------------------------------------------
                           Timer Class

    This module implements a polling timer class.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 1997,2017, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Sys/Timer.hpp"
#include "Util/NTP.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

#define NETWORK_TIME_ENABLED 0

TList<TimerBase*>  TimerManager::listTemplates;
TimerManager *     TimerManager::pInstance = NULL;
BOOL               TimerSignal::bRegistered = FALSE;

TimeTracker *      TimeTracker::pInstance = NULL;


//------------------------------------------------------------------------------
//  Time Tracker
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
VOID  TimeTracker::UpdateLocalTime (VOID)
  {
  static struct timeval  now;
  gettimeofday (&now, NULL);
  iDeviceTimeMs = SEC_TO_MS(now.tv_sec) + USEC_TO_MS(now.tv_usec);

  struct tm *  ptmLocal = localtime (&now.tv_sec);

  // UNIX Epoch was on a Thursday, so we add 4
  //INT64  iDayOfWeekTemp = (SEC_TO_DAY(now.tv_sec) + INT64(4)) % INT64(7);
  //iDayOfWeek = INT (iDayOfWeekTemp);
  iDayOfWeek = ptmLocal->tm_wday;

  //DBG_INFO ("UpdateLocalTime  (%d %d) %d", now.tv_sec, now.tv_usec, iDeviceTimeMs);
  //DBG_INFO ("UpdateLocalTime2 %d = %d + %d", iDeviceTimeMs, now.tv_sec * SEC_TO_MS, INT((FLOAT)now.tv_usec * USEC_TO_MS));
  };

//------------------------------------------------------------------------------
VOID  TimeTracker::UpdateNetworkTime (VOID)
  {
  UpdateLocalTime ();

  // TODO:  You need a timeout on this call so you don't spam the call.
  //          Perhaps only check every 10 seconds, and use the delta time
  //          to determine that passage of time.

  iNetworkLocalDeltaMs = 0;
  bHaveNetworkTime = FALSE;

  // TODO: Fix network timeouts before you reenable them.  Leaving local time for now.
  #if NETWORK_TIME_ENABLED
    INT64 iNetworkTimeSec;
    if (NTP::GetNetworkTimeSec (iNetworkTimeSec) == EStatus::kSuccess)
      {
      // we will use the local clock to track passage of time with the network
      //  clock, but take into consideration how many seconds they vary from
      //  one another.
      iNetworkLocalDeltaMs = iDeviceTimeMs - SEC_TO_MS(iNetworkTimeSec);

      bHaveNetworkTime = TRUE;
      };
  #endif
  };


//------------------------------------------------------------------------------
//  TimerBase
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TimerBase::TimerBase ()
  {
  pTimeTracker = TimeTracker::Instance ();
  Init ();
  ccType = MAKE_FOUR_CODE ("BASE");
  };

//------------------------------------------------------------------------------
TimerBase::~TimerBase ()
  {
  };

//------------------------------------------------------------------------------
VOID TimerBase::Init (VOID)
  {

  iStartTime = iNextTime = iCurrTime = iLocalTime = 0;
  iMillisecondsToWait = 1;
  pNext       = NULL;
  bOneShot    = FALSE;
  bPersistent = FALSE;
  eTimeSource = kLocalDelta;
  bPaused     = TRUE;
  };

//------------------------------------------------------------------------------
VOID TimerBase::Start (INT64        iMillisecondsToWaitIn,
                       BOOL         bOneShotIn,
                       ETimeSource  eSourceIn)
  {
  //Init (); // may need a reset, but not an init.
  iMillisecondsToWait = iMillisecondsToWaitIn;
  bOneShot            = bOneShotIn;
  eTimeSource         = eSourceIn;
  bPaused             = FALSE;

  if (eTimeSource == kLocalDelta)
    {
    BeginCountdown (0);
    }
  else if (eTimeSource == kLocalClock)
    {
    struct timeval  now;
    gettimeofday (&now, NULL);
    BeginCountdown (SEC_TO_MS(now.tv_sec) + USEC_TO_MS(now.tv_usec));
    }
  else if (eTimeSource == kUTC)
    {
    pTimeTracker->UpdateNetworkTime ();

    BeginCountdown (pTimeTracker->GetNetworkTimeMs ());
    };
  //DBG_INFO ("TimerBase Start %s with ms wait %d", strName.AsChar (), iMillisecondsToWaitIn);
  };

//------------------------------------------------------------------------------
BOOL TimerBase::Post (INT64  iMillisecondsSinceLast)
  {
  // Call the Post routine.  Return True if this timer should be deleted,
  //   false otherwise.

  // This function should be overridden so that posting the timer can be acted
  //  upon.

  return (bOneShot);
  };

//------------------------------------------------------------------------------
BOOL TimerBase::IncTime (INT64  iMillisecondsIn,
                         INT64  iDeviceTimeMsIn)
  {
  // returns true if the timer should be deleted

  if ((eTimeSource == kLocalDelta) && (bPaused))
    {
    return (FALSE);
    };

  if ((eTimeSource != kLocalDelta) || (!bPaused))
    {
    // local delta timers can be paused
    iCurrTime  += iMillisecondsIn;
    }

  iLocalTime  = iDeviceTimeMsIn;

  //DBG_INFO (" Base IncTime for %s %d (source %d time %d/%d) %s", strName.AsChar (), iCurrTime, eTimeSource, iCurrTime, iNextTime, bPaused ? "Paused" : "NotPaused");


  if (((eTimeSource == kLocalClock) && (iDeviceTimeMsIn >= iNextTime)) ||
      ((eTimeSource == kLocalDelta) && (iCurrTime >= iNextTime)) ||
      ((eTimeSource == kUTC)        && (pTimeTracker->GetNetworkTimeMs () >= iNextTime)))
    {
    // double-check the internet when a UTC timer expires
    if (eTimeSource == kUTC)
      {
      // NOTE:  This could be very slow.  It would be wiser to check the internet
      //  on a separate thread, and then re-check the results once that call came back.
      pTimeTracker->UpdateNetworkTime ();
      if (pTimeTracker->GetNetworkTimeMs () < iNextTime)
        {
        return (FALSE);
        }
      };

    BOOL  bReturn = Post (iCurrTime - iStartTime);

    // update time values for next posting
    iStartTime = iCurrTime;
    iNextTime  = iCurrTime + iMillisecondsToWait;

    return (bReturn);
    };
  return (FALSE);
  };

//------------------------------------------------------------------------------
VOID TimerBase::BeginCountdown  (INT64  iCurrTimeIn)
  {
  iStartTime = iCurrTime = iCurrTimeIn;
  iNextTime = iCurrTime + iMillisecondsToWait;
  };

//------------------------------------------------------------------------------
INT64  TimerBase::GetElapsedMs  (VOID)
  {
  if (eTimeSource == kLocalDelta)
    {
    return (iCurrTime - iStartTime);
    }
  else if (eTimeSource == kLocalClock)
    {
    return (iLocalTime - iStartTime);
    };

  // NTP time
  return (pTimeTracker->GetNetworkTimeMs () - iStartTime);

  };

//------------------------------------------------------------------------------
INT64  TimerBase::GetRemainingMs  (VOID)
  {
  if (eTimeSource == kLocalDelta)
    {
    return (iNextTime - iCurrTime);
    }
  else if (eTimeSource == kLocalClock)
    {
    return (iNextTime - iLocalTime);
    };
  return (iNextTime - pTimeTracker->GetNetworkTimeMs ());
  };


//------------------------------------------------------------------------------
VOID  TimerBase::Serialize  (ValueRegistry &  regIn)
  {
  RStrParser  parserOut;

  parserOut.SetU4_LEnd (0x01); // version, just in case
  parserOut.SetU8_LEnd (iStartTime);
  parserOut.SetU8_LEnd (iNextTime);
  parserOut.SetU8_LEnd (iCurrTime);
  parserOut.SetU8_LEnd (iLocalTime);  // we're storing this, though it will be obliterated next IncTime()
  parserOut.SetU8_LEnd (iMillisecondsToWait);

  UINT32  uTimeSourceOut = MAKE_FOUR_CODE("LDLT");
  if (eTimeSource == kLocalClock)
    {
    uTimeSourceOut = MAKE_FOUR_CODE("LCLK");
    }
  else if (eTimeSource == kUTC)
    {
    uTimeSourceOut = MAKE_FOUR_CODE("UTC_");
    };
  parserOut.SetU4_LEnd (uTimeSourceOut);
  parserOut.SetU4_LEnd (bOneShot ? 1 : 0);

  regIn.SetBlob ("base", &parserOut);
  };

//------------------------------------------------------------------------------
VOID  TimerBase::Deserialize  (ValueRegistry &  regIn)
  {
  RStrParser  parserIn;

  regIn.GetBlob ("base", parserIn);

  parserIn.ResetCursor ();

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  iStartTime          = parserIn.GetU8_LEnd ();
  iNextTime           = parserIn.GetU8_LEnd ();
  iCurrTime           = parserIn.GetU8_LEnd ();
  iLocalTime          = parserIn.GetU8_LEnd ();
  iMillisecondsToWait = parserIn.GetU8_LEnd ();

  UINT32  ccTimeSource = parserIn.GetU4_LEnd ();

  if (ccTimeSource == MAKE_FOUR_CODE("LDLT"))
    {
    eTimeSource = kLocalDelta;
    }
  else if (ccTimeSource == MAKE_FOUR_CODE("LCLK"))
    {
    eTimeSource = kLocalClock;
    }
  else if (ccTimeSource == MAKE_FOUR_CODE("UTC_"))
    {
    eTimeSource = kUTC;
    };

  bOneShot = parserIn.GetU4_LEnd () == 1 ? TRUE : FALSE;
  bPersistent = TRUE;
  };


//------------------------------------------------------------------------------
TimerBase &  TimerBase::operator=  (const TimerBase &  timerIn)
  {
  iStartTime          = timerIn.iStartTime;
  iNextTime           = timerIn.iNextTime;
  iCurrTime           = timerIn.iCurrTime;
  iLocalTime          = timerIn.iLocalTime;
  iMillisecondsToWait = timerIn.iMillisecondsToWait;
  eTimeSource         = timerIn.eTimeSource;
  bOneShot            = timerIn.bOneShot;
  bPersistent         = timerIn.bPersistent;

  // handle virtual instances of this class
  Copy (timerIn);

  return *this;
  };


//==============================================================================
//  TimerSignal
//==============================================================================

//------------------------------------------------------------------------------
BOOL  TimerSignal::Post  (INT64  iMSecondsSinceLast)
  {
  sigOnPost ();
  return (bOneShot);
  };


//==============================================================================
//  TimerManager
//==============================================================================

//------------------------------------------------------------------------------
TimerManager::TimerManager ()
  {
  pTimerList = NULL;

  pTimeTracker = TimeTracker::Instance ();


  // Query the NTP servers for network time.  Only do this once on startup
  //  because it is a blocking call.  If this takes too long or causes delays
  //  when we don't have a network connection, we may need to move the query
  //  into another thread.

  pTimeTracker->UpdateNetworkTime ();
  };

//------------------------------------------------------------------------------
TimerManager::~TimerManager ()
  {
  DeleteAllTimers ();
  DeleteTemplates ();
  };


//------------------------------------------------------------------------------
VOID TimerManager::IncTime (INT64  uMillisecondsIn)
  {
  TimerBase *  pCurr;
  TimerBase *  pPrev = NULL;
  TimerBase *  pNext;
  static INT64  uLocalQueryMs = 0;


  // once per second, read the local system time.  For long timers that are
  //  based on this, once per second should be plenty fast, avoid drift,
  //  and avoid spamming the gettimeofday call.
  uLocalQueryMs += uMillisecondsIn;
  //DBG_INFO ("TimerManager::IncTime MSIn %d  uLocalQueryMs %d > %d", (INT)uMillisecondsIn, (INT)uLocalQueryMs, 1 * SEC_TO_MS);
  if (uLocalQueryMs > SEC_TO_MS(1))
    {
    uLocalQueryMs = 0;
    //DBG_INFO ("UpdateLocalTime");
    pTimeTracker->UpdateLocalTime ();
    };

  pCurr = pTimerList;
  while (pCurr != NULL)
    {
    pNext = pCurr->pNext;

    if (pCurr->IncTime (uMillisecondsIn, pTimeTracker->GetLocalTimeMs ()))
      {
      //DBG_INFO ("Delete timer");
      // we need to delete this timer, if it still exists.
      if (IsValidTimer (pCurr))
        {
        delete pCurr;
        if (pPrev != NULL)
          {
          pPrev->pNext = pNext;
          }
        else
          {
          pTimerList = pNext;
          };
        };
      }
    else
      {
      // This timer has reset itself and is sticking around.
      pPrev = pCurr;
      }

    pCurr = pNext;
    };
  };

//------------------------------------------------------------------------------
INT  TimerManager::TimerCount (VOID)
  {
  INT  iCount = 0;
  TimerBase *  pCurr = pTimerList;
  while (pCurr != NULL)
    {
    ++iCount;
    pCurr = pCurr->pNext;
    };
  return (iCount);
  };

//------------------------------------------------------------------------------
TimerBase *  TimerManager::AddTimer (TimerBase *  pTimerIn)
  {
  if (pTimerIn == NULL) return (NULL);

  pTimerIn->pNext = pTimerList;
  pTimerList = pTimerIn;

  //DBG_INFO ("AddTimer %s %d", pTimerIn->strName.AsChar (), TimerCount ());

  return (pTimerIn);
  };

//------------------------------------------------------------------------------
VOID TimerManager::DeleteAllTimers (VOID)
  {
  while (pTimerList != NULL)
    {
    TimerBase *  pDelete = pTimerList;
    pTimerList = pTimerList->pNext;
    delete (pDelete);
    };
  };

//------------------------------------------------------------------------------
VOID TimerManager::DeleteTemplates (VOID)
  {
  for (TListItr<TimerBase*>  itrCurr = listTemplates.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listTemplates.Empty ();
  };

//------------------------------------------------------------------------------
TimerBase *  TimerManager::NewTimer (UINT32                   ccTypeIn)
  {
  for (TListItr<TimerBase*>  itrCurr = listTemplates.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->Type () == ccTypeIn)
      {
      return (AddTimer ((*itrCurr)->Duplicate ()));
      };
    };
  return (NULL);
  };

//------------------------------------------------------------------------------
VOID  TimerManager::DeleteTimer  (TimerBase *  pTimerIn)
  {
  TimerBase *  pPrev = NULL;
  TimerBase *  pCurr = pTimerList;

  //DBG_INFO ("DeleteTimer %s", pTimerIn->strName.AsChar ());

  while (pCurr != NULL)
    {
    if (pTimerIn == pCurr)
      {
      if (pPrev == NULL)
        {
        pTimerList = pCurr->pNext;
        }
      else
        {
        pPrev->pNext = pCurr->pNext;
        };
      delete pCurr;
      break;
      };
    pPrev = pCurr;
    pCurr = pCurr->pNext;
    };
  };

//------------------------------------------------------------------------------
TimerBase *  TimerManager::FindTimer  (const char *  szNameIn)
  {
  if (szNameIn == NULL)    return (NULL);
  if (szNameIn[0] == '\n') return (NULL);

  RStr  strMatch (szNameIn, TRUE);
  TimerBase *  pCurr = pTimerList;
  while (pCurr != NULL)
    {
    if (pCurr->strName.Equals (strMatch))
      {
      return (pCurr);
      };
    pCurr = pCurr->pNext;
    };
  return (NULL);
  };

//------------------------------------------------------------------------------
BOOL  TimerManager::IsValidTimer (TimerBase *  pIn)
  {
  TimerBase *  pCurr = pTimerList;
  while (pCurr != NULL)
    {
    if (pCurr == pIn)
      {
      return (TRUE);
      };
    pCurr = pCurr->pNext;
    };
  return (FALSE);
  };

//------------------------------------------------------------------------------
VOID  TimerManager::Save  (RStrParser &  parserOut)
  {
  INT                   iNumToExport = 0;
  TimerBase *           pCurr = pTimerList;
  ValueRegistrySimple   reg;

  while (pCurr != NULL)
    {
    if (pCurr->IsPersistent ())
      {
      ++iNumToExport;
      };
    pCurr = pCurr->pNext;
    };

  parserOut.SetU4_LEnd (0x01); // version, just in case
  parserOut.SetU4_LEnd (iNumToExport);

  pCurr = pTimerList;
  while (pCurr != NULL)
    {
    if (pCurr->IsPersistent ())
      {
      reg.Clear ();
      pCurr->Serialize (reg);

      parserOut.SetU4_LEnd (pCurr->Type ());
      parserOut.SetDataStr (pCurr->strName);

      reg.ToParser (parserOut);
      };
    pCurr = pCurr->pNext;
    };
  }

//------------------------------------------------------------------------------
VOID  TimerManager::Load  (RStrParser &  parserIn)
  {
  ValueRegistrySimple   reg;

  // Create and restore the timers desribed in the parserIn buffer.  If they
  //  share a name with an existing timer, replace the existing one.

  if (parserIn.IsEmpty ()) {return;};

  // verify version
  INT  iVersion = parserIn.GetU4_LEnd ();
  ASSERT (iVersion == 0x01);

  //DBG_INFO ("Timer Manager vers %d from offset %d", iVersion, parserIn.GetCursorStart ());

  if (iVersion != 0x01) return;

  INT  iNumTimers = parserIn.GetU4_LEnd ();

  for (INT  iIndex = 0; iIndex < iNumTimers; ++iIndex)
    {
    // use type and name for creation and replacement
    UINT32  ccType = parserIn.GetU4_LEnd ();
    RStr    strName;
    parserIn.GetDataStr (&strName);

    if (!strName.IsEmpty ())
      {
      // if a timer with this name already exists, remove it so we can replace it.
      TimerBase *  pTimer = FindTimer (strName.AsChar ());
      if (pTimer != NULL)
        {
        DeleteTimer (pTimer);
        };
      };

    TimerBase *  pNew = NewTimer (ccType);

    // restore settings.
    reg.Clear ();
    reg.FromParser (parserIn);
    pNew->Deserialize (reg);
    };
  }








