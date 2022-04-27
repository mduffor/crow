#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Net/HTTP.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (HTTP, Misc)
  {
  RStr  strResult;

  strResult = HTTP::HeaderBasicAuth ("Aladdin", "OpenSesame");
  ASSERT_STREQ (strResult.AsChar (), "Authorization: Basic QWxhZGRpbjpPcGVuU2VzYW1l\r\n");
  };


