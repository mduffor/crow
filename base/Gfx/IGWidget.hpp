/* -----------------------------------------------------------------
                            PolarFlow Widget

    This module implements the base class for Immediate Mode Gui 
    Widgets.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// IGWidget.hpp
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

#ifndef IGWIDGET_HPP
#define IGWIDGET_HPP

#include "CalcHash.hpp"


// forward declarations
class IGContext;



// If you're going to render widgets to the same
// UI from different source files, you can avoid
// ID collisions by defining IMGUI_SRC_ID before
// this define block:
//#ifdef IMGUI_SRC_ID
//#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
//#else
//#define GEN_ID (__LINE__)
//#endif

//#define STRINGERMACRO( x ) #x
//#define GEN_ID (int)(__FILE__ STRINGERMACRO(__LINE__))
//#define GEN_ID (__FILE__ STRINGERMACRO(__LINE__))


#define GEN_ID IGID (int (OP2(__FILE__, __LINE__)), 0)
#define GEN_ID_IDX(z) IGID (int (OP2(__FILE__, __LINE__)), z)
#define GEN_ID_STR(s) (CalcHashValue ((s), strlen(s)))
#define OP2(x,y) OP3(x, y)
#define OP3(x,y) ( x ", line " #y)


//#define GEN_ID OP2(__FILE__, __LINE__, 0)
//#define GEN_ID_IDX(z) OP2(__FILE__, __LINE__, z)
//#define OP(z) OP2(__FILE__, __LINE__, z)
//#define OP2(x,y,z) OP3(x, y, z)
//#define OP3(x,y,z) ( x ", line " #y ": " #z )



//------------------------------------------------------------------------------
class IGID
  {
  public:
    INT  iIdentifier;
    INT  iOrdinal;

  public:
            IGID          ()                    {iIdentifier = 0; iOrdinal = 0;};
            IGID          (INT  iIdentifierIn)  {iIdentifier = iIdentifierIn; iOrdinal = -1;};
            IGID          (INT  iIdentifierIn,
                           INT  iOrdinalIn)     {iIdentifier = iIdentifierIn; iOrdinal = iOrdinalIn;};

    BOOL    operator==    (const IGID &  idIn)        {return ((iIdentifier == idIn.iIdentifier) && (iOrdinal == idIn.iOrdinal));};
    BOOL    operator!=    (const IGID &  idIn)        {return ((iIdentifier != idIn.iIdentifier) || (iOrdinal != idIn.iOrdinal));};
    IGID &  operator=     (const IGID &  idIn)        {iIdentifier = idIn.iIdentifier; iOrdinal = idIn.iOrdinal; return *this;};
    BOOL    IsZero        (VOID)                {return ((iIdentifier == 0) && (iOrdinal == 0));};
    BOOL    IsUnavailable (VOID)                {return ((iIdentifier == -1) && (iOrdinal == -1));};
    IGID    SubID         (VOID)                {return IGID (iIdentifier, iOrdinal + 1);};
    IGID    SubIndex      (INT  iIndexIn)       {return IGID (iIdentifier, iOrdinal + iIndexIn);};
  };

extern IGID  igIdInvalid;


//------------------------------------------------------------------------------
class IGWidget
  {
  public:

    IGID          _id;
    INT           _iSizerFlags;

  public:
                  IGWidget  ();
    virtual       ~IGWidget ();

    virtual INT   Do        (IGID          idIn,
                             IGContext &   ctxIn,
                             INT           iXPosIn,
                             INT           iYPosIn,
                             INT           iWidthIn,
                             INT           iHeightIn,
                             INT           iSizerFlagsIn);

    virtual INT   DoStored  (IGContext &   ctxIn,
                             INT           iXPosIn,
                             INT           iYPosIn,
                             INT           iWidthIn,
                             INT           iHeightIn);

    virtual VOID  Render (IGID          idIn,
                          IGContext &   ctxIn,
                          INT           iXPosIn,
                          INT           iYPosIn,
                          INT           iWidthIn,
                          INT           iHeightIn) const;
  };









#endif // IGWIDGET_HPP
