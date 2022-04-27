
#ifndef CURRENCYREGENTIMED_HPP
#define CURRENCYREGENTIMED_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
//#include "Util/Signal.h"
#include "Sys/Timer.hpp"
#include "CurrencyManager.hpp"

/**
  @addtogroup gametech
  @{
*/

//-----------------------------------------------------------------------------
class CurrencyRegenItem
  {
  // This is an internal class used by CurrencyRegenTimed

  public:
    RStr                      strIdentifier;

    RStr                      strInProgressKey; // calculated
    RStr                      strTimerKey;      // calculated
    ValueElem *               pelemInProgress;  // calculated
    ValueElem *               pelemTimer;       // calculated

    RStr                      strSoftLimitKey;  // calculated
    ValueElem *               pelemSoftLimit;   // calculated

    INT64                     iTimerStartSec;   // calculated
    INT                       iTimeToRegenSec;
    INT                       iSoftLimit;
    UINT32                    uCurrencyType;



  public:
          CurrencyRegenItem  ()     {Clear ();};
          ~CurrencyRegenItem ()     {Clear ();};

    VOID  Clear              (VOID) {
                                    strIdentifier.Empty ();
                                    strInProgressKey.Empty ();
                                    strTimerKey.Empty ();
                                    pelemInProgress = NULL;
                                    pelemTimer = NULL;
                                    iTimerStartSec  = 0;
                                    iTimeToRegenSec = 0;
                                    iSoftLimit      = 0;
                                    uCurrencyType   = 0;
                                    };

    VOID  SetIdentifier      (const char *     szIdentifierIn,
                              ValueRegistry *  pRegIn);

    VOID  ResetTimer         (TimeTracker *  pTimeTrackerIn);

    VOID  SetCurrency        (UINT32           uCurrencyTypeIn,
                              INT              iSoftLimitIn);

    BOOL  CheckForCurrency   (TimeTracker *      pTimeTrackerIn,
                              CurrencyManager *  pCurrencyIn);

    INT   TimeTillSoftLimit  (TimeTracker *      pTimeTrackerIn,
                              CurrencyManager *  pCurrencyIn);
  };


//-----------------------------------------------------------------------------
class CurrencyRegenTimed
  {
  private:

    ValueRegistry *                pReg;
    CurrencyManager *              pCurrency;
    TimeTracker *                  pTimeTracker;

    static CurrencyRegenTimed *    pInstance;
    TList<CurrencyRegenItem *>     listItems;
    TimerSignal *                  pTimerInSeconds;



  public:
                                 CurrencyRegenTimed  ();

                                 ~CurrencyRegenTimed ();

    static CurrencyRegenTimed *  Instance         (VOID)                     {if (pInstance == NULL) {pInstance = new CurrencyRegenTimed;}; return (pInstance);};

    VOID                         SetValueRegistry   (ValueRegistry *    pRegIn)          {pReg = pRegIn;};
    VOID                         SetCurrencyManager (CurrencyManager *  pCurrencyIn)     {pCurrency = pCurrencyIn;};
    VOID                         SetTimeTracker     (TimeTracker *      pTimeTrackerIn)  {pTimeTracker = pTimeTrackerIn;};

    VOID                         ClearAll         (VOID);

    VOID                         SetRegen         (const char *  szIdentifierIn,
                                                   INT           iTimeToRegenSecIn,
                                                   INT           iSoftLimitIn,
                                                   UINT32        uCurrencyTypeIn);

    VOID                         OnTimerTick      (VOID);

    VOID                         Serialize        (RStrParser &  parserOut);

    VOID                         Deserialize      (RStrParser &  parserIn);

    INT                          TimeTillSoftLimit (const char *  szIdentifierIn);

  public: // only for unit testing

    CurrencyRegenItem *          Find             (const char *  szIdentifierIn);


  };
/** @} */ // end of gametech group

#endif // CURRENCYREGENTIMED_HPP
