#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/Timer.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md




//------------------------------------------------------------------------------
class MyTimer : public TimerBase
  {
  public:
    static INT          iTimesPosted;

                  MyTimer      ()   {ccType = MyTimer::TypeID(); bOneShot = FALSE;};

   virtual        ~MyTimer     () {};

   BOOL           Post         (INT64  iMSecondsSinceLast) override {++iTimesPosted; return bOneShot;};

   static UINT32  TypeID       (VOID)                               {return MAKE_FOUR_CODE ("UNIT");};

   VOID           Copy         (const TimerBase &  cmdIn)  override {return;};

   TimerBase *    Duplicate    (VOID) const                override {MyTimer * pNew = new MyTimer ();
                                                                     return pNew;};
  };


INT MyTimer::iTimesPosted = 0;

//------------------------------------------------------------------------------
TEST (Timer, Basic)
  {

  TimerManager *  ptHandler = TimerManager::Instance ();

  ptHandler->Register (new MyTimer);


  MyTimer *     pMyTimer = dynamic_cast<MyTimer *>(ptHandler->NewTimer (MyTimer::TypeID ()));

  ASSERT_EQ (MyTimer::iTimesPosted, 0);

  pMyTimer->Start (50);

  ptHandler->IncTime (60);
  ASSERT_EQ (MyTimer::iTimesPosted, 1);

  ptHandler->IncTime (60);
  ptHandler->IncTime (60);
  ASSERT_EQ (MyTimer::iTimesPosted, 3);

  ptHandler->DeleteAllTimers ();
  ptHandler->IncTime (60);
  ptHandler->IncTime (60);

  ASSERT_EQ (MyTimer::iTimesPosted, 3);


  ptHandler->DeleteTimer (pMyTimer);


  // TODO:  Test Local timer (maybe).  Test signal.

  TimerManager::DestroyInstance ();
  };


