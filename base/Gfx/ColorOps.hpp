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

#ifndef COLOROPS_HPP
#define COLOROPS_HPP

#include <math.h>

#include "Sys/Types.hpp"

//------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------


// Note:  Because all the following are inline #defines, parameters should be
//        enclosed in parenthesis when used in the equation.  This keeps
//        equations passed as parameters from changing the order of operation
//        of the functions.

// bias and gain functions, in Float and Double configurations

#define OpBiasF(fBias,fValue) (powf ((fValue), logf(fBias)/logf(0.5)))
#define OpBiasD(dBias,dValue) (pow  ((dValue), log (dBias)/log (0.5)))

#define OpGainF(fGain,fValue) ( ((fValue) < (0.5f)) ? (OpBiasF (1.0f-(fGain), 2.0f*(fValue)) / 2.0f) : (1.0f - OpBiasF (1.0f - (fGain), 2.0f - 2.0f * (fValue)) / 2.0f) )
#define OpGainD(dGain,dValue) ( ((dValue) < (0.5 )) ? (OpBiasD (1.0 -(dGain), 2.0 *(dValue)) / 2.0 ) : (1.0  - OpBiasD (1.0  - (fGain), 2.0  - 2.0  * (dValue)) / 2.0 ) )

// Bias and gain functions as per Graphic Gems IV, pp. 401-403
#define OpBias2(fBias,fValue) ((fValue) / (((1.0/(fBias)) - 2.0) * (1.0 - (fValue)) + 1.0))
#define OpGain2(fGain,fValue) (((fValue) < 0.5) ? ((fValue) / (((1.0/(fBias)) - 2.0) * (1.0 - 2.0*(fValue)) + 1.0)) : ( ((1/(fGain) - 2.0) * (1 - 2.0*(fValue)) - (fValue)) / ((1/(fGain) - 2.0) * (1 - 2.0*(fValue)) - 1.0)))

// Procedural texture functions from "Texturing & Modeling, a Procedural Approach"
#define OpStep(a, x)      (float ((x) >= (a)))
#define OpPulse(a,b,x)    (OpStep ((a),(x)) - OpStep ((b),(x)))
#define OpClamp(x, a, b)  ((x) < (a) ? (a): ((x) > (b) ? (b) : (x)))
#define OpAbs(x)          ((x) < 0.0f ? -(x) : (x))
#define OpFloor(x)        ((int)(x) - ((x) < 0 && (x) != (int)(x)))
#define OpCeil(x)         ((int)(x) + ((x) > 0 && (x) != (int)(x)))
#define OpSmoothStep(x)   ((x)*(x)*(3.0f - 2.0f*(x)))


FLOAT OpSmoothStepRange (FLOAT a, FLOAT b, FLOAT x);
FLOAT OpMod             (FLOAT a, FLOAT b);


// The following functions work linearly on the range [0.0-1.0]
// Functions based on Graphic Gems pp196-209
#define OpPhotoInvert(fValue)      (1.0f-(fValue))
#define OpPosterize(fSteps,fValue) ((floorf ((fValue)/(1.0/(fSteps)))) * (1.0f/((fSteps)-1.0f)))
#define OpContrast(fLow,fHigh,fValue) ((fValue) <= (fLow) ? 0.0f : ((fValue) >= (fHigh) ? 1.0f : (((fValue)-(fLow)) / ((fHigh)-(fLow)) )))
#define OpExpand(fInLow,fInHigh,fOutLow,fOutHigh,fValue) ((fValue) <= (fInLow) ? fOutLow : ((fValue) >= (fInHigh) ? fOutHigh : ( (  (((fValue)-(fInLow)) / ((fInHigh)-(fInLow))) * ((fOutHigh)-(fOutLow)) ) + (fOutLow) ) ) )
#define OpLerp(fFirst,fSecond,fValue)   ((fFirst) + (((fSecond) - (fFirst)) * (fValue)))
#define OpGamma(fGamma,fValue)          (powf ((fValue), (fGamma)))

// blend modes (using reference from  http://www.pegtop.net/delphi )
#define OpBlendMult(fFirst,fSecond)        ((fFirst) * (fSecond))
#define OpBlendScreen(fFirst,fSecond)      (1.0f - ((1.0f - (fFirst)) * (1.0f - (fSecond))))
#define OpBlendDarken(fFirst,fSecond)      (((fFirst) < (fSecond)) ? (fFirst) : (fSecond))
#define OpBlendLighten(fFirst,fSecond)     (((fFirst) > (fSecond)) ? (fFirst) : (fSecond))
#define OpBlendDifference(fFirst,fSecond)  (fabs ((fFirst) - (fSecond)))
#define OpBlendNegation(fFirst,fSecond)    (1.0f - fabsf (1.0f - (fFirst) - (fSecond)))
#define OpBlendExclusion(fFirst,fSecond)   ((fFirst) + (fSecond) - (2.0f * (fFirst) * (fSecond)))
#define OpBlendOverlay(fFirst,fSecond)     (((fFirst)  < 0.5f) ? (2.0f * (fFirst) * (fSecond)) : (1.0f - (2.0f * (1.0f - (fFirst)) * (1.0f - (fSecond)))))
#define OpBlendHardLight(fFirst,fSecond)   (((fSecond) < 0.5f) ? (2.0f * (fFirst) * (fSecond)) : (1.0f - (2.0f * (1.0f - (fFirst)) * (1.0f - (fSecond)))))
#define OpBlendSoftLight(fFirst,fSecond)   (((1.0-(fFirst)) * (fFirst) * (fSecond)) + ((fFirst) * (1.0f - (1.0f - (fFirst)) * (1.0f - (fSecond)))))
#define OpBlendColorDodge(fFirst,fSecond)  (((fSecond) >= 1.0f) ? 1.0f : (fFirst) / (1.0f - (fSecond)))
#define OpBlendColorBurn(fFirst,fSecond)   (((fSecond) <= 0.0f) ? 0.0f : 1.0f - ((1.0f - (fFirst)) / (fSecond)))
#define OpBlendAdditive(fFirst,fSecond)    ((fFirst) + (fSecond))
#define OpBlendSubtractive(fFirst,fSecond) ((fFirst) + (fSecond) - 1.0f)
#define OpBlendStamp(fFirst,fSecond)       ((fFirst) + (2.0f * (fSecond)) - 1.0f)
#define OpBlendInterpolation(fFirst,fSecond)  (0.5f - (0.25f * cosf (R_PI * (fFirst))) - (0.25f * cosf (R_PI * (fSecond))) )

// These two functions are designed so the fFirst and fSecond are only evaluated once.
//   This way you can use these to mix the above nodes in with the base layer.  For example:
//   OpOpacity (a, OpBlendDarken(a,b), fOpacity)
#define OpOpacity(fFirst,fSecond,fOpacity)           (((fOpacity) * (fSecond)) +  ((1.0f - (fOpacity)) * (fFirst)) )
#define OpTransparency(fFirst,fSecond,fTransparency) (((1.0f - (fTransparency)) * (fFirst)) +   ((fTransparency) * (fSecond)) )

// color space conversions

VOID  RGBtoHSL (FLOAT    R,
                FLOAT    G,
                FLOAT    B,
                FLOAT &  H,
                FLOAT &  S,
                FLOAT &  L);

VOID HSLtoRGB (FLOAT    H,
               FLOAT    S,
               FLOAT    L,
               FLOAT &  R,
               FLOAT &  G,
               FLOAT &  B);

VOID RGBtoHSV (FLOAT    R,
               FLOAT    G,
               FLOAT    B,
               FLOAT &  H,
               FLOAT &  S,
               FLOAT &  V);

VOID HSVtoRGB (FLOAT    H,
               FLOAT    S,
               FLOAT    V,
               FLOAT &  R,
               FLOAT &  G,
               FLOAT &  B);

#endif // COLOROPS_HPP

