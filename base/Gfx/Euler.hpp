/* -----------------------------------------------------------------
                             Euler

     This module implements functions for dealing with Euler angles.

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

#ifndef EULER_HPP
#define EULER_HPP

#include "Sys/Types.hpp"
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"

/**
  Euler angles are stored in degrees, and internally converted to radians as needed.
  */


//-----------------------------------------------------------------------------
class Euler : public RVec3
  {
  public:
    enum EOrder  {XYZ = 0, // Maya's default
                  XZY = 1,
                  YZX = 2,
                  YXZ = 3,
                  ZXY = 4,
                  ZYX = 5};

    EOrder  eOrder;

  public:

            Euler      ();

    virtual ~Euler     ()                          {};

            Euler      (const Euler &  eulIn);

    VOID  Set          (FLOAT   fXDegIn,
                        FLOAT   fYDegIn,
                        FLOAT   fZDegIn,
                        EOrder  eOrderIn = YXZ)      {fX = fXDegIn; fY = fYDegIn; fZ = fZDegIn; eOrder = eOrderIn;};

    VOID  ToQuaternion (RVec4 &    vecQuatOut);

    VOID  ToMatrixShoemaker (RMatrix &  matOut);

    VOID  ToMatrix     (RMatrix &  matOut);

    VOID  FromMatrix   (const RMatrix &  matIn,
                        Euler            eulBase);

    FLOAT ClampPi      (FLOAT  fIn);

    VOID  FromMatrix   (const RMatrix &  matIn,
                        EOrder           eOrderIn,
                        Euler            eulBase);

  };



#endif // EULER_HPP
