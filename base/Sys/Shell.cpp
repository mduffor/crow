
// SHELL.CPP
// Copyright 1996, Michael T. Duffy.  All Rights Reserved

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/Shell.hpp"
#include "Sys/Timer.hpp"
#include "Gfx/GLUtil.hpp"
//#include "RGlobal.hpp"

// The Composite classes are included here for initialization of templates.
#include "Composite/Attr.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrStringArray.hpp"

BOOL             Shell::bActive = TRUE;
BOOL             Shell::g_bCloseApp = FALSE;
UINT32           Shell::uLastMsTime = 0;
UINT32           Shell::uCurrMsTime = 0;

UINT32           Shell::uMsLogicTimeDelta = 0;
UINT32           Shell::uMsRenderTimeDelta = 0;

UINT             Shell::uCanvasWidth = 640;
UINT             Shell::uCanvasHeight = 480;
UINT             Shell::uCanvasBitDepth = 32;
UINT             Shell::uShellFlags;
INT              Shell::iTargetLogicFps  = 30;
INT              Shell::iTargetRenderFps = 60;
INT              Shell::iFramesToRender  = 0;

RStr             Shell::strName;
RStr             Shell::strTitle;
RStr             Shell::strVersion;

CHAR             Shell::szCommandLine [256];

INT              Shell::aiFrameCounts [5];
INT              Shell::iCurrFrameCount;
INT              Shell::iCurrFramesPerSecond = 0;

FLOAT            Shell::fGlobalTime = 0.0f;

//Shell *          Shell::pshellSingleton = NULL;
InputManager     Shell::inputManager;

Signal1<UINT32>           Shell::sigOnFixedUpdate;
Signal3<INT, INT, BOOL&>  Shell::sigOnKeyBroadcast;

Signal1<const char *>                         Shell::sigOnOpenURL;
Signal4<int, int, const char*, const char *>  Shell::sigOnSetNotification;
Signal1<int>                                  Shell::sigOnClearNotification;
Signal1<const char *>                         Shell::sigOnOpenTwitter;
Signal2<const char*, const char *>            Shell::sigOnOpenFacebook;
Signal2<UINT, UINT>                           Shell::sigOnCanvasResize;

//************************************************************************
//   Code
//************************************************************************

//------------------------------------------------------------------------------
VOID Shell::InitOpenGL (VOID)
  {
  InitOpenGLInternal ();
  }

//------------------------------------------------------------------------------
VOID Shell::InitOpenGLInternal (VOID)
  {
  GLPrintInfo ();

  // NOTE:  Need to check and see if we actually need the glew stuff now that
  //  we use GLFW
  #ifndef ANDROID_NDK
    #ifdef GLES
      glewInit ();
    #endif
  #endif

  #ifndef LINUX
    #ifdef GLES
    /*
      glewExperimental = GL_TRUE;
      glewInit ();


      // get version info
      const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
      const GLubyte* version = glGetString(GL_VERSION); // version as a string
      printf("Renderer: %s\n", renderer);
      printf("OpenGL version supported %s\n", version);
*/
    #endif
  #endif

  // OpenGL Initialization
  #ifdef GLES
    //glViewport   (0, 0, GetCanvasWidth (), GetCanvasHeight ());
  #endif

  /*
  // left, right, bottom, top, near, far
  //glOrtho (0, GetCanvasWidth (), GetCanvasHeight (), 0, 1, -1);
  DBG_INFO (" Shell::InitOpenGLInternal creating ortho matrix for canvas sized %d  %d", GetCanvasWidth (), GetCanvasHeight ());
  RMatrix  matOrtho;
  MatrixOrtho (matOrtho, 0, GetCanvasWidth (), GetCanvasHeight (), 0, 1, -1);
  igCtx.SetProjMatrix   (matOrtho);
  */

  //igCtx.SetModelMatrix   (RMatrix::kIdentity);
  };


//------------------------------------------------------------------------------
Shell::Shell ()
  {
  //Init ();
  };

//------------------------------------------------------------------------------
Shell::~Shell ()
  {
  Uninit ();
  };


//------------------------------------------------------------------------------
VOID Shell::Init ()
  {
  // initialize internal variables
  g_bCloseApp = FALSE;

  // start the default timer
  TimerBase *  pGameLoopTimerTemplate    = TimerManager::Register (new GameLoopTimer);
  TimerBase *  pFrameCountTimerTemplate  = TimerManager::Register (new FrameCountTimer);
  TimerBase *  pRenderFrameTimerTemplate = TimerManager::Register (new RenderFrameTimer);

  pGameLoopTimerTemplate->SetName ("GameLoop");
  pFrameCountTimerTemplate->SetName ("FrameCount");
  pRenderFrameTimerTemplate->SetName ("RenderFrame");

  TimerBase *  pGameLoopTimer    = TimerManager::Instance()->NewTimer (GameLoopTimer::TypeID ());
  TimerBase *  pFrameCountTimer  = TimerManager::Instance()->NewTimer (FrameCountTimer::TypeID ());
  TimerBase *  pRenderFrameTimer = TimerManager::Instance()->NewTimer (RenderFrameTimer::TypeID ());

  DBG_INFO ("---------============== Shell Init ===========-----------------");

  pGameLoopTimer->Start    (1000.0 / GetTargetLogicFps (), false);  // Update game state every TargetLogicFps
  pRenderFrameTimer->Start (1000.0 / GetTargetRenderFps (), false); // Render frame every TargetRenderFps
  pFrameCountTimer->Start  (1000.0, false);                         // count FPS every 1-second

  fGlobalTime = 0.0f;

  // Initialize Attr templates.
  // REFACTOR: This needs to go somewhere else, yet still be initialized centrally.  I'd
  //  prefer not requiring it to be in game code, but rather in Crow code.  But I don't
  //  know where that place is just yet.
  Attr::AddTemplate (static_cast<Attr*>(new AttrFloat  ("float")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrInt    ("int")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrString ("string")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrStringArray ("stringarray")));

  DBG_INFO ("Shell initialization complete");
  };

//------------------------------------------------------------------------------
VOID Shell::Uninit ()
  {
  // We've exited the main loop.  Clean up.
  TimerManager::Instance()->DeleteAllTimers ();
  };

//------------------------------------------------------------------------------
VOID Shell::CloseApplication  (VOID)
  {
  DBG_INFO ("CloseApplication() called\n");
  g_bCloseApp = TRUE;
  };

//------------------------------------------------------------------------------
VOID Shell::FatalError (const char *      szMsgIn)
  {
  DBG_ERROR ("");
  DBG_ERROR ("---===**************===---");
  DBG_ERROR ("****** FATAL ERROR *******\n  %s  ", szMsgIn);
  DBG_ERROR ("---===**************===---");
  DBG_ERROR ("");

  Shell::CloseApplication ();
  };

// NOTE:  I'm currently handling OS callback methods such as OpenURL () and
//         ShowNotification () as events that the OS level subscribes to.  This
//         is because the OS level is what creates the Shell.  The other approach,
//         which may work better, would be to have the OS level pass delegates
//         to Shell, which it would then use for the operations.  Either way
//         works, but if the events become too unweildy try the delegate approach.

//------------------------------------------------------------------------------
VOID Shell::OpenURL (const char *  szUrlIn)
  {
  sigOnOpenURL (szUrlIn);
  };

//------------------------------------------------------------------------------
VOID Shell::SetNotification (int           iCodeIn,
                             int           iSecondsUntilShownIn,
                             const char *  szTitleIn,
                             const char *  szTextIn)
  {
  sigOnSetNotification (iCodeIn, iSecondsUntilShownIn, szTitleIn, szTextIn);
  };

//------------------------------------------------------------------------------
VOID Shell::ClearNotification (int           iCodeIn)
  {
  sigOnClearNotification (iCodeIn);
  };

//------------------------------------------------------------------------------
VOID Shell::OpenTwitter (const char *  szUserNameIn)
  {
  sigOnOpenTwitter (szUserNameIn);
  };

//------------------------------------------------------------------------------
VOID Shell::OpenFacebook (const char *  szUserIdIn,
                          const char *  szUserNameIn)
  {
  sigOnOpenFacebook (szUserIdIn, szUserNameIn);
  };

//------------------------------------------------------------------------------
VOID  Shell::SetCanasSize  (UINT  uWidthIn,
                            UINT  uHeightIn)
  {
  UINT  uPrevWidth  = uCanvasWidth;
  UINT  uPrevHeight = uCanvasHeight;


  SetCanvasWidth (uWidthIn);
  SetCanvasHeight (uHeightIn);

  if ((uPrevWidth  != uCanvasWidth) ||
      (uPrevHeight != uCanvasHeight))
    {
    sigOnCanvasResize (uCanvasWidth, uCanvasHeight);
    };
  };

//------------------------------------------------------------------------------
VOID Shell::ReSizeScene  (INT             iWidthIn,
                          INT             iHeightIn)
  {
  // NOTE: What calls this?
  SetCanvasWidth  (iWidthIn);
  SetCanvasHeight (iHeightIn);

  // Reset The Current Viewport And Perspective Transformation
  //camMainCamera.SetWindowSize (iWidthIn, iHeightIn);
  };

/*
// NOTE: we no longer derive from Shell for Draw and Update
//------------------------------------------------------------------------------
VOID Shell::StaticUpdate      (UINT32  uMsDeltaIn)
  {
  // NOTE: this isn't called.  Once upon a time, it was probably called from
  //  GameLoopTimer for a regular updating.  Simply running IncTime on
  //  Shell::tmTimers would drive the updating and drawing.  Now Update and Draw
  //  are driven specifically by the class derived from Shell.
  if (pshellSingleton != NULL)
    {
    pshellSingleton->Update (uMsDeltaIn);
    pshellSingleton->Draw (RMatrix::kIdentity, RMatrix::kIdentity);
    };
  };
*/

//------------------------------------------------------------------------------
EStatus Shell::StaticGameLoop   (UINT32  uMillisecondDeltaIn)
  {
  // called once per 1/target_fps seconds by the GameLoopTimer
  sigOnFixedUpdate (uMillisecondDeltaIn);

  //if (pshellSingleton != NULL)
  //  {
  //  return (pshellSingleton->GameLoop (uMillisecondDeltaIn));
  //  };
  //return (EStatus::kFailure);
  return (EStatus::kSuccess);
  };

//------------------------------------------------------------------------------
VOID  Shell::IncTime (UINT32  uMsDeltaIn)
  {
  // increment timers, and as a result call the GameLoop on a regular interval.
  uCurrMsTime += uMsDeltaIn;
  TimerManager::Instance()->IncTime (uMsDeltaIn);
  uLastMsTime = uCurrMsTime;
  };


//==============================================================================
//  Game Loop Timer
//==============================================================================

//------------------------------------------------------------------------------
GameLoopTimer::GameLoopTimer  ()
  {
  ccType      = GameLoopTimer::TypeID();
  eTimeSource = TimerBase::kLocalDelta;
  bOneShot    = FALSE;
  };


//------------------------------------------------------------------------------
GameLoopTimer::GameLoopTimer  (INT64     iMillisecondsToWaitIn,
                               BOOL      bOneTimeIn)
  {
  ccType = GameLoopTimer::TypeID();
  Start (iMillisecondsToWaitIn, bOneTimeIn);
  };

//------------------------------------------------------------------------------
GameLoopTimer::~GameLoopTimer ()
  {
  // this space intentionally left empty
  };

//------------------------------------------------------------------------------
BOOL  GameLoopTimer::Post  (INT64  iMillisecondsSinceLast)
  {
  EStatus   status;

  //DBG_INFO ("GameLoopTimer::Post SetGlobalTime to %f", Shell::GetGlobalTime () + FLOAT (iMillisecondsSinceLast) / 1000.0f);

  Shell::SetGlobalTime (Shell::GetGlobalTime () + FLOAT (iMillisecondsSinceLast) / 1000.0f);

  Shell::SetLogicTimeDelta (iMillisecondsSinceLast);
  status = Shell::StaticGameLoop (iMillisecondsSinceLast);
  if (status == EStatus::kFailure)
    {
    Shell::FatalError (status.GetDescription ());
    // exit timer
    return (1);
    };

  // repeat timer
  return (0);
  };


//==============================================================================
//  Framecount Timer
//==============================================================================

//------------------------------------------------------------------------------
FrameCountTimer::FrameCountTimer  ()
  {
  ccType      = FrameCountTimer::TypeID();
  eTimeSource = TimerBase::kLocalDelta;
  bOneShot    = FALSE;
  };


//------------------------------------------------------------------------------
FrameCountTimer::FrameCountTimer  (INT64   iMillisecondsToWaitIn,
                                   BOOL    bOneTimeIn)
  {
  ccType = FrameCountTimer::TypeID();
  Start (iMillisecondsToWaitIn, bOneTimeIn);
  };

//------------------------------------------------------------------------------
FrameCountTimer::~FrameCountTimer ()
  {
  // this space intentionally left empty
  };

//------------------------------------------------------------------------------
BOOL  FrameCountTimer::Post  (INT64  iMillisecondsSinceLast)
  {
  INT  iIndex;

  //RStr  strOut;  strOut.Format ("Shell is at %x", pShell);  DBG_INFO (strOut.AsChar ());
  for (iIndex = 0; iIndex < 4; ++iIndex)
    {
    Shell::aiFrameCounts [iIndex] = Shell::aiFrameCounts [iIndex + 1];
    };
  Shell::aiFrameCounts [4] = Shell::iCurrFrameCount;
  Shell::iCurrFrameCount = 0;

  INT  iFrameCountSum = 0;
  for (iIndex = 0; iIndex < 5; ++iIndex)
    {
    iFrameCountSum += Shell::aiFrameCounts [iIndex];
    };
  Shell::iCurrFramesPerSecond = iFrameCountSum / 5;

  //DBG_RAW ("====== Frame Count (FPS): Avg: %d Curr: %d======\n", iFrameCountSum / 5, Shell::aiFrameCounts [4]);

  // repeat
  return (0);
  };


//==============================================================================
//  Render Frame Timer
//==============================================================================

//------------------------------------------------------------------------------
RenderFrameTimer::RenderFrameTimer  ()
  {
  ccType      = RenderFrameTimer::TypeID();
  eTimeSource = TimerBase::kLocalDelta;
  bOneShot    = FALSE;
  };


//------------------------------------------------------------------------------
RenderFrameTimer::RenderFrameTimer  (INT64     iMillisecondsToWaitIn,
                                     BOOL      bOneTimeIn)
  {
  ccType = RenderFrameTimer::TypeID();
  Start (iMillisecondsToWaitIn, bOneTimeIn);
  };

//------------------------------------------------------------------------------
RenderFrameTimer::~RenderFrameTimer ()
  {
  // this space intentionally left empty
  };

//------------------------------------------------------------------------------
BOOL  RenderFrameTimer::Post  (INT64  iMillisecondsSinceLast)
  {
  Shell::IncFramesToRender ();

  Shell::SetRenderTimeDelta (iMillisecondsSinceLast);

  // repeat timer
  return (0);
  };

