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

#ifndef SE_INCL_LIVEENTITY_H
#define SE_INCL_LIVEENTITY_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Entities/BaseClasses/Entity.h>

// Entity that is alive (has health)
class ENGINE_API CLiveEntity : public CEntity {
  public:
    FLOAT en_fHealth; // health of the entity

    // Copy entity from another entity of same class
    virtual void Copy(CEntity &enOther, ULONG ulFlags);

    // Read from stream
    virtual void Read_t(CTStream *istr); // throw char *

    // Write to stream
    virtual void Write_t(CTStream *ostr); // throw char *

  public:
    // Set health of the entity
    void SetHealth(FLOAT fHealth) {
      en_fHealth = fHealth;
    };

  public:
    // Constructor
    CLiveEntity(void);

    // Get health of the entity
    FLOAT GetHealth(void) const {
      return en_fHealth;
    };

    // Apply some damage to the entity (see event EDamage for more info)
    virtual void ReceiveDamage(CEntity *penInflictor, INDEX dmtType, FLOAT fDamage, const FLOAT3D &vHitPoint, const FLOAT3D &vDirection);

    // Return bytes of memory used by this object
    inline SLONG GetUsedMemory(void) {
      return (sizeof(CLiveEntity) - sizeof(CEntity) + CEntity::GetUsedMemory());
    };
};

extern "C" ENGINE_API class CDLLEntityClass CLiveEntity_DLLClass;

#endif /* include-once check. */
