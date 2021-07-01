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
// cg_draw.c
//

#include "cg_local.h"

/*
================
CG_DrawModel
================
*/
void CG_DrawModel (int x, int y, int w, int h, struct model_s *model, struct image_s *skin, vec3_t origin, vec3_t angles) {
	refDef_t	refDef;
	entity_t	entity;

	if (!model)
		return;

	// set up refDef
	memset (&refDef, 0, sizeof (refDef));

	refDef.x = x;
	refDef.y = y;
	refDef.width = w;
	refDef.height = h;
	refDef.fovX = 30;
	refDef.fovY = 30;
	refDef.time = cg.time * 0.001;
	refDef.rdFlags = RDF_NOWORLDMODEL;

	// set up the entity
	memset (&entity, 0, sizeof (entity));

	entity.model = model;
	entity.skin = skin;
	entity.scale = 1.0f;
	entity.flags = RF_FULLBRIGHT | RF_NOSHADOW | RF_FORCENOLOD;
	VectorCopy (origin, entity.origin);
	VectorCopy (entity.origin, entity.oldOrigin);

	AnglesToAxis (angles, entity.axis);

	cgi.R_ClearScene ();
	cgi.R_AddEntity (&entity);
	cgi.R_RenderScene (&refDef);
}
