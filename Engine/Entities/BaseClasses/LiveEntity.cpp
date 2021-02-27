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

#include "StdH.h"

#include <Engine/Entities/BaseClasses/LiveEntity.h>

#include <Engine/Base/Stream.h>

// Constructor
CLiveEntity::CLiveEntity(void) {
  en_fHealth = 0;
}

// Copy entity from another entity of same class
void CLiveEntity::Copy(CEntity &enOther, ULONG ulFlags) {
  CEntity::Copy(enOther, ulFlags);

  CLiveEntity *plenOther = (CLiveEntity *)(&enOther);
  en_fHealth = plenOther->en_fHealth;
}

// Read from stream
void CLiveEntity::Read_t(CTStream *istr) {
  CEntity::Read_t(istr);
  (*istr) >> en_fHealth;
}

// Write to stream
void CLiveEntity::Write_t(CTStream *ostr) {
  CEntity::Write_t(ostr);
  (*ostr) << en_fHealth;
}

// Apply some damage to the entity (see event EDamage for more info)
void CLiveEntity::ReceiveDamage(CEntity *penInflictor, enum DamageType dmtType, FLOAT fDamage,
                                const FLOAT3D &vHitPoint, const FLOAT3D &vDirection) {
  CEntityPointer penThis = this; // keep this entity alive during this function

  // reduce your health
  en_fHealth -= fDamage;

  // throw an event that you are damaged
  EDamage eDamage;
  eDamage.penInflictor = penInflictor;
  eDamage.vDirection = vDirection;
  eDamage.vHitPoint = vHitPoint;
  eDamage.fAmount = fDamage;
  eDamage.dmtType = dmtType;

  SendEvent(eDamage);

  // if health reached zero
  if (en_fHealth <= 0) {
    // throw an event that you have died
    EDeath eDeath;
    eDeath.eLastDamage = eDamage;

    SendEvent(eDeath);
  }
}
