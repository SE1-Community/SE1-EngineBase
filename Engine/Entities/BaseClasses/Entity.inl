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

// All standard smart pointer functions are here as inlines
inline CEntityPointer::CEntityPointer(void) : ep_pen(NULL) {};

inline CEntityPointer::~CEntityPointer(void) {
  ep_pen->RemReference();
};

inline CEntityPointer::CEntityPointer(const CEntityPointer &penOther) : ep_pen(penOther.ep_pen) {
  ep_pen->AddReference();
};

inline CEntityPointer::CEntityPointer(CEntity *pen) : ep_pen(pen) {
  ep_pen->AddReference();
};

inline const CEntityPointer &CEntityPointer::operator=(CEntity *pen) {
  pen->AddReference(); // must first add, then remove!
  ep_pen->RemReference();
  ep_pen = pen;

  return *this;
}

inline const CEntityPointer &CEntityPointer::operator=(const CEntityPointer &penOther) {
  penOther.ep_pen->AddReference(); // must first add, then remove!
  ep_pen->RemReference();
  ep_pen = penOther.ep_pen;

  return *this;
}

inline CEntity *CEntityPointer::operator->(void) const {
  return ep_pen;
}

inline CEntityPointer::operator CEntity *(void) const {
  return ep_pen;
}

inline CEntity &CEntityPointer::operator*(void) const {
  return *ep_pen;
}

// Reference counting functions
inline void CEntity::AddReference(void) {
  if (this != NULL) {
    ASSERT(en_ctReferences >= 0);
    en_ctReferences++;
  }
};

inline void CEntity::RemReference(void) {
  if (this != NULL) {
    ASSERT(en_ctReferences > 0);
    en_ctReferences--;

    if (en_ctReferences == 0) {
      delete this;
    }
  }
};
