/* -----------------------------------------------------------------
                         Rectangle Solver

     This class is for solving the contents of a RectF class.

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


#ifndef RECTSOLVER_HPP
#define RECTSOLVER_HPP

#include "Sys/Types.hpp"
#include "Math/Rect.hpp"
#include "Math/RVec.hpp"
#include <math.h>
#include <float.h>

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

// NOTE: The RectSolver can get the width and height from the parent rect,
//        but it should solve and return the rect in local space such that
//        the bottom left corner of the parent is considered the origin.

// Before anything that uses Rects in its display can draw, it has to call
//  transform.SolveRects().  This will find any transform where the rects
//  are marked diry, and will recursively call Solve on them.
//
// Before Solve () is called to calculate positions, Size is called
//  recursively to figure out the size of each rect element.

//------------------------------------------------------------------------
class RectSolver
//------------------------------------------------------------------------
  {
  public:
    RStr            strIdentifier;

  public:
                    RectSolver    ()        {};

                    RectSolver    (const char *  szIdIn)       {SetId (szIdIn);};

    virtual         ~RectSolver   ()        {};

    VOID            SetId         (const char *  szIdIn)       {strIdentifier.Set (szIdIn);};

    const char *    GetId         (VOID)                       {return (strIdentifier.AsChar ());};

    virtual UINT32  Type          ()        {return (MAKE_FOUR_CODE("NONE"));};

    virtual VOID    Solve         (RectF *  prectTargetIn,
                                   RectF *  prectParentIn = NULL) = 0;

    virtual VOID    Size         (RVec2 &   vecMinSizeOut) = 0;

  };

  #endif // RECTSOLVER_HPP
