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

5
%{
#include "StdH.h"
#define DECL_DLL ENGINE_API
#include <Engine/Entities/EntityEvent.h>
#include <Engine/Entities/EntityPointer.h>
#include <Engine/Math/Vector.h>
#include <Engine/Math/Plane.h>
#include <Engine/Entities/EntityProperties.h>
%}

// These events are used globally

// Internal jumping event - do not use
event EInternal {};

// General void type
event EVoid {};

// Sub-procedure return notify
event EReturn {};

// Sent to wait handler when started
event EBegin {};

// Timer elapsed
event ETimer {};

// One entity touched another while moving
event ETouch {
  CEntityPointer penOther, // other entity
  BOOL bThisMoved, // if this entity has touched other entity
  FLOATplane3D plCollision, // plane of collision
};

// One entity passed through another while moving
event EPass {
  CEntityPointer penOther, // other entity
  BOOL bThisMoved, // if this entity has touched other entity
};

// ONBLOCK_PUSH or ONBLOCK_STOP entity is blocked
event EBlock {
  CEntityPointer penOther, // other entity
  FLOATplane3D plCollision, // plane of collision
};

// Entity cannot move or it would fall over an edge
event EWouldFall {};

// Teleport has been activated in your vicinity
event ETeleport {};

// Notifying an entity that a level is about to change
event EPreLevelChange {
  INDEX iUserData,
};

// Notifying an entity that a level has just changed
event EPostLevelChange {
  INDEX iUserData,
};

// Notifying an entity that it is the first worldbase in the world
event EFirstWorldBase {};

// Entity has been damaged
event EDamage {
  CEntityPointer penInflictor, // entity that inflicted the damage
  FLOAT3D vDirection, // where the damage came from (in absolute space)
  FLOAT3D vHitPoint, // where the damage hit the entity (in absolute space)
  FLOAT fAmount, // amount of damage done
  INDEX dmtType, // type of damage
};

// When this entity dies (health reaches zero)
event EDeath {
  EDamage eLastDamage, // the damage event that caused the death
};

// When this entity takes air after being without it for some time
event ETakingBreath {
  TICK llBreathDelay, // how long it was without air (0 sec = little, 1 sec = drowning)
};
