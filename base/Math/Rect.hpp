/* -----------------------------------------------------------------
                         Rectangle Primitive

     This module gives a class for consistent handling of rectangles.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2020, Michael T. Duffy II.  All rights reserved.
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


#ifndef RECT_HPP
#define RECT_HPP

#include "Sys/Types.hpp"
//#include "Math/RVec.hpp"
#include <math.h>
#include <float.h>

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


// Rectangles are defined as having the fX1,fY1 coordinate as the lower-left
// hand corner of the rectangle, and fX2,fY2 as the upper-right hand corner.
// This is consistent with a coordinate system where X increases in value
// moving right, and Y increases in value moving up.

// fX1 <= fX2 and fY1 <= fY2.  Width and Height are thus always positive.

///@ingroup
//------------------------------------------------------------------------
class RectF
//------------------------------------------------------------------------
  {
  public:

    /// @union Testing tag
    union
      {
      FLOAT  fX1;     ///< min X coordinate
      FLOAT  fLeft;   ///< left-most x coordinate
      };
    union
      {
      FLOAT  fX2;     ///< max X coordinate
      FLOAT  fRight;  ///< right-most x coordinate
      };

    union
      {
      FLOAT  fY1;
      FLOAT  fBottom;
      };
    union
      {
      FLOAT  fY2;
      FLOAT  fTop;
      };

    // TODO: Define pivot as part of rect?


  public:
           RectF     ()        {fX1 = fX2 = fY1 = fY2 = 0.0f;};

           ~RectF    ()        {};

    FLOAT  GetWidth  (VOID) const  {return (fX2 - fX1);};

    FLOAT  GetHeight (VOID) const  {return (fY2 - fY1);};

    VOID   SetUnit   (VOID)        {fX1 = fY1 = -0.5f; fX2 = fY2 = 0.5f;};

    VOID   Set       (const RectF &  rectIn)  {fX1 = rectIn.fX1;
                                               fY1 = rectIn.fY1;
                                               fX2 = rectIn.fX2;
                                               fY2 = rectIn.fY2;};

    VOID   Set       (FLOAT  fX1In,
                      FLOAT  fY1In,
                      FLOAT  fX2In,
                      FLOAT  fY2In)     {fX1 = fX1In;
                                         fY1 = fY1In;
                                         fX2 = fX2In;
                                         fY2 = fY2In;};



  };

// NOTE: Not sure if this is used anywhere
//------------------------------------------------------------------------
class OffsetRectF
//------------------------------------------------------------------------
  {
  public:

    union
      {
      FLOAT  fX;
      };
    union
      {
      FLOAT  fY;
      };

    union
      {
      FLOAT  fW;
      FLOAT  fWidth;
      };
    union
      {
      FLOAT  fH;
      FLOAT  fHeight;
      };

    // TODO: Define pivot as part of rect?


  public:
           OffsetRectF     ()        {fX = fY = fW = fH = 0.0f; };

           ~OffsetRectF    ()        {};

    FLOAT  GetWidth  (VOID)    {return (fW);};

    FLOAT  GetHeight (VOID)    {return (fH);};

    VOID   SetUnit   (VOID)    {fX = fY = 0.0f; fW = fH = 1.0f;};

    VOID   Set       (const OffsetRectF &  rectIn)  {fX = rectIn.fX;
                                                     fY = rectIn.fY;
                                                     fW = rectIn.fW;
                                                     fH = rectIn.fH;};

    VOID   Set       (const RectF &  rectIn)        {fX = rectIn.fX1;
                                                     fY = rectIn.fY1;
                                                     fW = rectIn.GetWidth();
                                                     fH = rectIn.GetHeight();};

  };


#endif // RECT_HPP
