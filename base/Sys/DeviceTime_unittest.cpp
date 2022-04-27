#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/DeviceTime.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
TEST (DeviceTime, TimeDelta)
  {
  TimeDelta   delta (0);

  ASSERT_STREQ (delta.ToStopwatchString (), "00:00");

  delta = TimeDelta (7);
  ASSERT_STREQ (delta.ToStopwatchString (), "00:07");

  delta = TimeDelta (67);
  ASSERT_STREQ (delta.ToStopwatchString (), "01:07");
  ASSERT_STREQ (delta.ToLetterString (), "01m 07s");

  delta = TimeDelta (DAY_TO_SEC(3)+HOUR_TO_SEC(14)+MINUTE_TO_SEC(12)+27);
  ASSERT_STREQ (delta.ToStopwatchString (), "3:14:12:27");

  ASSERT_EQ (delta.Days(),     3);
  ASSERT_EQ (delta.Hours(),   14);
  ASSERT_EQ (delta.Minutes(), 12);
  ASSERT_EQ (delta.Seconds(), 27);

  // Test parsing various strings
  delta.FromString ("08");
  ASSERT_EQ (delta.Seconds(), 8);

  delta.FromString ("12");
  ASSERT_EQ (delta.Seconds(), 12);

  delta.FromString ("01:20");
  ASSERT_EQ (delta.Hours(), 0);
  ASSERT_EQ (delta.Minutes(), 1);
  ASSERT_EQ (delta.Seconds(), 20);

  delta.FromString ("12:21");
  ASSERT_EQ (delta.Hours(), 0);
  ASSERT_EQ (delta.Minutes(), 12);
  ASSERT_EQ (delta.Seconds(), 21);

  delta.FromString ("2h5m");
  ASSERT_EQ (delta.Hours(), 2);
  ASSERT_EQ (delta.Minutes(), 5);
  ASSERT_EQ (delta.Seconds(), 0);


  delta.FromString ("1d 30s");
  ASSERT_EQ (delta.Days(), 1);
  ASSERT_EQ (delta.Hours(), 0);
  ASSERT_EQ (delta.Minutes(), 0);
  ASSERT_EQ (delta.Seconds(), 30);

  delta.FromString ("3:14:12:27");

  ASSERT_EQ (delta.Days(),     3);
  ASSERT_EQ (delta.Hours(),   14);
  ASSERT_EQ (delta.Minutes(), 12);
  ASSERT_EQ (delta.Seconds(), 27);
  };


//------------------------------------------------------------------------------
TEST (DeviceTime, DeviceTime)
  {

  DeviceTime time100 (100);
  DeviceTime time200 (200);
  DeviceTime time250 (250);

  TimeDelta  delta50  ("50s");
  TimeDelta  delta100 ("100s");

  DeviceTime timeTest;
  TimeDelta  deltaTest;


  // test +
  timeTest = time100 + delta50;
  ASSERT_TRUE (timeTest == DeviceTime (150));

  // test +=
  timeTest = time100;
  timeTest += delta50;
  ASSERT_TRUE (timeTest == DeviceTime (150));


  // test -
  timeTest = time250 - delta50;
  ASSERT_TRUE (timeTest == DeviceTime (200));

  // test -=
  timeTest = time250;
  timeTest -= delta50;
  ASSERT_TRUE (timeTest == DeviceTime (200));

  // test -
  deltaTest = time200 - time100;
  ASSERT_TRUE (deltaTest == TimeDelta (100));
  deltaTest = time250 - time100;
  ASSERT_TRUE (deltaTest == TimeDelta (150));

  // test comparisons
  ASSERT_TRUE (time100 < time200);

  ASSERT_TRUE (time100 <= time200);
  ASSERT_TRUE (time100 <= time100);

  ASSERT_TRUE (time250 > time200);
  ASSERT_TRUE (time250 >= time200);

  timeTest.FromString ("2021-12-10 01:02:03");
  ASSERT_STREQ (timeTest.ToString (), "2021-12-10 01:02:03");

  };
