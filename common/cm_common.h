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
// cm_common.h
//

#define USE_BYTESWAP
#include "common.h"

#define MAX_CM_CMODELS		1024		// Must be >= Q2BSP_MAX_MODELS and >= Q3BSP_MAX_MODELS

typedef struct cBspModel_s {
	vec3_t					mins;
	vec3_t					maxs;

	int						headNode;

	// Q3BSP
	int						nummarkfaces;
	struct cQ3BspFace_s		**markfaces;

	int						nummarkbrushes;
	struct cQ3BspBrush_s	**markbrushes;
	// !Q3BSP
} cBspModel_t;

extern int					cm_numCModels;
extern cBspModel_t			cm_mapCModels[MAX_CM_CMODELS];

extern int					cm_numTraces;
extern int					cm_numBrushTraces;
extern int					cm_numPointContents;

extern cVar_t				*flushmap;
extern cVar_t				*cm_noAreas;
extern cVar_t				*cm_noCurves;
extern cVar_t				*cm_showTrace;

/*
=============================================================================

	QUAKE2 BSP

=============================================================================
*/

cBspModel_t	*CM_Q2BSP_LoadMap (uint32 *buffer);
void		CM_Q2BSP_PrepMap (void);
void		CM_Q2BSP_UnloadMap (void);

char		*CM_Q2BSP_EntityString (void);
char		*CM_Q2BSP_SurfRName (int texNum);
int			CM_Q2BSP_NumClusters (void);
int			CM_Q2BSP_NumTexInfo (void);

// ==========================================================================

int			CM_Q2BSP_LeafArea (int leafNum);
int			CM_Q2BSP_LeafCluster (int leafNum);
int			CM_Q2BSP_LeafContents (int leafNum);

int			CM_Q2BSP_HeadnodeForBox (vec3_t mins, vec3_t maxs);
int			CM_Q2BSP_PointLeafnum (vec3_t p);
int			CM_Q2BSP_BoxLeafnums (vec3_t mins, vec3_t maxs, int *list, int listSize, int *topNode);

int			CM_Q2BSP_PointContents (vec3_t p, int headNode);
int			CM_Q2BSP_TransformedPointContents (vec3_t p, int headNode, vec3_t origin, vec3_t angles);

trace_t		CM_Q2BSP_Trace (vec3_t start, vec3_t end, float size, int contentMask);
trace_t		CM_Q2BSP_BoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int headNode, int brushMask);
trace_t		CM_Q2BSP_TransformedBoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs,
						  int headNode, int brushMask, vec3_t origin, vec3_t angles);

byte		*CM_Q2BSP_ClusterPVS (int cluster);
byte		*CM_Q2BSP_ClusterPHS (int cluster);

void		CM_Q2BSP_SetAreaPortalState (int portalNum, qBool open);
qBool		CM_Q2BSP_AreasConnected (int area1, int area2);
int			CM_Q2BSP_WriteAreaBits (byte *buffer, int area);
void		CM_Q2BSP_WritePortalState (fileHandle_t fileNum);
void		CM_Q2BSP_ReadPortalState (fileHandle_t fileNum);
qBool		CM_Q2BSP_HeadnodeVisible (int nodeNum, byte *visBits);

/*
=============================================================================

	QUAKE3 BSP

=============================================================================
*/

cBspModel_t	*CM_Q3BSP_LoadMap (uint32 *buffer);
void		CM_Q3BSP_PrepMap (void);
void		CM_Q3BSP_UnloadMap (void);

char		*CM_Q3BSP_EntityString (void);
char		*CM_Q3BSP_SurfRName (int texNum);
int			CM_Q3BSP_NumClusters (void);
int			CM_Q3BSP_NumTexInfo (void);

// ==========================================================================

int			CM_Q3BSP_LeafArea (int leafNum);
int			CM_Q3BSP_LeafCluster (int leafNum);
int			CM_Q3BSP_LeafContents (int leafNum);

int			CM_Q3BSP_HeadnodeForBox (vec3_t mins, vec3_t maxs);
int			CM_Q3BSP_PointLeafnum (vec3_t p);
int			CM_Q3BSP_BoxLeafnums (vec3_t mins, vec3_t maxs, int *list, int listSize, int *topNode);

int			CM_Q3BSP_PointContents (vec3_t p, int headNode);
int			CM_Q3BSP_TransformedPointContents (vec3_t p, int headNode, vec3_t origin, vec3_t angles);

trace_t		CM_Q3BSP_Trace (vec3_t start, vec3_t end, float size, int contentMask);
trace_t		CM_Q3BSP_BoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int headNode, int brushMask);
trace_t		CM_Q3BSP_TransformedBoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs,
						  int headNode, int brushMask, vec3_t origin, vec3_t angles);

byte		*CM_Q3BSP_ClusterPVS (int cluster);
byte		*CM_Q3BSP_ClusterPHS (int cluster);

void		CM_Q3BSP_SetAreaPortalState (int portalNum, int area, int otherArea, qBool open);
qBool		CM_Q3BSP_AreasConnected (int area1, int area2);
int			CM_Q3BSP_WriteAreaBits (byte *buffer, int area);
void		CM_Q3BSP_WritePortalState (fileHandle_t fileNum);
void		CM_Q3BSP_ReadPortalState (fileHandle_t fileNum);
qBool		CM_Q3BSP_HeadnodeVisible (int nodeNum, byte *visBits);
