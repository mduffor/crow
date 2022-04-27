#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Net/URLBuilder.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (URLBuilder, Basic)
  {
  RStr       strTestURLA ("http://sample.net:8080/assets/gfx/env/CrateBox/crateA.mtl");
  RStr       strTestURLB ("http://192.168.1.0:76/assets/gfx/env/CrateBox/crateA.mtl");
  URLBuilder builderA;
  URLBuilder builderB (strTestURLB.AsChar());
  URLBuilder builderC;


  builderA.SetFullURL (strTestURLA.AsChar ());
  ASSERT_STREQ (builderA.GetFullURL (), strTestURLA.AsChar ());
  ASSERT_STREQ (builderB.GetFullURL (), strTestURLB.AsChar ());

  builderC = builderA;
  ASSERT_STREQ (builderC.GetFullURL (), strTestURLA.AsChar ());

  ASSERT_STREQ (builderA.GetPrefix(), "http");
  ASSERT_STREQ (builderA.GetServer(), "sample.net");
  ASSERT_STREQ (builderB.GetServer(), "192.168.1.0");
  ASSERT_EQ (builderA.GetPort(), 8080);
  ASSERT_STREQ (builderA.GetPath(), "/assets/gfx/env/CrateBox/crateA.mtl");

  builderA.SetPrefix ("ftp");
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://sample.net:8080/assets/gfx/env/CrateBox/crateA.mtl");

  builderA.SetServer ("foobar.com");
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:8080/assets/gfx/env/CrateBox/crateA.mtl");
  builderA.SetPort (80);
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:80/assets/gfx/env/CrateBox/crateA.mtl");

  builderA.SetPath ("/my/dir/myapi.php");
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:80/my/dir/myapi.php");

  builderA.SetParam ("one","two");
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:80/my/dir/myapi.php?one=two");
  builderA.SetParam ("three","four");
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:80/my/dir/myapi.php?one=two&three=four");

  builderA.ClearParams ();
  ASSERT_STREQ (builderA.GetFullURL (), "ftp://foobar.com:80/my/dir/myapi.php");
  /*





    VOID          SetPrefix      (const char *  szIn);

    VOID          SetServer      (const char *  szIn);

    VOID          SetPort        (int           iIn);

    VOID          SetPath        (const char *  szIn);

    VOID          ClearParams    (VOID);

    VOID          SetParam       (const char *  szKeyIn,
                                  const char *  szValueIn);

    VOID          BuildFullURL   (VOID);
   * */

  };
