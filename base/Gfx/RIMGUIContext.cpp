


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RTypes.hpp"

#include "RDebug.hpp"
ASSERTFILE (__FILE__)

#include "RCurve.hpp"

#include "RIMGUIContext.hpp"
#include "ext_image/RColors.hpp"
#include "ext_sprite/RDrawPrim.hpp"
#include "ext_image/RCanvas.hpp"
#include "ext_image/RPixelDesc.hpp"
#include "ext_widgets/RHotKeys.hpp"
#include "ext_mesh/RStateMan.hpp"
#include "ext_mesh/RGlobalMesh.hpp"


// global debug switches
static BOOL     g_DbgSizers = FALSE;
//static BOOL     g_DbgSizers = TRUE;


//==============================================================================
// IMGUI Region
//==============================================================================


//------------------------------------------------------------------------------
IGRegion::IGRegion ()
  {
  IGID  idZero (0,0);

  pNext = NULL;

  id          = idZero;
  iX          = 0;
  iY          = 0;
  iWidth      = 0;
  iHeight     = 0;
  iDepth      = 0;
  iSizerFlags = 0;
  };


//------------------------------------------------------------------------------
IGRegion::IGRegion  (IGID  idIn,
                     INT   iXIn,
                     INT   iYIn,
                     INT   iWidthIn,
                     INT   iHeightIn,
                     INT   iDepthIn,
                     INT   iFlagsIn)
  {
  pNext = NULL;

  id          = idIn;
  iX          = iXIn;
  iY          = iYIn;
  iWidth      = iWidthIn;
  iHeight     = iHeightIn;
  iDepth      = iDepthIn;
  iSizerFlags = iFlagsIn;
  };


//------------------------------------------------------------------------------
IGRegion::~IGRegion ()
  {
  };


//------------------------------------------------------------------------------
BOOL  IGRegion::IsHit (INT  iXIn,
                       INT  iYIn)
  {
  if ((iXIn < iX) ||
      (iYIn < iY) ||
      (iXIn >= iX + iWidth) ||
      (iYIn >= iY + iHeight))
    {
    return (FALSE);
    };
  return (TRUE);
  };


//------------------------------------------------------------------------------
VOID  IGRegion::Clear (VOID)
  {
  iX = iY = iWidth = iHeight = 0;
  pNext = NULL;
  };


//------------------------------------------------------------------------------
IGRegion &  IGRegion::operator= (IGRegion &  regIn)
  {
  id          = regIn.id;
  iX          = regIn.iX;
  iY          = regIn.iY;
  iWidth      = regIn.iWidth;
  iHeight     = regIn.iHeight;
  pNext       = NULL;
  iDepth      = regIn.iDepth;
  iSizerFlags = regIn.iSizerFlags;

  return *this;
  };



//==============================================================================
// IMGUI Region Stack
//==============================================================================

// NOTE: Since I'm not doing an IMGUI system, the regions can persist.
//  May be able to get rid of the stack nature of this, and use transform stack
//  directly.
//------------------------------------------------------------------------------
IGRegionStack::IGRegionStack ()
  {
  pPool  = NULL;
  pStack = NULL;
  pTail  = NULL;
  };


//------------------------------------------------------------------------------
IGRegionStack::~IGRegionStack ()
  {
  Release ();
  };


//------------------------------------------------------------------------------
VOID IGRegionStack::Release (VOID)
  {
  // move all items to the pool
  Reset ();

  // free the pool
  while (pPool)
    {
    IGRegion *  pToDelete = pPool;

    pPool = pPool->pNext;
    delete pToDelete;
    };
  pStack = pPool = pTail = NULL;
  };


//------------------------------------------------------------------------------
VOID IGRegionStack::Reset (VOID)
  {
  // while there are items in the stack, move them to the pool
  while (pStack != NULL)
    {
    IGRegion *  pCurr = pStack;
    pStack = pStack->pNext;
    pCurr->pNext = pPool;
    pPool = pCurr;
    };
  pTail = NULL;
  };


//------------------------------------------------------------------------------
VOID IGRegionStack::Push (IGRegion &  regIn)
  {
  IGRegion *    pCurr = NULL;

  // pull from the pool if available
  if (pPool)
    {
    pCurr = pPool;
    pPool = pPool->pNext;
    };

  // allocate if not available
  if (! pCurr)
    {
    pCurr = new IGRegion;
    };

  // store
  *pCurr = regIn;

  // push onto the tail of the stack
  pCurr->pNext = NULL;
  if (pTail != NULL)
    {
    pTail->pNext = pCurr;
    pTail        = pCurr;
    }
  else
    {
    // no tail.  empty stack
    pStack = pTail = pCurr;
    };

  // debugging
  /*
  IGRegion *  pCount = pStack;
  INT         iCount = 0;
  while (pCount)
    {
    ++iCount;
    pCount = pCount->pNext;
    };
  printf ("After push, stack size of %d\n", iCount);
  */
  };


//------------------------------------------------------------------------------
BOOL  IGRegionStack::Pop (IGRegion &  regOut)
  {
  regOut.Clear ();

  if (! pStack)
    {
    return (FALSE);
    };

  // get the return value
  regOut = *pStack;

  // now that we've copied the values, remove the top item of the stack to the pool
  IGRegion *  pCurr = pStack;

  pStack       = pStack->pNext;
  pCurr->pNext = pPool;
  pPool        = pCurr;

  if (pStack == NULL)
    {
    pTail = NULL;
    };

  return (TRUE);
  };





//==============================================================================
// IMGUI Rect
//==============================================================================

// NOTE:  This should be the simplest of IGWidgets to use as reference.

// Default Colors

UINT  IGRect::uDefColor_BGBackColor = RGB_GRAY_4;
UINT  IGRect::uDefColor_Border      = RGB_GRAY_2;


//------------------------------------------------------------------------------
IGRect::IGRect  ()
  {
  //uBGBackColor = uDefColor_BGBackColor;
  uBorderColor = uDefColor_Border;

  fillBGDefault.SetColor (uDefColor_BGBackColor);
  pfillBG = &fillBGDefault;
  };


//------------------------------------------------------------------------------
IGRect::~IGRect ()
  {
  };


//------------------------------------------------------------------------------
INT  IGRect::Do (IGID         idIn,
                  IGContext &  ctxIn,
                  INT          iXPosIn,
                  INT          iYPosIn,
                  INT          iWidthIn,
                  INT          iHeightIn,
                  INT          iSizerFlagsIn) const
  {
  IGRegion   regBounds;
  IGID       idZero   (0,0);

  ctxIn.SizerResize (idIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn, iSizerFlagsIn);
  if (ctxIn.eSizerMode == IGContext::kMeasure) return 0;

  regBounds.iX      = iXPosIn;
  regBounds.iY      = iYPosIn;
  regBounds.iWidth  = iWidthIn;
  regBounds.iHeight = iHeightIn;

  // frames don't interact with the mouse or keyboard.

  // render the frame

  if (! ctxIn.IsSkipMode ())
    {
    Render (idIn, ctxIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn);
    };

  // frames are inert
  return 0;
  };


//------------------------------------------------------------------------------
VOID  IGRect::Render (IGID        idIn,
                        IGContext & ctxIn,
                        INT         iXPosIn,
                        INT         iYPosIn,
                        INT         iWidthIn,
                        INT         iHeightIn) const
  {
  DBG_INFO ("IGRect::Render start");
  //printf ("RDrawPrim instance %s %d\n", __FILE__, __LINE__);
  RDrawPrim  dpDraw (ctxIn.pcnvDrawCanvas, ctxIn.Is3DMode (), ctxIn.psmSprites, ctxIn.pfntRegistry);


  // draw the backdrop
  pfillBG->Draw (ctxIn.pcnvDrawCanvas, iXPosIn, iYPosIn, iWidthIn, iHeightIn, ctxIn.Is3DMode ());


  // draw the border
  dpDraw.SetDrawColor (uBorderColor);
  dpDraw.Rectangle (iXPosIn,
                    iYPosIn,
                    iXPosIn + iWidthIn - 1,
                    iYPosIn + iHeightIn - 1);
  DBG_INFO ("IGRect::Render end");
  };


//==============================================================================
// IMGUI HSizer
//==============================================================================

// Default Colors

UINT  IGHSizer::uDefColor_BGBackColor = RGB_GRAY_4;
UINT  IGHSizer::uDefColor_Border      = RGB_GRAY_2;


//------------------------------------------------------------------------------
IGHSizer::IGHSizer  ()
  {
  uBGBackColor = uDefColor_BGBackColor;
  uBorderColor = uDefColor_Border;
  };


//------------------------------------------------------------------------------
IGHSizer::~IGHSizer ()
  {
  };


//------------------------------------------------------------------------------
INT  IGHSizer::Do (IGID         idIn,
                   IGContext &  ctxIn,
                   INT          iXPosIn,
                   INT          iYPosIn,
                   INT          iWidthIn,
                   INT          iHeightIn,
                   INT          iSizerFlagsIn) const
  {
  IGRegion   regBounds;
  IGID       idZero   (0,0);

  if (g_DbgSizers)  printf ("IGHSizer:Do - Begin\n");

  // sizers are themselves windows, so resize accordingly
  ctxIn.SizerResize (idIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn, iSizerFlagsIn);

  if (g_DbgSizers)  printf ("HSizer %d:%d expanded to size of of x:%d y:%d  w:%d  h:%d\n", idIn.iIdentifier, idIn.iOrdinal, iXPosIn, iYPosIn, iWidthIn, iHeightIn);

  // children will be at the next sizer depth
  ++ctxIn.iSizerDepth;

  // if this control is a frame, it has its own canvas.
  //   Reset its coordinates to the canvas origin.
  if (iSizerFlagsIn & IGRegion::kFrame)
    {
    iXPosIn = 0;
    iYPosIn = 0;
    };


  // debugging
  // let's see what the region stack looks like
  /*
    {
    printf ("\nSizerRegionStack for HSizer is\n");
    IGRegion *  pDbgCurr = ctxIn.stackSizer.pStack;
    while (pDbgCurr)
      {
      printf ("  Stack depth %d  x: %d y: %d w: %d h: %d\n",
              pDbgCurr->iDepth,
              pDbgCurr->iX, pDbgCurr->iY,
              pDbgCurr->iWidth, pDbgCurr->iHeight);

      pDbgCurr = pDbgCurr->pNext;
      };
    };
 */

  if (ctxIn.eSizerMode == IGContext::kMeasure)
    {
    // We measure ourself with the SizerResize call above.  We have to
    //  let all our children add themselves to the sizer list too, so
    //  for this pass we do nothing.
    }
  else if (ctxIn.eSizerMode == IGContext::kRender)
    {
    // The children widgets will soon be rendered.  Step through all
    //  children at this level and resize them according to their
    //  stored parameters and flags.

    // find our starting point
    IGRegion *  pCurr = ctxIn.stackSizer.pStack;
    IGRegion *  pFirstChild = NULL;
    INT         iChildDepth;
    while (pCurr)
      {
      if (pCurr->id == idIn)
        {
        // found ourself.  Start with our first child
        iChildDepth = pCurr->iDepth + 1;
        pFirstChild = pCurr->pNext;
        break;
        };
      pCurr = pCurr->pNext;
      };
    if (g_DbgSizers)  printf ("HSizer render, pos (%d,%d) size %dx%d\n", iXPosIn, iYPosIn, iWidthIn, iHeightIn);

    // now we have our first child node and all the info we need to traverse them.
    // First, step through the children and count them.  Also calculate how much
    // room is required for the flags.
    INT  iStaticReservedSpace       = 0;
    INT  iProportionalValueSum      = 0;
    INT  iNumChildren               = 0;
    INT  iNumPropChildren           = 0;

    if (g_DbgSizers)  if (pFirstChild) printf ("HSizer %d:%d has its first child at depth %d (%d)\n", idIn.iIdentifier, idIn.iOrdinal, pFirstChild->iDepth, iChildDepth);
    pCurr = pFirstChild;
    while (pCurr)
      {
      //printf ("   search depth of %d\n", pCurr->iDepth);
      if (pCurr->iDepth < iChildDepth)
        {
        // we've stepped out of our list of children.  Stop scanning.
        break;
        };
      if (pCurr->iDepth == iChildDepth)
        {
        // this is one of our children.
        ++iNumChildren;

        if (g_DbgSizers)  printf ("  HSizer %d:%d modifies child %d:%d (depth %d)\n", idIn.iIdentifier, idIn.iOrdinal, pCurr->id.iIdentifier, pCurr->id.iOrdinal, pCurr->iDepth);

        if (pCurr->iSizerFlags & IGRegion::kProportionalWidth)
          {
          iProportionalValueSum += pCurr->iWidth;
          ++iNumPropChildren;
          }
        else if (pCurr->iSizerFlags & IGRegion::kPercentWidth)
          {
          iStaticReservedSpace += (iWidthIn * pCurr->iWidth) / 100;
          }
        else if (pCurr->iSizerFlags & IGRegion::kFixedWidth)
          {
          iStaticReservedSpace += pCurr->iWidth;
          };
        };
      pCurr = pCurr->pNext;
      };


    // now that we have data about all the children, we can set the actual values.

    pCurr = pFirstChild;
    INT  iCurrX             = iXPosIn;
    INT  iChildIndex        = 0;
    INT  iChildPropIndex    = 0;
    INT  iProportionalSpace = iWidthIn - iStaticReservedSpace;
    INT  iProportionalSpaceRemaining = iProportionalSpace;

    if (g_DbgSizers)  printf ("Found %d children (%d prop children), prop sum %d, static %d, prop %d\n", iNumChildren, iNumPropChildren, iProportionalValueSum, iStaticReservedSpace, iProportionalSpace);

    while (pCurr)
      {
      if (pCurr->iDepth < iChildDepth)
        {
        // we've stepped out of our list of children.  Stop scanning.
        break;
        };
      if (pCurr->iDepth == iChildDepth)
        {
        // this is one of our children.
        ++iChildIndex;
        pCurr->iX      = iCurrX;
        pCurr->iY      = iYPosIn;
        pCurr->iHeight = iHeightIn;

        if (pCurr->iSizerFlags & IGRegion::kProportionalWidth)
          {
          ++iChildPropIndex;
          if (iChildPropIndex == iNumPropChildren)
            {
            // this is the last proportional child.  Give it all the remaining space
            pCurr->iWidth = iProportionalSpaceRemaining;
            }
          else
            {
            pCurr->iWidth = iProportionalSpace * pCurr->iWidth / iProportionalValueSum;
            iProportionalSpaceRemaining -= pCurr->iWidth;
            };
          }
        else if (pCurr->iSizerFlags & IGRegion::kPercentWidth)
          {
          pCurr->iWidth = (iWidthIn * pCurr->iWidth) / 100;
          }
        else if (pCurr->iSizerFlags & IGRegion::kFixedWidth)
          {
          // leave the width as-is.
          };

        if (g_DbgSizers)  printf ("Child set to (%d,%d) %dx%d\n", pCurr->iX, pCurr->iY, pCurr->iWidth, pCurr->iHeight);

        iCurrX += pCurr->iWidth;
        };
      pCurr = pCurr->pNext;
      };
    };

  if (ctxIn.eSizerMode == IGContext::kMeasure) return 0;

  regBounds.iX      = iXPosIn;
  regBounds.iY      = iYPosIn;
  regBounds.iWidth  = iWidthIn;
  regBounds.iHeight = iHeightIn;

  // frames don't interact with the mouse or keyboard.

  // render the frame
  if ((iSizerFlagsIn & IGRegion::kNoRender) == 0)
    {
    if (! ctxIn.IsSkipMode ())
      {
      Render (idIn, ctxIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn);
      };
    };
  if (g_DbgSizers)  printf ("IGHSizer:Do - End\n");

  // frames are inert
  return 0;
  };

//------------------------------------------------------------------------------
VOID  IGHSizer::End (IGContext & ctxIn)
  {
  if (g_DbgSizers)  printf ("  ------ IGHSizer:End ()\n");
  if (ctxIn.iSizerDepth)
    {
    --ctxIn.iSizerDepth;
    };
  };

//------------------------------------------------------------------------------
VOID  IGHSizer::Render (IGID        idIn,
                        IGContext & ctxIn,
                        INT         iXPosIn,
                        INT         iYPosIn,
                        INT         iWidthIn,
                        INT         iHeightIn) const
  {
  //printf ("############## HSizer Render Skip... should not be here #######\n");
  //return;
  //printf ("RDrawPrim instance %s %d\n", __FILE__, __LINE__);
  RDrawPrim  dpDraw (ctxIn.pcnvDrawCanvas, ctxIn.Is3DMode (), ctxIn.psmSprites, ctxIn.pfntRegistry);

  // draw the backdrop
  dpDraw.SetDrawColor (uBGBackColor);
  dpDraw.FilledRect (iXPosIn,
                     iYPosIn,
                     iXPosIn + iWidthIn  - 1,
                     iYPosIn + iHeightIn - 1);

  // draw the border
  // if draw border
  dpDraw.SetDrawColor (uBorderColor);
  dpDraw.Rectangle (iXPosIn,
                    iYPosIn,
                    iXPosIn + iWidthIn - 1,
                    iYPosIn + iHeightIn - 1);
  };



//==============================================================================
// IMGUI VSizer
//==============================================================================

// Default Colors

UINT  IGVSizer::uDefColor_BGBackColor = RGB_GRAY_4;
UINT  IGVSizer::uDefColor_Border      = RGB_GRAY_2;


//------------------------------------------------------------------------------
IGVSizer::IGVSizer  ()
  {
  uBGBackColor = uDefColor_BGBackColor;
  uBorderColor = uDefColor_Border;
  };


//------------------------------------------------------------------------------
IGVSizer::~IGVSizer ()
  {
  };


//------------------------------------------------------------------------------
INT  IGVSizer::Do (IGID         idIn,
                   IGContext &  ctxIn,
                   INT          iXPosIn,
                   INT          iYPosIn,
                   INT          iWidthIn,
                   INT          iHeightIn,
                   INT          iSizerFlagsIn) const
  {
  IGRegion   regBounds;
  IGID       idZero   (0,0);

  if (g_DbgSizers)  printf ("IGVSizer:Do - Begin\n");

  // sizers are themselves windows, so resize accordingly
  ctxIn.SizerResize (idIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn, iSizerFlagsIn);

  // children will be at the next sizer depth
  ++ctxIn.iSizerDepth;

  // if this control is a frame, it has its own canvas.
  //   Reset its coordinates to the canvas origin.
  if (iSizerFlagsIn & IGRegion::kFrame)
    {
    iXPosIn = 0;
    iYPosIn = 0;
    };


  // debugging
  // let's see what the region stack looks like
  /*
    {
    printf ("\nSizerRegionStack for VSizer is\n");
    IGRegion *  pDbgCurr = ctxIn.stackSizer.pStack;
    while (pDbgCurr)
      {
      printf ("  Stack depth %d  x: %d y: %d w: %d h: %d\n",
              pDbgCurr->iDepth,
              pDbgCurr->iX, pDbgCurr->iY,
              pDbgCurr->iWidth, pDbgCurr->iHeight);

      pDbgCurr = pDbgCurr->pNext;
      };
    };
  */


  if (ctxIn.eSizerMode == IGContext::kMeasure)
    {
    // We measure ourself with the SizerResize call above.  We have to
    //  let all our children add themselves to the sizer list too, so
    //  for this pass we do nothing.
    }
  else if (ctxIn.eSizerMode == IGContext::kRender)
    {
    // The children widgets will soon be rendered.  Step through all
    //  children at this level and resize them according to their
    //  stored parameters and flags.

    // find our starting point
    IGRegion *  pCurr = ctxIn.stackSizer.pStack;
    IGRegion *  pFirstChild = NULL;
    INT         iChildDepth;
    while (pCurr)
      {
      if (pCurr->id == idIn)
        {
        // found ourself.  Start with our first child
        iChildDepth = pCurr->iDepth + 1;
        pFirstChild = pCurr->pNext;
        break;
        };
      pCurr = pCurr->pNext;
      };


    // now we have our first child node and all the info we need to traverse them.
    // First, step through the children and count them.  Also calculate how much
    // room is required for the flags.
    INT  iStaticReservedSpace       = 0;
    INT  iProportionalValueSum      = 0;
    INT  iNumChildren               = 0;
    INT  iNumPropChildren           = 0;

    if (g_DbgSizers)  printf ("VSizer render, pos (%d,%d) size %dx%d\n", iXPosIn, iYPosIn, iWidthIn, iHeightIn);

    if (g_DbgSizers)  if (pFirstChild) printf ("VSizer %d:%d has its first child at depth %d (%d)\n", idIn.iIdentifier, idIn.iOrdinal, pFirstChild->iDepth, iChildDepth);
    pCurr = pFirstChild;
    while (pCurr)
      {
      if (g_DbgSizers)  printf ("   search depth of %d\n", pCurr->iDepth);
      if (pCurr->iDepth < iChildDepth)
        {
        // we've stepped out of our list of children.  Stop scanning.
        break;
        };
      if (pCurr->iDepth == iChildDepth)
        {
        // this is one of our children.
        ++iNumChildren;
        if (g_DbgSizers)  printf ("  HSizer %d:%d modifies child %d:%d (depth %d)\n", idIn.iIdentifier, idIn.iOrdinal, pCurr->id.iIdentifier, pCurr->id.iOrdinal, pCurr->iDepth);

        if (pCurr->iSizerFlags & IGRegion::kProportionalHeight)
          {
          iProportionalValueSum += pCurr->iHeight;
          ++iNumPropChildren;
          }
        else if (pCurr->iSizerFlags & IGRegion::kPercentHeight)
          {
          iStaticReservedSpace += (iHeightIn * pCurr->iHeight) / 100;
          }
        else if (pCurr->iSizerFlags & IGRegion::kFixedHeight)
          {
          iStaticReservedSpace += pCurr->iHeight;
          };
        };
      pCurr = pCurr->pNext;
      };


    // now that we have data about all the children, we can set the actual values.

    pCurr = pFirstChild;
    INT  iCurrY             = iYPosIn;
    INT  iChildIndex        = 0;
    INT  iChildPropIndex    = 0;
    INT  iProportionalSpace = iHeightIn - iStaticReservedSpace;
    INT  iProportionalSpaceRemaining = iProportionalSpace;

    if (g_DbgSizers)  printf ("Found %d children (%d prop children), prop sum %d, static %d, prop %d\n", iNumChildren, iNumPropChildren, iProportionalValueSum, iStaticReservedSpace, iProportionalSpace);

    while (pCurr)
      {
      if (pCurr->iDepth < iChildDepth)
        {
        // we've stepped out of our list of children.  Stop scanning.
        break;
        };
      if (pCurr->iDepth == iChildDepth)
        {
        // this is one of our children.
        ++iChildIndex;

        if (g_DbgSizers)  printf ("  Child %d: depth:%d x:%d y:%d  w:%d h:%d\n", iChildIndex, pCurr->iDepth, pCurr->iX, pCurr->iY, pCurr->iWidth, pCurr->iHeight);

        pCurr->iX     = iXPosIn;
        pCurr->iY     = iCurrY;
        pCurr->iWidth = iWidthIn;

        if (pCurr->iSizerFlags & IGRegion::kProportionalHeight)
          {
          ++iChildPropIndex;
          if (iChildPropIndex == iNumPropChildren)
            {
            // this is the last proportional child.  Give it all the remaining space
            pCurr->iHeight = iProportionalSpaceRemaining;
            }
          else
            {
            pCurr->iHeight = iProportionalSpace * pCurr->iHeight / iProportionalValueSum;
            iProportionalSpaceRemaining -= pCurr->iHeight;
            };
          }
        else if (pCurr->iSizerFlags & IGRegion::kPercentHeight)
          {
          pCurr->iHeight = (iHeightIn * pCurr->iHeight) / 100;
          }
        else if (pCurr->iSizerFlags & IGRegion::kFixedHeight)
          {
          // leave the width as-is.
          };

        iCurrY += pCurr->iHeight;
        if (g_DbgSizers)  printf ("Child set to (%d,%d) %dx%d\n", pCurr->iX, pCurr->iY, pCurr->iWidth, pCurr->iHeight);
        };
      pCurr = pCurr->pNext;
      };
    };

  if (ctxIn.eSizerMode == IGContext::kMeasure) return 0;

  regBounds.iX      = iXPosIn;
  regBounds.iY      = iYPosIn;
  regBounds.iWidth  = iWidthIn;
  regBounds.iHeight = iHeightIn;

  // frames don't interact with the mouse or keyboard.

  // render the frame
  if ((iSizerFlagsIn & IGRegion::kNoRender) == 0)
    {
    if (! ctxIn.IsSkipMode ())
      {
      Render (idIn, ctxIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn);
      };
    };

  if (g_DbgSizers)  printf ("IGVSizer:Do - End\n");
  // frames are inert
  return 0;
  };

//------------------------------------------------------------------------------
VOID  IGVSizer::End (IGContext & ctxIn)
  {
  if (ctxIn.iSizerDepth)
    {
    --ctxIn.iSizerDepth;
    };
  };

//------------------------------------------------------------------------------
VOID  IGVSizer::Render (IGID        idIn,
                        IGContext & ctxIn,
                        INT         iXPosIn,
                        INT         iYPosIn,
                        INT         iWidthIn,
                        INT         iHeightIn) const
  {
  if (g_DbgSizers)  printf ("IGVSizer::Render  Draw canvas is %x\n", INT (ctxIn.pcnvDrawCanvas));
  //printf ("RDrawPrim instance %s %d\n", __FILE__, __LINE__);
  RDrawPrim  dpDraw (ctxIn.pcnvDrawCanvas, ctxIn.Is3DMode (), ctxIn.psmSprites, ctxIn.pfntRegistry);


  // draw the backdrop
  dpDraw.SetDrawColor (uBGBackColor);
  dpDraw.FilledRect (iXPosIn,
                     iYPosIn,
                     iXPosIn + iWidthIn  - 1,
                     iYPosIn + iHeightIn - 1);

  // draw the border
  // if draw border
  dpDraw.SetDrawColor (uBorderColor);
  dpDraw.Rectangle (iXPosIn,
                    iYPosIn,
                    iXPosIn + iWidthIn - 1,
                    iYPosIn + iHeightIn - 1);
  };










//==============================================================================
// IMGUI Context
//==============================================================================

//------------------------------------------------------------------------------
IGContext::IGContext  ()
  {
  Reset ();
  };


//------------------------------------------------------------------------------
IGContext::~IGContext ()
  {
  stackSizer.Release ();

  RGfxStateManager * pStateMan = RGlobalMesh::GetGfxStateMan ();
  if ((iShaderProgram != -1) && (pStateMan != NULL))
    {
    pStateMan->DeleteProgram (&iShaderProgram);
    }
  };


//------------------------------------------------------------------------------
VOID IGContext::Reset  (VOID)
  {
  IGID             idZero (0,0);

  //iMouseX          = 0;
  //iMouseY          = 0;
  //iMouseDownLeft   = 0;
  //iMouseDownRight  = 0;
  //iMouseDownCenter = 0;

  bShiftDown       = 0;
  bControlDown     = 0;
  bAltDown         = 0;
  bMetaDown        = 0;

  phMouse          = NULL;

  bDrawOnly        = FALSE;
  b3DMode          = FALSE;

  bActiveItemLocked = FALSE;
  idHotItem         = idZero;
  idActiveItem      = idZero;
  idKbdItem         = idZero;

  iKeyRead = 0;
  iKeyWrite = 0;
  INT  iKeyIndex;
  for (iKeyIndex = 0; iKeyIndex < g_iIGContextKeyBufferSize; ++iKeyIndex)
    {
    aiKeyEntered [iKeyIndex] = 0;
    aiKeyMod     [iKeyIndex] = 0;
    };
  for (iKeyIndex = 0; iKeyIndex < 256; ++iKeyIndex)
    {
    bKeyDown [iKeyIndex] = 0;
    };

  idLastWidget     = idZero;

  arrayCurrGlobalPosX.Clear ();
  arrayCurrGlobalPosY.Clear ();
  arrayCurrGlobalWidth.Clear ();
  arrayCurrGlobalHeight.Clear ();
  arrayCurrGlobalCanvas.Clear ();

  SetSkipMode (FALSE);
  ClearEvents ();

  matProjection = RMatrix::kIdentity;
  matModel      = RMatrix::kIdentity;

  iShaderProgram = -1;
  DBG_INFO ("IGContext reset!!!");
  };



//------------------------------------------------------------------------------
VOID IGContext::Prepare  (VOID)
  {
  IGID             idZero   (0,0);

  idHotItem = idZero;
  };


//------------------------------------------------------------------------------
VOID IGContext::Finish  (VOID)
  {
  IGID             idZero   (0,0);
  IGID             idNegOne (-1,-1);

  // Don't reset active item if a button is down, otherwise we
  //  will cancel dragging.
  if ((GetMouseDownLeft () == 0) &&
      (GetMouseDownRight () == 0) &&
      (GetMouseDownCenter () == 0))
    {
    idActiveItem = idZero;
    }
  else
    {
    if (idActiveItem == idZero)
      idActiveItem = idNegOne;
    };
  };


//------------------------------------------------------------------------------
VOID  IGContext::SizerStart (VOID)
  {
  eSizerMode = kMeasure;
  SetSkipMode (FALSE);
  };


//------------------------------------------------------------------------------
INT  IGContext::SizerLoop (VOID)
  {
  if (eSizerMode == kMeasure)
    {
    // measure
    //printf ("\n\nIGContext Sizer Loop: Measure\n");
    }
  else if (eSizerMode == kRender)
    {
    // render
    //printf ("\n\nIGContext Sizer Loop: Render\n");
    }
  else if (eSizerMode == kCleanup)
    {
    // cleanup
    //printf ("\n\nIGContext Sizer Loop: Cleanup\n");
    stackSizer.Reset ();

    return (0);
    };
  return (1);
  };


//------------------------------------------------------------------------------
VOID  IGContext::SizerInc (VOID)
  {
  if      (eSizerMode == kMeasure)  {eSizerMode = kRender;}
  else if (eSizerMode == kRender)   {eSizerMode = kCleanup;};
  };


//------------------------------------------------------------------------------
VOID  IGContext::SizerResize (IGID   idIn,
                              INT &  iXPosIn,
                              INT &  iYPosIn,
                              INT &  iWidthIn,
                              INT &  iHeightIn,
                              INT    iSizerFlagsIn)
  {
  if (eSizerMode == kMeasure)
    {
    // measure
    IGRegion  regNew (idIn, iXPosIn, iYPosIn, iWidthIn, iHeightIn, iSizerDepth, iSizerFlagsIn);

    // push dimentions onto rect stack
    stackSizer.Push (regNew);
    }
  else if (eSizerMode == kRender)
    {
    // render
    //read dimentions from rect stack
    //inc rect stack index

    // check the iSizerFlagsIn for any flags that might affect the size of the widget
    if ((iSizerFlagsIn & (IGRegion::kProportionalX      |
                          IGRegion::kProportionalY      |
                          IGRegion::kProportionalWidth  |
                          IGRegion::kProportionalHeight |
                          IGRegion::kPercentX           |
                          IGRegion::kPercentY           |
                          IGRegion::kPercentWidth       |
                          IGRegion::kPercentHeight      |
                          IGRegion::kFixedX             |
                          IGRegion::kFixedY             |
                          IGRegion::kFixedWidth         |
                          IGRegion::kFixedHeight        )) == 0)
      {
      // if no sizer flags are set, then we leave the dimensions as they are, since
      //  they are not expected to be adjusted by sizers.
      //printf ("  No adjust for %d:%d\n", idIn.iIdentifier, idIn.iOrdinal);
      return;
      };


    // debugging
    // let's see what the region stack looks like
    /*
    printf ("\nSizerRegionStack for IGContext::SizerResize is\n");
    IGRegion *  pDbgCurr = stackSizer.pStack;
    while (pDbgCurr)
      {
      printf ("  Stack depth %d id:%d:%d x: %d y: %d w: %d h: %d\n",
              pDbgCurr->iDepth,
              pDbgCurr->id.iIdentifier, pDbgCurr->id.iOrdinal,
              pDbgCurr->iX, pDbgCurr->iY,
              pDbgCurr->iWidth, pDbgCurr->iHeight);

      pDbgCurr = pDbgCurr->pNext;
      };
    */

    IGRegion *  pCurr = stackSizer.pStack;
    while (pCurr)
      {
      if (pCurr->id == idIn)
        {
        // found our entry.  Retrieve our size.
        iXPosIn   = pCurr->iX;
        iYPosIn   = pCurr->iY;
        iWidthIn  = pCurr->iWidth;
        iHeightIn = pCurr->iHeight;

        //printf ("Returning size for id %d:%d  x:%d y:%d  w:%d h:%d\n", pCurr->id.iIdentifier, pCurr->id.iOrdinal,       iXPosIn,       iYPosIn,       iWidthIn,       iHeightIn);
        break;
        };
      pCurr = pCurr->pNext;
      };
    };
  };


//------------------------------------------------------------------------------
VOID  IGContext::Set3DMode (BOOL  bStatusIn)
  {
  /*
  static const char szDefaultVertexShader[] =
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "  gl_Position = a_position;\n"
    "}\n";

  static const char szDefaultFragmentShader[] =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);\n"
    "}\n";
  */
  static const char szDefaultVertexShader2[] =
//        "uniform   mat4 u_MVmatrix;   \n"
//        "uniform   mat4 u_Pmatrix;    \n"
        "attribute vec4 a_position;   \n"
        "attribute vec3 a_normal;     \n"
        "attribute vec3 a_vertcolor;  \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "varying vec3 v_vertcolor;    \n"
        "varying vec3 v_normal;       \n"
        "void main()                  \n"
        "{                            \n"
//        "   gl_Position = u_Pmatrix * u_MVmatrix * vec4(a_position, 1.0); \n"
        "   gl_Position = a_position; \n"
        "   v_vertcolor = a_vertcolor;\n"
        "   v_normal    = a_normal;   \n"
        "   v_texCoord  = a_texCoord; \n"
        "}                            \n";


  static const char szDefaultFragmentShader2[] =
        "precision mediump float;                            \n"
        "uniform vec3 u_diffcolor;                           \n"
//        "uniform vec3 u_speccolor;                           \n"
        "uniform vec3 u_ambcolor;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "varying vec3 v_vertcolor;                           \n"
//        "varying vec3 v_normal;                              \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = \n"
//        "                 vec4(1.0, 1.0, 1.0, 1.0)           \n"
//        "                  vec4(v_vertcolor, 1.0)           \n"
        "                  vec4(u_diffcolor, 1.0)           \n"
        "                 * texture2D(s_texture, v_texCoord) \n"
        "                 + vec4(u_ambcolor, 1.0)            \n"
        ";}\n";


  static RNode *  pnodeIMGUIMaterial = NULL;


  if (b3DMode != bStatusIn)
    {
    b3DMode = bStatusIn;

    RGfxStateManager * pStateMan = RGlobalMesh::GetGfxStateMan ();
    //pStateMan->SetProjMatrix (igCtx.GetProjMatrix ());
    //pStateMan->SetModelMatrix (igCtx.GetModelMatrix ());
    if (pStateMan)
      {
      if (! b3DMode)
        {
        // turning 3D mode off
        //DBG_INFO ("IGContext::Set3DMode () deleting shader program %d", iShaderProgram);
        //pStateMan->DeleteProgram (&iShaderProgram);
        }
      else
        {
        // turning 3D mode on

        //RNode *  pnodeMaterial =
        if (pnodeIMGUIMaterial == NULL)
          {
          RWorld *  pwldWorld = RGlobal::GetWorld();
          pnodeIMGUIMaterial = pwldWorld->FindFirstNodeByName("mtlIMGUI");
          };
        if (pnodeIMGUIMaterial != NULL)
          {
          pnodeIMGUIMaterial->ForceAttrSolve("enable");

          RNode * pnodeShader = pnodeIMGUIMaterial->GetAttrNodePtr ("shaderNode");
          if (pnodeShader != NULL)
            {
            INT iShaderProgram = pnodeShader->GetAttrInt ("programId");

            pStateMan->SetPositionAttrib (iShaderProgram, "a_position");
            pStateMan->SetNormalAttrib (iShaderProgram, "a_normal");
            pStateMan->SetVertColorAttrib (iShaderProgram, "a_vertColor");
            }
          };
/*
        if (iShaderProgram == -1)
          {
          iShaderProgram = pStateMan->CreateProgram (szDefaultVertexShader2, szDefaultFragmentShader2);
          DBG_INFO ("IGContext::Set3DMode () created shader program %d", iShaderProgram);
          EnableShaderProgram ();

          // vertex shader attribs
          iPositionAttrib    = glGetAttribLocation  (iShaderProgram, "a_position");
          checkGlError ("glGetAttribLocation a_position");
          iNormalAttrib      = glGetAttribLocation  (iShaderProgram, "a_normal");
          checkGlError ("glGetAttribLocation a_normal");
          iVertColorAttrib   = glGetAttribLocation  (iShaderProgram, "a_vertColor");
          checkGlError ("glGetAttribLocation a_vertColor");
          iTexCoordAttrib    = glGetAttribLocation  (iShaderProgram, "a_texCoord");
          checkGlError ("glGetAttribLocation a_texCoord");

          int  iModelMatrixAttrib = glGetUniformLocation (iShaderProgram, "u_MVmatrix");
          checkGlError ("glGetAttribLocation u_MVmatrix");
          int  iProjMatrixAttrib  = glGetUniformLocation (iShaderProgram, "u_Pmatrix");
          checkGlError ("glGetAttribLocation u_Pmatrix");

          // fragment shader attribs
          int  iDiffColorAttrib   = glGetUniformLocation (iShaderProgram, "u_diffcolor");
          checkGlError ("glGetAttribLocation u_diffcolor");
          int  iSpecColorAttrib   = glGetUniformLocation (iShaderProgram, "u_speccolor");
          checkGlError ("glGetAttribLocation u_speccolor");
          int  iAmbColorAttrib    = glGetUniformLocation (iShaderProgram, "u_ambcolor");
          checkGlError ("glGetAttribLocation u_ambcolor");
          int  iSamplerAttrib     = glGetUniformLocation (iShaderProgram, "s_texture");
          checkGlError ("glGetAttribLocation s_texture");

          RGfxStateManager *  pStateMan = RGlobalMesh::GetGfxStateMan ();

          //FLOAT  afMatOut [16];
          //pStateMan->GetProjMatrix ().GetFloatArray (afMatOut);
          //glUniformMatrix4fv (iProjMatrixAttrib, 1, false, afMatOut);

          //pStateMan->GetModelMatrix ().GetFloatArray (afMatOut);
          //glUniformMatrix4fv (iModelMatrixAttrib, 1, false, afMatOut);

          if (iDiffColorAttrib != -1) {glUniform3f (iDiffColorAttrib, 1.0, 1.0, 1.0); DBG_INFO ("Set Diff Color");};
          //if (iSpecColorAttrib != -1) {glUniform3f (iSpecColorAttrib, 0.0, 0.0, 0.0); DBG_INFO ("Set Spec Color");};
          if (iAmbColorAttrib  != -1) {glUniform3f (iAmbColorAttrib,  0.0, 0.0, 0.0); DBG_INFO ("Set Amb Color");};
          //if (iSamplerAttrib   != -1) {glUniform1f (iSamplerAttrib, 1.0); DBG_INFO ("Set Sampler Rate");};

          DBG_INFO ("Enabled shader program %d\n  pos %d\n  nrm %d\n  clr %d\n  uvs  %d\n  dif %d\n  spc %d\n  amb %d\n  smp %d\n",
                    iShaderProgram,
                    iPositionAttrib,
                    iNormalAttrib,
                    iVertColorAttrib,
                    iTexCoordAttrib,
                    iDiffColorAttrib,
                    iSpecColorAttrib,
                    iAmbColorAttrib,
                    iSamplerAttrib
                   );
          }
        else
          {
          EnableShaderProgram ();
          };
*/


        };
      };

    }
  };


//------------------------------------------------------------------------------
VOID  IGContext::EnableShaderProgram (VOID)
  {
  if (b3DMode)
    {
    RGfxStateManager * pStateMan = RGlobalMesh::GetGfxStateMan ();
    if (pStateMan)
      {
      pStateMan->UseProgram (iShaderProgram);
      };
    };
  };


//------------------------------------------------------------------------------
BOOL  IGContext::GetNextKey  (INT &  iKeyOut,
                              INT &  iKeyModOut)
  {
  if (aiKeyEntered [iKeyRead] == 0)
    {
    return (0);
    };

  // read from the buffer
  iKeyOut    = aiKeyEntered [iKeyRead];
  iKeyModOut = aiKeyMod     [iKeyRead];

  // clear the buffer now that we have the key
  aiKeyEntered [iKeyRead] = 0;
  aiKeyMod     [iKeyRead] = 0;

  // increment to the next read position
  iKeyRead = (iKeyRead + 1) % g_iIGContextKeyBufferSize;
  return (1);
  };


//------------------------------------------------------------------------------
VOID  IGContext::StoreKey (INT  iKeyIn,
                           INT  iKeyModIn)
  {
  INT  iKeyNextWrite = (iKeyWrite + 1) % g_iIGContextKeyBufferSize;

  // make sure the buffer isn't full
  if (iKeyNextWrite == iKeyRead)
    {
    // buffer full
    return;
    };

  aiKeyEntered [iKeyWrite] = iKeyIn;
  aiKeyMod     [iKeyWrite] = iKeyModIn;

  // increment to the next write position
  iKeyWrite = iKeyNextWrite;
  };


//------------------------------------------------------------------------------
VOID  IGContext::SetKeyDown (INT  iKeyIn)
  {
  if (iKeyIn < 256)
    {
    bKeyDown [iKeyIn] = 1;
    };
  };


//------------------------------------------------------------------------------
VOID  IGContext::SetKeyUp (INT  iKeyIn)
  {
  if (iKeyIn < 256)
    {
    bKeyDown [iKeyIn] = 0;
    };
  };


//------------------------------------------------------------------------------
VOID  IGContext::SetHotItem  (IGID  idIn)
  {
  idHotItem = idIn;
  };


//------------------------------------------------------------------------------
VOID  IGContext::SetActiveItem  (IGID  idIn)
  {
  if (! bActiveItemLocked)
    {
    idActiveItem = idIn;
    };
  };


//------------------------------------------------------------------------------
VOID  IGContext::SetLastWidget  (IGID  idIn)
  {
  idLastWidget = idIn;
  };



/*
//------------------------------------------------------------------------------
VOID  IGContext::PushActiveDrawRegion (INT        iXIn,
                                       INT        iYIn,
                                       INT        iWidthIn,
                                       INT        iHeightIn,
                                       RCanvas *  pcnvCanvasIn)
  {

  INT  iCurIdx = arrayCurrGlobalPosX.Length () - 1;
  if (iCurIdx >= 0)
    {
    INT  iParentX, iParentY = 0; if (pcnvDrawCanvas != NULL) pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
    printf ("Push Active Draw Region from x:%d y:%d w:%d h:%d cnv:(%d,%d %dx%d %d,%d)\n",
            arrayCurrGlobalPosX [iCurIdx],
            arrayCurrGlobalPosY [iCurIdx],
            arrayCurrGlobalWidth [iCurIdx],
            arrayCurrGlobalHeight [iCurIdx],
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iXOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iYOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iWidth : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iHeight : -1,
            iParentX, iParentY);
    };

  arrayCurrGlobalPosX.Append (iXIn);
  arrayCurrGlobalPosY.Append (iYIn);
  arrayCurrGlobalWidth.Append (iWidthIn);
  arrayCurrGlobalHeight.Append (iHeightIn);
  if (pcnvCanvasIn == NULL)
    {
    arrayCurrGlobalCanvas.Append (this->pcnvDrawCanvas);
    }
  else
    {
    arrayCurrGlobalCanvas.Append (this->pcnvDrawCanvas);
    pcnvDrawCanvas = pcnvCanvasIn;
    };
  INT  iParentX, iParentY = 0; if (pcnvDrawCanvas != NULL) pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
  printf ("Push Active Draw Region to x:%d y:%d w:%d h:%d  cnv:(%d,%d %dx%d %d,%d)\n", iXIn, iYIn, iWidthIn, iHeightIn,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iXOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iYOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iWidth : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iHeight : -1,
            iParentX, iParentY);
  };
*/
/*
//------------------------------------------------------------------------------
VOID  IGContext::PopActiveDrawRegion (VOID)
  {
  INT  iCurIdx = arrayCurrGlobalPosX.Length () - 1;
  INT  iParentX, iParentY = 0; if (pcnvDrawCanvas != NULL) pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);

  printf ("Pop Active Draw Region from x:%d y%d w:%d h:%d cnv:(%d,%d %dx%d %d,%d)\n",
          arrayCurrGlobalPosX [iCurIdx],
          arrayCurrGlobalPosY [iCurIdx],
          arrayCurrGlobalWidth [iCurIdx],
          arrayCurrGlobalHeight [iCurIdx],
          (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iXOffset : -1,
          (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iYOffset : -1,
          (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iWidth : -1,
          (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iHeight : -1,
          iParentX, iParentY);



  INT  iIndex = arrayCurrGlobalPosX.Length () - 1;
  // don't pop the 0th index, since it should be set to the entire canvas.
  if (iIndex > 0)
    {
    arrayCurrGlobalPosX.Remove (iIndex, 1);
    arrayCurrGlobalPosY.Remove (iIndex, 1);
    arrayCurrGlobalWidth.Remove (iIndex, 1);
    arrayCurrGlobalHeight.Remove (iIndex, 1);
    arrayCurrGlobalCanvas.Remove (iIndex, 1);
    this->pcnvDrawCanvas = arrayCurrGlobalCanvas.Index (iIndex);

    INT  iCurIdx = arrayCurrGlobalPosX.Length () - 1;
    INT  iParentX, iParentY = 0; if (pcnvDrawCanvas != NULL) pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
    printf ("Pop Active Draw Region to x:%d y%d w:%d h:%d cnv:(%d,%d %dx%d %d,%d)\n",
            arrayCurrGlobalPosX [iCurIdx],
            arrayCurrGlobalPosY [iCurIdx],
            arrayCurrGlobalWidth [iCurIdx],
            arrayCurrGlobalHeight [iCurIdx],
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iXOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iYOffset : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iWidth : -1,
            (pcnvDrawCanvas != NULL) ? pcnvDrawCanvas->iHeight : -1,
            iParentX, iParentY);

    };
  };
*/
/*
//------------------------------------------------------------------------------
VOID  IGContext::GetActiveDrawRegion (INT &        iXOut,
                                      INT &        iYOut,
                                      INT &        iWidthOut,
                                      INT &        iHeightOut,
                                      RCanvas * &  pcnvCanvasOut)
  {
  INT  iIndex = arrayCurrGlobalPosX.Length () - 1;
  if (iIndex >= 0)
    {
    iXOut = arrayCurrGlobalPosX [iIndex];
    iYOut = arrayCurrGlobalPosY [iIndex];
    iWidthOut = arrayCurrGlobalWidth [iIndex];
    iHeightOut = arrayCurrGlobalHeight [iIndex];
    pcnvCanvasOut = arrayCurrGlobalCanvas [iIndex];
    };
  };
*/

//------------------------------------------------------------------------------
VOID  IGContext::PushActiveDrawCanvas  (RCanvas *  pcnvIn)
  {
  if (pcnvIn == NULL)
    {
    ASSERT (0);
    return;
    };
  INT  iCurIdx = arrayDrawCanvas.Length () - 1;
  INT  iParentX = 0;
  INT  iParentY = 0;
  if ((iCurIdx >= 0) && (pcnvDrawCanvas != NULL))
    {
    pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
    printf ("Push Draw Canvas from (%d,%d %dx%d) parent: %d,%d\n",
            pcnvDrawCanvas->iXOffset,
            pcnvDrawCanvas->iYOffset,
            pcnvDrawCanvas->iWidth,
            pcnvDrawCanvas->iHeight,
            iParentX, iParentY);
    };

  // pcnvDrawCanvas should point to the top item on the stack.
  arrayDrawCanvas.Append (pcnvIn);
  pcnvDrawCanvas = pcnvIn;
  iParentX = 0;
  iParentY = 0;

  pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
  printf ("Push Draw Canvas  to (%d,%d %dx%d) parent: %d,%d\n",
            pcnvDrawCanvas->iXOffset,
            pcnvDrawCanvas->iYOffset,
            pcnvDrawCanvas->iWidth,
            pcnvDrawCanvas->iHeight,
            iParentX, iParentY);
  };

//------------------------------------------------------------------------------
VOID  IGContext::PopActiveDrawCanvas   (VOID)
  {
  INT  iCurIdx = arrayDrawCanvas.Length () - 1;
  INT  iParentX = 0;
  INT  iParentY = 0;
  if ((iCurIdx >= 0) && (pcnvDrawCanvas != NULL))
    {
    pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
    printf ("Push Draw Canvas from (%d,%d %dx%d) parent: %d,%d\n",
            pcnvDrawCanvas->iXOffset,
            pcnvDrawCanvas->iYOffset,
            pcnvDrawCanvas->iWidth,
            pcnvDrawCanvas->iHeight,
            iParentX, iParentY);
    };

  INT  iIndex = arrayDrawCanvas.Length () - 1;
  // don't pop the 0th index, since it should be set to the entire canvas.
  if (iIndex > 0)
    {
    delete (pcnvDrawCanvas);

    arrayCurrGlobalCanvas.Remove (iIndex, 1);
    this->pcnvDrawCanvas = arrayDrawCanvas.Index (iIndex - 1);

    //INT  iCurIdx = arrayCurrGlobalPosX.Length () - 1;
    iParentX = 0;
    iParentY = 0;
    pcnvDrawCanvas->GetHeirarchyPos (iParentX, iParentY);
    printf ("Push Draw Canvas from (%d,%d %dx%d) parent: %d,%d\n",
            pcnvDrawCanvas->iXOffset,
            pcnvDrawCanvas->iYOffset,
            pcnvDrawCanvas->iWidth,
            pcnvDrawCanvas->iHeight,
            iParentX, iParentY);
    };
  };

/*
//------------------------------------------------------------------------------
RCanvas *  IGContext::GetActiveDrawCanvas   (VOID)
  {
  INT  iIndex = arrayCurrGlobalPosX.Length () - 1;
  if (iIndex >= 0)
    {
    return (arrayCurrGlobalCanvas [iIndex]);
    };
  return (NULL);
  };
*/

//------------------------------------------------------------------------------
VOID  IGContext::MapToDrawCanvas (RVec3 &  vecPosIn,
                                  RVec3 &  vecSizeIn,
                                  INT &    iXOut,
                                  INT &    iYOut,
                                  INT &    iWidthOut,
                                  INT &    iHeightOut)
  {
  // Take the given size and position and map to the current draw canvas.
  // If the coordinate space is between 0.0 and 1.0, it acts as a UV mapping
  //  to the entire canvas area.  If the coordinate space is over 1, then
  //  the size and pos are treated as pixel coordinates.  The only drawback
  //  is that pixel coordinates cannot specify a size of 1x1 or a position of
  //  1,1, but hopefully this will not prove to be a problem.  Using a pixel
  //  size or position of 1.01 should give the desired size/location.

  iXOut = INT (vecPosIn.fX);
  iYOut = INT (vecPosIn.fY);

  iWidthOut  = INT (vecSizeIn.fX);
  iHeightOut = INT (vecSizeIn.fY);

  if (vecPosIn.fX <= 1.0)  iXOut = INT (FLOAT (pcnvDrawCanvas->iWidth)  * vecPosIn.fX);
  if (vecPosIn.fY <= 1.0)  iYOut = INT (FLOAT (pcnvDrawCanvas->iHeight) * vecPosIn.fY);

  if (vecSizeIn.fX <= 1.0)  iWidthOut  = INT (FLOAT (pcnvDrawCanvas->iWidth)  * vecSizeIn.fX);
  if (vecSizeIn.fY <= 1.0)  iHeightOut = INT (FLOAT (pcnvDrawCanvas->iHeight) * vecSizeIn.fY);
  };

/*
//------------------------------------------------------------------------------
VOID  IGContext::MapToActiveDrawRegion (RVec3 &  vecPosIn,
                                        RVec3 &  vecSizeIn,
                                        INT &    iXOut,
                                        INT &    iYOut,
                                        INT &    iWidthOut,
                                        INT &    iHeightOut)
  {
  // old code that worked on draw regions
  // Take the given size and position in 0.0-1.0 coordinate space
  //  and map to the current draw region.


  INT  iDrawX = 0;
  INT  iDrawY = 0;
  INT  iDrawWidth = 0;
  INT  iDrawHeight = 0;
  RCanvas *  pcnvCanvas = NULL;

  GetActiveDrawRegion  (iDrawX, iDrawY, iDrawWidth, iDrawHeight, pcnvCanvas);

  // debugging
  //printf ("MapToActiveDrawRegion x %d y %d  w %d h %d\n", iDrawX, iDrawY, iDrawWidth, iDrawHeight);

  iXOut = iDrawX + INT (vecPosIn.fX);
  iYOut = iDrawY + INT (vecPosIn.fY);

  iWidthOut  = INT (vecSizeIn.fX);
  iHeightOut = INT (vecSizeIn.fY);

  if (vecPosIn.fX <= 1.0)  iXOut = iDrawX + INT (FLOAT (iDrawWidth)  * vecPosIn.fX);
  if (vecPosIn.fY <= 1.0)  iYOut = iDrawY + INT (FLOAT (iDrawHeight) * vecPosIn.fY);

  if (vecSizeIn.fX <= 1.0)  iWidthOut  = INT (FLOAT (iDrawWidth)  * vecSizeIn.fX);
  if (vecSizeIn.fY <= 1.0)  iHeightOut = INT (FLOAT (iDrawHeight) * vecSizeIn.fY);
  };
*/





/*
for (ctx.SizerStart (); ctx.SizerLoop (); ctx.SizerInc ())
  {
  At Sizer Start
    If Measure Mode
      Push -1 as prev index
      sizer itself needs to be a frame widget (with bg?).  call SizerResize first?
    If Render Mode
      ctxIn.SizerResize (iXPosIn, iYPosIn, iWidthIn, iHeightIn) to get own size
      Draw BG
      step through children, and set their sizes in the rect stack

  At Widget Start
    ctxIn.SizerResize (iXPosIn, iYPosIn, iWidthIn, iHeightIn)

  At Sizer End
    If Measure Mode
      Pop prev index
    If Render Mode
  };

ctx::SizerResize (iXPosIn, iYPosIn, iWidthIn, iHeightIn)
  {
  if Measure Mode
    push dimentions onto rect stack
  if Render Mode
    read dimentions from rect stack
    inc rect stack index
  };

SizerLoop cleans up on third pass (measure, render, cleanup) and returns false so loop will exit.
*/



