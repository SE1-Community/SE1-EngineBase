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

3
%{
#include "StdH.h"
#include <Engine/Entities/InternalClasses.h>
%}

// Brush entity that can move
class export CMovableBrushEntity : CMovableEntity {
name      "MovableBrushEntity";
thumbnail "";

properties:
components:
functions:
  // Calculate physics for moving (override from CMovableEntity)
  export void DoMoving(void) {
    CMovableEntity::DoMoving();

    // recalculate all bounding boxes relative to new position
    //en_pbrBrush->CalculateBoundingBoxes(); // !!! why here (its done in SetPlacement()?)
  }

  // Copy entity from another entity of same class
  /*CMovableBrushEntity &operator=(CMovableBrushEntity &enOther) {
    CMovableEntity::operator=(enOther);
    return *this;
  }*/

  // Read from stream
  export void Read_t(CTStream *istr) {
    CMovableEntity::Read_t(istr);
  }

  // Write to stream
  export void Write_t(CTStream *ostr) {
    CMovableEntity::Write_t(ostr);
  }

procedures:
  // Must have at least one procedure per class
  Dummy() {};
};
