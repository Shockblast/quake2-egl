/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// rf_local.h
//

#include "r_local.h"

#define ENTLIST_OFFSET	2
#define MODLIST_OFFSET	1

/*
=============================================================================

	MESH BUFFERING

=============================================================================
*/

meshBuffer_t *R_AddMeshToList (material_t *mat, float matTime, refEntity_t *ent, struct mQ3BspFog_s *fog, meshType_t meshType, void *mesh);

/*
===============================================================================

	FUNCTION PROTOTYPES

===============================================================================
*/

//
// rf_2d.c
//
void		RF_2DInit (void);
