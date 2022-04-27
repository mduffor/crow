/* -----------------------------------------------------------------
                           Timer Class

    This module implements a timer class.

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

#ifndef TIMER_HPP
#define TIMER_HPP

#include "Sys/Types.hpp"
#include "Util/Signal.h"
#include "ValueRegistry/ValueRegistry.hpp"

// REFACTOR:  Move from Sys to Util

using namespace Gallant;

//------------------------------------------------------------------------------
class TimeTracker
  {
  // This class is for querying local and network time.  The singleton instance
  //  can be called by either Timer or TimerManager by default

  private:

    INT64         iDeviceTimeMs;        // Device time in UTC
    INT64         iNetworkLocalDeltaMs;
    BOOL          bHaveNetworkTime;     // TODO: Not used?
    INT           iDayOfWeek;           // 0 - Sunday, 6 - Saturday

    static TimeTracker * pInstance;

  public:
                          TimeTracker          ()      {
                                                       iDeviceTimeMs        = 0;
                                                       iNetworkLocalDeltaMs = 0;
                                                       bHaveNetworkTime     = FALSE;
                                                       iDayOfWeek           = 0;
                                                       };

                          ~TimeTracker         ()      {};

    static TimeTracker *  Instance             (VOID)  {if (pInstance == NULL) {pInstance = new TimeTracker;}; return (pInstance);};

    VOID                  UpdateLocalTime      (VOID);

    VOID                  UpdateNetworkTime    (VOID);

    VOID                  ForceTimeInc         (INT  iDeltaMsIn)   {iDeviceTimeMs += iDeltaMsIn;};

    INT64                 GetLocalTimeMs       (VOID)  {return (iDeviceTimeMs);};

    INT64                 GetNetworkDeltaMs    (VOID)  {return (iNetworkLocalDeltaMs);};

    INT64                 GetNetworkTimeMs     (VOID)  {return (iDeviceTimeMs - iNetworkLocalDeltaMs);};

    INT                   GetDayOfWeek         (VOID)  {UpdateLocalTime (); return (iDayOfWeek);};

  };

//------------------------------------------------------------------------------
class TimerBase
  {
  // subclass this for each type of timer you need
  public:
    enum ETimeSource
      {
      kLocalDelta = 0, ///< Tied to the delta update.  Only increments while program is running.
      kLocalClock = 1, ///< Tied to the local clock.  Valid across sessions, but can be hacked by changing device time/date.
      kUTC        = 2  ///< Tied to internet time servers.  Valid across sessions.  Requires internet connection.
      };

  protected:
    INT64    iStartTime;          ///< When the timer started, in Ms
    INT64    iNextTime;           ///< When the timer expires.  iStartTime + iMillisecondsToWait
    INT64    iCurrTime;           ///< Incrementing time counter for kLocalDelta
    INT64    iLocalTime;          ///< Incrementing time counter for kLocalClock
    INT64    iMillisecondsToWait; ///< Length of the timer interval, in milliseconds

    ETimeSource   eTimeSource;
    BOOL          bOneShot;      ///< If true, the timer will delete itself after it posts.  If false, timer will repeat after it fires.
    BOOL          bPersistent;
    BOOL          bPaused;

    UINT32        ccType;        ///< User defined descriptor for the timer.  Helpful when timer system saves and loads from disk.

    TimeTracker *  pTimeTracker;

  public:
    RStr          strName;    ///< Name to identify timer for later lookup, esp after loading.  Public for easier serialization.

    TimerBase *   pNext;      ///< Pointer for linked list.  Accessed by TimerManager.

   protected:
                                  /** @brief Initialize internal variables
                                   */
    VOID          Init            (VOID);

    VOID          BeginCountdown  (INT64  iCurrTimeIn);

  public:

                                  /** @brief Constructor
                                  */
                  TimerBase      ();

                                  /** @brief Destructor
                                  */
    virtual       ~TimerBase     ();

    UINT32        Type           (VOID)                     {return ccType;};

    VOID          SetName        (const char *  szNameIn)   {strName.Set (szNameIn, TRUE);};

    const char *  Name           (VOID)                     {return strName.AsChar ();};

    BOOL          MatchesName    (const RStr &  strIn)      {return strName.Equals (strIn);};

    virtual BOOL  Post           (INT64  iMillisecondsSinceLast);  ///> Call the Post routine.  Return True if this timer should be deleted, false otherwise.

                                  /** @brief Increment the timer so it detects passage of time.
                                      @param uMillisecondsIn The number of milliseconds that have passed since IncTime () was last called.
                                      @return True if the timer should be deleted.  False if the timer reset itself.
                                  */
    BOOL          IncTime         (INT64  iMillisecondsIn,
                                   INT64  iDeviceTimeMsIn);

                                  /** @brief Constructor
                                      @param uMillisecondsToWaitIn The number of milliseconds to wait before posting the timer.
                                      @param bOneShotIn If true, the timer will delete itself after posting.  If false, the timer will reset itself after posting so it can fire again.
                                      @param eSourceIn Type of clock to use to increment timer (kLocalDelta, kLocalClock, or kUTC)
                                  */
    VOID          Start          (INT64         iMillisecondsToWaitIn,
                                  BOOL          bOneShotIn   = FALSE,
                                  ETimeSource   eSourceIn    = kLocalDelta);

    VOID          Pause           (VOID)   {bPaused = TRUE;};

    VOID          Resume          (VOID)   {bPaused = FALSE;};

    INT64         GetElapsedMs    (VOID);

    INT64         GetRemainingMs  (VOID);

    VOID          SetPersistent   (BOOL  bStatusIn)          {bPersistent = bStatusIn;};

    BOOL          IsPersistent    (VOID)                     {return bPersistent;};

    virtual VOID  Serialize       (ValueRegistry &  regIn);

    virtual VOID  Deserialize     (ValueRegistry &  regIn);

    TimerBase &               operator=        (const TimerBase &  timerIn);

    void                      Set              (const TimerBase &  timerIn)  {*this = timerIn;};

                                                /** @brief Called by the operator= override
                                                    @param timerIn The timer to copy from.
                                                */
    virtual VOID              Copy             (const TimerBase &  timerIn) = 0;

                                                /** @brief Called by the factory to instantiate a new instance from the template
                                                    return A copy of this.
                                                */
    virtual TimerBase *       Duplicate        (VOID) const = 0;

  };

//------------------------------------------------------------------------------
class TimerSignal : public TimerBase
  {
  public:


                              /// Attach with pTimer->sigOnPost.Connect( pMyClassInst, &MyClass::OnPostListener );
    Signal0<>     sigOnPost;  ///< Event fired when the timer posts.
    static BOOL   bRegistered;

  public:

                         TimerSignal  ()   {ccType = TimerSignal::TypeID();};

    explicit             TimerSignal  (INT64                    iMillisecondsToWaitIn,
                                       BOOL                     bOneShotIn   = FALSE,
                                       TimerBase::ETimeSource   eSourceIn    = kLocalDelta)
                                                                    {
                                                                    ccType = TimerSignal::TypeID();
                                                                    Start (iMillisecondsToWaitIn, bOneShotIn, eSourceIn);
                                                                    };

                         ~TimerSignal ()                            {};

    static UINT32        TypeID       (VOID)                        {return MAKE_FOUR_CODE ("TSIG");};


    virtual VOID         Copy         (const TimerBase &  timerIn) override {return;};

    virtual TimerBase *  Duplicate    (VOID) const override        {TimerSignal * pNew = new TimerSignal ();
                                                                    pNew->Set (*this);
                                                                    //*pNew = *this; // this line fucks shit up
                                                                    return pNew;};

    virtual BOOL         Post         (INT64  iMSecondsSinceLast) override;
  };

//------------------------------------------------------------------------------
class TimerManager
  {
  private:
    static TList<TimerBase*>  listTemplates;
    static TimerManager *     pInstance;

    TimerBase *               pTimerList;
    TimeTracker *             pTimeTracker;


  public:

                           TimerManager      ();

                           ~TimerManager     ();

    static TimerManager *  Instance          (VOID)                     {
                                                                        if (pInstance == NULL)
                                                                          {
                                                                          pInstance = new TimerManager;
                                                                          };
                                                                        if (!TimerSignal::bRegistered)
                                                                          {
                                                                          TimerSignal::bRegistered = TRUE;
                                                                          Register (new TimerSignal);
                                                                          };
                                                                        return pInstance;
                                                                        };

    static VOID            DestroyInstance   (VOID)                     {if (pInstance != NULL) {delete pInstance;}; pInstance = NULL;};

    static TimerBase *     Register          (TimerBase *  timerIn)     {listTemplates.PushBack (timerIn); return timerIn;};

    VOID                   IncTime           (INT64  uMillisecondsIn);

    INT                    TimerCount        (VOID);

    TimerBase *            AddTimer          (TimerBase *  pTimerIn);

    VOID                   DeleteAllTimers   (VOID);

    VOID                   DeleteTemplates   (VOID);

    TimerBase *            NewTimer          (UINT32  ccTypeIn);

    VOID                   DeleteTimer       (TimerBase *  pTimerIn);

    TimerBase *            FindTimer         (const char *  szNameIn);

    BOOL                   IsValidTimer      (TimerBase *  pIn);

    VOID                   Save              (RStrParser &  parserOut); ///< Save current timers into parserIn

    VOID                   Load              (RStrParser &  parserIn);  ///< Load and create timers from parserIn
  };

#endif // TIMER_HPP
