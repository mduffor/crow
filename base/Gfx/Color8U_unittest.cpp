#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Color8U.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
TEST (Color8U, Basic)
  {
  UINT32      u8Red = MakeRGB (255,0,0);
  UINT32      u8Green = MakeRGB (0,255,0);

  // OpenGL has Red in the LSB, as per a little endian system.

  //DBG_INFO ("MakeRGB is %x", MakeRGB(255,255,255));
  ASSERT_EQ (MakeRGB(255,255,255), 0xffffffff);
  ASSERT_EQ (MakeRGB(255,0,0), 0xff0000ff);
  ASSERT_EQ (MakeRGB(0,255,0), 0xff00ff00);
  ASSERT_EQ (MakeRGB(0,0,255), 0xffff0000);
  ASSERT_EQ (MakeRGB(0,0,0), 0xff000000);

  ASSERT_EQ (MakeRGBA(255,255,255,255), 0xffffffff);

  ASSERT_EQ (LERP8U(u8Red, u8Green, 0.0f), 0xff0000ff);
  ASSERT_EQ (LERP8U(u8Red, u8Green, 0.5f), 0xff007f7f);
  ASSERT_EQ (LERP8U(u8Red, u8Green, 1.0f), 0xff00ff00);

  ASSERT_EQ (RGBA_GetRed (MakeRGB (3,4,5)), 3);
  };


//------------------------------------------------------------------------------
TEST (Color8U, HexValues)
  {
  Color8U  color;

  ASSERT_EQ (MakeRGBA(255,255,255,255), color.Set ("0xffffffff"));

  ASSERT_EQ (MakeRGB(255,255,255), color.Set ("0xffffff"));
  ASSERT_EQ (MakeRGB(255,255,255), color.Set ("0xFFFFFF"));
  ASSERT_EQ (MakeRGB(255,255,255), color.Set ("#FFFFFF"));
  ASSERT_EQ (MakeRGB(255,255,255), color.Set ("FFFFFF"));

  ASSERT_EQ (MakeRGB(255,0,0), color.Set ("0xff0000"));
  ASSERT_EQ (MakeRGB(255,0,0), color.Set ("0xFF0000"));
  ASSERT_EQ (MakeRGB(255,0,0), color.Set ("#FF0000"));
  ASSERT_EQ (MakeRGB(255,0,0), color.Set ("FF0000"));


  ASSERT_EQ (MakeRGB(0,255,0), color.Set ("0x00ff00"));
  ASSERT_EQ (MakeRGB(0,255,0), color.Set ("0x00FF00"));
  ASSERT_EQ (MakeRGB(0,255,0), color.Set ("#00FF00"));
  ASSERT_EQ (MakeRGB(0,255,0), color.Set ("00FF00"));

  ASSERT_EQ (MakeRGB(0,0,255), color.Set ("0x0000ff"));
  ASSERT_EQ (MakeRGB(0,0,255), color.Set ("0x0000FF"));
  ASSERT_EQ (MakeRGB(0,0,255), color.Set ("#0000FF"));
  ASSERT_EQ (MakeRGB(0,0,255), color.Set ("0000FF"));

  ASSERT_EQ (MakeRGB(0,0,0), color.Set ("0x000000"));
  ASSERT_EQ (MakeRGB(0,0,0), color.Set ("0x000000"));
  ASSERT_EQ (MakeRGB(0,0,0), color.Set ("#000000"));
  ASSERT_EQ (MakeRGB(0,0,0), color.Set ("000000"));


  ASSERT_EQ (MakeRGB(0x12,0,0), color.Set ("0x120000"));
  ASSERT_EQ (MakeRGB(0,0x56,0), color.Set ("0x005600"));
  ASSERT_EQ (MakeRGB(0,0,0x9a), color.Set ("#00009a"));
  ASSERT_EQ (MakeRGBA(0,0,0,0xde), color.Set ("000000DE"));
  };

  /*
   *
   *

#define MakeRGB(r,g,b)    ((UINT) ((((UINT)r) << RED_SHIFT_8U) | (((UINT)g) << GREEN_SHIFT_8U) | (((UINT)b) << BLUE_SHIFT_8U)) | (0x00ff << ALPHA_SHIFT_8U))
#define MakeRGBA(r,g,b,a) ((UINT) ((((UINT)r) << RED_SHIFT_8U) | (((UINT)g) << GREEN_SHIFT_8U) | (((UINT)b) << BLUE_SHIFT_8U)) | (((UINT)a) << ALPHA_SHIFT_8U))

#define MakeRGBAf(r,g,b,a) ((UINT) ((((UINT)(r*255.0)) << RED_SHIFT_8U) | (((UINT)(g*255.0)) << GREEN_SHIFT_8U) | (((UINT)(b*255.0)) << BLUE_SHIFT_8U)) | (((UINT)(a*255.0)) << ALPHA_SHIFT_8U))



#define RGBA_GetRed(c)    ((UINT32(c) >> RED_SHIFT_8U)   & 0x000000ff)
#define RGBA_GetGreen(c)  ((UINT32(c) >> GREEN_SHIFT_8U) & 0x000000ff)
#define RGBA_GetBlue(c)   ((UINT32(c) >> BLUE_SHIFT_8U)  & 0x000000ff)
#define RGBA_GetAlpha(c)  ((UINT32(c) >> ALPHA_SHIFT_8U) & 0x000000ff)

#define RGBA_MakeAlpha(c) (UINT32(c) << 24)


//-----------------------------------------------------------------------------
class Color8U
  {
  public:
    UINT  uRGBA;

  public:

               Color8U        ()                          {uRGBA = 0xffffffff;};

               ~Color8U       ()                          {};

               Color8U        (const Color8U &  colorIn)  {uRGBA = colorIn.uRGBA;};

               Color8U        (const INT32      iColorIn)  {uRGBA = (UINT) iColorIn;};

               Color8U        (UINT  RIn,
                               UINT  GIn,
                               UINT  BIn,
                               UINT  AIn = 0x00ff)        {uRGBA = MakeRGBA(RIn, GIn, BIn, AIn);};

    VOID       Set            (UINT  RIn,
                               UINT  GIn,
                               UINT  BIn,
                               UINT  AIn = 0x00ff)        {uRGBA = MakeRGBA(RIn, GIn, BIn, AIn);};

    VOID       Set            (FLOAT  RIn,
                               FLOAT  GIn,
                               FLOAT  BIn,
                               FLOAT  AIn = 0x00ff)       {uRGBA = MakeRGBAf(RIn, GIn, BIn, AIn);};

    VOID       SetR           (UINT  RIn)                 {uRGBA = (uRGBA & RED_MASK_8U)   | (RIn << RED_SHIFT_8U);};
    VOID       SetG           (UINT  GIn)                 {uRGBA = (uRGBA & GREEN_MASK_8U) | (GIn << GREEN_SHIFT_8U);};
    VOID       SetB           (UINT  BIn)                 {uRGBA = (uRGBA & BLUE_MASK_8U)  | (BIn << BLUE_SHIFT_8U);};
    VOID       SetA           (UINT  AIn)                 {uRGBA = (uRGBA & ALPHA_MASK_8U) | (AIn << ALPHA_SHIFT_8U);};


    UINT       R              (VOID) const   {return (RGBA_GetRed   (uRGBA));};
    UINT       G              (VOID) const   {return (RGBA_GetGreen (uRGBA));};
    UINT       B              (VOID) const   {return (RGBA_GetBlue  (uRGBA));};
    UINT       A              (VOID) const   {return (RGBA_GetAlpha (uRGBA));};

    // return color in floating point gamma space
    FLOAT      GammaR         (VOID) const   {return (R() / 255.0f);};
    FLOAT      GammaG         (VOID) const   {return (G() / 255.0f);};
    FLOAT      GammaB         (VOID) const   {return (B() / 255.0f);};
    FLOAT      GammaA         (VOID) const   {return (A() / 255.0f);};

  };

BOOL operator== (const Color8U & colorOne, const Color8U & colorTwo);
BOOL operator!= (const Color8U & colorOne, const Color8U & colorTwo);
BOOL operator== (const Color8U & colorOne, UINT32    uColorTwo);
BOOL operator== (UINT32 uColorOne, const Color8U & colorTwo);

// LERP8U takes an 8U integer color for A and B, and a floating point 0-1 blend value for T.
//  This equation would be clearer if split up, but it wouldn't inline as well.  So we leave some
//  duplicate processing in there and let the compiler sort it out.
#define LERP8U(A, B, T) ((((((A & 0x00ff00ff) * (256 - (INT(256*T))) ) + ( (B & 0x00ff00ff) * INT(256*T) )) >> 8 ) & 0x00ff00ff) |\
                         (((((A>>8) & 0x00ff00ff) * (256 - (INT(256*T))) ) + ( ((B>>8) & 0x00ff00ff) * INT(256*T) )) & 0xff00ff00))

   *
   *
  RStr  strOut;
  Base64::Encode (szDecA, strlen (szDecA), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncA);

  strOut.Reset ();
  Base64::Encode (szDecB, strlen (szDecB), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncB);

  strOut.Reset ();
  Base64::Encode (szDecC, strlen (szDecC), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncC);

  strOut.Reset ();
  Base64::Encode (szDecD, strlen (szDecD), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncD);

  strOut.Reset ();
  Base64::Encode (szDecE, strlen (szDecE), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncE);

  strOut.Reset ();
  Base64::Encode (szDecF, strlen (szDecF), strOut);
  ASSERT_STREQ (strOut.AsChar (), szEncF);
  */
