/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_TRANSLATION_H
#define SE_INCL_TRANSLATION_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

// Init translation routines with given translation table
ENGINE_API void InitTranslation(void);

// Add given translation table
ENGINE_API void AddTranslationTable_t(const CTFileName &fnmTable); // throw char *

// Add several tables from a directory using wildcards
ENGINE_API void AddTranslationTablesDir_t(const CTFileName &fnmDir, const CTFileName &fnmPattern); // throw char *

// Finish translation table building
ENGINE_API void FinishTranslationTable(void);

// Read given translation table from file - for internal use
ENGINE_API void ReadTranslationTable_t(CDynamicArray<class CTranslationPair> &atpPairs,
                                       const CTFileName &fnmTable); // throw char *

// Translate a string
ENGINE_API char *Translate(char *str, INDEX iOffset = 0);
ENGINE_API const char *TranslateConst(const char *str, INDEX iOffset = 0);

// Macro for inserting a string for translation into executables
#define TRANS(str) Translate("ETRS" str, 4)

// Macro for translating a variable string (usually can be CTString)
#define TRANSV(str) TranslateConst(str, 0)

#endif /* include-once check. */
