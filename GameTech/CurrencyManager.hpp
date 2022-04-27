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

#ifndef CURRENCYMANAGER_HPP
#define CURRENCYMANAGER_HPP

#include "Sys/Types.hpp"
#include "Util/Signal.h"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"

/**
  @addtogroup gametech
  @{
*/

/**
  @class Currency

  @brief Data class for holding info about a singlle currency.
  @details
*/

//-----------------------------------------------------------------------------
class Currency
  {
  public:
    RStr      strKey;     // Name of currency used in registry keys.  i.e. "Gold"
    UINT32    ccType;     // 4-cc code of currency for quick comparisons.
    INT       iAmount;    // TODO:  Obfuscate this value in memory.
    INT       iReserved;  // Currency in the process of being spent
    INT       iEscrow;    // Currency in the process of being awarded.
    BOOL      bInfinite;  // True if transactions agains this currency don't reduce its amount.
    INT       iBonus;     // Percent.  For example, a value of 25 is "+25%".  Tracked buff that can be used by the game to modify transactions.  Not used directly by CurrencyManager.

  public:
    Currency (UINT32  ccTypeIn,
              INT     iAmountIn)    {
                                    ccType = ccTypeIn;
                                    iAmount = iAmountIn;
                                    iReserved = 0;
                                    iEscrow = 0;
                                    iBonus = 0;
                                    };
  };

/**
  @class CurrencyManager

  @brief Container for all currency types.
  @details  This class manages the creation and manipulation of currencies.
    A currency can be defined as a named data type with a numerical value,
    and these value are spent to acquire in-game items or currency.
*/

//-----------------------------------------------------------------------------
class CurrencyManager
  {
  private:

    TList<Currency*>          listCurrencies;
    ValueRegistry *           pReg;
    BOOL                      bNeedToSave;
    RStr                      strRegistryPrefix;

    static CurrencyManager *  pInstance;


  public:

   // Attach with CurrencyManager::Instance()->sigOnSpent.Connect (myClassInstancePtr, &myClass::OnSpentListener);

    Signal3<UINT32, const char *, INT>  sigOnSpent;   ///< Event fired when currency is spent or consumed
                                                      //    UINT32        ccType
                                                      //    const char *  szTarget
                                                      //    INT           iAmount

    Signal3<UINT32, const char *, INT>  sigOnAwarded; ///< Event fired when currency is awarded/earned
                                                      //    UINT32        ccType
                                                      //    const char *  szSource
                                                      //    INT           iAmount

  public:

                              CurrencyManager  ();

                              ~CurrencyManager ();

    static CurrencyManager *  Instance         (VOID)    {CreateInstance (); return (pInstance);};
    static VOID               CreateInstance   (VOID)    {if (pInstance == NULL) {pInstance = new CurrencyManager;};};
    static VOID               DestroyInstance  (VOID)    {if (pInstance != NULL) {delete pInstance; pInstance = NULL;};};

    VOID                      SetValueRegistry (ValueRegistry *  pRegIn)         {pReg = pRegIn;};

    INT                       Get              (UINT32        ccCurrencyType);

    INT                       GetAvailable     (UINT32        ccCurrencyType);

    INT                       Set              (UINT32        ccCurrencyType,
                                                INT           iAmount,
                                                const char *  szKeyIn = NULL,
                                                INT           iEscrowIn = -1);

    VOID                      SetKey           (UINT32        ccCurrencyTypeIn,
                                                const char *  szKeyIn);

    INT                       Add              (UINT32        ccCurrencyType,
                                                INT           iAmount,
                                                const char *  szKeyIn = NULL,
                                                INT           iEscrowIn = -1);

    VOID                      Serialize        (RStrParser &  parserOut);

    VOID                      Deserialize      (RStrParser &  parserIn);

    BOOL                      SetSaveFlag      (BOOL   bNeedToSaveIn)      {BOOL  bPrevState = bNeedToSave; bNeedToSave = bNeedToSaveIn; return (bPrevState);};

    VOID                      UpdateRegistry   (UINT32        ccCurrencyTypeIn,
                                                BOOL          bUpdatingIn);

    Currency *                FindByType       (UINT32        ccCurrencyTypeIn);

    Currency *                FindByKey        (const char *  szKeyIn);

    BOOL                      Verify           (UINT32        ccCurrencyTypeIn,
                                                INT           iAmountIn);        // make sure you have enough

    BOOL                      Spend            (UINT32        ccCurrencyTypeIn,
                                                INT           iAmountIn,
                                                const char *  szTargetIn);

    BOOL                      Reserve          (UINT32        ccCurrencyTypeIn,
                                                INT           iAmountIn);        // set aside currency, but don't spend it

    BOOL                      Consume          (UINT32        ccCurrencyTypeIn,
                                                INT           iAmountIn,
                                                const char *  szTargetIn);        // consume the reserved currency

    BOOL                      Escrow           (UINT32        ccCurrencyTypeIn,
                                                INT           iAmountIn);        // set aside currency, but don't award it

    BOOL                      Award            (UINT32        ccCurrencyTypeIn,  // award the escrowed currency
                                                const char *  szSourceIn,
                                                BOOL          bAnimateUpdating = TRUE);

    VOID                      SetInfinite      (UINT32        ccCurrencyTypeIn,
                                                BOOL          bInfiniteIn);

    BOOL                      IsInfinite       (UINT32        ccCurrencyTypeIn);

    VOID                      SetBonus         (UINT32        ccCurrencyTypeIn,
                                                INT           iBonusIn);

    INT                       GetBonus         (UINT32        ccCurrencyTypeIn);

    VOID                      DebugPrint       (VOID);


  };

/** @} */ // end of gametech group


#endif // CURRENCYMANAGER_HPP
