#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RStr.hpp"

#include "CurrencyManager.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


#define CURRENCY_PRIMARY  MAKE_FOUR_CODE("CPRI")
#define CURRENCY_PREMIUM  MAKE_FOUR_CODE("CPRE")
#define CURRENCY_PRIMARY_KEY "Primary"
#define CURRENCY_PREMIUM_KEY "Premium"

#define CURRENCY_FULL_PRIMARY_KEY "Currency.Primary"
#define CURRENCY_FULL_PREMIUM_KEY "Currency.Premium"

//------------------------------------------------------------------------------
TEST (CurrencyManager, Basic)
  {
  ValueRegistrySimple  registry;

  ASSERT_FALSE (CurrencyManager::Instance () == NULL);
  CurrencyManager::DestroyInstance ();

  CurrencyManager *  pMan = new CurrencyManager();

  // set the keys on initial creation
  pMan->Set (CURRENCY_PRIMARY, 5, CURRENCY_PRIMARY_KEY);
  pMan->Add (CURRENCY_PREMIUM, 8, CURRENCY_PREMIUM_KEY);

  ASSERT_EQ (pMan->Get (CURRENCY_PRIMARY), 5);
  ASSERT_EQ (pMan->Get (CURRENCY_PREMIUM), 8);

  ASSERT_EQ (pMan->Add (CURRENCY_PRIMARY, 3), 8);
  ASSERT_EQ (pMan->Add (CURRENCY_PREMIUM, 3), 11);

  ASSERT_TRUE (registry.GetInt (CURRENCY_FULL_PRIMARY_KEY) == 0);
  ASSERT_TRUE (registry.GetInt (CURRENCY_FULL_PREMIUM_KEY) == 0);

  pMan->SetValueRegistry (&registry);

  ASSERT_EQ (pMan->Add (CURRENCY_PRIMARY, 3), 11);
  ASSERT_EQ (pMan->Add (CURRENCY_PREMIUM, 3), 14);

  ASSERT_EQ (registry.GetInt (CURRENCY_FULL_PRIMARY_KEY), 11);
  ASSERT_EQ (registry.GetInt (CURRENCY_FULL_PREMIUM_KEY), 14);

  delete (pMan);
  };


