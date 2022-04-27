/* -----------------------------------------------------------------
                             Color Ops

     This module implements various operations on color channels.

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


#include "ColorOps.hpp"

//------------------------------------------------------------------------------
FLOAT OpSmoothStepRange (FLOAT a, FLOAT b, FLOAT x)
    {
    if (x < a)  return 0;
    if (x >= b) return 1;
    x = (x - a)/(b - a);
    return (OpSmoothStep (x));
    };

//------------------------------------------------------------------------------
FLOAT OpMod (FLOAT a, FLOAT b)
  {
  int n = (int)(a/b);
  a -= n*b;
  if (a < 0)  a += b;
  return a;
  };


//------------------------------------------------------------------------------
VOID  RGBtoHSL (FLOAT    R,
                FLOAT    G,
                FLOAT    B,
                FLOAT &  H,
                FLOAT &  S,
                FLOAT &  L)
  {
  // equations from http://www.easyrgb.com
  FLOAT  fMin = RMin (R, RMin (G, B));    // Min. value of RGB
  FLOAT  fMax = RMax (R, RMax (G, B));    // Max. value of RGB
  FLOAT  fDeltaMax = fMax - fMin;         // Delta RGB value

  L = ( fMax + fMin ) / 2.0;

  if ( fDeltaMax == 0 )                   // This is a gray, no chroma...
    {
    H = 0.0;                              // HSL results from 0 to 1
    S = 0.0;
    }
  else                                    // Chromatic data...
    {
    if ( L < 0.5 ) S = fDeltaMax / ( fMax + fMin );
    else           S = fDeltaMax / ( 2.0 - fMax - fMin );

    FLOAT  fDeltaR = ( ( ( fMax - R ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;
    FLOAT  fDeltaG = ( ( ( fMax - G ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;
    FLOAT  fDeltaB = ( ( ( fMax - B ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;

    if      ( R == fMax ) H = fDeltaB - fDeltaG;
    else if ( G == fMax ) H = ( 1.0 / 3.0 ) + fDeltaR - fDeltaB;
    else if ( B == fMax ) H = ( 2.0 / 3.0 ) + fDeltaG - fDeltaR;

    if ( H < 0.0 ) H += 1.0;
    if ( H > 1.0 ) H -= 1.0;
    };
  };


//------------------------------------------------------------------------------
FLOAT  Hue_2_RGB  (FLOAT v1, FLOAT v2, FLOAT vH )
  {
  // equations from http://www.easyrgb.com
  if ( vH < 0.0 ) vH += 1.0;
  if ( vH > 1.0 ) vH -= 1.0;
  if ( ( 6.0 * vH ) < 1.0 ) return ( v1 + ( v2 - v1 ) * 6.0 * vH );
  if ( ( 2.0 * vH ) < 1.0 ) return ( v2 );
  if ( ( 3.0 * vH ) < 2.0 ) return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6.0 );
  return ( v1 );
  };


//------------------------------------------------------------------------------
VOID HSLtoRGB (FLOAT    H,
               FLOAT    S,
               FLOAT    L,
               FLOAT &  R,
               FLOAT &  G,
               FLOAT &  B)
  {
  // equations from http://www.easyrgb.com

  if ( S == 0 )                 // HSL from 0 to 1
    {
    R = L;                      // RGB results from 0 to 255
    G = L;
    B = L;
    }
  else
    {
    FLOAT        var_1, var_2;
    if ( L < 0.5 ) {var_2 = L * ( 1.0 + S );}
    else           {var_2 = ( L + S ) - ( S * L );};

    var_1 = 2.0 * L - var_2;

    R = Hue_2_RGB( var_1, var_2, H + ( 1.0 / 3.0 ) );
    G = Hue_2_RGB( var_1, var_2, H );
    B = Hue_2_RGB( var_1, var_2, H - ( 1.0 / 3.0 ) );
    };
  };


//------------------------------------------------------------------------------
VOID RGBtoHSV (FLOAT    R,
               FLOAT    G,
               FLOAT    B,
               FLOAT &  H,
               FLOAT &  S,
               FLOAT &  V)
  {
  // equations from http://www.easyrgb.com
  FLOAT  fMin      = RMin ( R, RMin (G, B) );    // Min. value of RGB
  FLOAT  fMax      = RMax ( R, RMax (G, B) );    // Max. value of RGB
  FLOAT  fDeltaMax = fMax - fMin;                // Delta RGB value

  V = fMax;

  if ( fDeltaMax == 0 )                          // This is a gray, no chroma...
    {
    H = 0.0;                                     // HSV results from 0 to 1
    S = 0.0;
    }
  else                                           // Chromatic data...
    {
    S = fDeltaMax / fMax;

    FLOAT  fDeltaR = ( ( ( fMax - R ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;
    FLOAT  fDeltaG = ( ( ( fMax - G ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;
    FLOAT  fDeltaB = ( ( ( fMax - B ) / 6.0 ) + ( fDeltaMax / 2.0 ) ) / fDeltaMax;

    if      ( R == fMax ) H = fDeltaB - fDeltaG;
    else if ( G == fMax ) H = ( 1.0 / 3.0 ) + fDeltaR - fDeltaB;
    else if ( B == fMax ) H = ( 2.0 / 3.0 ) + fDeltaG - fDeltaR;

    if ( H < 0 ) H += 1;
    if ( H > 1 ) H -= 1;
    }
  };


//------------------------------------------------------------------------------
VOID HSVtoRGB (FLOAT    H,
               FLOAT    S,
               FLOAT    V,
               FLOAT &  R,
               FLOAT &  G,
               FLOAT &  B)
  {
  // equations from http://www.easyrgb.com
  if ( S == 0 )                                  // HSV from 0 to 1
    {
    R = V;
    G = V;
    B = V;
    }
  else
    {
    FLOAT  var_h = H * 6.0;
    if ( var_h == 6 ) var_h = 0.0;               // H must be < 1
    FLOAT  var_i = int( var_h );                 // Or ... var_i = floor( var_h )
    FLOAT  var_1 = V * ( 1 - S );
    FLOAT  var_2 = V * ( 1 - S * ( var_h - var_i ) );
    FLOAT  var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );

    if      ( var_i == 0 ) { R = V     ; G = var_3 ; B = var_1; }
    else if ( var_i == 1 ) { R = var_2 ; G = V     ; B = var_1; }
    else if ( var_i == 2 ) { R = var_1 ; G = V     ; B = var_3; }
    else if ( var_i == 3 ) { R = var_1 ; G = var_2 ; B = V;     }
    else if ( var_i == 4 ) { R = var_3 ; G = var_1 ; B = V;     }
    else                   { R = V     ; G = var_1 ; B = var_2; };
    }
  };

