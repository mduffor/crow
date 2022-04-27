/* -----------------------------------------------------------------
                             Color

     This module implements functions for dealing with Color.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#ifndef COLOR8U_HPP
#define COLOR8U_HPP

#include "Sys/Types.hpp"
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"

/**
  */

// OpenGL has Red in the LSB, as per a little endian system.
#define RED_SHIFT_8U     0
#define GREEN_SHIFT_8U   8
#define BLUE_SHIFT_8U   16
#define ALPHA_SHIFT_8U  24


#define RED_MASK_8U   ~(0x000000ff << RED_SHIFT_8U)
#define GREEN_MASK_8U ~(0x000000ff << GREEN_SHIFT_8U)
#define BLUE_MASK_8U  ~(0x000000ff << BLUE_SHIFT_8U)
#define ALPHA_MASK_8U ~(0x000000ff << ALPHA_SHIFT_8U)

#define MakeRGB(r,g,b)    ((UINT) ((((UINT)r) << RED_SHIFT_8U) | (((UINT)g) << GREEN_SHIFT_8U) | (((UINT)b) << BLUE_SHIFT_8U)) | (0x00ff << ALPHA_SHIFT_8U))
#define MakeRGBA(r,g,b,a) ((UINT) ((((UINT)r) << RED_SHIFT_8U) | (((UINT)g) << GREEN_SHIFT_8U) | (((UINT)b) << BLUE_SHIFT_8U)) | (((UINT)a) << ALPHA_SHIFT_8U))

#define MakeRGBAf(r,g,b,a) ((UINT) ((((UINT)(r*255.0)) << RED_SHIFT_8U) | (((UINT)(g*255.0)) << GREEN_SHIFT_8U) | (((UINT)(b*255.0)) << BLUE_SHIFT_8U)) | (((UINT)(a*255.0)) << ALPHA_SHIFT_8U))


#define RED_SHIFT_555    11
#define GREEN_SHIFT_555   6
#define BLUE_SHIFT_555    1
#define ALPHA_SHIFT_5551  0

#define MakeRGB555(r,g,b) ((USHORT)((UINT)(((r >>  3) & 0x001f) << RED_SHIFT_555)   | \
                                    (UINT)(((g >>  3) & 0x001f) << GREEN_SHIFT_555) | \
                                    (UINT)(((b >>  3) & 0x001f) << ALPHA_SHIFT_555) )))

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

    explicit   Color8U        (const INT32      iColorIn)  {uRGBA = (UINT) iColorIn;};

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

    INT        ReadHex        (const char *  szHexIn,
                               INT           iLengthIn,
                               INT &         iIndexInOut);

    INT        Set            (const char *  szHexIn);

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


#endif // COLOR8U_HPP
