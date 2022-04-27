/* -----------------------------------------------------------------
                               Curve

     This module implements Cubic Curves.

   ----------------------------------------------------------------- */

// Curve.hpp
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

#ifndef CURVE_HPP
#define CURVE_HPP


#include "Sys/Types.hpp"
#include "Containers/FloatArray.hpp"
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"
#include "Containers/RVecArray.hpp"
#include "Containers/RMatrixArray.hpp"



//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

#define RCURVE_DEFAULT_SUBSEGMENTS   20


//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


///  Curve tangents are stored such that the vectors are scaled for hermite
///    curves, and both point the same direction (to the "right").  The X value
///    holds the time in seconds, and the Y value holds the value in either
///    meters (for linear measure), or degrees (for angular measure).
//-----------------------------------------------------------------------------
class Curve
  {
  public:

    enum  EInsertPoint    {kStart = 0, kEnd = 1, kInsertByX = 3};
    enum  EInterpolation  {kCatmullRom = 0, kLinear = 1, kFlat = 2};
    enum  ETwistMode      {kFullLength = 0, kSegment = 1};

    // Note:  Probably need to add out-of-range options.
    //  to be implemented

    EInterpolation    eDefaultInterp;
    ETwistMode        eTwistMode;

    static INT32      iApproximationRecursionLevel;

    static FLOAT      fKeyTimeEpsilon;

  private:

    RVec3Array     avecControlVerts;
    RVec3Array     avecInTangents;
    RVec3Array     avecOutTangents;
    FloatArray     afTwistWeights;
    FloatArray     afBaseSegmentLengths;

  public:

              Curve                  ();
    virtual   ~Curve                 ();

    VOID      Init                   (VOID);

    VOID      SetPoints              (const RVec3Array &  avecControlVertsIn)  {avecControlVerts  = avecControlVertsIn;};
    VOID      SetInTangents          (const RVec3Array &  avecInTangentsIn)    {avecInTangents    = avecInTangentsIn;};
    VOID      SetOutTangents         (const RVec3Array &  avecOutTangentsIn)   {avecOutTangents   = avecOutTangentsIn;};
    VOID      SetTwistWeights        (const FloatArray &  afTwistWeightsIn)    {afTwistWeights    = afTwistWeightsIn;};

    VOID        SetTwistMode         (ETwistMode  eModeIn)    {eTwistMode = eModeIn;};
    ETwistMode  GetTwistMode         (VOID)                   {return (eTwistMode);};

    VOID      SetDefaultTangentType  (EInterpolation  kInterpIn)   {eDefaultInterp = kInterpIn;
                                                                    CalcTangents (0, avecControlVerts.Length () - 1);};

    RVec3     GetPointOnCurve        (FLOAT  fT) const;

    RVec3     GetPointOnCurve_X      (FLOAT  fXIn);

    EStatus   SampleCurveSegment     (INT32         iStartIndex,
                                      INT32         iArrayStartIndex,
                                      RVec3Array &  arrayOut,
                                      INT32         iNumSamples);


                                     /** @brief  Add a CV to the curve
                                         @param  vecPointIn
                                         @param  pvecInTangentIn
                                         @param  pvecOutTangentIn
                                         @param  eInsertIn
                                         @param  fTwistWeightIn
                                         @param  fSegLengthIn
                                         @return Index of the added point
                                     */
    INT32     AddPoint               (const RVec3 & vecPointIn,
                                      RVec3 *       pvecInTangentIn  = NULL,
                                      RVec3 *       pvecOutTangentIn = NULL,
                                      EInsertPoint  eInsertIn        = kInsertByX,
                                      FLOAT         fTwistWeightIn   = 1.0f,
                                      FLOAT         fSegLengthIn     = 1.0f);  // length of the segment after this cv

    INT32     AddPoint               (const RVec4 & vecPointIn,
                                      RVec4 *       pvecInTangentIn  = NULL,
                                      RVec4 *       pvecOutTangentIn = NULL,
                                      EInsertPoint  eInsertIn        = kInsertByX,
                                      FLOAT         fTwistWeightIn   = 1.0f,
                                      FLOAT         fSegLengthIn     = 1.0f);  // length of the segment after this cv

    RVec3     SolveForPoint          (INT32        iFirstIndex,
                                      FLOAT        fT) const;


                                     /// EndIndex is the inclusive end of the range of points whose tangents will be calculated.
    EStatus   CalcTangents           (INT32        iStartIndex = 0,
                                      INT32        iEndIndex = -1);

    EStatus   CalcCatmullRomTangents (INT32        iStartIndex = 0,
                                      INT32        iEndIndex = -1);

    EStatus   CalcLinearTangents     (INT32        iStartIndex = 0,
                                      INT32        iEndIndex = -1);


    Curve &   operator=              (const Curve &  curveIn);
    VOID      Set                    (const Curve &  curveIn) {*this = curveIn;};
    BOOL      operator==             (const Curve &  curveIn);
    BOOL      operator!=             (const Curve &  curveIn) {return (!(*this == curveIn));};

    RVec3     GetPoint               (INT32  iIndexIn)     {return avecControlVerts [iIndexIn];};
    RVec3     GetInTangent           (INT32  iIndexIn)     {return avecInTangents [iIndexIn];};
    RVec3     GetOutTangent          (INT32  iIndexIn)     {return avecOutTangents [iIndexIn];};
    FLOAT     GetTwistWeight         (INT32  iIndexIn)     {return afTwistWeights [iIndexIn];};

    FLOAT     GetPointTime           (INT32  iIndexIn)     {return avecControlVerts [iIndexIn].fX;};
    FLOAT     GetPointValue          (INT32  iIndexIn)     {return avecControlVerts [iIndexIn].fY;};

    INT32     GetNumKeys             (VOID) const          {return avecControlVerts.Length ();};

    VOID      RemovePoint            (INT32   iIndexIn);

    VOID      RemovePointByTime      (FLOAT   fTimeIn)     {return RemoveKeyframeRange (fTimeIn - fKeyTimeEpsilon, fTimeIn + fKeyTimeEpsilon);};

    VOID      RemoveKeyframeRange    (FLOAT   fTimeStartIn,
                                      FLOAT   fTimeEndIn);

    VOID      ClearPoints            (VOID);

    VOID      ReduceKeyframes        (VOID);

    VOID      GetKeyRange            (RVec3 &       vecMin,
                                      RVec3 &       vecMax);

    EStatus   GetSegmentLength       (INT32         iStartIndexIn,
                                      INT32         iNumSamplesIn,
                                      FLOAT &       fLengthOut);

    FLOAT     GetTotalLength         (INT32         iNumSamplesIn = -1);

    VOID      SampleCurve            (FloatArray &    afSegmentLengths,  // intermediate data
                                      RVec3Array  &   arrayPoints,       // intermediate data
                                      RMatrixArray *  parrayCvMatrix     = NULL,  // orientations of cvs
                                      RMatrixArray *  parrayOrientations = NULL,  // intermediate data
                                      FloatArray *    parrayTwists       = NULL,  // intermediate data
                                      FloatArray *    parrayScales       = NULL); // intermediate data

    EStatus   SolveForPointDist      (FLOAT           fDistIn,
                                      FloatArray &    afSegmentLengths,  // intermediate data
                                      RVec3Array  &   arrayPoints,       // intermediate data
                                      RVec3 &         vecPointOut,
                                      INT32 &         iSegmentIndexOut,
                                      FLOAT &         fSegmentPosOut,
                                      RMatrixArray *  parrayCvMatrix     = NULL,
                                      RMatrixArray *  parrayOrientations = NULL, // intermediate data
                                      FloatArray *    parrayTwists       = NULL, // intermediate data
                                      FloatArray *    parrayScales       = NULL, // intermediate data
                                      RMatrix *       pmatOrientOut = NULL);

    VOID      PrintDebug             (VOID);


  };






#endif // CURVE_HPP


