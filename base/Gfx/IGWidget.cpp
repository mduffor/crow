/* -----------------------------------------------------------------
                            PolarFlow Widget

    This module implements the base class for Immediate Mode Gui 
    Widgets.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// IGWidget.cpp
// Copyright 2009, Michael T. Duffy.  
// contact:  mduffor@users.sourceforge.net

// This file is part of PolarFlow.
//
// PolarFlow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// PolarFlow is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PolarFlow.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RTypes.hpp"
#include "RStr.hpp"
#include "IGWidget.hpp"
#include "RDebug.hpp"
ASSERTFILE (__FILE__)

IGID  igIdInvalid (-1, -1);


//==============================================================================
// IMGUI Widget (base class)
//==============================================================================

//------------------------------------------------------------------------------
IGWidget::IGWidget  ()
  {
  _id          = igIdInvalid;
  _iSizerFlags = 0;
  };


//------------------------------------------------------------------------------
IGWidget::~IGWidget ()
  {
  // no body
  };


//------------------------------------------------------------------------------
INT  IGWidget::Do  (IGID          idIn,
                    IGContext &   ctxIn,
                    INT           iXPosIn,
                    INT           iYPosIn,
                    INT           iWidthIn,
                    INT           iHeightIn,
                    INT           iSizerFlagsIn)
  {
  // no body
  return (0);
  };


//------------------------------------------------------------------------------
INT  IGWidget::DoStored  (IGContext &   ctxIn,
                          INT           iXPosIn,
                          INT           iYPosIn,
                          INT           iWidthIn,
                          INT           iHeightIn)
  {
  // no body
  return (0);
  };


//------------------------------------------------------------------------------
VOID  IGWidget::Render (IGID          idIn,
                        IGContext &   ctxIn,
                        INT           iXPosIn,
                        INT           iYPosIn,
                        INT           iWidthIn,
                        INT           iHeightIn) const
  {
  // no body
  };




