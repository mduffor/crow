#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/InputManager.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (InputManager, Keyboard)
  {
  InputManager  input;
    
  input.Enable (TRUE);
  
  INT  iKey = 0;
  INT  iKeyMod = 0;
  
  ASSERT_FALSE (input.GetNextKey (iKey, iKeyMod));

  // press a key  (thus storing it.  This tests StoreKey())
  input.OnKeyDown ('a');
  ASSERT_TRUE (input.IsKeyDown ('a'));
  
  // release a key
  input.OnKeyUp ('a');
  ASSERT_FALSE (input.IsKeyDown ('a'));
  
  // read a stored key
  ASSERT_TRUE (input.GetNextKey (iKey, iKeyMod));
  ASSERT_EQ (iKey, 'a');
  ASSERT_EQ (iKeyMod, 0);

  // make sure all the keys were read
  ASSERT_FALSE (input.GetNextKey (iKey, iKeyMod));

  // test key searching  
  ASSERT_FALSE (input.GetNextKey (iKey, iKeyMod));
  
  input.OnKeyDown ('b');
  input.OnKeyUp ('b');
  
  ASSERT_FALSE (input.GetKey ('a', iKeyMod));
  ASSERT_TRUE  (input.GetKey ('b', iKeyMod));
  ASSERT_FALSE (input.GetKey ('b', iKeyMod));

  // test key name
  RStr  strKeyName;
  
  input.GetKeyName (VKEY_ENTER, strKeyName);
  ASSERT_STREQ (strKeyName.AsChar(), "Enter");

  input.OnKeyDown (VKEY_CONTROL);
  input.GetKeyName (VKEY_ENTER, strKeyName);
  ASSERT_STREQ (strKeyName.AsChar(), "CtrlEnter");
  input.OnKeyUp (VKEY_CONTROL);
  };
  
  
    /*
    // TODO: The below still need to be tested
    
    VOID  PushMousePos        (FLOAT  fXIn,
                               FLOAT  fYIn,
                               FLOAT  fPressureIn = 0.0f);
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
                               FLOAT *  pfPressureOut = NULL);

    VOID  GetLastPos          (FLOAT *  pfXOut,
                               FLOAT *  pfYOut,
                               UINT *   puButtonsOut,
                               FLOAT *  pfPressureOut = NULL);

    VOID  GetIndexPos         (INT      iIndexIn,
                               FLOAT *  pfXOut,
                               FLOAT *  pfYOut,
                               UINT *   puButtonsOut,
                               FLOAT *  pfPressureOut = NULL);

    FLOAT  GetSlashLength     (UINT  uPixelToleranceIn);

    VOID   SetScreenWidth     (FLOAT  fWidthIn)   {fScreenWidth  = fWidthIn;};

    VOID   SetScreenHeight    (FLOAT  fHeightIn)  {fScreenHeight = fHeightIn;};

    FLOAT  GetScreenWidth     (VOID)              {return fScreenWidth;};

    FLOAT  GetScreenHeight    (VOID)              {return fScreenHeight;};

    VOID   SetHMDXForm        (RMatrix &  matIn)     {matHMDXForm = matIn;};

    VOID   GetHMDXForm        (RMatrix &  matOut)    {matOut = matHMDXForm;};

    VOID   SetHMDXFormDelta   (RMatrix &  matIn)     {matHMDXFormDelta = matIn;};

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
     * */
