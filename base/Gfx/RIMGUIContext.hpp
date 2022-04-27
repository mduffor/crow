

#ifndef RIMGUICONTEXT_HPP
#define RIMGUICONTEXT_HPP

#include <string.h>
#include "RTypes.hpp"
#include "IGWidget.hpp"
#include "ext_image/RCanvas.hpp"
#include "ext_sprite/RDrawPrim.hpp"
#include "ext_sprite/RFill.hpp"
//#include "ext_widgets/RWin.hpp"
#include "ext_widgets/RFormattedText.hpp"
#include "ext_sprite/RSprMan.hpp"
#include "ext_shell/RMouseHandler.hpp"



// So the way this worked is that the hierarchy was stored in an unrolled, depth-first traversed
//  stack with the Depth value specifying how deep in the hierarchy the region is.  A sizer would
//  work on all nodes that were one depth deeper, until the depth passed itself again.  The entire
//  heirarchy was specified during the first measure pass, at which time all base sizes were
//  gathered.  Then things would be hit-tested and drawn during the next pass (render), except the
//  sizers would adjust the size of the next level down, right before it was drawn.
//  On the third pass, only the context would respond, and it would return all the regions to the
//  stack for re-use in the next frame.




// Stores info about rectangular regions.
// ESizerFlag not really used properly.  Handles size, but not positioning.
//------------------------------------------------------------------------------
class IGRegion
  {
  public:
    IGID        id;

    INT         iX;
    INT         iY;
    INT         iWidth;
    INT         iHeight;

    IGRegion *  pNext; // NOTE: Used for IGRegionStack

    // variables for window graph traversing
    enum  ESizerFlag {kProportionalX      = 0x00000001,
                      kProportionalY      = 0x00000002,
                      kProportionalWidth  = 0x00000004,
                      kProportionalHeight = 0x00000008,
                      kPercentX           = 0x00000010,
                      kPercentY           = 0x00000020,
                      kPercentWidth       = 0x00000040,
                      kPercentHeight      = 0x00000080,
                      kFixedX             = 0x00000100,
                      kFixedY             = 0x00000200,
                      kFixedWidth         = 0x00000400,
                      kFixedHeight        = 0x00000800,
                      kFrame              = 0x00001000,
                      kNoRender           = 0x00002000};

    INT         iDepth;
    INT         iSizerFlags; // Needs to be of ESizerFlag type


  public:
                IGRegion   ();

                IGRegion   (IGID  idIn,
                            INT   iXIn,
                            INT   iYIn,
                            INT   iWidthIn,
                            INT   iHeightIn,
                            INT   iDepthIn = -1,
                            INT   iFlagsIn = 0);

                ~IGRegion  ();

    BOOL        IsHit      (INT  iXIn,
                            INT  iYIn);

    VOID        Clear      (VOID);
    IGRegion &  operator=  (IGRegion &  regIn);
  };


//------------------------------------------------------------------------------
class IGRegionStack
  {
  public:
    IGRegion *  pPool;
    IGRegion *  pStack;
    IGRegion *  pTail;

  public:

         IGRegionStack  ();
         ~IGRegionStack ();
    VOID Release        (VOID);                //< free all memory
    VOID Reset          (VOID);                //< return all pushed regions to the pool
    VOID Push           (IGRegion &  regIn);   //< push the given region to the
    BOOL Pop            (IGRegion &  regOut);

  };

#define g_iIGContextKeyBufferSize 20

//------------------------------------------------------------------------------
class IGContext
  {
  public:

    // Mouse status
    //INT  iMouseX;
    //INT  iMouseY;
    //INT  iMouseDownLeft;
    //INT  iMouseDownRight;
    //INT  iMouseDownCenter;
    RMouseHandler *  phMouse;

    INT  iMouseLastX;      ///< useable by the active widget.  Not used by the context.
    INT  iMouseLastY;      ///< useable by the active widget.  Not used by the context.


    // keyboard status
    BOOL bShiftDown;
    BOOL bControlDown;
    BOOL bAltDown;
    BOOL bMetaDown;

    IGID idKbdItem;

    INT  iKeyRead;
    INT  iKeyWrite;
    INT  aiKeyEntered [g_iIGContextKeyBufferSize];
    INT  aiKeyMod     [g_iIGContextKeyBufferSize];
    BOOL bKeyDown     [256];

    // event handling
    BOOL bSkipMode;
    RStrArray  astrEvents;

  // focus tracking.  Now they should only be accessed through accessors
  private:
    IGID idHotItem;
    IGID idActiveItem;
    IGID idLastWidget;
    BOOL bActiveItemLocked;

    // 3D mode status (display via OpenGL)
    BOOL           b3DMode;

  public:
    // Drawing objects
    RCanvas *              pcnvDrawCanvas;
    RSpriteManager *       psmSprites;
    RFontRegistry *        pfntRegistry;
    RFormattedTextManager  ftTextManager;

    // Sizing status tracking
    // variables for multi-pass sizing
    enum  ESizerMode  {kMeasure = 0, kRender = 1, kCleanup = 2};

    ESizerMode     eSizerMode;
    BOOL           bDrawOnly;
    IGRegionStack  stackSizer;
    INT            iSizerDepth;    //< current depth of the sizer stack


    // Drawing State
    RIntArray     arrayCurrGlobalPosX;   // active region on the global canvas widgets should draw to
    RIntArray     arrayCurrGlobalPosY;   // active region on the global canvas widgets should draw to
    RIntArray     arrayCurrGlobalWidth;  // active region on the global canvas widgets should draw to
    RIntArray     arrayCurrGlobalHeight; // active region on the global canvas widgets should draw to
    RCanvasArray  arrayCurrGlobalCanvas; // the global canvas itself.

    RCanvasArray  arrayDrawCanvas; // the global canvas itself.

    RMatrix       matProjection;
    RMatrix       matModel;

  public:
    INT            iShaderProgram;
    INT     iPositionAttrib;
    INT     iVertColorAttrib;
    INT     iNormalAttrib;
    INT     iTexCoordAttrib;

    INT   GetShaderProgram    (VOID)              {return (iShaderProgram);};

    VOID  EnableShaderProgram (VOID);


  public:
          IGContext        ();
          ~IGContext       ();

    VOID  Reset               (VOID);
    /*
    VOID  SetMouseMove        (INT  iXIn,
                               INT  iYIn)   {iMouseX = iXIn;  iMouseY = iYIn;};
    VOID  SetMouseDownLeft    (VOID)        {iMouseDownLeft = 1;};
    VOID  SetMouseUpLeft      (VOID)        {iMouseDownLeft = 0;};
    VOID  SetMouseDownRight   (VOID)        {iMouseDownRight = 1;};
    VOID  SetMouseUpRight     (VOID)        {iMouseDownRight = 0;};
    VOID  SetMouseDownCenter  (VOID)        {iMouseDownCenter = 1;};
    VOID  SetMouseUpCenter    (VOID)        {iMouseDownCenter = 0;};
    */

    VOID  SetSpriteManager    (RSpriteManager *  psmIn)     {psmSprites   = psmIn; ftTextManager.AssociateSpriteManager (psmIn);};
    RSpriteManager *  GetSpriteManager    (VOID)            {return (psmSprites);};
    VOID  SetFontRegistry     (RFontRegistry *   pfrIn)     {pfntRegistry = pfrIn; ftTextManager.SetFontRegistry (pfrIn);};
    VOID  SetColorRegistry    (RColorRegistry *  pclrRegIn) {ftTextManager.SetColorRegistry (pclrRegIn);};

    VOID  Prepare             (VOID);
    VOID  Finish              (VOID);

    VOID  SizerStart          (VOID);
    INT   SizerLoop           (VOID);
    VOID  SizerInc            (VOID);
    VOID  SizerResize         (IGID   idIn,
                               INT &  iXPosIn,
                               INT &  iYPosIn,
                               INT &  iWidthIn,
                               INT &  iHeightIn,
                               INT    iSizerFlagsIn);

    BOOL  IsMeasureMode       (VOID)              {return (eSizerMode == kMeasure);};

    BOOL  IsRenderMode        (VOID)              {return (eSizerMode == kRender);};

    BOOL  IsSkipMode          (VOID)              {return (bSkipMode);};

    VOID  SetSkipMode         (BOOL  bStatusIn)   {bSkipMode = bStatusIn;};

    BOOL  Is3DMode            (VOID)              {return (b3DMode);};

    VOID  Set3DMode           (BOOL  bStatusIn);

    VOID  AddEvent            (RStr  strEventIn)  {if (astrEvents.Find (strEventIn) == -1) astrEvents.Append (strEventIn);};

    VOID  ClearEvents         (VOID)              {astrEvents.Clear ();};

    BOOL  HasEvent            (RStr  strEventIn)  {return (astrEvents.Find (strEventIn) != -1);};

    BOOL  GetNextKey          (INT &  iKeyOut,
                               INT &  iKeyModOut);

    VOID  StoreKey            (INT  iKeyIn,
                               INT  iKeyModIn);

    VOID  SetKeyDown          (INT  iKeyIn);

    VOID  SetKeyUp            (INT  iKeyIn);

    BOOL  IsKeyDown           (INT  iKeyIn) {if (iKeyIn < 256) return bKeyDown [iKeyIn]; return 0;};


    INT    GetMouseX                  (VOID)  {return (phMouse->GetX ());};
    INT    GetMouseY                  (VOID)  {return (phMouse->GetY ());};
    INT    GetMouseLastX              (VOID)  {return (phMouse->GetLastX ());};
    INT    GetMouseLastY              (VOID)  {return (phMouse->GetLastY ());};

    BOOL   GetMouseDownLeft           (VOID)  {return (phMouse->GetDownLeft ());};
    BOOL   GetMouseDownRight          (VOID)  {return (phMouse->GetDownRight ());};
    BOOL   GetMouseDownCenter         (VOID)  {return (phMouse->GetDownCenter ());};
    BOOL   GetMouseDownScrollUp       (VOID)  {printf ("GetMouseDownScrollUp is %d",phMouse->GetDownScrollUp ());   return (phMouse->GetDownScrollUp ());};
    BOOL   GetMouseDownScrollDn       (VOID)  {return (phMouse->GetDownScrollDn ());};

    INT    GetMouseNumLeftClicks      (VOID)  {return (phMouse->GetNumLeftClicks ());};
    INT    GetMouseNumRightClicks     (VOID)  {return (phMouse->GetNumRightClicks ());};
    INT    GetMouseNumCenterClicks    (VOID)  {return (phMouse->GetNumCenterClicks ());};
    INT    GetMouseNumScrollUpClicks  (VOID)  {return (phMouse->GetNumScrollUpClicks ());};
    INT    GetMouseNumScrollDnClicks  (VOID)  {return (phMouse->GetNumScrollDnClicks ());};


    VOID   SetHotItem                 (IGID  idIn);
    IGID   GetHotItem                 (VOID)               {return idHotItem;};
    VOID   SetActiveItem              (IGID  idIn);
    IGID   GetActiveItem              (VOID)               {return idActiveItem;};
    VOID   SetLastWidget              (IGID  idIn);
    IGID   GetLastWidget              (VOID)               {return idLastWidget;};
    VOID   SetKbdItem                 (IGID  idIn)         {idKbdItem = idIn;};
    IGID   GetKbdItem                 (VOID)               {return idKbdItem;};
    VOID   LockActiveItem             (BOOL  bStatusIn)    {bActiveItemLocked = bStatusIn;};
    BOOL   ActiveItemIsLocked         (VOID)               {return (bActiveItemLocked);};
    BOOL   ItemIsLockedActive         (IGID  idIn)         {return ((idActiveItem == idIn) && (bActiveItemLocked == TRUE));};


    //VOID  PushActiveDrawRegion (INT        iXIn,
    //                            INT        iYIn,
    //                            INT        iWidthIn,
    //                            INT        iHeightIn,
    //                            RCanvas *  pcnvCanvasIn = NULL);

    //VOID  PopActiveDrawRegion  (VOID);

    //VOID  GetActiveDrawRegion  (INT &        iXOut,
    //                            INT &        iYOut,
    //                            INT &        iWidthOut,
    //                            INT &        iHeightOut,
    //                            RCanvas * &  pcnvCanvasOut);

    VOID       PushActiveDrawCanvas  (RCanvas *  pcnvIn);

    VOID       PopActiveDrawCanvas   (VOID);

    //RCanvas *  GetActiveDrawCanvas   (VOID);

    /*
    VOID  MapToActiveDrawRegion  (RVec3 &  vecPosIn,
                                  RVec3 &  vecSizeIn,
                                  INT &    iXOut,
                                  INT &    iYOut,
                                  INT &    iWidthOut,
                                  INT &    iHeightOut);
    */

    VOID       MapToDrawCanvas (RVec3 &  vecPosIn,
                                RVec3 &  vecSizeIn,
                                INT &    iXOut,
                                INT &    iYOut,
                                INT &    iWidthOut,
                                INT &    iHeightOut);

    VOID       SetProjMatrix   (RMatrix &  matIn)  {matProjection = matIn;};
    VOID       SetModelMatrix  (RMatrix &  matIn)  {matModel      = matIn;};

    RMatrix &  GetProjMatrix   (VOID)              {return matProjection;};
    RMatrix &  GetModelMatrix  (VOID)              {return matModel;};


  };





//------------------------------------------------------------------------------
class IGRect
  {
  public:

    static UINT   uDefColor_BGBackColor;
    static UINT   uDefColor_Border;

    RFill             fillBGDefault;
    RFill *           pfillBG;

    //UINT   uBGBackColor;
    UINT   uBorderColor;

  public:
    IGRect  ();
    ~IGRect ();

    INT  Do   (IGID          idIn,
               IGContext &   ctxIn,
               INT           iXPosIn,
               INT           iYPosIn,
               INT           iWidthIn,
               INT           iHeightIn,
               INT           iSizerFlagsIn) const;

    virtual VOID  Render    (IGID          idIn,
                             IGContext &   ctxIn,
                             INT           iXPosIn,
                             INT           iYPosIn,
                             INT           iWidthIn,
                             INT           iHeightIn) const;

    VOID          SetBGFill (RFill *       pfillIn)        {pfillBG = pfillIn;};

  };


//------------------------------------------------------------------------------
class IGHSizer
  {
  public:

    static UINT   uDefColor_BGBackColor;
    static UINT   uDefColor_Border;

    UINT   uBGBackColor;
    UINT   uBorderColor;

  public:
    IGHSizer  ();
    ~IGHSizer ();

    INT  Do   (IGID          idIn,
               IGContext &   ctxIn,
               INT           iXPosIn,
               INT           iYPosIn,
               INT           iWidthIn,
               INT           iHeightIn,
               INT           iSizerFlagsIn) const;

    virtual VOID  Render (IGID          idIn,
                          IGContext &   ctxIn,
                          INT           iXPosIn,
                          INT           iYPosIn,
                          INT           iWidthIn,
                          INT           iHeightIn) const;

    static VOID   End    (IGContext & ctxIn);
  };


//------------------------------------------------------------------------------
class IGVSizer
  {
  public:

    static UINT   uDefColor_BGBackColor;
    static UINT   uDefColor_Border;

    UINT   uBGBackColor;
    UINT   uBorderColor;

  public:
    IGVSizer  ();
    ~IGVSizer ();

    INT  Do   (IGID          idIn,
               IGContext &   ctxIn,
               INT           iXPosIn,
               INT           iYPosIn,
               INT           iWidthIn,
               INT           iHeightIn,
               INT           iSizerFlagsIn) const;

    virtual VOID  Render (IGID          idIn,
                          IGContext &   ctxIn,
                          INT           iXPosIn,
                          INT           iYPosIn,
                          INT           iWidthIn,
                          INT           iHeightIn) const;

    static VOID   End    (IGContext & ctxIn);
  };


#endif // RIMGUICONTEXT_HPP
