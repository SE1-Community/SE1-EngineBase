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

#ifndef SE_INCL_STOCKS_DEFINITION_H
#define SE_INCL_STOCKS_DEFINITION_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Stocks/Stocks.h>

// [Cecil] 2021-06-05: Define stock pointers

CStock<CAnimData> *_pAnimStock = NULL;
CStock<CEntityClass> *_pEntityClassStock = NULL;
CStock<CShader> *_pShaderStock = NULL;
CStock<CTextureData> *_pTextureStock = NULL;
CStock<CModelData> *_pModelStock = NULL;
CStock<CAnimSet> *_pAnimSetStock = NULL;
CStock<CMesh> *_pMeshStock = NULL;
CStock<CSkeleton> *_pSkeletonStock = NULL;
CStock<CSoundData> *_pSoundStock = NULL;

// [Cecil] 2021-06-05: Create stocks
ENGINE_API extern void SE_CreateStocks(void) {
  _pAnimStock = new CStock<CAnimData>;
  _pEntityClassStock = new CStock<CEntityClass>;
  _pShaderStock = new CStock<CShader>;
  _pTextureStock = new CStock<CTextureData>;
  _pModelStock = new CStock<CModelData>;
  _pAnimSetStock = new CStock<CAnimSet>;
  _pMeshStock = new CStock<CMesh>;
  _pSkeletonStock = new CStock<CSkeleton>;
  _pSoundStock = new CStock<CSoundData>;
};

#endif /* include-once check. */
