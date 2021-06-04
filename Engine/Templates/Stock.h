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

#ifndef SE_INCL_STOCK_TEMPLATE_H
#define SE_INCL_STOCK_TEMPLATE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Stream.h>
#include <Engine/Templates/DynamicContainer.cpp>
#include <Engine/Templates/NameTable.h>

// [Cecil] 2021-06-03: Reworked into actual templates

// Template for stock of some kind of objects that can be saved and loaded
template<class Type> class CStock {
  public:
    CDynamicContainer<Type> st_ctObjects; // objects on stock
    CNameTable<Type> st_ntObjects; // name table for fast lookup

  public:
    // Constructor
    CStock(void);

    // Destructor
    ~CStock(void);

    // Obtain an object from stock - loads if not loaded
    Type *Obtain_t(const CTFileName &fnmFileName); // throw char *

    // Release an object when not needed any more
    void Release(Type *ptObject);

    // Free all unused elements of the stock
    void FreeUnused(void);

    // Calculate amount of memory used by all objects in the stock
    SLONG CalculateUsedMemory(void);

    // Dump memory usage report to a file
    void DumpMemoryUsage_t(CTStream &strm); // throw char *

    // Get number of total elements in stock
    INDEX GetTotalCount(void);

    // Get number of used elements in stock
    INDEX GetUsedCount(void);
};

#include <Engine/Templates/Stock.inl>

#endif /* include-once check. */
