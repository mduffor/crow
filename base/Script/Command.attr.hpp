/* -----------------------------------------------------------------
                             Command

     This module implements a command system, so that functions
   can be run by name.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
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

//Deprecated

#ifndef COMMAND_HPP
#define COMMAND_HPP

#ifdef REFERENCE


// NOTE:  This file currently is not compiled into Crow.  It is the version
//        of Command where parameters are based on RAttr instances.  A newer
//        version based on ValueRegistry parameters is in the Crow/Commands
//        directory.  Eventually once you port RScript to Crow, you will need
//        to rework it to use the ValueRegistry based one.  This file is kept
//        for reference only.



#include "Sys/Types.hpp"
#include "Composite/Attr.hpp"
#include "Containers/TList.hpp"


//-----------------------------------------------------------------------------
class Command
  {
  protected:

    RStr                      strName;        ///< Name of the command.  Used to find the command during parsing.

    TList<Attr*>              listParameters; ///< List of parameters that this command can take.

  public:

                  Command      ();

                  Command      (const char *  szNameIn);

    virtual       ~Command     ();

    VOID          SetName      (const char *  szNameIn)    {strName = szNameIn;};

    const char *  Name         (VOID)                      {return (strName.AsChar ());};

    VOID          FreeParams   (VOID);

    Attr *        SetParam     (const char *  szParamNameIn,
                                INT           iValueIn);

    Attr *        SetParam     (const char *  szParamNameIn,
                                FLOAT         fValueIn);

    Attr *        SetParam     (const char *  szParamNameIn,
                                const char *  szValueIn);

    Attr *        GetParam     (const char *  szParamNameIn);

    VOID          ClearParams  (VOID);

    static VOID   Invoke       (const char * szNameIn)        {Invoke (szNameIn, NULL);};

    static VOID   Invoke       (const char *    szNameIn,
                                TList<Attr*> *  pParamsIn);

    virtual VOID  Invoke       (VOID) = 0;


  };

#endif // REFERENCE

#endif // COMMAND_HPP
