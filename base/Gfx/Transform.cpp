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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"
#include "Gfx/Transform.hpp"

#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO

//-----------------------------------------------------------------------------
//  Transform
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Transform::Transform  ()
  {
  Reset ();
  bLocalDirty = TRUE;
  bWorldDirty = TRUE;
  bRectDirty  = TRUE;
  bSizeDirty  = TRUE;
  pParent     = NULL;
  pSibling    = NULL;
  pChildren   = NULL;

  bEulerRotate        = TRUE;
  bCalcFromComponents = TRUE;

  matLocal.Identity ();
  matWorld.Identity ();

  pRectSolver = NULL;

  pszID = NULL;
  };


//-----------------------------------------------------------------------------
Transform::~Transform ()
  {

  if (pRectSolver != NULL)
    {
    delete (pRectSolver);
    pRectSolver = NULL;
    };
  };

//-----------------------------------------------------------------------------
VOID Transform::Reset (VOID)
  {
  vecTranslate.Set (0.0f, 0.0f, 0.0f);
  eulRotate.Set  (0.0f, 0.0f, 0.0f);
  vecScale.Set     (1.0f, 1.0f, 1.0f);
  };

//-----------------------------------------------------------------------------
Transform &  Transform::SetPosition (FLOAT          fX,
                                     FLOAT          fY,
                                     FLOAT          fZ)
  {
  vecTranslate.Set (fX, fY, fZ);

  OPT_DBG_INFO ("Transform::SetPosition trans %f %f %f ID: %s", vecTranslate.fX, vecTranslate.fY, vecTranslate.fZ, pszID);

  bCalcFromComponents = TRUE;
  bLocalDirty = TRUE;
  MarkHierarchyDirty ();
  return (*this);
  };

//-----------------------------------------------------------------------------
Transform &  Transform::CalcLocalMatrix ()
  {
  if (bLocalDirty)
    {
    if (bCalcFromComponents)
      {
      RMatrix  matScale (vecScale.fX, 0.0f, 0.0f, 0.0f,
                        0.0f, vecScale.fY, 0.0f, 0.0f,
                        0.0f, 0.0f, vecScale.fZ, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);

      RMatrix  matRotate;
      if (bEulerRotate)
        {
        eulRotate.ToMatrix (matRotate);
        }
      else
        {
        matRotate.FromQuat (quatRotate);
        };

      // TODO: Does scale need to be on the right?  Doesn't matter for proportional
      //  scaling, but is it correct for non-proportional scaling?
      matLocal = matScale * matRotate;
      matLocal.SetTrans (vecTranslate.fX, vecTranslate.fY, vecTranslate.fZ);
      OPT_DBG_INFO ("Transform::CalcLocalMatrix trans %f %f %f ID: %s", vecTranslate.fX, vecTranslate.fY, vecTranslate.fZ, pszID);
      };

    bLocalDirty = FALSE;
    }
  return (*this);
  };


//-----------------------------------------------------------------------------
Transform &  Transform::CalcWorldMatrix ()
  {
  if (bWorldDirty)
    {
    CalcLocalMatrix ();

    if (pParent != NULL)
      {
      //DBG_INFO ("Solving for parent matrix");
      RMatrix  matParent;
      pParent->GetWorldMatrix (matParent);

      // combine parent and local matrix to get the world matrix.
      matWorld = matParent * matLocal;
      }
    else
      {
      // world matrix is same as local matrix
      matWorld = matLocal;
      };
    bWorldDirty = FALSE;
    };
  return (*this);
  };


//-----------------------------------------------------------------------------
VOID Transform::GetLocalMatrix (RMatrix &  matOut)
  {
  CalcLocalMatrix ();
  matOut = matLocal;
  };


//-----------------------------------------------------------------------------
VOID Transform::GetWorldMatrix (RMatrix &  matOut)
  {
  CalcWorldMatrix ();
  matOut = matWorld;
  };

//-----------------------------------------------------------------------------
VOID Transform::SetParent (Transform *  pParentIn)
  {
  UnParent ();
  pParent = pParentIn;
  if (pParent != NULL)
    {
    pSibling = pParent->pChildren;
    pParent->pChildren = this;
    pParent->MarkHierarchyDirty ();
    };
  }

//-----------------------------------------------------------------------------
VOID Transform::UnParent (VOID)
  {
  // remove from parent's children
  if (pParent != NULL)
    {
    Transform *  pCurr;
    Transform *  pPrev = NULL;

    pCurr = pParent->pChildren;
    while (pCurr != NULL)
      {
      if (pCurr == this)
        {
        // found ourself in the children list of our parent.  Remove.
        if (pPrev == NULL)
          {
          pParent->pChildren = pCurr->pSibling;
          }
        else
          {
          pPrev->pSibling = pCurr->pSibling;
          }
        pParent = NULL;
        pSibling = NULL;
        return;
        }
      pPrev = pCurr;
      pCurr = pCurr->pSibling;
      };
    // didn't find ourself as a child of our parent.  Error!
    DBG_ERROR ("Transform::Unparent() : Corrupt heirarchy detected!");
    };
  }

//-----------------------------------------------------------------------------
VOID Transform::ClearParent (VOID)
  {
  bLocalDirty = TRUE;
  bWorldDirty = TRUE;
  pParent     = NULL;
  pSibling    = NULL;
  pChildren   = NULL;
  };

//-----------------------------------------------------------------------------
VOID Transform::MarkHierarchyDirty (VOID)
  {
  // mark self
  bWorldDirty = TRUE;

  // mark children
  Transform *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    pCurr->MarkHierarchyDirty ();
    pCurr = pCurr->pSibling;
    };
  }

//-----------------------------------------------------------------------------
BOOL  Transform::AncestorsAreDirty (VOID)
  {
  Transform *  pCurr = this;

  while (pCurr != NULL)
    {
    if (pCurr->bLocalDirty || pCurr->bWorldDirty)
      {
      return (TRUE);
      };

    pCurr = pCurr->pParent;
    };
  return (FALSE);
  };

//-----------------------------------------------------------------------------
VOID Transform::MarkRectHierarchyDirty (VOID)
  {
  OPT_DBG_INFO ("Transform::MarkRectHierarchyDirty ()");
  // mark self
  bRectDirty = TRUE;
  bSizeDirty = TRUE;

  // mark children (for dirty rect)
  Transform *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    pCurr->MarkRectHierarchyDirtyRecurse ();
    pCurr = pCurr->pSibling;
    };

  // mark parents (for dirty size)
  pCurr = pParent;
  while (pCurr != NULL)
    {
    pCurr->bSizeDirty = TRUE;
    pCurr = pCurr->pParent;
    };
  }

//-----------------------------------------------------------------------------
VOID Transform::MarkRectHierarchyDirtyRecurse (VOID)
  {
  // mark self
  bRectDirty = TRUE;

  // mark children
  Transform *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    pCurr->MarkRectHierarchyDirtyRecurse ();
    pCurr = pCurr->pSibling;
    };
  }

//-----------------------------------------------------------------------------
Transform *  Transform::FindCleanRectParent (VOID)
  {
  // step up the heirarchy until you find a rect that isn't dirty

  if (bRectDirty == FALSE) return (this);

  // search parent
  if (pParent == NULL) return (this);

  return (pParent->FindCleanRectParent ());
  }

//-----------------------------------------------------------------------------
VOID Transform::SolveRects (BOOL  bForceSolveThis)
  {
  // Solve rectangles from this transform down.  Can be called on
  //  results from FindCleanRectParent
  OPT_DBG_INFO ("Transform::SolveRects Start for solver %x %s", (long)pRectSolver, pszID);

  if (bSizeDirty) {SolveRectSize ();};

  if ((! bRectDirty) && (bForceSolveThis == FALSE))
    {
    DBG_INFO ("Transform::SolveRects %s not dirty", pszID);
    return;
    };

  // solve self
  if (pRectSolver == NULL)
    {
    // if there is no rect solver, use the parent's dimensions.
    if (pParent == NULL)
      {
      rect.SetUnit ();
      }
    else
      {
      FLOAT  fHalfWidth  = pParent->rect.GetWidth  () * 0.5;
      FLOAT  fHalfHeight = pParent->rect.GetHeight () * 0.5;

      rect.Set (vecTranslate.fX - fHalfWidth, vecTranslate.fY - fHalfHeight,
                vecTranslate.fX + fHalfWidth, vecTranslate.fY + fHalfHeight);
      };
    }
  else
    {
    pRectSolver->Solve (&rect, (pParent == NULL) ? NULL : &(pParent->rect));

    OPT_DBG_INFO ("Transform::SolveRects for \"%s\" (num children: %d) XDim (%0.2f to %0.2f W: %0.2f) YDim (%0.2f to %0.2f H: %0.2f)",
                  pszID, CountChildren (),
                  rect.fX1, rect.fX2, rect.fX2 - rect.fX1,
                  rect.fY1, rect.fY2, rect.fY2 - rect.fY1);

    // (ptr: %x)(long)this,

    // NOTE:  Need to determine if this translation setting is needed or not.
    // TODO: Take some sort of animatable offset into consideration?
    //        Or is the better handled at the TransformComponent?
    //        Current plan... implement animated offset for translate,
    //        use transforms rotate and scale for those anims.

    // We can't call SetPosition because it will dirty the hierarchy at the same
    //  time we are solving the hierarcy.  Instead, just set the translate and
    //  mark the local matrix to be recalculated if it is read.
    vecTranslate.Set ((rect.fX1 + rect.fX2) / 2.0f ,
                      (rect.fY1 + rect.fY2) / 2.0f ,
                      vecTranslate.fZ);
    //bLocalDirty = TRUE;
    };
  bRectDirty = FALSE;
  //DBG_INFO ("Transform::SolveRects Solving Rect for %x %s position (%f, %f) from %f,%f", (long)this, pszID,
  //          (rect.fX1 + rect.fX2) / 2.0f ,
  //          (rect.fY1 + rect.fY2) / 2.0f,
  //          rect.fX1, rect.fX2);

  // solve children
  Transform *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    pCurr->SolveRects ();
    pCurr = pCurr->pSibling;
    };
  };


//-----------------------------------------------------------------------------
VOID Transform::SolveRectSize (VOID)
  {
  // NOTE:  I don't think this gives us any useful information.
  //   For this to work, solvers like AnchorOffset would have to
  //   take minimum dimentions from children like text strings and
  //   sprites, and recalc the AO min dimension from that.  Sizers
  //   would likewise have to pass that back up.  This is all to
  //   support dynamic sizing controls.
  // You will also need to develop text-sizers, or set sizers that are
  //   updated by the FormattedText control (TextComponent)
  // For now, I'm implementing Grid and Horiz sizers that don't
  //   dynamically resize.


  // depth-first traverse children to solve sizes marked dirty.

  if (! bSizeDirty) return;

  // solve children first
  Transform *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    pCurr->SolveRectSize ();
    pCurr = pCurr->pSibling;
    };

  // solve self
  bSizeDirty = FALSE;

  if (pRectSolver == NULL)
    {
    vecMinSize.Set (-1.0f, -1.0f);
    }
  else
    {
    pRectSolver->Size (vecMinSize);
    };
  };


//-----------------------------------------------------------------------------
VOID  Transform::SetRectSolver (RectSolver *  pSolverIn,
                                const char *  pszIDIn)
  {
  if (pRectSolver != NULL)
    {
    delete (pRectSolver);
    };
  OPT_DBG_INFO ("Setting Rect solver of transform %x to %x", (long)this, (long)pSolverIn);
  pRectSolver = pSolverIn;
  pRectSolver->strIdentifier.Set(pszIDIn);
  pszID = pszIDIn;
  MarkRectHierarchyDirty ();
  };


