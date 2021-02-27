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

#include <Engine/Models/Normals.h>
#include <Engine/Math/Vector.h>

extern FLOAT3D avGouraudNormals[MAX_GOURAUDNORMALS] = {
  FLOAT3D(1.000000f, 0.000000f, 0.000000f),    FLOAT3D(0.980785f, 0.000000f, 0.195090f),
  FLOAT3D(0.980785f, 0.195090f, 0.000000f),    FLOAT3D(0.923880f, 0.000000f, 0.382683f),
  FLOAT3D(0.959683f, 0.198757f, 0.198757f),    FLOAT3D(0.923880f, 0.382683f, 0.000000f),
  FLOAT3D(0.831470f, 0.000000f, 0.555570f),    FLOAT3D(0.890320f, 0.208847f, 0.404615f),
  FLOAT3D(0.707107f, 0.000000f, 0.707107f),    FLOAT3D(0.788675f, 0.211325f, 0.577350f),
  FLOAT3D(0.816497f, 0.408248f, 0.408248f),    FLOAT3D(0.831470f, 0.555570f, 0.000000f),
  FLOAT3D(0.788675f, 0.577350f, 0.211325f),    FLOAT3D(0.707107f, 0.707107f, 0.000000f),
  FLOAT3D(0.555570f, 0.000000f, 0.831470f),    FLOAT3D(0.577350f, 0.211325f, 0.788675f),
  FLOAT3D(0.382683f, 0.000000f, 0.923880f),    FLOAT3D(0.404615f, 0.208847f, 0.890320f),
  FLOAT3D(0.408248f, 0.408248f, 0.816497f),    FLOAT3D(0.195090f, 0.000000f, 0.980785f),
  FLOAT3D(0.198757f, 0.198757f, 0.959683f),    FLOAT3D(0.000000f, 0.000000f, 1.000000f),
  FLOAT3D(0.000000f, 0.195090f, 0.980785f),    FLOAT3D(0.000000f, 0.382683f, 0.923880f),
  FLOAT3D(0.208847f, 0.404615f, 0.890320f),    FLOAT3D(0.211325f, 0.577350f, 0.788675f),
  FLOAT3D(0.000000f, 0.555570f, 0.831470f),    FLOAT3D(0.000000f, 0.707107f, 0.707107f),
  FLOAT3D(0.577350f, 0.788675f, 0.211325f),    FLOAT3D(0.639602f, 0.639602f, 0.426401f),
  FLOAT3D(0.408248f, 0.816497f, 0.408248f),    FLOAT3D(0.639602f, 0.426401f, 0.639602f),
  FLOAT3D(0.426401f, 0.639602f, 0.639602f),    FLOAT3D(0.211325f, 0.788675f, 0.577350f),
  FLOAT3D(0.555570f, 0.831470f, 0.000000f),    FLOAT3D(0.404615f, 0.890320f, 0.208847f),
  FLOAT3D(0.382683f, 0.923880f, 0.000000f),    FLOAT3D(0.208847f, 0.890320f, 0.404615f),
  FLOAT3D(0.000000f, 0.831470f, 0.555570f),    FLOAT3D(0.000000f, 0.923880f, 0.382683f),
  FLOAT3D(0.198757f, 0.959683f, 0.198757f),    FLOAT3D(0.195090f, 0.980785f, 0.000000f),
  FLOAT3D(0.000000f, 0.980785f, 0.195090f),    FLOAT3D(0.000000f, 1.000000f, 0.000000f),
  FLOAT3D(-0.195090f, 0.980785f, 0.000000f),   FLOAT3D(-0.198757f, 0.959683f, 0.198757f),
  FLOAT3D(-0.382683f, 0.923880f, 0.000000f),   FLOAT3D(-0.208847f, 0.890320f, 0.404615f),
  FLOAT3D(-0.211325f, 0.788675f, 0.577350f),   FLOAT3D(-0.408248f, 0.816497f, 0.408248f),
  FLOAT3D(-0.404615f, 0.890320f, 0.208847f),   FLOAT3D(-0.555570f, 0.831470f, 0.000000f),
  FLOAT3D(-0.577350f, 0.788675f, 0.211325f),   FLOAT3D(-0.707107f, 0.707107f, 0.000000f),
  FLOAT3D(-0.211325f, 0.577350f, 0.788675f),   FLOAT3D(-0.208847f, 0.404615f, 0.890320f),
  FLOAT3D(-0.408248f, 0.408248f, 0.816497f),   FLOAT3D(-0.198757f, 0.198757f, 0.959683f),
  FLOAT3D(-0.195090f, 0.000000f, 0.980785f),   FLOAT3D(-0.382683f, 0.000000f, 0.923880f),
  FLOAT3D(-0.404615f, 0.208847f, 0.890320f),   FLOAT3D(-0.577350f, 0.211325f, 0.788675f),
  FLOAT3D(-0.555570f, 0.000000f, 0.831470f),   FLOAT3D(-0.707107f, 0.000000f, 0.707107f),
  FLOAT3D(-0.788675f, 0.577350f, 0.211325f),   FLOAT3D(-0.639602f, 0.639602f, 0.426401f),
  FLOAT3D(-0.816497f, 0.408248f, 0.408248f),   FLOAT3D(-0.426401f, 0.639602f, 0.639602f),
  FLOAT3D(-0.639602f, 0.426401f, 0.639602f),   FLOAT3D(-0.788675f, 0.211325f, 0.577350f),
  FLOAT3D(-0.831470f, 0.555570f, 0.000000f),   FLOAT3D(-0.890320f, 0.404615f, 0.208847f),
  FLOAT3D(-0.923880f, 0.382683f, 0.000000f),   FLOAT3D(-0.890320f, 0.208847f, 0.404615f),
  FLOAT3D(-0.831470f, 0.000000f, 0.555570f),   FLOAT3D(-0.923880f, 0.000000f, 0.382683f),
  FLOAT3D(-0.959683f, 0.198757f, 0.198757f),   FLOAT3D(-0.980785f, 0.195090f, 0.000000f),
  FLOAT3D(-0.980785f, 0.000000f, 0.195090f),   FLOAT3D(-1.000000f, 0.000000f, 0.000000f),
  FLOAT3D(-0.980785f, -0.195090f, 0.000000f),  FLOAT3D(-0.959683f, -0.198757f, 0.198757f),
  FLOAT3D(-0.923880f, -0.382683f, 0.000000f),  FLOAT3D(-0.890320f, -0.208847f, 0.404615f),
  FLOAT3D(-0.788675f, -0.211325f, 0.577350f),  FLOAT3D(-0.816497f, -0.408248f, 0.408248f),
  FLOAT3D(-0.890320f, -0.404615f, 0.208847f),  FLOAT3D(-0.831470f, -0.555570f, 0.000000f),
  FLOAT3D(-0.788675f, -0.577350f, 0.211325f),  FLOAT3D(-0.707107f, -0.707107f, 0.000000f),
  FLOAT3D(-0.577350f, -0.211325f, 0.788675f),  FLOAT3D(-0.404615f, -0.208847f, 0.890320f),
  FLOAT3D(-0.408248f, -0.408248f, 0.816497f),  FLOAT3D(-0.198757f, -0.198757f, 0.959683f),
  FLOAT3D(0.000000f, -0.195090f, 0.980785f),   FLOAT3D(0.000000f, -0.382683f, 0.923880f),
  FLOAT3D(-0.208847f, -0.404615f, 0.890320f),  FLOAT3D(-0.211325f, -0.577350f, 0.788675f),
  FLOAT3D(0.000000f, -0.555570f, 0.831470f),   FLOAT3D(0.000000f, -0.707107f, 0.707107f),
  FLOAT3D(-0.577350f, -0.788675f, 0.211325f),  FLOAT3D(-0.639602f, -0.639602f, 0.426401f),
  FLOAT3D(-0.408248f, -0.816497f, 0.408248f),  FLOAT3D(-0.639602f, -0.426401f, 0.639602f),
  FLOAT3D(-0.426401f, -0.639602f, 0.639602f),  FLOAT3D(-0.211325f, -0.788675f, 0.577350f),
  FLOAT3D(-0.555570f, -0.831470f, 0.000000f),  FLOAT3D(-0.404615f, -0.890320f, 0.208847f),
  FLOAT3D(-0.382683f, -0.923880f, 0.000000f),  FLOAT3D(-0.208847f, -0.890320f, 0.404615f),
  FLOAT3D(0.000000f, -0.831470f, 0.555570f),   FLOAT3D(0.000000f, -0.923880f, 0.382683f),
  FLOAT3D(-0.198757f, -0.959683f, 0.198757f),  FLOAT3D(-0.195090f, -0.980785f, 0.000000f),
  FLOAT3D(0.000000f, -0.980785f, 0.195090f),   FLOAT3D(0.000000f, -1.000000f, 0.000000f),
  FLOAT3D(0.195090f, -0.980785f, 0.000000f),   FLOAT3D(0.198757f, -0.959683f, 0.198757f),
  FLOAT3D(0.382683f, -0.923880f, 0.000000f),   FLOAT3D(0.208847f, -0.890320f, 0.404615f),
  FLOAT3D(0.211325f, -0.788675f, 0.577350f),   FLOAT3D(0.408248f, -0.816497f, 0.408248f),
  FLOAT3D(0.404615f, -0.890320f, 0.208847f),   FLOAT3D(0.555570f, -0.831470f, 0.000000f),
  FLOAT3D(0.577350f, -0.788675f, 0.211325f),   FLOAT3D(0.707107f, -0.707107f, 0.000000f),
  FLOAT3D(0.211325f, -0.577350f, 0.788675f),   FLOAT3D(0.208847f, -0.404615f, 0.890320f),
  FLOAT3D(0.408248f, -0.408248f, 0.816497f),   FLOAT3D(0.198757f, -0.198757f, 0.959683f),
  FLOAT3D(0.404615f, -0.208847f, 0.890320f),   FLOAT3D(0.577350f, -0.211325f, 0.788675f),
  FLOAT3D(0.788675f, -0.577350f, 0.211325f),   FLOAT3D(0.639602f, -0.639602f, 0.426401f),
  FLOAT3D(0.816497f, -0.408248f, 0.408248f),   FLOAT3D(0.426401f, -0.639602f, 0.639602f),
  FLOAT3D(0.639602f, -0.426401f, 0.639602f),   FLOAT3D(0.788675f, -0.211325f, 0.577350f),
  FLOAT3D(0.831470f, -0.555570f, 0.000000f),   FLOAT3D(0.890320f, -0.404615f, 0.208847f),
  FLOAT3D(0.923880f, -0.382683f, 0.000000f),   FLOAT3D(0.890320f, -0.208847f, 0.404615f),
  FLOAT3D(0.959683f, -0.198757f, 0.198757f),   FLOAT3D(0.980785f, -0.195090f, 0.000000f),
  FLOAT3D(0.980785f, 0.000000f, -0.195090f),   FLOAT3D(0.959683f, 0.198757f, -0.198757f),
  FLOAT3D(0.923880f, 0.000000f, -0.382683f),   FLOAT3D(0.890320f, 0.404615f, -0.208847f),
  FLOAT3D(0.788675f, 0.577350f, -0.211325f),   FLOAT3D(0.816497f, 0.408248f, -0.408248f),
  FLOAT3D(0.890320f, 0.208847f, -0.404615f),   FLOAT3D(0.831470f, 0.000000f, -0.555570f),
  FLOAT3D(0.788675f, 0.211325f, -0.577350f),   FLOAT3D(0.707107f, 0.000000f, -0.707107f),
  FLOAT3D(0.577350f, 0.788675f, -0.211325f),   FLOAT3D(0.404615f, 0.890320f, -0.208847f),
  FLOAT3D(0.408248f, 0.816497f, -0.408248f),   FLOAT3D(0.198757f, 0.959683f, -0.198757f),
  FLOAT3D(0.000000f, 0.923880f, -0.382683f),   FLOAT3D(0.208847f, 0.890320f, -0.404615f),
  FLOAT3D(0.211325f, 0.788675f, -0.577350f),   FLOAT3D(0.000000f, 0.831470f, -0.555570f),
  FLOAT3D(0.000000f, 0.707107f, -0.707107f),   FLOAT3D(0.577350f, 0.211325f, -0.788675f),
  FLOAT3D(0.639602f, 0.426401f, -0.639602f),   FLOAT3D(0.408248f, 0.408248f, -0.816497f),
  FLOAT3D(0.639602f, 0.639602f, -0.426401f),   FLOAT3D(0.426401f, 0.639602f, -0.639602f),
  FLOAT3D(0.211325f, 0.577350f, -0.788675f),   FLOAT3D(0.555570f, 0.000000f, -0.831470f),
  FLOAT3D(0.404615f, 0.208847f, -0.890320f),   FLOAT3D(0.382683f, 0.000000f, -0.923880f),
  FLOAT3D(0.208847f, 0.404615f, -0.890320f),   FLOAT3D(0.000000f, 0.555570f, -0.831470f),
  FLOAT3D(0.000000f, 0.382683f, -0.923880f),   FLOAT3D(0.198757f, 0.198757f, -0.959683f),
  FLOAT3D(0.195090f, 0.000000f, -0.980785f),   FLOAT3D(0.000000f, 0.195090f, -0.980785f),
  FLOAT3D(0.000000f, 0.000000f, -1.000000f),   FLOAT3D(-0.198757f, 0.959683f, -0.198757f),
  FLOAT3D(-0.404615f, 0.890320f, -0.208847f),  FLOAT3D(-0.577350f, 0.788675f, -0.211325f),
  FLOAT3D(-0.408248f, 0.816497f, -0.408248f),  FLOAT3D(-0.208847f, 0.890320f, -0.404615f),
  FLOAT3D(-0.211325f, 0.788675f, -0.577350f),  FLOAT3D(-0.788675f, 0.577350f, -0.211325f),
  FLOAT3D(-0.890320f, 0.404615f, -0.208847f),  FLOAT3D(-0.816497f, 0.408248f, -0.408248f),
  FLOAT3D(-0.959683f, 0.198757f, -0.198757f),  FLOAT3D(-0.980785f, 0.000000f, -0.195090f),
  FLOAT3D(-0.923880f, 0.000000f, -0.382683f),  FLOAT3D(-0.890320f, 0.208847f, -0.404615f),
  FLOAT3D(-0.788675f, 0.211325f, -0.577350f),  FLOAT3D(-0.831470f, 0.000000f, -0.555570f),
  FLOAT3D(-0.707107f, 0.000000f, -0.707107f),  FLOAT3D(-0.211325f, 0.577350f, -0.788675f),
  FLOAT3D(-0.426401f, 0.639602f, -0.639602f),  FLOAT3D(-0.408248f, 0.408248f, -0.816497f),
  FLOAT3D(-0.639602f, 0.639602f, -0.426401f),  FLOAT3D(-0.639602f, 0.426401f, -0.639602f),
  FLOAT3D(-0.577350f, 0.211325f, -0.788675f),  FLOAT3D(-0.208847f, 0.404615f, -0.890320f),
  FLOAT3D(-0.404615f, 0.208847f, -0.890320f),  FLOAT3D(-0.555570f, 0.000000f, -0.831470f),
  FLOAT3D(-0.382683f, 0.000000f, -0.923880f),  FLOAT3D(-0.198757f, 0.198757f, -0.959683f),
  FLOAT3D(-0.195090f, 0.000000f, -0.980785f),  FLOAT3D(-0.959683f, -0.198757f, -0.198757f),
  FLOAT3D(-0.890320f, -0.404615f, -0.208847f), FLOAT3D(-0.788675f, -0.577350f, -0.211325f),
  FLOAT3D(-0.816497f, -0.408248f, -0.408248f), FLOAT3D(-0.890320f, -0.208847f, -0.404615f),
  FLOAT3D(-0.788675f, -0.211325f, -0.577350f), FLOAT3D(-0.577350f, -0.788675f, -0.211325f),
  FLOAT3D(-0.404615f, -0.890320f, -0.208847f), FLOAT3D(-0.408248f, -0.816497f, -0.408248f),
  FLOAT3D(-0.198757f, -0.959683f, -0.198757f), FLOAT3D(0.000000f, -0.980785f, -0.195090f),
  FLOAT3D(0.000000f, -0.923880f, -0.382683f),  FLOAT3D(-0.208847f, -0.890320f, -0.404615f),
  FLOAT3D(-0.211325f, -0.788675f, -0.577350f), FLOAT3D(0.000000f, -0.831470f, -0.555570f),
  FLOAT3D(0.000000f, -0.707107f, -0.707107f),  FLOAT3D(-0.577350f, -0.211325f, -0.788675f),
  FLOAT3D(-0.639602f, -0.426401f, -0.639602f), FLOAT3D(-0.408248f, -0.408248f, -0.816497f),
  FLOAT3D(-0.639602f, -0.639602f, -0.426401f), FLOAT3D(-0.426401f, -0.639602f, -0.639602f),
  FLOAT3D(-0.211325f, -0.577350f, -0.788675f), FLOAT3D(-0.404615f, -0.208847f, -0.890320f),
  FLOAT3D(-0.208847f, -0.404615f, -0.890320f), FLOAT3D(0.000000f, -0.555570f, -0.831470f),
  FLOAT3D(0.000000f, -0.382683f, -0.923880f),  FLOAT3D(-0.198757f, -0.198757f, -0.959683f),
  FLOAT3D(0.000000f, -0.195090f, -0.980785f),  FLOAT3D(0.198757f, -0.959683f, -0.198757f),
  FLOAT3D(0.404615f, -0.890320f, -0.208847f),  FLOAT3D(0.577350f, -0.788675f, -0.211325f),
  FLOAT3D(0.408248f, -0.816497f, -0.408248f),  FLOAT3D(0.208847f, -0.890320f, -0.404615f),
  FLOAT3D(0.211325f, -0.788675f, -0.577350f),  FLOAT3D(0.788675f, -0.577350f, -0.211325f),
  FLOAT3D(0.890320f, -0.404615f, -0.208847f),  FLOAT3D(0.816497f, -0.408248f, -0.408248f),
  FLOAT3D(0.959683f, -0.198757f, -0.198757f),  FLOAT3D(0.890320f, -0.208847f, -0.404615f),
  FLOAT3D(0.788675f, -0.211325f, -0.577350f),  FLOAT3D(0.211325f, -0.577350f, -0.788675f),
  FLOAT3D(0.426401f, -0.639602f, -0.639602f),  FLOAT3D(0.408248f, -0.408248f, -0.816497f),
  FLOAT3D(0.639602f, -0.639602f, -0.426401f),  FLOAT3D(0.639602f, -0.426401f, -0.639602f),
  FLOAT3D(0.577350f, -0.211325f, -0.788675f),  FLOAT3D(0.208847f, -0.404615f, -0.890320f),
  FLOAT3D(0.404615f, -0.208847f, -0.890320f),  FLOAT3D(0.198757f, -0.198757f, -0.959683f),
};

// Find nearest Gouraud normal for a vector.
INDEX GouraudNormal(const FLOAT3D &vNormal) {
  INDEX i, besti;
  FLOAT p, bestp;
  // find the Gouraud normal that makes smallest dot product with given vector
  besti = 0;
  bestp = 0.0f;
  for (i = 0; i < MAX_GOURAUDNORMALS; i++) {
    p = avGouraudNormals[i] % vNormal;
    if (p > bestp) {
      bestp = p;
      besti = i;
    }
  }
  return besti;
}
