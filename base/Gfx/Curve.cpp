/* -----------------------------------------------------------------
                               Curve

     This module implements Cubic Curves.

   ----------------------------------------------------------------- */

// Curve.cpp
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

#include <float.h>
#include <math.h>

#include "Sys/Types.hpp"
#include "Gfx/Curve.hpp"
#include "Containers/DoubleArray.hpp"

INT32  Curve::iApproximationRecursionLevel = 10;
FLOAT  Curve::fKeyTimeEpsilon              = 0.001f;  // one one-thousandth of a second

//-----------------------------------------------------------------------------
Curve::Curve  ()
  {
  Init ();
  };

//-----------------------------------------------------------------------------
Curve::~Curve  ()
  {
  };

//-----------------------------------------------------------------------------
VOID Curve::Init (VOID)
  {
  avecControlVerts.Clear ();
  avecInTangents.Clear ();
  avecOutTangents.Clear ();
  afTwistWeights.Clear ();
  afBaseSegmentLengths.Clear ();

  eDefaultInterp = kCatmullRom;

  SetTwistMode  (kSegment);
  };

//-----------------------------------------------------------------------------
RVec3  Curve::GetPointOnCurve  (FLOAT  fT) const
  {
  FLOAT  fFlooredT = floorf (fT);
  FLOAT  fFractionT = fT - fFlooredT;
  INT32  iFirstIndex = INT32 (fFlooredT);

  return (SolveForPoint (iFirstIndex, fFractionT));
  };

//-----------------------------------------------------------------------------
RVec3  Curve::GetPointOnCurve_X (FLOAT  fXIn)
  {
  // Given the passed X value, find the first instance where the curve crosses that point.
  //  Assume that Time is laid out on the X axis.

  // Use piecewise approximation to find the intersection.

  INT32  iFirstIndex = 0;
  INT32  iNextIndex  = 0;
  INT32  iNumKeys = avecControlVerts.Length ();
  for (iNextIndex = 0; iNextIndex < iNumKeys; ++iNextIndex)
    {
    if (avecControlVerts [iNextIndex].fX > fXIn)
      {
      // found one larger than the given value, so this is the first key to the right of the sample X.
      break;
      };
    };

  iFirstIndex = iNextIndex - 1;

  if (iFirstIndex < 0)
    {
    // sample point is to the left of all the keys.  Handle out-of-bounds calculations.
    // To be implemented

    // Forcing to constant for now.
    return (avecControlVerts [0]);
    }
  else if (iNextIndex >= iNumKeys)
    {
    // sample point is to the right of all the keys.  Handle out-of-bounds calculations.
    // To be implemented

    // Forcing to constant for now.
    return (avecControlVerts [iNumKeys - 1]);
    };


  // perform piecewise approximation

  RVec3  vecLeft    = avecControlVerts [iFirstIndex];
  RVec3  vecRight   = avecControlVerts [iNextIndex];
  RVec3  vecMidpoint;

  FLOAT  fTimeLeft  = 0;
  FLOAT  fTimeRight = 1;

  // check for exact matches so we can avoid expensive recursion if possible
  if (fabs (vecLeft.fX  - fXIn) < fKeyTimeEpsilon) return (vecLeft);
  if (fabs (vecRight.fX - fXIn) < fKeyTimeEpsilon) return (vecRight);


  INT32  iStepsLeft = iApproximationRecursionLevel;

  while (iStepsLeft > 0)
    {
    FLOAT  fTimeMidpoint = (fTimeLeft + fTimeRight) / 2.0f;
    vecMidpoint = SolveForPoint (iFirstIndex, fTimeMidpoint);

    if (vecMidpoint.fX > fXIn)
      {
      vecRight   = vecMidpoint;
      fTimeRight = fTimeMidpoint;
      }
    else
      {
      vecLeft    = vecMidpoint;
      fTimeLeft  = fTimeMidpoint;
      };

    --iStepsLeft;
    };

  // The left and right points should be pretty tight around fXIn now.  Do a LERP
  //  between those endpoints.
  FLOAT  fT = (fXIn - vecLeft.fX) / (vecRight.fX - vecLeft.fX);

  return (LERP (vecLeft, vecRight, fT));
  };

//-----------------------------------------------------------------------------
EStatus  Curve::SampleCurveSegment  (INT32         iStartIndex,
                                     INT32         iArrayStartIndex,
                                     RVec3Array &  arrayOut,
                                     INT32         iNumSamples)
  {
  // This routine returns iNumSamples number of samples between iStartIndex and iStartIndex + 1,
  //  exclusive.  That means that the first value is the solution for iStartIndex, and the last
  //  value is before iStartIndex + 1.  This way if you sample several segments in a row, you
  //  won't be solving the overlapping endpoints multiple times

  for (INT32  iIndex = 0; iIndex < iNumSamples; ++iIndex)
    {
    FLOAT  fT = FLOAT (iIndex) / FLOAT (iNumSamples);

    arrayOut [iIndex + iArrayStartIndex] = SolveForPoint (iStartIndex, fT);

    //printf ("SampleCurveSegment fT %g point %d is %g %g %g\n", fT, iIndex + iArrayStartIndex, arrayOut [iIndex + iArrayStartIndex].fX, arrayOut [iIndex + iArrayStartIndex].fY, arrayOut [iIndex + iArrayStartIndex].fZ);
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
INT32  Curve::AddPoint  (const RVec4 & vecPointIn,
                         RVec4 *       pvecInTangentIn,
                         RVec4 *       pvecOutTangentIn,
                         EInsertPoint  eInsertIn,
                         FLOAT         fTwistWeightIn,
                         FLOAT         fSegLengthIn)  // length of the segment after this cv
  {
  RVec3   vecPoint (vecPointIn.fX, vecPointIn.fY, vecPointIn.fZ);
  RVec3   vecTanIn;
  RVec3   vecTanOut;
  RVec3 * pvecTanIn  = NULL;
  RVec3 * pvecTanOut = NULL;

  if (pvecInTangentIn != NULL)
    {
    vecTanIn = *pvecInTangentIn;
    pvecTanIn = &vecTanIn;
    };
  if (pvecOutTangentIn != NULL)
    {
    vecTanOut = *pvecOutTangentIn;
    pvecTanOut = &vecTanOut;
    };
  return (AddPoint (vecPoint, pvecTanIn, pvecTanOut, eInsertIn, fTwistWeightIn, fSegLengthIn));
  };

//-----------------------------------------------------------------------------
INT32  Curve::AddPoint  (const RVec3 & vecPointIn,
                         RVec3 *       pvecInTangentIn,
                         RVec3 *       pvecOutTangentIn,
                         EInsertPoint  eInsertIn,
                         FLOAT         fTwistWeightIn,
                         FLOAT         fSegLengthIn)  // length of the segment after this cv
  {
  INT32    iInsertPoint = 0;
  BOOL     bOverwrite   = false;

  // calculate the insertion point

  switch (eInsertIn)
    {
    case kStart:      iInsertPoint = 0; break;
    case kEnd:        iInsertPoint = avecControlVerts.Length (); break;
    case kInsertByX:
         {
         // Insert By X assumes that the Points array is already sorted by X
         for (iInsertPoint = 0; iInsertPoint < avecControlVerts.Length (); ++iInsertPoint)
           {
           if (vecPointIn.fX <= avecControlVerts [iInsertPoint].fX)
             {
             break;
             };
           };
         };
         break;
    };

  // if the value in has the same fX as an existing key, overwrite it instead of
  //  inserting a new key

  if ((avecControlVerts.Length  () > 0) && (iInsertPoint < avecControlVerts.Length ()))
    {
    if ((iInsertPoint > 0) && ((fabs (avecControlVerts [iInsertPoint - 1].fX - vecPointIn.fX)) < fKeyTimeEpsilon))
      {
      // overwrite the previous key
      //printf ("Overwrite due to same time %g %g \n", avecControlVerts [iInsertPoint - 1].fX, vecPointIn.fX);
      bOverwrite = true;
      --iInsertPoint;
      }
    else if ((fabs (avecControlVerts [iInsertPoint].fX - vecPointIn.fX)) < fKeyTimeEpsilon)
      {
      //printf ("Overwrite due to same time Two %g %g \n", avecControlVerts [iInsertPoint].fX, vecPointIn.fX);
      bOverwrite = true;
      };
    };

  // add the point
  if (! bOverwrite)
    {
    avecControlVerts.Insert (iInsertPoint, 1);
    avecInTangents.Insert   (iInsertPoint, 1);
    avecOutTangents.Insert  (iInsertPoint, 1);
    afTwistWeights.Insert   (iInsertPoint, 1);
    afBaseSegmentLengths.Insert (iInsertPoint, 1);
    };

  // debugging code
  //static INT  iDebug = 0;
  //printf ("InsertPoint %d Size %d Debug %d \n", iInsertPoint, avecControlVerts.Length (), iDebug);
  //++iDebug;

  avecControlVerts     [iInsertPoint] = vecPointIn;
  afTwistWeights       [iInsertPoint] = fTwistWeightIn;
  afBaseSegmentLengths [iInsertPoint] = fSegLengthIn;

  if (! bOverwrite)
    {
    // if either tangent is unspecified, then calculate the default tangent values
    if ((pvecInTangentIn == NULL) || (pvecOutTangentIn == NULL))
      {
      CalcTangents (iInsertPoint - 1, RMin (iInsertPoint + 1, avecControlVerts.Length () - 1));
      };
    };

  if (pvecInTangentIn != NULL)
    {
    avecInTangents [iInsertPoint] = *pvecInTangentIn;
    };

  if (pvecOutTangentIn != NULL)
    {
    avecOutTangents [iInsertPoint] = *pvecOutTangentIn;
    };

  return (iInsertPoint);
  };

//-----------------------------------------------------------------------------
RVec3  Curve::SolveForPoint  (INT32        iFirstIndex,
                              FLOAT        fT) const
  {
  // Note:  We are storing tangents of the endpoints so that both the out
  //        tangent of the first point and the in tangent of the second
  //        point, both point towards the center of the curve.  The
  //        cubic curve equation expects both tangents to point forwards.
  //        Thus to account for this, we have to reverse the polarity of
  //        the second tangent (by reversing the sign on fH4).

  // handle the case of the last point specially, so you don't try to multiply against
  //   data that doesn't exist.

  if (iFirstIndex >= GetNumKeys () - 1)
    {
    return (avecControlVerts [GetNumKeys () - 1]);
    };

  FLOAT   fT2 = fT * fT;
  FLOAT   fT3 = fT2 * fT;


  FLOAT   fH1 =  (2.0f * fT3) - (3.0f * fT2) + 1.0f;
  FLOAT   fH2 = (-2.0f * fT3) + (3.0f * fT2);

  FLOAT   fH3 =          fT3  - (2.0f * fT2) + fT;
  FLOAT   fH4 =          fT3  -         fT2;

  return ((avecControlVerts [iFirstIndex]     * fH1) +
          (avecControlVerts [iFirstIndex + 1] * fH2) +
          (avecOutTangents  [iFirstIndex]     * fH3) +
          (avecInTangents   [iFirstIndex + 1] * -fH4));
  };


//-----------------------------------------------------------------------------
EStatus  Curve::CalcTangents (INT32        iStartIndex,
                              INT32        iEndIndex)
  {

  switch (eDefaultInterp)
    {
    case kCatmullRom:
         return CalcCatmullRomTangents (iStartIndex, iEndIndex);

    case kLinear:
         return CalcLinearTangents (iStartIndex, iEndIndex);

    case kFlat:
         break;
    }
  return (EStatus::kFailure);
  };


//-----------------------------------------------------------------------------
EStatus  Curve::CalcCatmullRomTangents  (INT32        iStartIndex,
                                         INT32        iEndIndex)
  {
  if ((iEndIndex > avecControlVerts.Length ()) ||
      (avecControlVerts.Length () < 2))
    {
    return (EStatus::kFailure);
    };

  // make sure we have room for the tangents
  if (avecInTangents.Length () != avecControlVerts.Length ())
    {
    avecInTangents.SetLength (avecControlVerts.Length ());
    };
  if (avecOutTangents.Length () != avecControlVerts.Length ())
    {
    avecOutTangents.SetLength (avecControlVerts.Length ());
    };

  // run through the tangents and set them.
  INT32  iIndex;
  INT32  iFinalIndex = avecControlVerts.Length () - 1;

  if (iEndIndex == -1) iEndIndex = iStartIndex;

  for (iIndex = iStartIndex; iIndex <= iEndIndex; ++iIndex)
    {
    if (iIndex == 0)
      {
      avecOutTangents [iIndex] = (avecControlVerts [iIndex + 1] - avecControlVerts [iIndex]) * 0.5f;
      avecInTangents  [iIndex] = avecOutTangents [iIndex] * -1.0f;
      }
    else if (iIndex == iFinalIndex)
      {
      avecInTangents  [iIndex] = (avecControlVerts [iIndex - 1] - avecControlVerts [iIndex]) * 0.5f;
      avecOutTangents [iIndex] = avecInTangents [iIndex] * -1.0f;
      }
    else
      {
      avecInTangents  [iIndex] = (avecControlVerts [iIndex - 1] - avecControlVerts [iIndex + 1]) * 0.5f;
      avecOutTangents [iIndex] = avecInTangents [iIndex] * -1.0f;
      };
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  Curve::CalcLinearTangents  (INT32        iStartIndex,
                                     INT32        iEndIndex)
  {
  if ((iEndIndex > avecControlVerts.Length ()) ||
      (avecControlVerts.Length () < 2))
    {
    return (EStatus::kFailure);
    };

  // make sure we have room for the tangents
  if (avecInTangents.Length () != avecControlVerts.Length ())
    {
    avecInTangents.SetLength (avecControlVerts.Length ());
    };
  if (avecOutTangents.Length () != avecControlVerts.Length ())
    {
    avecOutTangents.SetLength (avecControlVerts.Length ());
    };

  // run through the tangents and set them.
  INT32  iIndex;
  INT32  iFinalIndex = avecControlVerts.Length () - 1;

  if (iEndIndex == -1) iEndIndex = iStartIndex;

  for (iIndex = iStartIndex; iIndex <= iEndIndex; ++iIndex)
    {
    if (iIndex == 0)
      {
      avecOutTangents [iIndex] = (avecControlVerts [iIndex + 1] - avecControlVerts [iIndex]) * 0.5f;
      avecInTangents  [iIndex] = avecOutTangents [iIndex] * -1.0f;
      }
    else if (iIndex == iFinalIndex)
      {
      avecInTangents  [iIndex] = (avecControlVerts [iIndex - 1] - avecControlVerts [iIndex]) * 0.5f;
      avecOutTangents [iIndex] = avecInTangents [iIndex] * -1.0f;
      }
    else
      {
      avecInTangents  [iIndex] = (avecControlVerts [iIndex - 1] - avecControlVerts [iIndex]) * 0.5f;
      avecOutTangents [iIndex] = (avecControlVerts [iIndex + 1] - avecControlVerts [iIndex]) * 0.5f;
      };
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
Curve &  Curve::operator=  (const Curve &  curveIn)
  {
  eDefaultInterp  = curveIn.eDefaultInterp ;

  avecControlVerts = curveIn.avecControlVerts;
  avecInTangents   = curveIn.avecInTangents ;
  avecOutTangents  = curveIn.avecOutTangents;
  afTwistWeights   = curveIn.afTwistWeights ;
  afBaseSegmentLengths = curveIn.afBaseSegmentLengths;
  eTwistMode           = curveIn.eTwistMode;

  return (*this);
  };

//-----------------------------------------------------------------------------
BOOL  Curve::operator==  (const  Curve & arrayIn)
  {

  if (avecControlVerts != arrayIn.avecControlVerts) return (false);
  if (avecInTangents   != arrayIn.avecInTangents)   return (false);
  if (avecOutTangents  != arrayIn.avecOutTangents)  return (false);
  if (afTwistWeights   != arrayIn.afTwistWeights)   return (false);
  if (afBaseSegmentLengths != arrayIn.afBaseSegmentLengths) return (false);

  return (true);
  };

//-----------------------------------------------------------------------------
VOID  Curve::RemoveKeyframeRange    (FLOAT   fTimeStartIn,
                                      FLOAT   fTimeEndIn)
  {
  // call RemovePoint () to delete any keys whose time lies between
  //  the start and end values.

  INT32  iNumKeys = GetNumKeys  ();

  for (INT32  iIndex = iNumKeys; iIndex >= 0; --iIndex)
    {
    if ((avecControlVerts[iIndex].fX >= fTimeStartIn) &&
        (avecControlVerts[iIndex].fX <= fTimeEndIn))
      {
      printf ("Removing point %d at time %f (between %f and %f)\n", (INT)iIndex, avecControlVerts[iIndex].fX, fTimeStartIn, fTimeEndIn);

      RemovePoint (iIndex);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  Curve::ClearPoints  (VOID)
  {
  avecControlVerts.Clear ();
  avecInTangents  .Clear ();
  avecOutTangents .Clear ();
  afTwistWeights  .Clear ();
  afBaseSegmentLengths.Clear ();
  };


//-----------------------------------------------------------------------------
VOID  Curve::RemovePoint  (INT32  iIndexIn)
  {

  // make sure the given index is valid
  if (iIndexIn >= avecControlVerts.Length ())
    {
    return;
    };

  avecControlVerts.Remove (iIndexIn, 1);
  avecInTangents  .Remove (iIndexIn, 1);
  avecOutTangents .Remove (iIndexIn, 1);
  afTwistWeights  .Remove (iIndexIn, 1);
  afBaseSegmentLengths.Remove (iIndexIn, 1);

  CalcTangents (iIndexIn - 1, RMin (iIndexIn, avecControlVerts.Length () - 1));
  };

//-----------------------------------------------------------------------------
VOID  Curve::ReduceKeyframes (VOID)
  {
  // Step through the keys on the curve, and remove those that do not
  //  contribute to a change in shape of the curve.

  //  For now, this simply means removing keys of the same value, but it
  //   should be expanded in the future to remove keys along any linear path.

  INT32  iNumKeys = GetNumKeys  ();

  // start with the last key, and delete it if it matches the previous one.  We
  //  start at the end so we don't have a lot of shuffing of values as holes
  //  in the array are collapsed if all the values are the same.

  INT32  iNumToRemove = 0;
  for (INT32  iIndex = iNumKeys - 1; iIndex > 0; --iIndex)
    {
    if ((avecControlVerts [iIndex] == avecControlVerts [iIndex - 1]) &&
        (avecInTangents   [iIndex] == avecInTangents   [iIndex - 1]) &&
        (avecOutTangents  [iIndex] == avecOutTangents  [iIndex - 1]))
      {
      // we have a matching key.
      ++iNumToRemove;
      }
    else
      {
      // different key.  If there were any matches before this one, remove them.

      if (iNumToRemove > 0)
        {
        avecControlVerts.Remove (iIndex + 2, iNumToRemove);
        avecInTangents  .Remove (iIndex + 2, iNumToRemove);
        avecOutTangents .Remove (iIndex + 2, iNumToRemove);
        afTwistWeights  .Remove (iIndex + 2, iNumToRemove);
        afBaseSegmentLengths.Remove (iIndex + 2, iNumToRemove);

        iNumToRemove = 0;
        };
      };
    };

  // End of searching.  Remove any indexes that match the first entry.
  if (iNumToRemove > 0)
    {
    avecControlVerts.Remove (1, iNumToRemove);
    avecInTangents  .Remove (1, iNumToRemove);
    avecOutTangents .Remove (1, iNumToRemove);
    afTwistWeights  .Remove (1, iNumToRemove);
    afBaseSegmentLengths.Remove (1, iNumToRemove);
    };
  };

//-----------------------------------------------------------------------------
VOID  Curve::GetKeyRange  (RVec3 &  vecMin,
                           RVec3 &  vecMax)
  {
  // return the minimum and maximum values in the key frames

  INT32  iNumKeys = GetNumKeys  ();

  if (iNumKeys == 0)
    {
    vecMin = RVec3 (0,0,0);
    vecMax = RVec3 (1,1,1);
    return;
    };

  vecMin = avecControlVerts [0];
  vecMax = avecControlVerts [0];

  for (INT32  iIndex = 1; iIndex < iNumKeys; ++iIndex)
    {
    vecMin.fX = RMin (vecMin.fX, avecControlVerts[iIndex].fX);
    vecMin.fY = RMin (vecMin.fY, avecControlVerts[iIndex].fY);
    vecMin.fY = RMin (vecMin.fY, avecControlVerts[iIndex].fY);

    vecMax.fX = RMax (vecMax.fX, avecControlVerts[iIndex].fX);
    vecMax.fY = RMax (vecMax.fY, avecControlVerts[iIndex].fY);
    vecMax.fY = RMax (vecMax.fY, avecControlVerts[iIndex].fY);
    };
  };

//-----------------------------------------------------------------------------
EStatus  Curve::GetSegmentLength (INT32         iStartIndexIn,
                                  INT32         iNumSamplesIn,
                                  FLOAT &       fLengthOut)
  {
  // returns the length of the curve segment between iStartIndexIn and
  //  (iStartIndexIn + 1).  The curve will be sampled iNumSamples times, and the
  //  resulting line segments added together to determine the segment length.
  //  The higher the number of samples, the closer the result is to the
  //  actual curve length, but the longer the calculation will take.

  RVec3Array    arrayPoints;
  INT32         iNumSamples = iNumSamplesIn;

  if (iNumSamples == -1)  iNumSamples = RCURVE_DEFAULT_SUBSEGMENTS;

  arrayPoints.SetLength (iNumSamples + 1);

  if (SampleCurveSegment (iStartIndexIn, 0, arrayPoints, iNumSamples) == EStatus::kSuccess)
    {
    FLOAT  fLength = 0.0f;
    arrayPoints [iNumSamples] = SolveForPoint (iStartIndexIn + 1, 0.0);

    for (INT iIndex = 0; iIndex < iNumSamples; ++iIndex)
      {
      RVec3 vecCurr = arrayPoints [iIndex] - arrayPoints [iIndex + 1];

      fLength += vecCurr.Length ();
      };

    fLengthOut = fLength;
    return (EStatus::kSuccess);
    };
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
FLOAT  Curve::GetTotalLength  (INT32         iNumSamplesIn)
  {
  EStatus   statResult;
  INT       iNumSegments       = avecControlVerts.Length () - 1;
  FLOAT     fTotalLength = 0.0f;
  FLOAT     fCurrLength;
  INT32     iNumSamples = iNumSamplesIn;

  if (iNumSamples == -1)  iNumSamples = RCURVE_DEFAULT_SUBSEGMENTS;
  for (INT  iIndex = 0; iIndex < iNumSegments; ++iIndex)
    {
    GetSegmentLength (iIndex, iNumSamples, fCurrLength);

    fTotalLength += fCurrLength;
    };
  return (fTotalLength);
  };

//-----------------------------------------------------------------------------
VOID  Curve::SampleCurve (FloatArray &    afSegmentLengths,   // intermediate data
                          RVec3Array  &   arrayPoints,        // intermediate data
                          RMatrixArray *  parrayCvMatrix,
                          RMatrixArray *  parrayOrientations, // intermediate data
                          FloatArray *    parrayTwists,       // intermediate data
                          FloatArray *    parrayScales)       // intermediate data
  {
  INT         iNumSegmentSamples = RCURVE_DEFAULT_SUBSEGMENTS;
  INT         iNumSegments       = avecControlVerts.Length () - 1;
  INT         iNumSamplesTotal   = (iNumSegmentSamples * iNumSegments) + 1;
  INT         iIndex;
  INT         iSegIndex;
  FLOAT       fCurrDist = 0.0f;
  RMatrix     matTwist;
  RMatrix     matTwistedOrient;
  RMatrix     matScale;

  // Initialize the sample positions, segment lengths, twist amount, and any
  //  other data that doesn't change if the curve CVs don't change.
  //  This makes sampling multiple points on the same curve faster.
  if (afSegmentLengths.Length () == 0)
    {
    // not yet initialized
    arrayPoints.SetLength      (iNumSamplesTotal);
    afSegmentLengths.SetLength (iNumSamplesTotal);

    // calc the point location of the samples in each segment.
    for (iSegIndex = 0; iSegIndex < iNumSegments; ++iSegIndex)
      {
      SampleCurveSegment (iSegIndex, (iSegIndex * iNumSegmentSamples), arrayPoints, iNumSegmentSamples);
      };
    // handle the final point sample
    arrayPoints [(iNumSegmentSamples * iNumSegments)] = SolveForPoint (iNumSegments, 0.0);

    // calc the length of each sample
    afSegmentLengths [0] = 0.0f;
    for (iIndex = 0; iIndex < (iNumSamplesTotal - 1); ++iIndex)
      {
      RVec3 vecCurr = arrayPoints [iIndex] - arrayPoints [iIndex + 1];

      fCurrDist += vecCurr.Length ();
      afSegmentLengths [iIndex + 1] = fCurrDist;

      };
    // end sample position initialization
    };



  // calc the orientation for each sample
  if ((parrayCvMatrix != NULL) &&
      (parrayOrientations != NULL) &&
      (parrayOrientations->Length () == 0) &&
      (parrayTwists != NULL)
      )
    {
    DoubleArray   arrayCvTwist;
    DoubleArray   arrayCvScale;
    RVec3         posTarget;
    RVec3         vecUp;
    RVec3         vecCurrX;
    RVec3         vecCurrY;
    RVec3         vecCurrZ;
    RVec3         vecTargetDir;

    RMatrix       matOrigYCalc;
    RVec3         vecProjectedY;
    RVec3         vecPropogatedY;


    parrayOrientations->SetLength (iNumSamplesTotal);

    // Initialize the twist variables
    parrayTwists->SetLength (iNumSamplesTotal);
    parrayTwists->InitValues  (0, iNumSamplesTotal);
    arrayCvTwist.SetLength (avecControlVerts.Length ());
    arrayCvTwist.InitValues  (0, arrayCvTwist.Length ());
    arrayCvScale.SetLength (avecControlVerts.Length ());
    arrayCvScale.InitValues  (0, arrayCvScale.Length ());

    parrayScales->SetLength (iNumSamplesTotal);
    parrayScales->InitValues  (0, iNumSamplesTotal, 1.0f);


    // get the initial rotation
    (*parrayCvMatrix) [0].GetRotateY (vecCurrY);

    //------------------------------------------
    // First, propogate the original CV's Y down all of the point samples on the curve.  This will establish
    //   a base "up" vector without any discontinuities in it.
    // first and middle samples
    for (iIndex = 0; iIndex < (iNumSamplesTotal - 1); ++iIndex)
      {
      // propagate y vector to each subsequent sample
      MatrixLookAt ((*parrayOrientations) [iIndex],     // out matrix
                    arrayPoints [iIndex],               // position
                    arrayPoints [iIndex + 1],           // target
                    vecCurrY);                          // up vec

      // initialize up vector for the next run
      (*parrayOrientations) [iIndex].GetRotateY (vecCurrY);
      };

    // last sample
    // propagate y vector.  Use the same forward vector (x) as the previous sample to create a target point.
    vecTargetDir  = arrayPoints [iNumSamplesTotal - 1] - arrayPoints [iNumSamplesTotal - 2];
    MatrixLookAt ((*parrayOrientations) [iNumSamplesTotal - 1],      // out matrix
                  arrayPoints [iNumSamplesTotal - 1],                // position
                  arrayPoints [iNumSamplesTotal - 1] + vecTargetDir, // target
                  vecCurrY);                                         // up vec

    //------------------------------------------
    // Secondly, for every CV you want to calculate the difference between that CV's up vector (Y axis) and
    //   the propogaged non-flipping Y axis.

    DOUBLE   dScaleA;
    DOUBLE   dScaleB;
    DOUBLE   dAngleDeltaLast;
    DOUBLE   dPrevTwist = 0.0f;

    for (iSegIndex = 0; iSegIndex <= iNumSegments; ++iSegIndex)
      {
      int  iSampleIndex = iSegIndex * iNumSegmentSamples;

      // project final CV Y into calculated space (propogated coordinate system)
      (*parrayCvMatrix) [iSegIndex].GetRotateY (vecCurrY);
      (*parrayOrientations) [iSampleIndex].GetRotateZ (vecCurrZ);
      MatrixLookAt (matOrigYCalc,                          // out matrix
                    arrayPoints [iSampleIndex],            // position
                    arrayPoints [iSampleIndex] + vecCurrZ, // target
                    vecCurrY);                             // up vec

      // read the calculated (propogated) and projected (from cv) Y vectors
      matTwist.SetRotateZ (dPrevTwist);
      matTwistedOrient =(*parrayOrientations) [iSampleIndex] * matTwist;
      matTwistedOrient.GetRotateY (vecPropogatedY);
      matTwistedOrient.GetRotateX (vecCurrX);

      matOrigYCalc.GetRotateY (vecProjectedY);

      // calc rotational difference in y
      // Note:  acos is very susceptible to rounding error as we approach -1 and 1,
      //         so we have to enforce the range.
      DOUBLE dDotY  = vecPropogatedY * vecProjectedY;
      DOUBLE dAngle = acos (RMax (-1.0, RMin (1.0, dDotY)));
      if (vecCurrX * vecProjectedY > 0.0) dAngle *= -1.0;

      arrayCvTwist [iSegIndex] = dPrevTwist + (dAngle * afTwistWeights [iSegIndex]);
      dPrevTwist = arrayCvTwist [iSegIndex];

      // calculate the CV scale

      // solving for a, the amount to scale x and y, given b the change in z's length
      // xyz = axaybz
      // z = aabz
      // aa = z/bz
      // a = sqrt (z/bz)
      // b = Z/z
      // bz = Z
      // a = sqrt (z/Z)

      if (iSegIndex < iNumSegments)
        {
        arrayCvScale [iSegIndex] = sqrt (afBaseSegmentLengths [iSegIndex] / afSegmentLengths [iNumSegmentSamples * (iSegIndex + 1)]);
        }
      else
        {
        arrayCvScale [iSegIndex] = 1.0f;
        };

      printf ("Scale for %i is sqrt (%f / %f) = %f\n", iSegIndex, afBaseSegmentLengths [iSegIndex], afSegmentLengths [iNumSegmentSamples * (iSegIndex + 1)], arrayCvScale [iSegIndex]);
      };

    //------------------------------------------
    // now that you have the rotation differences for the CVs, weighted by the twist weight array, you can
    // step back through all the orientations and LERP the CV rotations to find the rotation for each sample.

    DOUBLE  dAngleDeltaA;
    DOUBLE  dAngleDeltaB;
    dAngleDeltaLast = arrayCvTwist [iNumSegments];
    for (iSegIndex = 0; iSegIndex < iNumSegments; ++iSegIndex)
      {
      dAngleDeltaA = arrayCvTwist [iSegIndex];
      dAngleDeltaB = arrayCvTwist [iSegIndex + 1];

      dScaleA = arrayCvScale [iSegIndex];
      dScaleB = arrayCvScale [iSegIndex + 1];

      for (INT  iSegSampleIndex = 0; iSegSampleIndex < iNumSegmentSamples; ++iSegSampleIndex)
        {
        iIndex = (iSegIndex * iNumSegmentSamples) + iSegSampleIndex;
        if (eTwistMode == kSegment)
          {
          (*parrayTwists) [iIndex] = (FLOAT)(dAngleDeltaA + (dAngleDeltaB - dAngleDeltaA) * (DOUBLE (iSegSampleIndex) / DOUBLE (iNumSegmentSamples)));
          }
        else if (eTwistMode == kFullLength)
          {
          (*parrayTwists) [iIndex] = (FLOAT)(dAngleDeltaLast * (afSegmentLengths [iIndex] / fCurrDist));
          };
        (*parrayScales) [iIndex] = (FLOAT)(dScaleA + (dScaleB - dScaleA) * (DOUBLE (iSegSampleIndex) / DOUBLE (iNumSegmentSamples)));
        printf ("Scale for index %i is saved as %f (A %f B %f)\n", iIndex, (*parrayScales) [iIndex], dScaleA, dScaleB);
        };
      };
    // handle the last element
    dAngleDeltaA = arrayCvTwist [iNumSegments];
    (*parrayTwists) [iNumSamplesTotal - 1] = (FLOAT)dAngleDeltaA;

    dScaleA = arrayCvScale [iNumSegments];
    (*parrayScales) [iNumSamplesTotal - 1] = (FLOAT)dScaleA;
    printf ("Scale for index %i is saved as %f (A %f)\n", iNumSamplesTotal - 1, (*parrayScales) [iNumSamplesTotal - 1], dScaleA);

    // End of optimization pre-calc

    printf ("End of pre-calc.  Length %d\n", parrayScales->Length());
    for (int  iScaleIndex = 0; iScaleIndex < parrayScales->Length(); ++iScaleIndex)
      {
      printf ("  %i: %f\n", iScaleIndex, (*parrayScales)[iScaleIndex]);
      };

    };
  };

//-----------------------------------------------------------------------------
EStatus  Curve::SolveForPointDist (FLOAT           fDistIn,
                                   FloatArray &    afSegmentLengths,  // intermediate data
                                   RVec3Array  &   arrayPoints,       // intermediate data
                                   RVec3 &         vecPointOut,
                                   INT32 &         iSegmentIndexOut,
                                   FLOAT &         fSegmentPosOut,
                                   RMatrixArray *  parrayCvMatrix,
                                   RMatrixArray *  parrayOrientations, // intermediate data
                                   FloatArray *    parrayTwists,       // intermediate data
                                   FloatArray *    parrayScales,       // intermediate data
                                   RMatrix *       pmatOrientOut)
  {
  INT         iNumSegmentSamples = RCURVE_DEFAULT_SUBSEGMENTS;
  INT         iNumSegments       = avecControlVerts.Length () - 1;
  INT         iNumSamplesTotal   = (iNumSegmentSamples * iNumSegments) + 1;
  INT         iIndex;
  INT         iSegIndex;
  INT         iSampleIndex;
  FLOAT       fSegmentPosCurr;
  RMatrix     matTwist;
  RMatrix     matTwistedOrient;
  RMatrix     matScale;

  if (pmatOrientOut != NULL)
    {
    pmatOrientOut->Identity ();
    };

  SampleCurve (afSegmentLengths,
               arrayPoints,
               parrayCvMatrix,
               parrayOrientations,
               parrayTwists,
               parrayScales);

  // now that we have initialized our intermediate data, find which samples the requested
  //  distance lies between.

  iIndex = 0;
  for (iSegIndex = 0; iSegIndex < iNumSegments; ++iSegIndex)
    {
    for (iSampleIndex = 0; iSampleIndex < iNumSegmentSamples; ++iSampleIndex, ++iIndex)
      {
      if (fDistIn <= afSegmentLengths [iIndex])
        {
        // the index just passed our test distance.  The point lies between
        //  iIndex and (iIndex - 1)
        if (iIndex == 0)
          {
          // first point or before.

          vecPointOut      = arrayPoints [0];
          iSegmentIndexOut = 0;
          fSegmentPosOut   = 0.0f;

          if (pmatOrientOut != NULL)
            {
            *pmatOrientOut = (*parrayOrientations) [0];

            RVec4 vecFrom = (*parrayOrientations) [0].ToQuat ();
            RVec4 vecTo   = (*parrayOrientations) [0].ToQuat ();

            RVec4 vecNewPos = vecFrom.QuatSLERP (vecTo, 0.1);

            pmatOrientOut->FromQuat (vecNewPos);
            };
          }
        else
          {
          // somewhere in the middle.

          // calculate fraction along length
          fSegmentPosCurr  = (fDistIn - afSegmentLengths [iIndex - 1]) /
                             (afSegmentLengths [iIndex] -  afSegmentLengths [iIndex - 1]);
          fSegmentPosOut   = fSegmentPosCurr + (float (iSampleIndex) / float (iNumSegmentSamples));
          iSegmentIndexOut = iIndex - 1;

          vecPointOut      = LERP (arrayPoints [iIndex - 1],
                                   arrayPoints [iIndex],
                                   fSegmentPosCurr);
          if ((parrayCvMatrix != NULL) &&
              (parrayOrientations != NULL) &&
              (pmatOrientOut != NULL) &&
              (parrayTwists != NULL))
            {
            RVec4 quatFrom = (*parrayOrientations) [iIndex - 1].ToQuat ();
            RVec4 quatTo   = (*parrayOrientations) [iIndex].ToQuat ();

            RVec4 quatNewPos = quatFrom.QuatSLERP (quatTo, fSegmentPosCurr);

            pmatOrientOut->FromQuat (quatNewPos);

            DOUBLE  dTwistAngle = (*parrayTwists) [iIndex - 1] + ((*parrayTwists) [iIndex] - (*parrayTwists) [iIndex - 1]) * fSegmentPosCurr;
            matTwist.SetRotateZ (dTwistAngle);

            (*pmatOrientOut) = (*pmatOrientOut) * matTwist;
            };


          if ((pmatOrientOut != NULL) &&
              (parrayScales != NULL))
            {
            RVec4 quatFrom = (*parrayOrientations) [iIndex - 1].ToQuat ();
            RVec4 quatTo   = (*parrayOrientations) [iIndex].ToQuat ();

            RVec4 quatNewPos = quatFrom.QuatSLERP (quatTo, fSegmentPosCurr);

            pmatOrientOut->FromQuat (quatNewPos);

            DOUBLE  dScale = (*parrayScales) [iIndex - 1] + ((*parrayScales) [iIndex] - (*parrayScales) [iIndex - 1]) * fSegmentPosCurr;
            printf ("Scale at dist %f is index %i value A %g B %g blend %g final %f\n", fDistIn, iIndex, (*parrayScales) [iIndex-1], (*parrayScales) [iIndex], fSegmentPosCurr, dScale);
            matScale.SetScale (1.0, dScale, dScale);

            (*pmatOrientOut) = (*pmatOrientOut) * matScale;
            };
          };
        return (EStatus::kSuccess);
        };
      };
    };

  // past the end.  Return the last point
  vecPointOut      = arrayPoints [iNumSamplesTotal - 1];
  iSegmentIndexOut = iNumSegments;
  fSegmentPosOut   = 1.0f;
  if (pmatOrientOut != NULL)
    {
    matTwist.SetRotateZ ((*parrayTwists) [iNumSamplesTotal - 1]);
    *pmatOrientOut = (*parrayOrientations) [iNumSamplesTotal - 1] * matTwist;
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
VOID  Curve::PrintDebug (VOID)
  {
  int  iIndex;

  printf ("    Curve - avecControlVerts      size %d\n", avecControlVerts.Length ());
  for (iIndex = 0; iIndex < avecControlVerts.Length (); ++iIndex)
    {
    printf ("        %i: %g %g %g\n", iIndex, avecControlVerts[iIndex].fX, avecControlVerts[iIndex].fY, avecControlVerts[iIndex].fZ);
    };
  printf ("    Curve - avecInTangents  size %d\n", avecInTangents.Length ());
  for (iIndex = 0; iIndex < avecInTangents.Length (); ++iIndex)
    {
    printf ("        %i: %g %g %g\n", iIndex, avecInTangents[iIndex].fX, avecInTangents[iIndex].fY, avecInTangents[iIndex].fZ);
    };
  printf ("    Curve - avecOutTangents size %d\n", avecOutTangents.Length ());
  for (iIndex = 0; iIndex < avecOutTangents.Length (); ++iIndex)
    {
    printf ("        %i: %g %g %g\n", iIndex, avecOutTangents[iIndex].fX, avecOutTangents[iIndex].fY, avecOutTangents[iIndex].fZ);
    };
  printf ("    Curve - afTwistWeights  size %d\n", afTwistWeights.Length ());
  for (iIndex = 0; iIndex < afTwistWeights.Length (); ++iIndex)
    {
    printf ("        %i: %g\n", iIndex, afTwistWeights[iIndex]);
    };
  };


