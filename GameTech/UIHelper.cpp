#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Composite/Node.hpp"
#include "Sys/Shell.hpp"
//#include "Util/Signal.h"
#include "UIHelper.hpp"
//#include "Script/Expression.hpp"
//#include "Net/Analytics.hpp"
#include "Composite/ComponentDefaultLooper.hpp"
#include "GameTech/DialogComponent.hpp"

UIHelper *      UIHelper::pInstance = NULL;


//-----------------------------------------------------------------------------
UIHelper::UIHelper ()
  {
  pReg = ValueRegistry::Root ();
  ASSERT (pReg != NULL);


//  pReg->SetString ("UI.Screen", "");
//  ValueElem *  pElem = pReg->Find (CalcHashValue ("UI.Screen"));
//  if (pElem != NULL)
//    {
//    pElem->sigOnChanged.Connect (this, &UIHelper::OnScreenChangedListener);
//    };

//  UIHelper::Instance()->SetScreen ("ModeSelect");

  };

//-----------------------------------------------------------------------------
UIHelper::~UIHelper ()
  {
  // clear the static instance pointer if we are the static instance.
  if (pInstance == this)
    {
    pInstance = NULL;
    };
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::TransitionPopScreen  (VOID)
  {
  // Pop the top screen in the stack into the active one.

  INT   iArraySize = pReg->GetArrayLength ("UI.Screen.Stack");

  if (iArraySize > 0)
    {
    pReg->SetString ("UI.Screen", pReg->GetStringInArray ("UI.Screen.Stack", iArraySize - 1));
    pReg->DeleteArrayElem ("UI.Screen.Stack", -1); // delete last elem
    };
  //pReg->SetBool  ("UI.Screen.Transition", TRUE);
  //pReg->SetInt   ("UI.Screen.FaderAnimTo", 0);
  //pReg->SetFloat ("UI.Screen.FaderReset", 1.0f);
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::PopScreen  (VOID)
  {
  // Pop the top screen in the stack into the active one.

  INT   iArraySize = pReg->GetArrayLength ("UI.Screen.Stack");
  RStr  strNewScreen;

  if (iArraySize > 0)
    {
    strNewScreen = pReg->GetStringInArray ("UI.Screen.Stack", iArraySize - 1);
    pReg->DeleteArrayElem ("UI.Screen.Stack", -1); // delete last elem
    ShowScreen  (strNewScreen.AsChar ());
    };
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::PushScreen  (const char *  szScreenNameIn)
  {
  // Push current screen onto stack, and replace it.
  if (! strempty (pReg->GetString ("UI.Screen")))
    {
    pReg->SetStringInArray ("UI.Screen.Stack", pReg->GetString ("UI.Screen"), -1);
    };
  ShowScreen (szScreenNameIn);
  };


//-----------------------------------------------------------------------------
VOID  UIHelper::QueueScreen  (const char *  szScreenNameIn)
  {
  // Push given screen to stack, leaving existing screen as-is
  pReg->SetStringInArray ("UI.Screen.Stack", szScreenNameIn, -1);
  };


//-----------------------------------------------------------------------------
VOID  UIHelper::ShowScreen  (const char *  szScreenNameIn)
  {

  // NOTE: The UI.Screen.Transition code, along with FaderAnimTo and FaderReset
  //   were part of the HMan approach to fade in/ fade out.  This has been
  //   replaced by Tweens and the WIndowBaseComponent / ScreenComponent system.
  //   I need to remove the old way once I'm sure the new way is working.

  pReg->SetString ("UI.Screen", szScreenNameIn);


  //pReg->SetStringInArray ("UI.Screen.Stack", szScreenNameIn, -1);
  //pReg->SetBool  ("UI.Screen.Transition", TRUE);
  //pReg->SetInt   ("UI.Screen.FaderAnimTo", 1);
  //pReg->SetFloat ("UI.Screen.FaderReset", 0.0f);
  };


//-----------------------------------------------------------------------------
VOID  UIHelper::PlayIntro (Node *  pnodeIn)
  {
  // first, make sure the node is active
  pnodeIn->SetActive (TRUE);

  // Call OnShowing to start tweens.

  ComponentDefaultLooper looper;
  looper.IterateNodes (ComponentDefaultLooper::kEvent,
                       pnodeIn,
                       NULL,
                       HASH("OnIntro"),
                       1);  // Only iterate this node
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::PlayOutro (Node *  pnodeIn)
  {
  // Call OnOutro to start tweens.

  ComponentDefaultLooper looper;
  looper.IterateNodes (ComponentDefaultLooper::kEvent,
                       pnodeIn,
                       NULL,
                       HASH("OnOutro"),
                       1);  // Only iterate this node
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::PlayIdle (Node *  pnodeIn)
  {
  // Call OnIdle to start idle tweens.

  ComponentDefaultLooper looper;
  looper.IterateNodes (ComponentDefaultLooper::kEvent,
                       pnodeIn,
                       NULL,
                       HASH("OnIdle"),
                       1);  // Only iterate this node
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::PlayHidden (Node *  pnodeIn)
  {
  // Call OnHidden to stop tweens.

  ComponentDefaultLooper looper;
  looper.IterateNodes (ComponentDefaultLooper::kEvent,
                       pnodeIn,
                       NULL,
                       HASH("OnHidden"),
                       1);  // Only iterate this node
  };


//-----------------------------------------------------------------------------
VOID  UIHelper::ShowDialog (const char *  szDialogStateKeyIn)
  {
  // TODO

  // Set the DialogState variable
  pReg->SetBool (szDialogStateKeyIn, TRUE);
  };

//-----------------------------------------------------------------------------
VOID  UIHelper::HideAllDialogs (VOID)
  {
  INT  iNumDialogs = pReg->GetArrayLength (szVisibleDialogsKey);

  for (INT  iIndex = 0; iIndex < iNumDialogs; ++iIndex)
    {
    const char *  szDialogStateKey = pReg->GetStringInArray (szVisibleDialogsKey, iIndex);

    pReg->SetBool (szDialogStateKey, FALSE);
    };

  pReg->ClearArray (szVisibleDialogsKey);
  };






