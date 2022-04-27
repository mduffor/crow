#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RStr.hpp"

#include "CurrencyRegenTimed.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

#define CURRENCY_PRIMARY  MAKE_FOUR_CODE("CPRI")
#define CURRENCY_PREMIUM  MAKE_FOUR_CODE("CPRE")
#define CURRENCY_PRIMARY_KEY "Primary"
#define CURRENCY_PREMIUM_KEY "Premium"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (CurrencyRegenTimed, Basic)
  {
  ValueRegistrySimple  registry;
  CurrencyManager      currency;
  CurrencyRegenTimed   regen;
  TimeTracker          timeTracker;


  ASSERT_FALSE (CurrencyRegenTimed::Instance () == NULL);

  // setup and hookup our test framework
  currency.SetValueRegistry (&registry);
  regen.SetValueRegistry    (&registry);
  regen.SetCurrencyManager  (&currency);
  regen.SetTimeTracker      (&timeTracker);
  currency.Set (CURRENCY_PRIMARY, 0, CURRENCY_PRIMARY_KEY);
  currency.Set (CURRENCY_PREMIUM, 0, CURRENCY_PREMIUM_KEY);


  // Test CurrencyRegenItem

  CurrencyRegenItem *  pItem = new CurrencyRegenItem;

  //----------
  pItem->SetIdentifier ("Rabbit", &registry);

  ASSERT_STREQ (pItem->strIdentifier.AsChar (),    "Rabbit");
  ASSERT_STREQ (pItem->strInProgressKey.AsChar (), "UI.Regen.Rabbit.InProgress");
  ASSERT_STREQ (pItem->strTimerKey.AsChar (),      "UI.Regen.Rabbit.Timer");

  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Rabbit.InProgress"));
  ASSERT_TRUE (registry.IsBool   ("UI.Regen.Rabbit.InProgress"));
  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Rabbit.Timer"));
  ASSERT_TRUE (registry.IsString ("UI.Regen.Rabbit.Timer"));

  ASSERT_TRUE (pItem->pelemInProgress != NULL);
  ASSERT_TRUE (pItem->pelemTimer != NULL);

  //----------
  pItem->SetCurrency (CURRENCY_PRIMARY, 50);

  ASSERT_EQ (pItem->iSoftLimit, 50);
  ASSERT_EQ (pItem->uCurrencyType, CURRENCY_PRIMARY);


  //----------
  pItem->Clear ();
  ASSERT_TRUE (pItem->pelemInProgress == NULL);
  ASSERT_TRUE (pItem->pelemTimer == NULL);
  ASSERT_EQ (pItem->iTimerStartSec, 0);
  ASSERT_EQ (pItem->iTimeToRegenSec, 0);
  ASSERT_EQ (pItem->iSoftLimit, 0);
  ASSERT_EQ (pItem->uCurrencyType, 0);


  delete (pItem);

  //----------
  // Test CurrencyRegenTimed

  regen.SetRegen ("Premium", 90, 50, CURRENCY_PREMIUM);
  pItem = regen.Find ("Premium");

  //ASSERT_TRUE (pItem != NULL);

  ASSERT_STREQ (pItem->strIdentifier.AsChar (),    "Premium");
  ASSERT_STREQ (pItem->strInProgressKey.AsChar (), "UI.Regen.Premium.InProgress");
  ASSERT_STREQ (pItem->strTimerKey.AsChar (),      "UI.Regen.Premium.Timer");
  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Premium.InProgress"));
  ASSERT_TRUE (registry.IsBool   ("UI.Regen.Premium.InProgress"));
  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Premium.Timer"));
  ASSERT_TRUE (registry.IsString ("UI.Regen.Premium.Timer"));
  ASSERT_TRUE (pItem->pelemInProgress != NULL);
  ASSERT_TRUE (pItem->pelemTimer != NULL);
  ASSERT_EQ (pItem->iSoftLimit, 50);
  ASSERT_EQ (pItem->uCurrencyType, CURRENCY_PREMIUM);


  pItem = regen.Find ("Premium");
  ASSERT_TRUE (pItem != NULL);

  RStrParser   parser;

  //----------
  regen.Serialize (parser);
  regen.ClearAll ();

  pItem = regen.Find ("Premium");
  ASSERT_TRUE (pItem == NULL);

  parser.ResetCursor ();
  regen.Deserialize (parser);

  pItem = regen.Find ("Premium");
  ASSERT_NE (pItem, nullptr);
  ASSERT_STREQ (pItem->strIdentifier.AsChar (),    "Premium");
  ASSERT_STREQ (pItem->strInProgressKey.AsChar (), "UI.Regen.Premium.InProgress");
  ASSERT_STREQ (pItem->strTimerKey.AsChar (),      "UI.Regen.Premium.Timer");
  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Premium.InProgress"));
  ASSERT_TRUE (registry.IsBool   ("UI.Regen.Premium.InProgress"));
  ASSERT_TRUE (registry.HasKey   ("UI.Regen.Premium.Timer"));
  ASSERT_TRUE (registry.IsString ("UI.Regen.Premium.Timer"));
  ASSERT_TRUE (pItem->pelemInProgress != NULL);
  ASSERT_TRUE (pItem->pelemTimer != NULL);
  ASSERT_EQ (pItem->iSoftLimit, 50);
  ASSERT_EQ (pItem->uCurrencyType, CURRENCY_PREMIUM);



  //----------
  timeTracker.ForceTimeInc (SEC_TO_MS (1));
  regen.OnTimerTick ();

  //pItem = regen.Find ("Premium");

  ASSERT_TRUE (registry.GetBool ("UI.Regen.Premium.InProgress"));

  ASSERT_STREQ (registry.GetString ("UI.Regen.Premium.Timer"), " 1:29");

  timeTracker.ForceTimeInc (SEC_TO_MS (1));
  regen.OnTimerTick ();
  ASSERT_STREQ (registry.GetString ("UI.Regen.Premium.Timer"), " 1:28");

  timeTracker.ForceTimeInc (SEC_TO_MS (30));
  regen.OnTimerTick ();
  ASSERT_STREQ (registry.GetString ("UI.Regen.Premium.Timer"), " 0:58");

  timeTracker.ForceTimeInc (SEC_TO_MS (30));
  regen.OnTimerTick ();
  ASSERT_STREQ (registry.GetString ("UI.Regen.Premium.Timer"), " 0:28");

  timeTracker.ForceTimeInc (SEC_TO_MS (20));
  regen.OnTimerTick ();
  ASSERT_STREQ (registry.GetString ("UI.Regen.Premium.Timer"), " 0:08");

  };

//------------------------------------------------------------------------------
TEST (CurrencyRegenTimed, TimeCheck)
  {
  ValueRegistrySimple  registry;
  CurrencyManager      currency;
  CurrencyRegenTimed   regen;
  TimeTracker          timeTracker;


  ASSERT_FALSE (CurrencyRegenTimed::Instance () == NULL);

  // setup and hookup our test framework
  currency.SetValueRegistry (&registry);
  regen.SetValueRegistry    (&registry);
  regen.SetCurrencyManager  (&currency);
  regen.SetTimeTracker      (&timeTracker);
  currency.Set (CURRENCY_PRIMARY, 0, CURRENCY_PRIMARY_KEY);
  currency.Set (CURRENCY_PREMIUM, 0, CURRENCY_PREMIUM_KEY);


  regen.SetRegen ("Premium", 90, 50, CURRENCY_PREMIUM);
  CurrencyRegenItem *  pItem = regen.Find ("Premium");

  ASSERT_EQ (pItem->TimeTillSoftLimit (&timeTracker, &currency), 90*50);

  timeTracker.ForceTimeInc (SEC_TO_MS (90*2));
  regen.OnTimerTick ();

  ASSERT_EQ (pItem->TimeTillSoftLimit (&timeTracker, &currency), 90*48);

  timeTracker.ForceTimeInc (SEC_TO_MS (90*2));
  regen.OnTimerTick ();

  ASSERT_EQ (regen.TimeTillSoftLimit ("Premium"), 90*46);
  };

