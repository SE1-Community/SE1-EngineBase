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

#ifndef SE_INCL_SHELL_H
#define SE_INCL_SHELL_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Synchronization.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Base/Shell_internal.h>

#define NEXTARGUMENT(type) (*((type *&)pArgs)++)

// Object that takes care of shell functions, variables, macros etc.
class ENGINE_API CShell {
  public:
    CTCriticalSection sh_csShell; // critical section for access to shell data
    CDynamicArray<CShellSymbol> sh_assSymbols; // all defined symbols

    // Get a shell symbol by its name
    CShellSymbol *GetSymbol(const CTString &strName, BOOL bDeclaredOnly);

    // Report error in shell script processing
    void ErrorF(const char *strFormat, ...);

    // Constructor
    CShell(void);

    // Destructor
    ~CShell(void);

    // Initialize the shell
    void Initialize(void);

    // Declare a symbol in the shell
    void DeclareSymbol(const CTString &strDeclaration, void *pvValue);

    // Execute command(s)
    void Execute(const CTString &strCommands);

    // Save shell commands to restore persistent symbols to a script file
    void StorePersistentSymbols(const CTFileName &fnScript);

    // Get symbols
    FLOAT GetFLOAT(const CTString &strName);
    INDEX GetINDEX(const CTString &strName);
    CTString GetString(const CTString &strName);
    CTString GetValue(const CTString &strName);
    
    // Set symbols
    void SetFLOAT(const CTString &strName, FLOAT fValue);
    void SetINDEX(const CTString &strName, INDEX iValue);
    void SetString(const CTString &strName, const CTString &strValue);
    void SetValue(const CTString &strName, const CTString &strValue);
};

// Pointer to global shell object
ENGINE_API extern CShell *_pShell;

#endif /* include-once check. */
