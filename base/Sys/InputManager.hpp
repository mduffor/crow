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

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "Sys/Types.hpp"
#include "Containers/IntArray.hpp"
#include "Containers/FloatArray.hpp"
#include "Containers/KVPIntArray.hpp"
#include "Util/RStr.hpp"
#include "Math/RMatrix.hpp"
/**
  @addtogroup base
  @{
*/
#define VKEY_INVALID          0x0000
#define VKEY_SPACE            0x0020
#define VKEY_ESC              0x0100
#define VKEY_BACKSPACE        0x0101
#define VKEY_TAB              0x0102
#define VKEY_BACKTAB          0x0103
#define VKEY_PAUSE            0x0104
#define VKEY_PAGEUP           0x0105
#define VKEY_PAGEDOWN         0x0106
#define VKEY_END              0x0107
#define VKEY_HOME             0x0108
#define VKEY_LEFT             0x0109
#define VKEY_UP               0x010a
#define VKEY_RIGHT            0x010b
#define VKEY_DOWN             0x010c
#define VKEY_INSERT           0x010d
#define VKEY_DELETE           0x010e
#define VKEY_ENTER            0x010f
#define VKEY_F1               0x0110
#define VKEY_F2               0x0111
#define VKEY_F3               0x0112
#define VKEY_F4               0x0113
#define VKEY_F5               0x0114
#define VKEY_F6               0x0115
#define VKEY_F7               0x0116
#define VKEY_F8               0x0117
#define VKEY_F9               0x0118
#define VKEY_F10              0x0119
#define VKEY_F11              0x011a
#define VKEY_F12              0x011b
#define VKEY_F13              0x011c
#define VKEY_F14              0x011d
#define VKEY_F15              0x011e
#define VKEY_CONTROL          0x011f
#define VKEY_SHIFT            0x0120
#define VKEY_ALT              0x0121
#define VKEY_META             0x0122

#define VKEY_MENU             0x0123
#define VKEY_CAPITAL          0x0124
#define VKEY_HELP             0x0125
#define VKEY_LBUTTON          0x0126
#define VKEY_RBUTTON          0x0127
#define VKEY_MBUTTON          0x0128
#define VKEY_NUMPAD0          0x0129
#define VKEY_NUMPAD1          0x012a
#define VKEY_NUMPAD2          0x012b
#define VKEY_NUMPAD3          0x012c
#define VKEY_NUMPAD4          0x012d
#define VKEY_NUMPAD5          0x012f
#define VKEY_NUMPAD6          0x0130
#define VKEY_NUMPAD7          0x0131
#define VKEY_NUMPAD8          0x0132
#define VKEY_NUMPAD9          0x0133
#define VKEY_NUMPAD_SPACE     0x0134
#define VKEY_NUMPAD_TAB       0x0135
#define VKEY_NUMPAD_ENTER     0x0136
#define VKEY_NUMPAD_F1        0x0137
#define VKEY_NUMPAD_F2        0x0138
#define VKEY_NUMPAD_F3        0x0139
#define VKEY_NUMPAD_F4        0x013a
#define VKEY_NUMPAD_HOME      0x013b
#define VKEY_NUMPAD_LEFT      0x013c
#define VKEY_NUMPAD_UP        0x013d
#define VKEY_NUMPAD_RIGHT     0x013e
#define VKEY_NUMPAD_DOWN      0x013f

//#define VKEY_NUMPAD_PRIOR     0x0140
#define VKEY_NUMPAD_PAGEUP     0x0141
//#define VKEY_NUMPAD_NEXT      0x0142
#define VKEY_NUMPAD_PAGEDOWN   0x0143
#define VKEY_NUMPAD_END        0x0144
#define VKEY_NUMPAD_BEGIN      0x0145
#define VKEY_NUMPAD_INSERT     0x0146
#define VKEY_NUMPAD_DELETE     0x0147
#define VKEY_NUMPAD_EQUAL      0x0148
#define VKEY_NUMPAD_MULTIPLY   0x0149
#define VKEY_NUMPAD_ADD        0x014a
#define VKEY_NUMPAD_SEPARATOR  0x014b
#define VKEY_NUMPAD_SUBTRACT   0x014c
#define VKEY_NUMPAD_DECIMAL    0x014d
#define VKEY_NUMPAD_DIVIDE     0x014e

#define VKEY_MULTIPLY          0x014f
#define VKEY_ADD               0x0150
#define VKEY_SUBTRACT          0x0151
#define VKEY_DECIMAL           0x0152
#define VKEY_DIVIDE            0x0153
#define VKEY_NUMLOCK           0x0154
#define VKEY_SCROLL            0x0155
#define VKEY_SEPARATOR         0x0156

#define GAMEPAD_BUTTON_1       0x0157
#define GAMEPAD_BUTTON_2       0x0158
#define GAMEPAD_BUTTON_3       0x0159
#define GAMEPAD_BUTTON_4       0x015a
#define GAMEPAD_BUTTON_5       0x015b
#define GAMEPAD_BUTTON_6       0x015c
#define GAMEPAD_BUTTON_7       0x015d
#define GAMEPAD_BUTTON_8       0x015e
#define GAMEPAD_BUTTON_9       0x015f
#define GAMEPAD_BUTTON_10      0x0160
#define GAMEPAD_BUTTON_11      0x0161
#define GAMEPAD_BUTTON_12      0x0162
#define GAMEPAD_BUTTON_13      0x0163
#define GAMEPAD_BUTTON_14      0x0164
#define GAMEPAD_BUTTON_15      0x0165
#define GAMEPAD_BUTTON_16      0x0166

#define BUTTON_A               0x0167
#define BUTTON_B               0x0168
#define BUTTON_C               0x0169
#define BUTTON_X               0x016a
#define BUTTON_Y               0x016b
#define BUTTON_Z               0x016c

#define BUTTON_L1              0x016d
#define BUTTON_L2              0x016e
#define BUTTON_R1              0x016f
#define BUTTON_R2              0x0170
#define BUTTON_THUMBL          0x0171
#define BUTTON_THUMBR          0x0172

#define BUTTON_MODE            0x0173
#define BUTTON_SELECT          0x0174
#define BUTTON_START           0x0175

#define DPAD_CENTER            0x0176
#define DPAD_DOWN              0x0177
#define DPAD_LEFT              0x0179
#define DPAD_RIGHT             0x017a
#define DPAD_UP                0x017b

#define VKEY_SCROLLUP          0x017c
#define VKEY_SCROLLDOWN        0x017d
#define VKEY_TOUCH_0           0x017e
#define VKEY_TOUCH_1           0x017f
#define VKEY_TOUCH_2           0x0180
#define VKEY_TOUCH_3           0x0181
#define VKEY_TOUCH_4           0x0182

#define VKEY_MAX               0x0182

#define VKFLG_SHIFT          0x8000
#define VKFLG_CONTROL        0x4000
#define VKFLG_ALT            0x2000
#define VKFLG_META           0x1000
#define VKFLG_RELEASE        0x0800
#define VKFLG_MASK           0xf800

#define CTRL_A              (0x0001 | VKFLG_CONTROL)
#define CTRL_B              (0x0002 | VKFLG_CONTROL)
#define CTRL_C              (0x0003 | VKFLG_CONTROL)
#define CTRL_D              (0x0004 | VKFLG_CONTROL)
#define CTRL_E              (0x0005 | VKFLG_CONTROL)
#define CTRL_F              (0x0006 | VKFLG_CONTROL)
#define CTRL_G              (0x0007 | VKFLG_CONTROL)
#define CTRL_H              (0x0008 | VKFLG_CONTROL)
#define CTRL_I              (0x0009 | VKFLG_CONTROL)
#define CTRL_J              (0x000a | VKFLG_CONTROL)
#define CTRL_K              (0x000b | VKFLG_CONTROL)
#define CTRL_L              (0x000c | VKFLG_CONTROL)
#define CTRL_M              (0x000d | VKFLG_CONTROL)
#define CTRL_N              (0x000e | VKFLG_CONTROL)
#define CTRL_O              (0x000f | VKFLG_CONTROL)
#define CTRL_P              (0x0010 | VKFLG_CONTROL)
#define CTRL_Q              (0x0011 | VKFLG_CONTROL)
#define CTRL_R              (0x0012 | VKFLG_CONTROL)
#define CTRL_S              (0x0013 | VKFLG_CONTROL)
#define CTRL_T              (0x0014 | VKFLG_CONTROL)
#define CTRL_U              (0x0015 | VKFLG_CONTROL)
#define CTRL_V              (0x0016 | VKFLG_CONTROL)
#define CTRL_W              (0x0017 | VKFLG_CONTROL)
#define CTRL_X              (0x0018 | VKFLG_CONTROL)
#define CTRL_Y              (0x0019 | VKFLG_CONTROL)
#define CTRL_Z              (0x001a | VKFLG_CONTROL)

#define KEY_BUFFER_SIZE     20

//-----------------------------------------------------------------------------
class InputManager
  {
  private:


    INT  iMouseLastX;      ///< useable by the active widget.  Not used by the context.
    INT  iMouseLastY;      ///< useable by the active widget.  Not used by the context.

    FLOAT  fGlobalTime;    ///< Global time value for determining when input events occurred.

    // keyboard status
    BOOL bShiftDown;
    BOOL bControlDown;
    BOOL bAltDown;
    BOOL bMetaDown;

    KVPIntArray   arrayMappings;

    INT  iKeyRead;     ///< read index for the KeyEntered/KeyMode arrays
    INT  iKeyWrite;    ///< write index for the KeyEntered/KeyMode arrays
    INT  aiKeyEntered [KEY_BUFFER_SIZE];  ///< Circular buffer of stored key presses
    INT  aiKeyMod     [KEY_BUFFER_SIZE];  ///< Circular buffer of stored key modifiers

    BOOL bKeyDown     [VKEY_MAX+1];  ///< Array for tracking if a key is currently pressed.

    // mouse status (may need to expand into an array for multiple touches)
    INT                   iQueueSize;  ///< max number of mouse events in the mouse state queue
    INT                   iQueueIndex; ///< top of the mouse state queue
    IntArray              auButtons;   ///< button states and keyboard flags at time of mouse event (shift, control, alt, meta)
    FloatArray            afX;         ///< X-position of the mouse event.  Given in app-window pixel coordinates (top left origin).
    FloatArray            afY;         ///< Y-position of the mouse event.  Given in app-window pixel coordinates (top left origin).
    FloatArray            afPressure;  ///< Tablet stylus pressure at the time of the mouse event 0.0 - 1.0
    FloatArray            afEventTime; ///< Tablet stylus pressure at the time of the mouse event 0.0 - 1.0

    // mouse buttons
    INT                   iLeftClicks;
    INT                   iRightClicks;
    INT                   iCenterClicks;
    INT                   iScrollUpClicks;
    INT                   iScrollDnClicks;

    FLOAT                 fScreenWidth;  ///< In pixels
    FLOAT                 fScreenHeight; ///< In pixels

    FLOAT                 fNDCX;         ///< Normalized Device Coordinates.  Ratio to multiply X axis.
    FLOAT                 fNDCY;         ///< Normalized Device Coordinates.  Ratio to multiply Y axis.

    BOOL                  bEnabled;

    // axis status
    // TODO:  See if mouse input can be generalized well into axis tracking

    static RMatrix      matHMDXForm;       // World space location and orientation of HMD, with delta already applied.
    static RMatrix      matHMDXFormDelta;  // Adjusts the HMD direction and position from its "home" space.

    static float        AXIS_THRESHOLD;    ///< threshold under which axis is considered to be zeroed.
    FloatArray          afAxis;            ///< Array of normalized axis values (-1.0 to 1.0).  Index is OS specific

    INT                 iAxisLeftStickX;   ///< Index to use for left joystick, x-axis.  Eventually user configured.
    INT                 iAxisLeftStickY;

    INT                 iAxisRightStickX;
    INT                 iAxisRightStickY;

    INT                 iAxisLeftTrigger;
    INT                 iAxisRightTrigger;


  public:

          InputManager        ();
          ~InputManager       ();

    VOID  Enable              (BOOL  bEnabledIn)    {bEnabled = bEnabledIn;};

    BOOL  GetNextKey          (INT &  iKeyOut,
                               INT &  iKeyModOut);

    BOOL  GetKey              (INT    iSearchKeyIn,
                               INT &  iKeyModOut);

    VOID  StoreKey            (INT  iKeyIn,
                               INT  iKeyModIn);

    BOOL  IsKeyDown           (INT  iKeyIn)     {if (iKeyIn <= VKEY_MAX) return bKeyDown [iKeyIn]; return 0;};

    FLOAT GetGlobalTime       (VOID)            {return fGlobalTime;}; ///< NOTE: May lose precision over time, since stored as float.

    VOID  SetGlobalTime       (FLOAT  fTimeIn)  {fGlobalTime = fTimeIn;};

    VOID  PushMousePos        (FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn = 0.0f);

    VOID  OnKeyDown           (INT  iVKeyIn);

    VOID  OnKeyUp             (INT  iVKeyIn);

    VOID  OnMouseMove         (INT    iVKeyIn,
                               FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn);

    VOID  OnMouseDown         (INT    iVKeyIn,
                               FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn);

    VOID  OnMouseUp           (INT    iVKeyIn,
                               FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn);

    VOID  ClearButtonClicks   (VOID);

    VOID  GetPos              (FLOAT *  pfXOut,
                               FLOAT *  pfYOut,
                               UINT *   puButtonsOut,
                               FLOAT *  pfPressureOut = NULL,
                               FLOAT *  pfTimeOut = NULL);

    VOID  GetLastPos          (FLOAT *  pfXOut,
                               FLOAT *  pfYOut,
                               UINT *   puButtonsOut,
                               FLOAT *  pfPressureOut = NULL,
                               FLOAT *  pfTimeOut = NULL);

    VOID  GetIndexPos         (INT      iIndexIn,
                               FLOAT *  pfXOut,
                               FLOAT *  pfYOut,
                               UINT *   puButtonsOut,
                               FLOAT *  pfPressureOut = NULL,
                               FLOAT *  pfTimeOut = NULL);

    VOID   GetPosDelta        (FLOAT    fTimeDeltaIn,
                               FLOAT *  pfXOut,           // app-window pixel coordinates
                               FLOAT *  pfYOut,           // app-window pixel coordinates
                               FLOAT *  pfPressureOut);

    FLOAT  GetSlashLength     (UINT  uPixelToleranceIn);

    VOID   GetKeyName         (INT32   iVirtKeyIn,
                               RStr &  strNameOut);

    VOID   NameToVKey         (const char *  szNameIn,
                               INT32 &       iVirtKeyOut,
                               INT32 &       iModOut);

    VOID   SetScreenWidth     (FLOAT  fWidthIn)   {fScreenWidth  = RMax (1.0f, fWidthIn); CalcNDC ();};

    VOID   SetScreenHeight    (FLOAT  fHeightIn)  {fScreenHeight = RMax (1.0f, fHeightIn); CalcNDC ();};

    FLOAT  GetScreenWidth     (VOID)              {return fScreenWidth;};

    FLOAT  GetScreenHeight    (VOID)              {return fScreenHeight;};

    VOID   CalcNDC            (VOID);

    FLOAT  GetNDCX            (VOID)              {return fNDCX;};

    FLOAT  GetNDCY            (VOID)              {return fNDCY;};

    VOID   SetHMDXForm        (const RMatrix &  matIn)     {matHMDXForm = matIn;};

    VOID   GetHMDXForm        (RMatrix &  matOut)          {matOut = matHMDXForm;};

    VOID   SetHMDXFormDelta   (const RMatrix &  matIn)     {matHMDXFormDelta = matIn;};

    VOID   GetHMDXFormDelta   (RMatrix &  matOut);//    {matOut = matHMDXFormDelta;};

    VOID   SetNumAxis               (INT    iCountIn)  {afAxis.SetLength (iCountIn); afAxis.InitValues (0, iCountIn, 0.0f);};

                                     // clamp for a flat area in the center
    VOID   SetAxis                  (INT    iIndexIn,
                                     FLOAT  fValueIn)  {afAxis [iIndexIn] = (fValueIn >= -AXIS_THRESHOLD) ? ((fValueIn <= AXIS_THRESHOLD) ? 0.0f: fValueIn) : fValueIn;};

    FLOAT  GetAxis                  (INT    iIndexIn)  {return (afAxis [iIndexIn]);};

    VOID   SetAxisIndexLeftStickX   (INT    iIndexIn)  {iAxisLeftStickX = iIndexIn;};
    VOID   SetAxisIndexLeftStickY   (INT    iIndexIn)  {iAxisLeftStickY = iIndexIn;};

    VOID   SetAxisIndexRightStickX  (INT    iIndexIn)  {iAxisRightStickX = iIndexIn;};
    VOID   SetAxisIndexRightStickY  (INT    iIndexIn)  {iAxisRightStickY = iIndexIn;};

    VOID   SetAxisIndexLeftTrigger  (INT    iIndexIn)  {iAxisLeftTrigger = iIndexIn;};
    VOID   SetAxisIndexRightTrigger (INT    iIndexIn)  {iAxisRightTrigger = iIndexIn;};

    FLOAT  GetAxisLeftStickX        (VOID)             {return GetAxis (iAxisLeftStickX);};
    FLOAT  GetAxisLeftStickY        (VOID)             {return GetAxis (iAxisLeftStickY);};

    FLOAT  GetAxisRightStickX       (VOID)             {return GetAxis (iAxisRightStickX);};
    FLOAT  GetAxisRightStickY       (VOID)             {return GetAxis (iAxisRightStickY);};

    FLOAT  GetAxisLeftTrigger       (VOID)             {return GetAxis (iAxisLeftTrigger);};
    FLOAT  GetAxisRightTrigger      (VOID)             {return GetAxis (iAxisRightTrigger);};

    VOID   DebugPrint               (const char *  szSourceIn);

  };

/** @} */ // end of base group



#endif // INPUTMANAGER_HPP
