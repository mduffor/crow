
// SHELL.HPP
// Copyright 1995, 1996, 2000, Michael T. Duffy.  All Rights Reserved

#ifndef SHELL_HPP
#define SHELL_HPP

#include "Sys/Types.hpp"
#include "Sys/Timer.hpp"
#include "Sys/InputManager.hpp"
#include "Math/RMatrix.hpp"
#include "Util/Signal.h"

#ifndef ANDROID_NDK
// NOTE: These includes should be handled in Types.hpp
//  #include <GL/glew.h>
//  #include <GL/glu.h>
#endif // ANDROID_NDK

using namespace Gallant;

// abstract singleton representing the game app.  From this class, the OS-specific
//  app is derived.
/**
  @addtogroup base
  @{
*/
//------------------------------------------------------------------------------
class Shell
  {
  protected:

    static UINT32       uLastMsTime;          ///< Last time Update was called.  Not used?
    static UINT32       uCurrMsTime;          ///< Tracks the current running time of the engine in milliseconds.

    static UINT32       uMsLogicTimeDelta;    ///< time since last logic update in milliseconds
    static UINT32       uMsRenderTimeDelta;   ///< time since last render update in milliseconds

    static UINT         uCanvasWidth;         ///< Width of the display screen in pixels
    static UINT         uCanvasHeight;        ///< Height of the display screen in pixels
    static UINT         uCanvasBitDepth;      ///< Bits per pixel of the screen
    static UINT         uShellFlags;
    static INT          iTargetLogicFps;      ///< Desired times logic is run per second.  Drives frequency that GameLoop is called by GameLoopTimer.
    static INT          iTargetRenderFps;     ///< Desired render framerate.

    static RStr         strName;              ///< Name of app
    static RStr         strTitle;             ///< Message in titlebar for windowed apps

    static RStr         strVersion;           ///< Store the version string for this compile pass.  Set by game.

    static CHAR         szCommandLine [256];

    static BOOL         g_bCloseApp;         ///< Set if the application is closing
    static BOOL         bActive;             //< is application active?  Only used by some derived classes.  Move to those classes?

    static FLOAT        fGlobalTime;         ///< Current time in seconds. Same value as uCurrMsTime, but converted.  Used by animation?

    static Shell *      pshellSingleton;     ///< Pointer to the only instance of Shell

    static INT          iFramesToRender;     ///< Number of frames that should have rendered since last cleared.

  public:
    static InputManager inputManager;        ///< Delegate to track input.  TODO: Should consider how to expose this better for TouchInput.

    // performance tracking
    static INT          aiFrameCounts [5];   ///< tracks several frame count samples to smooth out FPS calculations
    static INT          iCurrFrameCount;     ///< Incremented by derived class every time a frame is drawn.
    static INT          iCurrFramesPerSecond;

                                               ///  Attach with Shell::sigOnFixedUpdate.Connect (&l, &Label::OnUpdateListener);
    static Signal1<UINT32>  sigOnFixedUpdate;  ///< Event fired when the game loop timer posts on a regular update.  Ms time delta passed.

    static Signal3<INT, INT, BOOL&>  sigOnKeyBroadcast;  ///< Event fired when a key is broadcast to any listeners.  (KeyCode, Mod)

    static Signal1<const char *>  sigOnOpenURL;  ///< Event fired when the OpenURL method is called.  Passed back up to OS layer.
    static Signal4<int, int, const char *, const char*>  sigOnSetNotification;    ///< Event fired when the SetNotification method is called.  Passed back up to OS layer.
    static Signal1<int>                                  sigOnClearNotification;  ///< Event fired when the ClearNotification method is called.  Passed back up to OS layer.

    static Signal1<const char *>               sigOnOpenTwitter;   ///< Event fired when the OpenTwitter method is called.  Passed back up to OS layer.
    static Signal2<const char *, const char*>  sigOnOpenFacebook;  ///< Event fired when the OpenFacebook method is called.  Passed back up to OS layer.

    static Signal2<UINT, UINT>                 sigOnCanvasResize;

    // TODO: Need a signal for window size change.

  public:
                     Shell ();

    virtual          ~Shell ();

    static VOID      Init ();

    static VOID      Uninit ();

    static VOID      InitOpenGL (VOID);

    static VOID      InitOpenGLInternal (VOID);


    // State Tracking

    static UINT      GetCanvasWidth    (VOID)               {return uCanvasWidth;};

    static UINT      GetCanvasHeight   (VOID)               {return uCanvasHeight;};

    static UINT      GetCanvasBitDepth (VOID)               {return uCanvasBitDepth;};

    static UINT      GetShellFlags     (VOID)               {return uShellFlags;};

    static INT       GetTargetLogicFps  (VOID)              {return iTargetLogicFps;};

    static INT       GetTargetRenderFps (VOID)              {return iTargetRenderFps;};

    const char *     GetName           (VOID)               {return strName.AsChar ();};

    const char *     GetTitle          (VOID)               {return strTitle.AsChar ();};

    static VOID      SetCanvasWidth    (UINT  uWidthIn)     {uCanvasWidth = uWidthIn;  inputManager.SetScreenWidth ((FLOAT) uWidthIn);};

    static VOID      SetCanvasHeight   (UINT  uHeightIn)    {uCanvasHeight = uHeightIn; inputManager.SetScreenHeight ((FLOAT) uHeightIn);};

    static VOID      SetCanasSize      (UINT  uWidthIn,
                                        UINT  uHeightIn);

    static VOID      SetCanvasBitDepth (UINT  uBitDepthIn)  {uCanvasBitDepth = uBitDepthIn;};

    static VOID      SetShellFlags     (UINT  uFlagsIn)     {uShellFlags = uFlagsIn;};

    static VOID      SetTargetLogicFps  (INT  iFpsIn)       {iTargetLogicFps = iFpsIn;};

    static VOID      SetTargetRenderFps (INT  iFpsIn)       {iTargetRenderFps = iFpsIn;};

    static VOID      SetName           (const char *  szNameIn)   {strName = szNameIn;};

    static VOID      SetTitle          (const char *  szTitleIn)  {strTitle = szTitleIn;};

    VOID             ReSizeScene       (INT  iWidth,
                                        INT  iHeight);

    static VOID      SetVersion        (const char *  szVersionIn)  {strVersion.Set (szVersionIn, TRUE);};

    static const char *  GetVersion    (VOID)                       {return strVersion.AsChar ();};



    // Time Tracking

    static VOID      SetMilliseconds   (UINT32  uMsIn)      {uLastMsTime = uCurrMsTime; uCurrMsTime = uMsIn;};

    static UINT32    GetMilliseconds   (VOID)               {return (uCurrMsTime);};

    static FLOAT     GetGlobalTime     (VOID)               {return fGlobalTime;}; ///< NOTE: May lose precision over time, since stored as float.

    static VOID      SetGlobalTime     (FLOAT  fTimeIn)     {fGlobalTime = fTimeIn; inputManager.SetGlobalTime (fGlobalTime);};

    static VOID      SetLogicTimeDelta  (UINT32  uMsDeltaIn) {uMsLogicTimeDelta = uMsDeltaIn;};

    static VOID      SetRenderTimeDelta (UINT32  uMsDeltaIn) {uMsRenderTimeDelta = uMsDeltaIn;};

    static UINT32    GetLogicTimeDeltaMs    (VOID)           {return uMsLogicTimeDelta;};

    static FLOAT     GetLogicTimeDeltaSec   (VOID)           {return FLOAT(uMsLogicTimeDelta) / 1000.0f;};

    static UINT32    GetRenderTimeDeltaMs   (VOID)           {return uMsRenderTimeDelta;};

    static FLOAT     GetRenderTimeDeltaSec  (VOID)           {return FLOAT(uMsRenderTimeDelta) / 1000.0f;};

    static VOID      IncTime           (UINT32  uMsDeltaIn);

    static VOID      IncFrameCount     (VOID)               {++iCurrFrameCount;}; // render frame count

    static INT       FramesToRender    (BOOL  bClear)       {INT  iReturn = iFramesToRender; if (bClear) {iFramesToRender = 0;}; return iReturn;};

    static VOID      IncFramesToRender (VOID)               {++iFramesToRender;};


    // Flow Control

    //static INT       Loop (VOID);

    /*
    // NOTE: we no longer derive from Shell for Draw and Update
    virtual VOID     Update            (UINT32  uMsDeltaIn) = 0; ///< Framework should call this regularly.

    static VOID      StaticUpdate      (UINT32  uMsDeltaIn);

    virtual EStatus  GameLoop          (UINT32  uMillisecondDeltaIn) = 0; ///< update internal game state
    */

    static EStatus   StaticGameLoop    (UINT32  uMillisecondDeltaIn);

    /*
    // NOTE: we no longer derive from Shell for Draw and Update
    // Perform any draw buffer operations that are needed before drawing possible L/R eye images
    virtual VOID     DrawBegin         (VOID) = 0;

    // draw current frame to the screen/eye.  Called by Framework at appropriate time.
    virtual VOID     Draw              (const RMatrix &  matCameraIn,
                                        const RMatrix &  matProjectionIn) = 0;

    // Perform any draw buffer operations that are needed after the frame is drawn
    virtual VOID     DrawEnd           (VOID) = 0;
    */

    // Other shell related functions

    //VOID             ParseCommandLine  (PCHAR  szCmdLineIn); // where is this used or defined?

    static PCHAR     GetCommandLine    (VOID)               {return (szCommandLine);};

    static VOID      CloseApplication  (VOID);

    static BOOL      AppIsClosing      (VOID)               {return (g_bCloseApp);};

    static VOID      FatalError        (const char *  szMsgIn);

    static VOID      OpenURL           (const char *  szUrlIn);

    static VOID      SetNotification   (int           iCodeIn,
                                        int           iSecondsUntilShowIn,
                                        const char *  szTitleIn,
                                        const char *  szTextIn);

    static VOID      ClearNotification (int           iCodeIn);

    static VOID      OpenTwitter       (const char *  szUserNameIn);

    static VOID      OpenFacebook      (const char *  szUserIdIn,
                                        const char *  szUserNameIn);

    // InputManager Facade

    static VOID      OnKeyDown         (INT    iVKeyIn)       {inputManager.OnKeyDown (iVKeyIn);};

    static VOID      OnKeyUp           (INT    iVKeyIn)       {inputManager.OnKeyUp (iVKeyIn);};

    static VOID      OnPointerMove     (INT    iVKeyIn,
                                        FLOAT  fXIn,
                                        FLOAT  fYIn,
                                        FLOAT  fPressureIn)   {inputManager.OnMouseMove (iVKeyIn, fXIn, fYIn, fPressureIn);};

    static VOID      OnPointerDown     (INT    iVKeyIn,
                                        FLOAT  fXIn,
                                        FLOAT  fYIn,
                                        FLOAT  fPressureIn)   {inputManager.OnMouseDown (iVKeyIn, fXIn, fYIn, fPressureIn);};

    static VOID      OnPointerUp       (INT    iVKeyIn,
                                        FLOAT  fXIn,
                                        FLOAT  fYIn,
                                        FLOAT  fPressureIn)   {inputManager.OnMouseUp (iVKeyIn, fXIn, fYIn, fPressureIn);};

    static BOOL      GetNextKey        (INT &  iVKeyOut,
                                        INT &  iVKeyModOut)   {return (inputManager.GetNextKey (iVKeyOut, iVKeyModOut));};

    static BOOL      GetKey            (INT    iSearchKeyIn,
                                        INT &  iVKeyModOut)    {return (inputManager.GetKey (iSearchKeyIn, iVKeyModOut));};

    static VOID      GetKeyName        (INT32   iVirtKeyIn,
                                        RStr &  strNameOut)   {inputManager.GetKeyName (iVirtKeyIn, strNameOut);};

    static VOID      KeyNameToVKey     (const char *  szNameIn,
                                        INT32 &       iVirtKeyOut,
                                        INT32 &       iModOut)  {inputManager.NameToVKey (szNameIn, iVirtKeyOut, iModOut);};

    static BOOL      IsKeyDown         (INT    iVKeyIn)       {return (inputManager.IsKeyDown (iVKeyIn));};

    static VOID      BroadcastKey      (INT    iVKeyIn,
                                        INT    iVKeyModIn,
                                        BOOL   bHandled)    {sigOnKeyBroadcast (iVKeyIn, iVKeyModIn, bHandled);};

    static VOID      EnableInput       (BOOL   bEnabledIn)       {inputManager.Enable (bEnabledIn);};

    static VOID      SetHMDXForm       (const RMatrix &  matIn)  {inputManager.SetHMDXForm (matIn);};

    static VOID      GetHMDXForm       (RMatrix &  matOut)       {inputManager.GetHMDXForm (matOut);};

    static VOID      SetHMDXFormDelta  (const RMatrix &  matIn)  {inputManager.SetHMDXFormDelta (matIn);};

    static VOID      GetHMDXFormDelta  (RMatrix &  matOut)       {inputManager.GetHMDXFormDelta (matOut);};

    static VOID      SetNumAxis               (INT    iCountIn)  {inputManager.SetNumAxis (iCountIn);};

    static VOID      SetAxis                  (INT    iIndexIn,
                                               FLOAT  fValueIn)  {inputManager.SetAxis (iIndexIn, fValueIn);};

    static FLOAT     GetAxis                  (INT    iIndexIn)  {return inputManager.GetAxis (iIndexIn);};

    static VOID      SetAxisIndexLeftStickX   (INT    iIndexIn)  {inputManager.SetAxisIndexLeftStickX (iIndexIn);};
    static VOID      SetAxisIndexLeftStickY   (INT    iIndexIn)  {inputManager.SetAxisIndexLeftStickY (iIndexIn);};

    static VOID      SetAxisIndexRightStickX  (INT    iIndexIn)  {inputManager.SetAxisIndexRightStickX (iIndexIn);};
    static VOID      SetAxisIndexRightStickY  (INT    iIndexIn)  {inputManager.SetAxisIndexRightStickY (iIndexIn);};

    static VOID      SetAxisIndexLeftTrigger  (INT    iIndexIn)  {inputManager.SetAxisIndexLeftTrigger (iIndexIn);};
    static VOID      SetAxisIndexRightTrigger (INT    iIndexIn)  {inputManager.SetAxisIndexRightTrigger (iIndexIn);};

    static FLOAT     GetAxisLeftStickX        (VOID)             {return inputManager.GetAxisLeftStickX ();};
    static FLOAT     GetAxisLeftStickY        (VOID)             {return inputManager.GetAxisLeftStickY ();};

    static FLOAT     GetAxisRightStickX       (VOID)             {return inputManager.GetAxisRightStickX ();};
    static FLOAT     GetAxisRightStickY       (VOID)             {return inputManager.GetAxisRightStickY ();};

    static FLOAT     GetAxisLeftTrigger       (VOID)             {return inputManager.GetAxisLeftTrigger ();};
    static FLOAT     GetAxisRightTrigger      (VOID)             {return inputManager.GetAxisRightTrigger ();};

    static VOID      GetPointerDelta          (FLOAT    fTimeDeltaIn,
                                               FLOAT *  pfXOut,
                                               FLOAT *  pfYOut,
                                               FLOAT *  pfPressureOut)  {return inputManager.GetPosDelta (fTimeDeltaIn,
                                                                                                          pfXOut,
                                                                                                          pfYOut,
                                                                                                          pfPressureOut);};

  };


//------------------------------------------------------------------------------
class GameLoopTimer  : public TimerBase
  {
  private:

  public:
                   GameLoopTimer  ();

    explicit       GameLoopTimer  (INT64     iMillisecondsToWaitIn,
                                   BOOL      bOneTimeIn = FALSE);

    virtual        ~GameLoopTimer ();

    BOOL           Post         (INT64  uMillisecondsSinceLast) override;

    static UINT32  TypeID       (VOID)                                   {return MAKE_FOUR_CODE ("GMLP");};

    VOID           Copy         (const TimerBase &  timerIn)    override {return;};

    TimerBase *    Duplicate    (VOID) const override                    {
                                                                         GameLoopTimer * pNew = new GameLoopTimer;
                                                                         pNew->Set (*this);
                                                                         return pNew;
                                                                         };

  };


//------------------------------------------------------------------------------
class RenderFrameTimer  : public TimerBase
  {
  private:

  public:
                   RenderFrameTimer  ();

    explicit       RenderFrameTimer  (INT64     iMillisecondsToWaitIn,
                                      BOOL      bOneTimeIn = FALSE);

    virtual        ~RenderFrameTimer ();

    BOOL           Post  (INT64  uMillisecondsSinceLast)     override;

    static UINT32  TypeID       (VOID)                                {return MAKE_FOUR_CODE ("RNDR");};

    VOID           Copy         (const TimerBase &  timerIn) override {return;};

    TimerBase *    Duplicate    (VOID) const override                 {
                                                                      RenderFrameTimer * pNew = new RenderFrameTimer;
                                                                      pNew->Set (*this);
                                                                      return pNew;
                                                                      };
  };


//------------------------------------------------------------------------------
class FrameCountTimer  : public TimerBase
  {
  private:

  public:
                   FrameCountTimer  ();

    explicit       FrameCountTimer  (INT64     iMillisecondsToWaitIn,
                                     BOOL      bOneTimeIn = FALSE);

    virtual        ~FrameCountTimer ();

    BOOL           Post             (INT64  iMillisecondsSinceLast) override;

    static UINT32  TypeID           (VOID)                                   {return MAKE_FOUR_CODE ("FRME");};

    VOID           Copy             (const TimerBase &  timerIn)    override {return;};

    TimerBase *    Duplicate        (VOID) const override                    {
                                                                             FrameCountTimer * pNew = new FrameCountTimer;
                                                                             pNew->Set (*this);
                                                                             return pNew;
                                                                             };

  };
/** @} */ // end of base group

#endif // SHELL_HPP
