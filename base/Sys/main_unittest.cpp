#include <gtest/gtest.h>
#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Script/ExpressionDefaultFn.hpp"

int main(int argc, char **argv)
  {
  DebugMessagesFactory::Initialize ();
  RegisterDefaultExpressionFn ();

  DBG_INFO ("Running unit tests");

  testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();

  Expression::UnregisterAllFunctions ();
  DebugMessagesFactory::Uninitialize  ();

  return result;
  }