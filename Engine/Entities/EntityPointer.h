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

#ifndef SE_INCL_ENTITYPOINTER_H
#define SE_INCL_ENTITYPOINTER_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

// Smart pointer to entity objects, does the book-keeping for reference counting
class CEntityPointer {
  public:
    CEntity *ep_pen; // the pointer itself

  public:
    // All standard smart pointer functions are defined as inlines in Entity.h
    // (due to strange order of inclusion needed for events and enums)

    // Constructor
    inline CEntityPointer(void);

    // Copy constructor
    inline CEntityPointer(const CEntityPointer &penOther);
    
    // Constructor from a pointer
    inline CEntityPointer(CEntity *pen);
    
    // Destructor
    inline ~CEntityPointer(void);

    // Assignment
    inline const CEntityPointer &operator=(CEntity *pen);
    inline const CEntityPointer &operator=(const CEntityPointer &penOther);

    // Get pointer to the entity itself
    inline CEntity *operator->(void) const;
    inline operator CEntity *(void) const;
    inline CEntity &operator*(void) const;
};

#endif /* include-once check. */
