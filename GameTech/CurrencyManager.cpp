/* -----------------------------------------------------------------
                         Currency Manager

     This module implements a manager to track currencies (player-owned
   variables that can increase or decrease)

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "CurrencyManager.hpp"

CurrencyManager*  CurrencyManager::pInstance = NULL;

//-----------------------------------------------------------------------------
CurrencyManager::CurrencyManager ()
  {
  pReg = NULL;
  bNeedToSave = FALSE;
  strRegistryPrefix.Set ("Currency.");
  };

//-----------------------------------------------------------------------------
CurrencyManager::~CurrencyManager ()
  {
  for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listCurrencies.Empty ();
  };

//-----------------------------------------------------------------------------
INT  CurrencyManager::Get (UINT32  ccCurrencyTypeIn)
  {
  for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ccType == ccCurrencyTypeIn)
      {
      return ((*itrCurr)->iAmount);
      }
    }
  // didn't find it
  return (0);
  };

//-----------------------------------------------------------------------------
INT  CurrencyManager::GetAvailable (UINT32  ccCurrencyTypeIn)
  {
  for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ccType == ccCurrencyTypeIn)
      {
      return ((*itrCurr)->iAmount - (*itrCurr)->iReserved);
      }
    }
  // didn't find it
  return (0);
  };

//-----------------------------------------------------------------------------
VOID  CurrencyManager::SetKey (UINT32        ccCurrencyTypeIn,
                               const char *  szKeyIn)
  {
  for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ccType == ccCurrencyTypeIn)
      {
      (*itrCurr)->strKey.Set (szKeyIn, TRUE);
      }
    }
  // didn't find it. error.
  };


//-----------------------------------------------------------------------------
INT  CurrencyManager::Set (UINT32        ccCurrencyTypeIn,
                           INT           iAmountIn,
                           const char *  szKeyIn,
                           INT           iEscrowIn)
  {
  Currency *           pCurr = NULL;
  TListItr<Currency*>  itrCurr;

  for (itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ccType == ccCurrencyTypeIn)
      {
      pCurr = (*itrCurr);
      // change existing amount
      //DBG_INFO ("currency Set %d now is %d for Currency with key %s (%s)", pCurr->iAmount, iAmountIn, pCurr->strKey.AsChar(), szKeyIn);

      pCurr->iAmount = iAmountIn;
      if (iEscrowIn >= 0)
        {
        pCurr->iEscrow = iEscrowIn;
        };
      break;
      }
    }
  if (!itrCurr.IsValid ())
    {
    // didn't find it.  Add the currency type.
    pCurr = listCurrencies.PushBack (new Currency (ccCurrencyTypeIn, iAmountIn));
    if (szKeyIn != NULL)
      {
      pCurr->strKey.Set (szKeyIn, TRUE);
      };
    if (iEscrowIn >= 0)
      {
      pCurr->iEscrow = iEscrowIn;
      };
    pCurr->bInfinite = FALSE;
    pCurr->iBonus    = 0;

    //DBG_INFO ("Setting currency amount to %d res %d for Currency with key %s (%s)", pCurr->iAmount, pCurr->iReserved, pCurr->strKey.AsChar(), szKeyIn);
    };

  UpdateRegistry (ccCurrencyTypeIn, FALSE);

  return ((pCurr == NULL) ? 0 : pCurr->iAmount);
  };

//-----------------------------------------------------------------------------
INT  CurrencyManager::Add (UINT32        ccCurrencyTypeIn,
                           INT           iAmountIn,
                           const char *  szKeyIn,
                           INT           iEscrowIn)
  {
  // NOTE: Currently only used in unit tests?
  return (Set (ccCurrencyTypeIn, Get (ccCurrencyTypeIn) + iAmountIn, szKeyIn, iEscrowIn));
  };



//-----------------------------------------------------------------------------
VOID CurrencyManager::Serialize (RStrParser &  parserOut)
  {

  parserOut.SetU4_LEnd (0x01); // version, just in case


  INT  iNumCurrencies = listCurrencies.Size ();
  parserOut.SetU4_LEnd (iNumCurrencies);

  for (TListItr<Currency*>  itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    DBG_INFO ("Saving Currency %c%c%c%c : %d",
        (UINT32)(*itrCurr)->ccType >> 0 & 0x00ff,
        (UINT32)(*itrCurr)->ccType >> 8 & 0x00ff,
        (UINT32)(*itrCurr)->ccType >> 16 & 0x00ff,
        (UINT32)(*itrCurr)->ccType >> 24 & 0x00ff,
        (*itrCurr)->iAmount);

    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->ccType);
    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->iAmount);
    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->iReserved);
    parserOut.SetU4_LEnd ((UINT32)(*itrCurr)->iEscrow);

    parserOut.SetU4_LEnd ((*itrCurr)->bInfinite ? 1 : 0); // if needed, this can become a flag variable
    parserOut.SetU4_LEnd ((*itrCurr)->iBonus);
    parserOut.SetDataStr ((*itrCurr)->strKey);
    }
  };

//-----------------------------------------------------------------------------
VOID CurrencyManager::Deserialize (RStrParser &  parserIn)
  {
  if (parserIn.IsEmpty ()) return;

  parserIn.ResetCursor ();

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  INT  iNumCurrencies = parserIn.GetU4_LEnd ();

  for (INT  iIndex = 0; iIndex < iNumCurrencies; ++iIndex)
    {
    RStr      strKey;

    UINT32  ccType    = parserIn.GetU4_LEnd ();
    INT     iAmount   = parserIn.GetU4_LEnd ();
    INT     iReserved = parserIn.GetU4_LEnd ();
    INT     iEscrow   = parserIn.GetU4_LEnd ();
    BOOL    bInfinite = (parserIn.GetU4_LEnd () & 0x01) ? TRUE : FALSE;
    INT     iBonus    = parserIn.GetU4_LEnd ();
    parserIn.GetDataStr (&strKey);

    DBG_INFO ("currency loading \"%s\" %d + %d", strKey.AsChar(), iAmount, iReserved);

    Set (ccType, iAmount + iReserved, NULL, iEscrow);

    SetInfinite (ccType, bInfinite);
    SetBonus    (ccType, iBonus);
    SetKey      (ccType, strKey);
    UpdateRegistry (ccType, FALSE);
    };
  };


//-----------------------------------------------------------------------------
VOID CurrencyManager::UpdateRegistry (UINT32  ccCurrencyTypeIn,
                                      BOOL    bUpdatingIn)
  {
  Currency *           pCurr = NULL;

  // NOTE:  If ccCurrencyTypeIn is 0, then all currencies will be updated.

  if (pReg != NULL)
    {
    for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
      {
      pCurr = (*itrCurr);

      BOOL  bTypeMatches = (pCurr->ccType == ccCurrencyTypeIn) || (ccCurrencyTypeIn == 0);

      if (bTypeMatches && (!pCurr->strKey.IsEmpty ()))
        {
        RStr  strBaseKey (strRegistryPrefix);
        strBaseKey += pCurr->strKey;
        RStr  strInfiniteKey (strBaseKey);
        strInfiniteKey.AppendString (".Infinite");

        pReg->SetBool (strInfiniteKey.AsChar (), pCurr->bInfinite);
        //DBG_INFO ("Setting %s to %s", strInfiniteKey.AsChar (), pCurr->bInfinite ? "TRUE" : "FALSE");
        //DBG_INFO ("---- Setting currency %s to %d.  Updating : %s", strBaseKey.AsChar (), pCurr->iAmount - pCurr->iReserved, bUpdatingIn ? "true" : "false");
        pReg->SetInt (strBaseKey.AsChar (), pCurr->iAmount - pCurr->iReserved, bUpdatingIn);
        };
      };
    };
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Verify  (UINT32        ccCurrencyTypeIn,
                                INT           iAmountIn)
  {
  // make sure you have enough currency
  if (IsInfinite (ccCurrencyTypeIn)) return (TRUE);
  return (GetAvailable (ccCurrencyTypeIn) >= iAmountIn);
  };

//-----------------------------------------------------------------------------
Currency *  CurrencyManager::FindByType (UINT32        ccCurrencyTypeIn)
  {
  for (TListItr<Currency*>  itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ccType == ccCurrencyTypeIn)
      {
      return (*itrCurr);
      }
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
Currency *  CurrencyManager::FindByKey (const char *  szKeyIn)
  {
  RStr  strKeyIn (szKeyIn, TRUE);

  for (TListItr<Currency*>  itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
    {
    //DBG_INFO ("CurrencyManager::FindByKey compare \"%s\" to \"%s\"", (*itrCurr)->strKey.AsChar (), strKeyIn.AsChar ());
    if ((*itrCurr)->strKey.Equals (strKeyIn))
      {
      return (*itrCurr);
      }
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Spend  (UINT32        ccCurrencyTypeIn,
                               INT           iAmountIn,
                               const char *  szTargetIn) // what it was spent on
  {
  // Spend() is essentially a Reserve() followed by a Consume().  It is meant
  //  for immediate spending of currency.

  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr == NULL)
    {
    return (FALSE);
    };

  // Handle infinite currency!

  if (!pCurr->bInfinite)
    {
    if ((pCurr->iAmount - pCurr->iReserved) < iAmountIn)
      {
      return (FALSE);
      };
    pCurr->iAmount   -= iAmountIn;
    };

  UpdateRegistry (ccCurrencyTypeIn, TRUE);

    DBG_INFO ("Spent Currency %c%c%c%c : Amount: %d Remain : %d",
        ccCurrencyTypeIn >> 0 & 0x00ff,
        ccCurrencyTypeIn >> 8 & 0x00ff,
        ccCurrencyTypeIn >> 16 & 0x00ff,
        ccCurrencyTypeIn >> 24 & 0x00ff,
        iAmountIn,
        pCurr->iAmount);

  sigOnSpent (ccCurrencyTypeIn, szTargetIn, iAmountIn);

  return (TRUE);
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Reserve  (UINT32        ccCurrencyTypeIn,
                                 INT           iAmountIn)
  {
  // set aside currency, but don't consider it spent yet
  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr == NULL)
    {
    return (FALSE);
    };
  if (pCurr->bInfinite) return (TRUE);
  // Make sure you have enough currency to reserve
  if ((pCurr->iAmount - pCurr->iReserved) < iAmountIn)
    {
    return (FALSE);
    };
  pCurr->iReserved += iAmountIn;
  UpdateRegistry (ccCurrencyTypeIn, TRUE);
  return (TRUE);
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Consume  (UINT32        ccCurrencyTypeIn,
                                 INT           iAmountIn,
                                 const char *  szTargetIn)
  {
  // consume the reserved currency

  Currency *           pCurr = FindByType (ccCurrencyTypeIn);
  if (pCurr == NULL)
    {
    return (FALSE);
    };

  if (pCurr->bInfinite) return (TRUE);

  if (pCurr->iReserved < iAmountIn)
    {
    return (FALSE);
    };
  pCurr->iReserved -= iAmountIn;
  DBG_INFO ("currency consume %d - %d", pCurr->iAmount, iAmountIn);
  pCurr->iAmount   -= iAmountIn;
  UpdateRegistry (ccCurrencyTypeIn, TRUE);

    DBG_INFO ("Consumed Currency %c%c%c%c : Amount: %d  Reserved: %d  Remain : %d",
        ccCurrencyTypeIn >> 0 & 0x00ff,
        ccCurrencyTypeIn >> 8 & 0x00ff,
        ccCurrencyTypeIn >> 16 & 0x00ff,
        ccCurrencyTypeIn >> 24 & 0x00ff,
        iAmountIn,
        pCurr->iReserved,
        pCurr->iAmount);

  sigOnSpent (ccCurrencyTypeIn, szTargetIn, iAmountIn);

  return (TRUE);
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Escrow  (UINT32        ccCurrencyTypeIn,
                                INT           iAmountIn)
  {
  // Put currency to award in an escrow account, but don't award it yet.
  //  Call Award() to finally add it to the currency total, but if the process is
  //  interrupted and the system restarts, it is still considered part of the
  //  currency.

  Currency *           pCurr = FindByType (ccCurrencyTypeIn);
  if (pCurr == NULL)
    {
    return (FALSE);
    };
  pCurr->iEscrow += iAmountIn;
  return (TRUE);
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::Award  (UINT32        ccCurrencyTypeIn,
                               const char *  szSourceIn,
                               BOOL          bAnimateUpdating)
  {
  // If an amount is given to award, then award it.  Otherwise use the
  //  Escrow amount.  You have to be careful with Escrowed currency because
  //  timed awards could unintentionally affect the escrow.
  // TODO

  // award the escrowed currency

  Currency *           pCurr = FindByType (ccCurrencyTypeIn);
  if (pCurr == NULL)
    {
    return (FALSE);
    };
  INT  iAwardAmount = pCurr->iEscrow;

  //DBG_INFO ("currency award %d += %d", pCurr->iAmount, iAwardAmount);

  pCurr->iAmount   += iAwardAmount;
  pCurr->iEscrow    = 0;
  UpdateRegistry (ccCurrencyTypeIn, bAnimateUpdating);  // Update is true, so UI will animate.

  sigOnAwarded (ccCurrencyTypeIn, szSourceIn, iAwardAmount);

  return (TRUE);
  };

//-----------------------------------------------------------------------------
VOID  CurrencyManager::SetInfinite  (UINT32        ccCurrencyTypeIn,
                                     BOOL          bInfiniteIn)
  {
  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr != NULL)
    {
    //DBG_INFO ("SetInfinite to %s", bInfiniteIn ? "TRUE" : "FALSE");
    pCurr->bInfinite = bInfiniteIn;
    UpdateRegistry (ccCurrencyTypeIn, TRUE);
    };
  };

//-----------------------------------------------------------------------------
BOOL  CurrencyManager::IsInfinite  (UINT32        ccCurrencyTypeIn)
  {
  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr != NULL)
    {
    return (pCurr->bInfinite);
    };
  return (FALSE);
  };

//-----------------------------------------------------------------------------
VOID  CurrencyManager::SetBonus  (UINT32        ccCurrencyTypeIn,
                                  INT           iBonusIn)
  {
  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr != NULL)
    {
    pCurr->iBonus = iBonusIn;
    // NOTE: Since the bonus isn't currently written to the registry, the
    //  UpdateRegistry call is unneeded.  Check on this.
    //UpdateRegistry (ccCurrencyTypeIn, TRUE);
    };
  };

//-----------------------------------------------------------------------------
INT  CurrencyManager::GetBonus  (UINT32        ccCurrencyTypeIn)
  {
  Currency *           pCurr = FindByType (ccCurrencyTypeIn);

  if (pCurr != NULL)
    {
    return (pCurr->iBonus);
    };
  return (0);
  };

//-----------------------------------------------------------------------------
VOID CurrencyManager::DebugPrint (VOID)
  {
  DBG_INFO ("---- Currency Status ----");
  if (pReg != NULL)
    {
    for (TListItr<Currency*> itrCurr = listCurrencies.First (); itrCurr.IsValid (); ++itrCurr)
      {
      Currency *  pCurr = (*itrCurr);

      RStr  strBaseKey (strRegistryPrefix);
      strBaseKey += pCurr->strKey;
      RStr  strInfiniteKey (strBaseKey);
      strInfiniteKey.AppendString (".Infinite");

      DBG_INFO ("Currency %s has amount %d reserved %d escrow %d bInfinite %s and registry \"%s\" of %d",
                pCurr->strKey.AsChar (),
                pCurr->iAmount,
                pCurr->iReserved,
                pCurr->iEscrow,
                (pCurr->bInfinite ? "True" : "False"),
                strBaseKey.AsChar(),
                pReg->GetInt (strBaseKey.AsChar ()));
      };
    };
  };




