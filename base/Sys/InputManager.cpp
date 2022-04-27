/* -----------------------------------------------------------------
                            Input Manager

     This module implements a central interface for setting,
     storing, retrieving, and notifying about input from the
     mouse, touchscreen, keyboard, joysticks, or HMD.  It is
     the clearing house for input from the user to the app.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2014, Michael T. Duffy II.  All rights reserved.
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

#include "Sys/InputManager.hpp"
#include "Math/RVec.hpp"

float InputManager::AXIS_THRESHOLD = 0.01f;
RMatrix  InputManager::matHMDXForm;
RMatrix  InputManager::matHMDXFormDelta;

//------------------------------------------------------------------------------
InputManager::InputManager  ()
  {
  iMouseLastX = 0;
  iMouseLastY = 0;

  fGlobalTime = 0.0f;
  iQueueSize = 100;
  iQueueIndex = 0;

  auButtons.SetLength (iQueueSize);
  afX.SetLength (iQueueSize);
  afY.SetLength (iQueueSize);
  afPressure.SetLength (iQueueSize);
  afEventTime.SetLength (iQueueSize);

  memset (aiKeyEntered, 0, sizeof (aiKeyEntered));
  memset (aiKeyMod, 0, sizeof (aiKeyMod));
  memset (bKeyDown, 0, sizeof (bKeyDown));

  bShiftDown   = FALSE;
  bControlDown = FALSE;
  bAltDown     = FALSE;
  bMetaDown    = FALSE;

  iKeyRead  = 0;
  iKeyWrite = 0;

  matHMDXForm.Identity ();
  matHMDXFormDelta.Identity ();

  fScreenWidth  = 1.0f;
  fScreenHeight = 1.0f;
  fNDCX         = 1.0f;
  fNDCY         = 1.0f;

  iLeftClicks     = 0;
  iRightClicks    = 0;
  iCenterClicks   = 0;
  iScrollUpClicks = 0;
  iScrollDnClicks = 0;

  bEnabled = FALSE;

  iAxisLeftStickX   = 0;
  iAxisLeftStickY   = 0;
  iAxisRightStickX  = 0;
  iAxisRightStickY  = 0;
  iAxisLeftTrigger  = 0;
  iAxisRightTrigger = 0;


  arrayMappings.Append ("BkSpace", VKEY_BACKSPACE);
  arrayMappings.Append ("Tab",     VKEY_TAB);
  arrayMappings.Append ("Enter",   VKEY_ENTER);
  arrayMappings.Append ("Esc",     VKEY_ESC);
  arrayMappings.Append ("Space",   VKEY_SPACE);
  arrayMappings.Append ("LButton", VKEY_LBUTTON);
  arrayMappings.Append ("RButton", VKEY_RBUTTON);
  arrayMappings.Append ("MButton", VKEY_MBUTTON);

  arrayMappings.Append ("Menu",  VKEY_MENU);
  arrayMappings.Append ("Pause", VKEY_PAUSE);
  arrayMappings.Append ("Caps",  VKEY_CAPITAL);
  arrayMappings.Append ("End",   VKEY_END);
  arrayMappings.Append ("Home",  VKEY_HOME);
  arrayMappings.Append ("Left",  VKEY_LEFT);
  arrayMappings.Append ("Up",    VKEY_UP);
  arrayMappings.Append ("Right", VKEY_RIGHT);
  arrayMappings.Append ("Down",  VKEY_DOWN);
  arrayMappings.Append ("Ins",   VKEY_INSERT);
  arrayMappings.Append ("Del",   VKEY_DELETE);
  arrayMappings.Append ("Help",  VKEY_HELP);


  //arrayMappings.Append ("Cancel",  VKEY_CANCEL);
  //arrayMappings.Append ("Clr",  VKEY_CLEAR);
  //arrayMappings.Append ("Prev",  VKEY_PRIOR);
  //arrayMappings.Append ("Next",  VKEY_NEXT);
  //arrayMappings.Append ("Select",  VKEY_SELECT);
  //arrayMappings.Append ("Print",  VKEY_PRINT);
  //arrayMappings.Append ("Exec",  VKEY_EXECUTE);
  //arrayMappings.Append ("Snapshot",  VKEY_SNAPSHOT);

  arrayMappings.Append ("Num0", VKEY_NUMPAD0);
  arrayMappings.Append ("Num1", VKEY_NUMPAD1);
  arrayMappings.Append ("Num2", VKEY_NUMPAD2);
  arrayMappings.Append ("Num3", VKEY_NUMPAD3);
  arrayMappings.Append ("Num4", VKEY_NUMPAD4);
  arrayMappings.Append ("Num5", VKEY_NUMPAD5);
  arrayMappings.Append ("Num6", VKEY_NUMPAD6);
  arrayMappings.Append ("Num7", VKEY_NUMPAD7);
  arrayMappings.Append ("Num8", VKEY_NUMPAD8);
  arrayMappings.Append ("Num9", VKEY_NUMPAD9);

  arrayMappings.Append ("NumSpace", VKEY_NUMPAD_SPACE);
  arrayMappings.Append ("NumTab",   VKEY_NUMPAD_TAB);
  arrayMappings.Append ("NumEnter", VKEY_NUMPAD_ENTER);
  arrayMappings.Append ("NumF1",    VKEY_NUMPAD_F1);
  arrayMappings.Append ("NumF2",    VKEY_NUMPAD_F2);
  arrayMappings.Append ("NumF3",    VKEY_NUMPAD_F3);
  arrayMappings.Append ("NumF4",    VKEY_NUMPAD_F4);
  arrayMappings.Append ("NumHome",  VKEY_NUMPAD_HOME);
  arrayMappings.Append ("NumLeft",  VKEY_NUMPAD_LEFT);
  arrayMappings.Append ("NumUp",    VKEY_NUMPAD_UP);
  arrayMappings.Append ("NumRight", VKEY_NUMPAD_RIGHT);
  arrayMappings.Append ("NumDown",  VKEY_NUMPAD_DOWN);
  arrayMappings.Append ("NumPgUp",  VKEY_NUMPAD_PAGEUP);
  arrayMappings.Append ("NumPgDn",  VKEY_NUMPAD_PAGEDOWN);
  arrayMappings.Append ("NumEnd",   VKEY_NUMPAD_END);
  arrayMappings.Append ("NumBegin", VKEY_NUMPAD_BEGIN);
  arrayMappings.Append ("NumIns",   VKEY_NUMPAD_INSERT);
  arrayMappings.Append ("NumDel",   VKEY_NUMPAD_DELETE);
  arrayMappings.Append ("NumEqual", VKEY_NUMPAD_EQUAL);
  arrayMappings.Append ("NumMult",  VKEY_NUMPAD_MULTIPLY);
  arrayMappings.Append ("NumAdd",   VKEY_NUMPAD_ADD);
  arrayMappings.Append ("NumSep",   VKEY_NUMPAD_SEPARATOR);
  arrayMappings.Append ("NumSub",   VKEY_NUMPAD_SUBTRACT);
  arrayMappings.Append ("NumDec",   VKEY_NUMPAD_DECIMAL);
  arrayMappings.Append ("NumDiv",   VKEY_NUMPAD_DIVIDE);
  //arrayMappings.Append ("NumPrev",   VKEY_NUMPAD_PRIOR);
  //arrayMappings.Append ("NumNext",   VKEY_NUMPAD_NEXT);

  arrayMappings.Append ("Mult",    VKEY_MULTIPLY);
  arrayMappings.Append ("Add",     VKEY_ADD);
  arrayMappings.Append ("Sep",     VKEY_SEPARATOR);
  arrayMappings.Append ("Sub",     VKEY_SUBTRACT);
  arrayMappings.Append ("Dec",     VKEY_DECIMAL);
  arrayMappings.Append ("Div",     VKEY_DIVIDE);
  arrayMappings.Append ("F1",      VKEY_F1);
  arrayMappings.Append ("F2",      VKEY_F2);
  arrayMappings.Append ("F3",      VKEY_F3);
  arrayMappings.Append ("F4",      VKEY_F4);
  arrayMappings.Append ("F5",      VKEY_F5);
  arrayMappings.Append ("F6",      VKEY_F6);
  arrayMappings.Append ("F7",      VKEY_F7);
  arrayMappings.Append ("F8",      VKEY_F8);
  arrayMappings.Append ("F9",      VKEY_F9);
  arrayMappings.Append ("F10",     VKEY_F10);
  arrayMappings.Append ("F11",     VKEY_F11);
  arrayMappings.Append ("F12",     VKEY_F12);
  arrayMappings.Append ("F13",     VKEY_F13);
  arrayMappings.Append ("F14",     VKEY_F14);
  arrayMappings.Append ("F15",     VKEY_F15);
  arrayMappings.Append ("NumLock", VKEY_NUMLOCK);
  arrayMappings.Append ("Scroll",  VKEY_SCROLL);
  arrayMappings.Append ("PgUp",    VKEY_PAGEUP);
  arrayMappings.Append ("PgDn",    VKEY_PAGEDOWN);

  arrayMappings.Append ("GamepadBtn1", GAMEPAD_BUTTON_1);
  arrayMappings.Append ("GamepadBtn2", GAMEPAD_BUTTON_2);
  arrayMappings.Append ("GamepadBtn3", GAMEPAD_BUTTON_3);
  arrayMappings.Append ("GamepadBtn4", GAMEPAD_BUTTON_4);
  arrayMappings.Append ("GamepadBtn5", GAMEPAD_BUTTON_5);
  arrayMappings.Append ("GamepadBtn6", GAMEPAD_BUTTON_6);
  arrayMappings.Append ("GamepadBtn7", GAMEPAD_BUTTON_7);
  arrayMappings.Append ("GamepadBtn8", GAMEPAD_BUTTON_8);
  arrayMappings.Append ("GamepadBtn9", GAMEPAD_BUTTON_9);
  arrayMappings.Append ("GamepadBtn10", GAMEPAD_BUTTON_10);
  arrayMappings.Append ("GamepadBtn11", GAMEPAD_BUTTON_11);
  arrayMappings.Append ("GamepadBtn12", GAMEPAD_BUTTON_12);
  arrayMappings.Append ("GamepadBtn13", GAMEPAD_BUTTON_13);
  arrayMappings.Append ("GamepadBtn14", GAMEPAD_BUTTON_14);
  arrayMappings.Append ("GamepadBtn15", GAMEPAD_BUTTON_15);
  arrayMappings.Append ("GamepadBtn16", GAMEPAD_BUTTON_16);

  arrayMappings.Append ("BtnA",       BUTTON_A);
  arrayMappings.Append ("BtnB",       BUTTON_B);
  arrayMappings.Append ("BtnC",       BUTTON_C);
  arrayMappings.Append ("BtnX",       BUTTON_X);
  arrayMappings.Append ("BtnY",       BUTTON_Y);
  arrayMappings.Append ("BtnZ",       BUTTON_Z);
  arrayMappings.Append ("BtnL1",      BUTTON_L1);
  arrayMappings.Append ("BtnL2",      BUTTON_L2);
  arrayMappings.Append ("BtnR1",      BUTTON_R1);
  arrayMappings.Append ("BtnR2",      BUTTON_R2);
  arrayMappings.Append ("BtnThumbL",  BUTTON_THUMBL);
  arrayMappings.Append ("BtnThumbR",  BUTTON_THUMBR);
  arrayMappings.Append ("BtnMode",    BUTTON_MODE);
  arrayMappings.Append ("BtnSelect",  BUTTON_SELECT);
  arrayMappings.Append ("BtnStart",   BUTTON_START);
  arrayMappings.Append ("DPadCenter", DPAD_CENTER);
  arrayMappings.Append ("DPadDown",   DPAD_DOWN);
  arrayMappings.Append ("DPadLeft",   DPAD_LEFT);
  arrayMappings.Append ("DPadRight",  DPAD_RIGHT);
  arrayMappings.Append ("DPadUp",     DPAD_UP);
  arrayMappings.Append ("ScrollUp",   VKEY_SCROLLUP);
  arrayMappings.Append ("ScrollDn",   VKEY_SCROLLDOWN);
  };

//------------------------------------------------------------------------------
InputManager::~InputManager  ()
  {
  };

//------------------------------------------------------------------------------
BOOL  InputManager::GetNextKey  (INT &  iKeyOut,
                                 INT &  iKeyModOut)
  {
  if (!bEnabled) return (FALSE);
  if (iKeyRead == iKeyWrite) return (FALSE);
  if (aiKeyEntered [iKeyRead] == 0)
    {
    return (FALSE);
    };

  // read from the buffer
  iKeyOut    = aiKeyEntered [iKeyRead];
  iKeyModOut = aiKeyMod     [iKeyRead];

  // clear the buffer now that we have the key
  aiKeyEntered [iKeyRead] = 0;
  aiKeyMod     [iKeyRead] = 0;

  // increment to the next read position
  iKeyRead = (iKeyRead + 1) % KEY_BUFFER_SIZE;
  return (TRUE);
  };

//------------------------------------------------------------------------------
BOOL  InputManager::GetKey  (INT    iSearchKeyIn,
                             INT &  iKeyModOut)
  {
  // find if the key is in the buffer
  INT  iMaxKey = iKeyRead + KEY_BUFFER_SIZE;
  for (INT  iIndex = iKeyRead; iIndex < iMaxKey; ++iIndex)
    {
    INT  iClippedIndex = iIndex % KEY_BUFFER_SIZE;
    if (iSearchKeyIn == aiKeyEntered [iClippedIndex])
      {
      // found the key we are looking for
      iKeyModOut = aiKeyMod     [iClippedIndex];

      // collapse the empty space
      INT  iPrevClippedIndex = iClippedIndex;
      for (++iIndex; iIndex < iMaxKey; iPrevClippedIndex = iClippedIndex, ++iIndex)
        {
        iClippedIndex = iIndex % KEY_BUFFER_SIZE;

        aiKeyEntered [iPrevClippedIndex] = aiKeyEntered [iClippedIndex];
        aiKeyMod     [iPrevClippedIndex] = aiKeyMod     [iClippedIndex];
        };
      iKeyWrite = (iKeyWrite + KEY_BUFFER_SIZE - 1) % KEY_BUFFER_SIZE;
      return (TRUE);
      };
    };
  return (FALSE);
  };

//------------------------------------------------------------------------------
VOID  InputManager::StoreKey (INT  iKeyIn,
                              INT  iKeyModIn)
  {
  if (!bEnabled) return;
  if (iKeyIn == 0) return;
  INT  iKeyNextWrite = (iKeyWrite + 1) % KEY_BUFFER_SIZE;

  // make sure the buffer isn't full
  if (iKeyNextWrite == iKeyRead)
    {
    // buffer full
    DBG_INFO ("Ignoring KeyCode %x because buffer is full.  The contents of the buffer are as follows:", iKeyIn);
    return;
    };

  //DBG_INFO ("Storing KeyCode %x at circular buffer index %d", iKeyIn, iKeyWrite);

  aiKeyEntered [iKeyWrite] = iKeyIn;
  aiKeyMod     [iKeyWrite] = iKeyModIn;

  // increment to the next write position
  iKeyWrite = iKeyNextWrite;
  };

//------------------------------------------------------------------------------
VOID  InputManager::PushMousePos    (FLOAT  fXIn,
                                     FLOAT  fYIn,
                                     FLOAT  fPressureIn)
  {
  if (!bEnabled) return;
  // called by the OS-specific main loop to set the current mouse position

  // queue up the mouse position
  ++iQueueIndex;
  if (iQueueIndex >= iQueueSize) iQueueIndex -= iQueueSize;
  auButtons  [iQueueIndex] = (bShiftDown   ? VKFLG_SHIFT   : 0) |
                             (bControlDown ? VKFLG_CONTROL : 0) |
                             (bAltDown     ? VKFLG_ALT     : 0) |
                             (bMetaDown    ? VKFLG_META    : 0);
  afX         [iQueueIndex] = fXIn;
  afY         [iQueueIndex] = fYIn;
  afPressure  [iQueueIndex] = fPressureIn;
  afEventTime [iQueueIndex] = fGlobalTime;

  //DBG_INFO ("PushMousePos Pointer %d %f %f", iQueueIndex, fXIn, fYIn);
  };

//------------------------------------------------------------------------------
VOID  InputManager::OnKeyDown (INT  iVKeyIn)
  {
  if (!bEnabled) return;
  if (iVKeyIn <= VKEY_MAX)
    {
    bKeyDown [iVKeyIn] = 1;
    };
  //DBG_INFO ("OnKeyDown %x", iVKeyIn);

  switch (iVKeyIn)
    {
    case VKEY_CONTROL:
      bControlDown = 1;
      break;
    case VKEY_SHIFT:
      bShiftDown = 1;
      break;
    case VKEY_ALT:
      bAltDown = 1;
      break;
    default:
      StoreKey (iVKeyIn, (bShiftDown   ? VKFLG_SHIFT   : 0) |
                         (bControlDown ? VKFLG_CONTROL : 0) |
                         (bAltDown     ? VKFLG_ALT     : 0) |
                         (bMetaDown    ? VKFLG_META    : 0));
      break;
    };
  };

//------------------------------------------------------------------------------
VOID  InputManager::OnKeyUp (INT  iVKeyIn)
  {
  if (!bEnabled) return;
  //BOOL  bModifierKey = FALSE;

  switch (iVKeyIn)
    {
    case VKEY_CONTROL:
      bControlDown = 0;
      //bModifierKey = TRUE;
      break;
    case VKEY_SHIFT:
      bShiftDown = 0;
      //bModifierKey = TRUE;
      break;
    case VKEY_ALT:
      bAltDown = 0;
      //bModifierKey = TRUE;
      break;

    case VKEY_LBUTTON:
      if (IsKeyDown (iVKeyIn)) {++iLeftClicks;}
      break;

    case VKEY_RBUTTON:
      if (IsKeyDown (iVKeyIn)) {++iRightClicks;}
      break;

    case VKEY_MBUTTON:
      if (IsKeyDown (iVKeyIn)) {++iCenterClicks;}
      break;

    case VKEY_SCROLLUP:
      if (IsKeyDown (iVKeyIn)) {++iScrollUpClicks;}
      break;

    case VKEY_SCROLLDOWN:
      if (IsKeyDown (iVKeyIn)) {++iScrollDnClicks;}
      break;

    default:
      break;
    }

  if (iVKeyIn <= VKEY_MAX)
    {
    bKeyDown [iVKeyIn] = 0;
    };
  /*
  if (!bModifierKey)
    {
    StoreKey (iVKeyIn, VKFLG_RELEASE |
                       (bShiftDown   ? VKFLG_SHIFT   : 0) |
                       (bControlDown ? VKFLG_CONTROL : 0) |
                       (bAltDown     ? VKFLG_ALT     : 0) |
                       (bMetaDown    ? VKFLG_META    : 0));
    };
  */
  };

//------------------------------------------------------------------------------
VOID  InputManager::OnMouseMove (INT    iVKeyIn,
                                 FLOAT  fXIn,
                                 FLOAT  fYIn,
                                 FLOAT  fPressureIn)
  {
  if (!bEnabled) return;
  if (iVKeyIn == VKEY_TOUCH_0)
    {
    PushMousePos (fXIn, fYIn, fPressureIn);
    }
  }

//------------------------------------------------------------------------------
VOID  InputManager::OnMouseDown (INT    iVKeyIn,
                                 FLOAT  fXIn,
                                 FLOAT  fYIn,
                                 FLOAT  fPressureIn)
  {
  //DBG_INFO ("OnMouseDown");
  if (!bEnabled)
    {
    DBG_INFO ("OnMouseDown ignored because InputManager is not enabled.");
    return;
    }
  OnKeyDown (iVKeyIn);
  OnMouseMove (iVKeyIn, fXIn, fYIn, fPressureIn);
  };

//------------------------------------------------------------------------------
VOID  InputManager::OnMouseUp (INT    iVKeyIn,
                               FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn)
  {
  if (!bEnabled) return;
  OnMouseMove (iVKeyIn, fXIn, fYIn, fPressureIn);
  OnKeyUp (iVKeyIn);
  };

//------------------------------------------------------------------------------
VOID  InputManager::ClearButtonClicks   (VOID)
  {
  iLeftClicks     = 0;
  iRightClicks    = 0;
  iCenterClicks   = 0;
  iScrollUpClicks = 0;
  iScrollDnClicks = 0;
  };

//------------------------------------------------------------------------------
VOID  InputManager::GetPos  (FLOAT *  pfXOut,
                             FLOAT *  pfYOut,
                             UINT *   puButtonsOut,
                             FLOAT *  pfPressureOut,
                             FLOAT *  pfTimeOut)
  {
  if (!bEnabled) return;
  // called by any code to get the current mouse position
  //DBG_INFO ("GetPos Pointer idx:%d x:%f y:%f btn:%d", iQueueIndex, afX [iQueueIndex], afY [iQueueIndex], auButtons [iQueueIndex]);

  if (pfXOut        != NULL) *pfXOut        = afX         [iQueueIndex];
  if (pfYOut        != NULL) *pfYOut        = afY         [iQueueIndex];
  if (puButtonsOut  != NULL) *puButtonsOut  = auButtons   [iQueueIndex];
  if (pfPressureOut != NULL) *pfPressureOut = afPressure  [iQueueIndex];
  if (pfTimeOut     != NULL) *pfTimeOut     = afEventTime [iQueueIndex];
  };

//------------------------------------------------------------------------------
VOID  InputManager::GetLastPos  (FLOAT *  pfXOut,
                                 FLOAT *  pfYOut,
                                 UINT *   puButtonsOut,
                                 FLOAT *  pfPressureOut,
                                 FLOAT *  pfTimeOut)
  {
  if (!bEnabled) return;
  INT  iLastIndex = iQueueIndex - 1;
  if (iLastIndex < 0) iLastIndex += iQueueSize;
  if (pfXOut        != NULL) *pfXOut        = afX         [iLastIndex];
  if (pfYOut        != NULL) *pfYOut        = afY         [iLastIndex];
  if (puButtonsOut  != NULL) *puButtonsOut  = auButtons   [iLastIndex];
  if (pfPressureOut != NULL) *pfPressureOut = afPressure  [iLastIndex];
  if (pfTimeOut     != NULL) *pfTimeOut     = afEventTime [iLastIndex];
  };


//------------------------------------------------------------------------------
VOID  InputManager::GetIndexPos (INT      iIndexIn,
                                 FLOAT *  pfXOut,
                                 FLOAT *  pfYOut,
                                 UINT *   puButtonsOut,
                                 FLOAT *  pfPressureOut,
                                 FLOAT *  pfTimeOut)
  {
  if (!bEnabled) return;
  // called by any code to get the indexed mouse position

  // bring iIndexIn into the range of (0, iQueueSize]
  while (iIndexIn < 0)           iIndexIn += iQueueSize;
  while (iIndexIn >= iQueueSize) iIndexIn += iQueueSize;

  if (pfXOut        != NULL) *pfXOut        = afX         [iIndexIn];
  if (pfYOut        != NULL) *pfYOut        = afY         [iIndexIn];
  if (puButtonsOut  != NULL) *puButtonsOut  = auButtons   [iIndexIn];
  if (pfPressureOut != NULL) *pfPressureOut = afPressure  [iIndexIn];
  if (pfTimeOut     != NULL) *pfTimeOut     = afEventTime [iIndexIn];
  };


//------------------------------------------------------------------------------
VOID  InputManager::GetPosDelta (FLOAT    fTimeDeltaIn,
                                 FLOAT *  pfXOut,
                                 FLOAT *  pfYOut,
                                 FLOAT *  pfPressureOut)
  {
  if (!bEnabled) return;

  FLOAT  fXDelta = 0.0f;
  FLOAT  fYDelta = 0.0f;
  FLOAT  fPressureDelta = 0.0f;


  FLOAT  fEarliestTime = fGlobalTime - fTimeDeltaIn;

  INT  iStopIndex = iQueueIndex + 1;
  if (iStopIndex > iQueueSize) iStopIndex -= iQueueSize;

  INT  iIndex = iQueueIndex;

  FLOAT  fXPrev        = afX        [iIndex];
  FLOAT  fYPrev        = afY        [iIndex];
  FLOAT  fPressurePrev = afPressure [iIndex];

  while (iIndex != iStopIndex)
    {
    if (afEventTime [iIndex] < fEarliestTime) break;

    fXDelta        += fXPrev - afX [iIndex];
    fYDelta        += fYPrev - afY [iIndex];
    fPressureDelta += fPressurePrev - afPressure [iIndex];

    fXPrev        = afX [iIndex];
    fYPrev        = afY [iIndex];
    fPressurePrev = afPressure [iIndex];

    --iIndex;
    if (iIndex < 0) iIndex += iQueueSize;
    };

  if (pfXOut        != NULL) *pfXOut        = fXDelta;
  if (pfYOut        != NULL) *pfYOut        = fYDelta;
  if (pfPressureOut != NULL) *pfPressureOut = fPressureDelta;
  };


//------------------------------------------------------------------------------
FLOAT  InputManager::GetSlashLength (UINT  uPixelToleranceIn)
  {
  if (!bEnabled) return (0.0f);
  // The goal of this routine is to look at the historic mouse data
  //   and determine if we are slashing across the screen.

  // We do this by stepping back through the history, and breaking the
  //   history into line segments that are just over a given tolerance
  //   factor.  We count the number of line segments that are roughly
  //   parallel to their previous segment and to the final segment (within
  //   a wider tolerance factor for the later).  We also take into account
  //   button changes that would signal the end of a slash.  When
  //   we have run out of samples or have found the end of a run, we
  //   return the sum of the length of all of the line segments.  If it is
  //   0, there is no slash.  Otherwise we can judge how large of a slash
  //   it is and act appropriately.

  FLOAT   fTotalTolerance = 0.8;
  FLOAT   fLocalTolerance = 0.8;

  INT     iToleranceSq = uPixelToleranceIn * uPixelToleranceIn;
  INT     iIndex;
  FLOAT   fTotalLength = 0.0f;
  FLOAT   fCurrLength;
  RVec3   vecSlashDir;
  RVec3   vecPrevDir;
  RVec3   vecCurrDir;
  INT     iIndexStart;

  FLOAT   fFirstX;        // very tip of the slash.  Current mouse position
  FLOAT   fFirstY;
  UINT    uFirstButtons;

  FLOAT   fSegX;         // start of the current segment
  FLOAT   fSegY;
  UINT    uSegButtons;

  FLOAT     fCurrX;        // search point for the end of the segent.
  FLOAT     fCurrY;
  UINT    uCurrButtons;


  // calculate the first segment
  iIndex = iIndexStart = iQueueIndex;
  GetIndexPos (iIndexStart, &fFirstX, &fFirstY, &uFirstButtons, NULL);

  --iIndex;
  iIndex = RMin (iQueueSize - 1, RMax (0, iIndex));

  fSegX       = fFirstX;
  fSegY       = fFirstY;
  uSegButtons = uFirstButtons;

  INT   iDeltaX;
  INT   iDeltaY;
  BOOL  bEndOfSlash = FALSE;
  while (iIndex != iIndexStart)
    {
    GetIndexPos (iIndex, &fCurrX, &fCurrY, &uCurrButtons);

    // end search if button state changes.
    if (uCurrButtons != uSegButtons)
      {
      bEndOfSlash = TRUE;
      break;
      };
    iDeltaX = INT (fSegX - fCurrX);
    iDeltaY = INT (fSegY - fCurrY);
    if ((iDeltaX * iDeltaX + iDeltaY * iDeltaY) >= iToleranceSq)
      {
      // we've found a segment
      vecCurrDir = RVec3 (FLOAT (iDeltaX), FLOAT (iDeltaY), 0);
      fCurrLength = vecCurrDir.Length ();
      vecCurrDir /= fCurrLength;
      fTotalLength += fCurrLength;

      // prepare for the next run
      fSegX       = fCurrX;
      fSegY       = fCurrY;
      uSegButtons = uCurrButtons;
      break;
      };
    --iIndex;
    iIndex = RMin (iQueueSize - 1, RMax (0, iIndex));
    };
  vecSlashDir = vecCurrDir;
  vecPrevDir  = vecCurrDir;

  if (bEndOfSlash) {return (0);};

  // now search out the other segments
  while (iIndex != iIndexStart)
    {
    GetIndexPos (iIndex, &fCurrX, &fCurrY, &uCurrButtons);

    // end search if button state changes.
    if (uCurrButtons != uSegButtons)
      {
      //bEndOfSlash = TRUE;
      break;
      };
    iDeltaX = INT (fSegX - fCurrX);
    iDeltaY = INT (fSegY - fCurrY);
    if ((iDeltaX * iDeltaX + iDeltaY * iDeltaY) >= iToleranceSq)
      {
      // we've found a segment
      vecCurrDir = RVec3 (FLOAT (iDeltaX), FLOAT (iDeltaY), 0);
      fCurrLength = vecCurrDir.Length ();
      vecCurrDir /= fCurrLength;

      // keep or reject this segment
      if (((vecPrevDir * vecCurrDir)  < fLocalTolerance) ||
          ((vecPrevDir * vecSlashDir) < fTotalTolerance))
        {
        // too bendy.  Reject.
        //bEndOfSlash = TRUE;
        break;
        };
      fTotalLength += fCurrLength;
      break;
      };
    --iIndex;
    iIndex = RMin (iQueueSize - 1, RMax (0, iIndex));
    };
  return (INT (fTotalLength));
  };

//------------------------------------------------------------------------------
VOID InputManager::GetKeyName (INT32   iVirtKeyIn,
                               RStr &  strNameOut)
  {
  if (!bEnabled) return;
  strNameOut.Empty ();

  if (bKeyDown [VKEY_ALT])     {strNameOut += "Alt";};
  if (bKeyDown [VKEY_CONTROL]) {strNameOut += "Ctrl";};
  if (bKeyDown [VKEY_META])    {strNameOut += "Meta";};
  if (bKeyDown [VKEY_SHIFT])   {strNameOut += "Shft";};

  // REFACTOR: This code could be cleaned up by having an array of structs
  //  with the VKEY and string, and then matching via a loop.  This array
  //  could be used in both GetKeyName () and NameToVKey ();

  if ((iVirtKeyIn >= 32) && (iVirtKeyIn <= 126))
    {
    strNameOut.AppendChar (iVirtKeyIn);
    }
  else
    {
    INT  iIndex = arrayMappings.FindIndexByValue (iVirtKeyIn);
    if (iIndex != -1)
      {
      RStr  strKey = arrayMappings.KeyAtIndex (iIndex);
      strNameOut += strKey;
      };
    };
  };


//------------------------------------------------------------------------------
VOID InputManager::NameToVKey (const char *  szNameIn,
                               INT32 &       iVirtKeyOut,
                               INT32 &       iModOut)
  {
  iVirtKeyOut = VKEY_INVALID;
  iModOut     = 0;

  RStrParser  parserName (szNameIn);

  if (parserName.StartsWith ("Alt"))  {iModOut |= VKFLG_ALT; parserName.SkipChars (strlen ("Alt"));};
  if (parserName.StartsWith ("Ctrl")) {iModOut |= VKFLG_CONTROL; parserName.SkipChars (strlen ("Ctrl"));};
  if (parserName.StartsWith ("Meta")) {iModOut |= VKFLG_META; parserName.SkipChars (strlen ("Meta"));};
  if (parserName.StartsWith ("Shft")) {iModOut |= VKFLG_SHIFT; parserName.SkipChars (strlen ("Shft"));};

  if (parserName.GetLineLength () == 1)
    {
    // single character
    iVirtKeyOut = parserName.GetChar ();
    }
  else
    {
    iVirtKeyOut = arrayMappings [parserName.AsChar ()];
    };
  };


//------------------------------------------------------------------------------
VOID  InputManager::CalcNDC (VOID)
  {
  // Normalized Device Coordinates (NDC) have X and Y ranging from 0.0 to 1.0.
  //   The longer axis is 0.0 to 1.0, and the shorter axis is 0.0 to <= 1.0.
  //   The ratios give square pixels.

  if (FLT_APPROX_EQUAL (fScreenWidth, 0.0f) || FLT_APPROX_EQUAL (fScreenHeight, 0.0f))
    {
    return;
    }

  if (fScreenWidth > fScreenHeight)
    {
    // width is the larger axis
    fNDCX = 1.0f;
    fNDCY = fScreenHeight / fScreenWidth;
    }
  else
    {
    // Height is the larger axis
    fNDCX = fScreenWidth / fScreenHeight;
    fNDCY = 1.0f;
    };
  };

//------------------------------------------------------------------------------
VOID  InputManager::DebugPrint (const char *  szSourceIn)
  {
  DBG_INFO ("%s Results: ReadPost %d  (%x) WritePost %d (%x)", szSourceIn, iKeyRead, aiKeyEntered[iKeyRead], iKeyWrite, aiKeyEntered[iKeyWrite]);
  for (INT  iIndex = 0; iIndex < KEY_BUFFER_SIZE; ++iIndex)
    {
    DBG_INFO ("%d : %x", iIndex, aiKeyEntered[iIndex]);
    };
  };


#ifdef SAMPLE

//------------------------------------------------------------------------------
VOID  RHotKeys::MouseToCameraVector (RNode *  pnodeCameraIn,
                                     FLOAT    fMouseXIn,   // viewport parametric position
                                     FLOAT    fMouseYIn)   // viewport parametric position
  {
  RNode *  pnodeHotkeys = GetHotkeyList ();

  if (pnodeCameraIn)
    {
    RAttr *  pattrCamera = pnodeHotkeys->GetAttr ("camera");
    RNodePtr  ptrCamera;
    pnodeCameraIn->SetSmartPtr (ptrCamera);

    RAttr *  pattrCursorVec     = pnodeHotkeys->GetAttr ("cursorVec");
    if (pattrCursorVec == NULL) return;
    if (pattrCamera != NULL) {pattrCamera->SetNodePtr (ptrCamera);};


    FLOAT    fFocalLength = pnodeCameraIn->GetAttrFloat ("fl");
    //FLOAT    fApertureX   = pnodeCameraIn->GetAttrFloat ("apx");
    //FLOAT    fApertureY   = pnodeCameraIn->GetAttrFloat ("apy");

    //FLOAT    fFovY = atanf ((fApertureY * 0.5f) / fFocalLength);
    //FLOAT    fApertureAspect = fApertureX / fApertureY;

    // Mouse position should be passed in the range of 0 to 1,

    // calculate the point on the projection plate where the mouse was clicked.
    //  This will give us a vector in camera space when we subtract the camera
    //  position.
    RVec4  vecPointOnAperture (fMouseXIn,
                               fMouseYIn,
                               fFocalLength * -1.0f,
                               1.0f);

    pattrCursorVec->SetVector (vecPointOnAperture);

    //start here

    // take x, y, camera FOV, and calculate the vector in camera space through that point.
    //  store/pass this vector

    // in routine:
    //  calc where old camera vector crossed plane at center of interest
    //  calc where new camera vector crosses plane at center of interest
    //  The vector between these two points is the amount to move.
    //  apply tranformations.
    };

  };

#endif // SAMPLE


 VOID InputManager::GetHMDXFormDelta  (RMatrix &  matOut)
 {
 matOut = matHMDXFormDelta;

 //RVec3 vec;
 //matOut.GetRotateY (vec);
 //DBG_INFO ("HMD XForm Delta Y %f %f %f", vec.fX, vec.fY, vec.fZ);

 };

