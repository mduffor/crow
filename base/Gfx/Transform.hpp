/* -----------------------------------------------------------------
                             Transform

     This module implements a transformation matrix consisting of
     translate, rotate, and scale components.  It also implements
     parenting for the purpose of implementing scene graphs.

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

#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Sys/Types.hpp"
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"
#include "Math/Rect.hpp"
#include "Math/RectSolver.hpp"
#include "Gfx/Euler.hpp"


/**

 TODO: UnitTest

  */


//-----------------------------------------------------------------------------
class Transform
  {
  public:

    RVec3        vecTranslate;
    Euler        eulRotate;
    RVec4        quatRotate;
    RVec3        vecScale;

    RMatrix      matLocal;
    RMatrix      matWorld;

    Transform *  pParent;
    Transform *  pSibling;
    Transform *  pChildren;

    BOOL         bLocalDirty;
    BOOL         bWorldDirty;

    BOOL         bRectDirty;
    BOOL         bSizeDirty;

    BOOL         bEulerRotate;
    BOOL         bCalcFromComponents;

    RectF         rect;
    RVec2         vecMinSize;  // The minimum size the element desires to be.
    RectSolver *  pRectSolver;

    const char *  pszID;

  public:

               Transform       ();

               ~Transform      ();

    VOID       Reset           (VOID)  ;

    Transform &  SetPosition   (FLOAT          fX,
                                FLOAT          fY,
                                FLOAT          fZ);//      {vecTranslate.Set (fX, fY, fZ); bCalcFromComponents = TRUE; bLocalDirty = TRUE; MarkHierarchyDirty ();};

    Transform &  SetEuler      (FLOAT          fXDeg,
                                FLOAT          fYDeg,
                                FLOAT          fZDeg,
                                Euler::EOrder  eOrder = Euler::XYZ)  {eulRotate.Set (fXDeg, fYDeg, fZDeg, eOrder); bCalcFromComponents = TRUE; bLocalDirty = TRUE; bEulerRotate = TRUE; MarkHierarchyDirty (); return (*this);};

    Transform &  SetQuaternion (FLOAT          fX,
                                FLOAT          fY,
                                FLOAT          fZ,
                                FLOAT          fW)      {quatRotate.Set (fX, fY, fZ, fW); bCalcFromComponents = TRUE; bLocalDirty = TRUE; bEulerRotate = FALSE; MarkHierarchyDirty (); return (*this);};

    Transform &  SetScale      (FLOAT          fX,
                                FLOAT          fY,
                                FLOAT          fZ)      {vecScale.Set (fX, fY, fZ); bCalcFromComponents = TRUE; bLocalDirty = TRUE; MarkHierarchyDirty (); return (*this);};

    Transform &  CalcLocalMatrix ();

    Transform &  CalcWorldMatrix ();


    Transform &  operator=   (const Transform &  trans)  {vecTranslate = trans.vecTranslate;
                                                          eulRotate = trans.eulRotate;
                                                          quatRotate = trans.quatRotate;
                                                          vecScale = trans.vecScale;
                                                          matLocal = trans.matLocal;
                                                          matWorld = trans.matWorld;
                                                          pParent = NULL;
                                                          pSibling = NULL;
                                                          pChildren = NULL;
                                                          bLocalDirty = TRUE;
                                                          bWorldDirty = TRUE;
                                                          bRectDirty = TRUE;
                                                          bSizeDirty = TRUE;
                                                          bEulerRotate = trans.bEulerRotate;
                                                          bCalcFromComponents = trans.bCalcFromComponents;
                                                          rect = trans.rect;
                                                          vecMinSize = trans.vecMinSize;
                                                          pRectSolver = NULL; // NOTE: Should we create a clone of the rect solver?
                                                          pszID = NULL;
                                                          return (*this);};


    VOID       GetLocalMatrix  (RMatrix &  matOut);

    VOID       GetWorldMatrix  (RMatrix &  matOut);

    RMatrix &  LocalMatrix     (VOID)                   {CalcLocalMatrix (); return matLocal;};

    RMatrix &  WorldMatrix     (VOID)                   {CalcWorldMatrix (); return matWorld;};

    RMatrix *  WorldMatrixPtr  (VOID)                   {CalcWorldMatrix (); return &matWorld;};

    VOID       SetLocalMatrix  (const FLOAT *  afArrayIn)   {bCalcFromComponents = FALSE; matLocal.SetF (afArrayIn); MarkHierarchyDirty ();};

    VOID       SetLocalMatrix  (const RMatrix &  matIn)     {bCalcFromComponents = FALSE; matLocal.Set (matIn); MarkHierarchyDirty ();};

    VOID       SetParent       (Transform *  pParentIn);

    VOID       UnParent        (VOID);  // Unparent properly removes a node from its parent hierarchy

    VOID       ClearParent     (VOID);  // ClearParent quickly just clears the parent and child.  Only run on an entire hierarchy.

    BOOL         IsRoot                 (VOID)     {return (pParent == NULL);};

    VOID         MarkHierarchyDirty     (VOID);

    BOOL         AncestorsAreDirty      (VOID);

    VOID         MarkRectDirty          (VOID)                       {bRectDirty = TRUE;};

    VOID         MarkRectHierarchyDirty (VOID);

    VOID         MarkRectHierarchyDirtyRecurse (VOID);

    Transform *  FindCleanRectParent    (VOID);

    VOID         SolveRects             (BOOL  bForceSolveThis = FALSE);

    VOID         SolveRectSize          (VOID);

    VOID         SetRectSolver          (RectSolver *  pSolverIn,
                                         const char *  pszIDIn);

    INT          CountChildren          (VOID)    {INT iCount = 0; for (Transform *  pNext = pChildren; pNext != NULL; pNext = pNext->pSibling) {++iCount;}; return iCount;};

  };



#endif // TRANSFORM_HPP
