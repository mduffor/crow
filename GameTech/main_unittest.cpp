#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/ExpressionDefaultFn.hpp"
//#include "CmdStandard.hpp"
#include "Image/BitmapLoader.hpp"
#include "Image/CodecPam.hpp"
#include "Image/CodecPng.hpp"
#include "Image/CodecTga.hpp"

int main(int argc, char **argv)
  {
  DebugMessagesFactory::Initialize ();
  RegisterDefaultExpressionFn ();
  //CmdStandard_Init ();

  //BitmapLoader::RegisterCodec (new CodecPam);
  //BitmapLoader::RegisterCodec (new CodecPng);
  //BitmapLoader::RegisterCodec (new CodecTga);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  }