/* -----------------------------------------------------------------
                             Calc Hash

     This module implements a string hashing function.

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

#ifndef CALCHASH_HPP
#define CALCHASH_HPP

#include "Sys/Types.hpp"
/**
  @addtogroup base
  @{
*/
// Define a type to hold a hash value.  You may need to increase this in the future,
//   either for performance or stability.

#define HASH_T  UINT32

#define HASH(a) (CalcHashValue(a))
// Use HASH_EXTEND to add onto a hash.  i.e. HASH_EXTEND (uPrevHash, "moreText")
#define HASH_EXTEND(a,b) (CalcHashValue(b,strlen(b),a))

// NOTE:  The H(a,b) macro is meant for optimizing inline hash generation.
//        For H(a,b), the value of b will be used if not zero,
//        otherwise the hash will be calculated on the string in a.  The goal here is
//        to run a pre-process on the code file, and calc the hash of any const strings,
//        and store the result in b.

#define H(a,b)  ((b != 0)?b:CalcHashValue(a))




                                   /** @brief Convert a string into a 32-bit hash value
                                       @param szStringIn The string to use as source material.
                                       @param ulStringLengthIn Number of characters to use in the hash calculation.  Terminating null characters do not stop string processing.
                                       @param uStartingHash Hash value to build off of.  For example, if you pass the hash for string "A", and then pass string "B" in szStringIn, the resulting hash would be for the string "AB".
                                       @return A 32-bit unsigned integer hash value
                                   */
HASH_T CalcHashValue  (const char *  szStringIn,
                       size_t        iStringLengthIn = 0,
                       HASH_T        uStartingHash = 0);


/** @} */ // end of base group


#endif // CALCHASH_HPP

