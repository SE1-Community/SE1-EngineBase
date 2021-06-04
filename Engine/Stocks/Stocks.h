/* Copyright (c) 2021 Dreamy Cecil
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

#ifndef SE_INCL_STOCKS_H
#define SE_INCL_STOCKS_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Templates/Stock.h>

// [Cecil] 2021-06-05: Declare stock pointers

#include <Engine/Anim/AnimData.h>
ENGINE_API extern CStock<CAnimData> *_pAnimStock;

#include <Engine/Entities/EntityClass.h>
ENGINE_API extern CStock<CEntityClass> *_pEntityClassStock;

#include <Engine/Graphics/Shader.h>
ENGINE_API extern CStock<CShader> *_pShaderStock;

#include <Engine/Graphics/Texture.h>
ENGINE_API extern CStock<CTextureData> *_pTextureStock;

#include <Engine/Models/ModelData.h>
ENGINE_API extern CStock<CModelData> *_pModelStock;

#include <Engine/SKA/AnimSet.h>
ENGINE_API extern CStock<CAnimSet> *_pAnimSetStock;

#include <Engine/SKA/Mesh.h>
ENGINE_API extern CStock<CMesh> *_pMeshStock;

#include <Engine/SKA/Skeleton.h>
ENGINE_API extern CStock<CSkeleton> *_pSkeletonStock;

#include <Engine/Sound/SoundData.h>
ENGINE_API extern CStock<CSoundData> *_pSoundStock;

#endif /* include-once check. */
