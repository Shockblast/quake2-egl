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
// m_vid_exts.c
//

#include "m_local.h"

/*
====================================================================

	OPENGL EXTENSIONS MENU

====================================================================
*/

typedef struct m_extensionsMenu_s {
	// Menu items
	uiFrameWork_t		frameWork;

	uiImage_t			banner;
	uiAction_t			header;

	uiList_t			extensions_toggle;

	uiList_t			multitexture_toggle;

	uiList_t			mtexcombine_toggle;

	uiList_t			cubemap_toggle;
	uiList_t			edgeclamp_toggle;
	uiList_t			genmipmap_toggle;
	uiList_t			texcompress_toggle;

	uiList_t			aniso_toggle;
	uiSlider_t			anisoamount_slider;

	uiAction_t			apply_action;
	uiAction_t			reset_action;
	uiAction_t			cancel_action;
} m_extensionsMenu_t;

static m_extensionsMenu_t	m_extensionsMenu;

static void GLExtsMenu_SetValues (void);
static void ResetDefaults (void *unused)
{
	GLExtsMenu_SetValues ();
}

static void CancelChanges (void *unused)
{
	M_PopMenu ();
}


/*
=============
GLExtsMenu_ApplyValues
=============
*/
static void GLExtsMenu_ApplyValues (void *unused)
{
	cgi.Cvar_SetValue ("r_allowExtensions",					m_extensionsMenu.extensions_toggle.curValue, qTrue);

	cgi.Cvar_SetValue ("r_ext_multitexture",				m_extensionsMenu.multitexture_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_textureEnvCombine",			m_extensionsMenu.mtexcombine_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_textureCubeMap",				m_extensionsMenu.cubemap_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_textureEdgeClamp",			m_extensionsMenu.edgeclamp_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_generateMipmap",				m_extensionsMenu.genmipmap_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_textureCompression",			m_extensionsMenu.texcompress_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_textureFilterAnisotropic",	m_extensionsMenu.aniso_toggle.curValue, qTrue);
	cgi.Cvar_SetValue ("r_ext_maxAnisotropy",				m_extensionsMenu.anisoamount_slider.curValue, qTrue);

	cgi.Cbuf_AddText ("vid_restart\n");
}


/*
=============
GLExtsMenu_SetValues
=============
*/
static void GLExtsMenu_SetValues (void)
{
	cgi.Cvar_SetValue ("r_allowExtensions",					clamp (cgi.Cvar_GetIntegerValue ("r_allowExtensions"), 0, 1), qTrue);
	m_extensionsMenu.extensions_toggle.curValue				= cgi.Cvar_GetIntegerValue ("r_allowExtensions");

	cgi.Cvar_SetValue ("r_ext_multitexture",				clamp (cgi.Cvar_GetIntegerValue ("r_ext_multitexture"), 0, 1), qTrue);
	m_extensionsMenu.multitexture_toggle.curValue			= cgi.Cvar_GetIntegerValue ("r_ext_multitexture");

	cgi.Cvar_SetValue ("r_ext_textureEnvCombine",			clamp (cgi.Cvar_GetIntegerValue ("r_ext_textureEnvCombine"), 0, 1), qTrue);
	m_extensionsMenu.mtexcombine_toggle.curValue			= cgi.Cvar_GetIntegerValue ("r_ext_textureEnvCombine");

	cgi.Cvar_SetValue ("r_ext_textureCubeMap",				clamp (cgi.Cvar_GetIntegerValue ("r_ext_textureCubeMap"), 0, 1), qTrue);
	m_extensionsMenu.cubemap_toggle.curValue				= cgi.Cvar_GetIntegerValue ("r_ext_textureCubeMap");

	cgi.Cvar_SetValue ("r_ext_textureEdgeClamp",			clamp (cgi.Cvar_GetIntegerValue ("r_ext_textureEdgeClamp"), 0, 1), qTrue);
	m_extensionsMenu.edgeclamp_toggle.curValue				= cgi.Cvar_GetIntegerValue ("r_ext_textureEdgeClamp");

	cgi.Cvar_SetValue ("r_ext_generateMipmap",				clamp (cgi.Cvar_GetIntegerValue ("r_ext_generateMipmap"), 0, 1), qTrue);
	m_extensionsMenu.genmipmap_toggle.curValue				= cgi.Cvar_GetIntegerValue ("r_ext_generateMipmap");

	cgi.Cvar_SetValue ("r_ext_textureCompression",			clamp (cgi.Cvar_GetIntegerValue ("r_ext_textureCompression"), 0, 5), qTrue);
	m_extensionsMenu.texcompress_toggle.curValue			= cgi.Cvar_GetIntegerValue ("r_ext_textureCompression");

	cgi.Cvar_SetValue ("r_ext_textureFilterAnisotropic",	clamp (cgi.Cvar_GetIntegerValue ("r_ext_textureFilterAnisotropic"), 0, 1), qTrue);
	m_extensionsMenu.aniso_toggle.curValue					= cgi.Cvar_GetIntegerValue ("r_ext_textureFilterAnisotropic");

	cgi.Cvar_SetValue ("r_ext_maxAnisotropy",				clamp (cgi.Cvar_GetIntegerValue ("r_ext_maxAnisotropy"), 0, 16), qTrue);
	m_extensionsMenu.anisoamount_slider.curValue			= cgi.Cvar_GetIntegerValue ("r_ext_maxAnisotropy");
}


/*
=============
GLExtsMenu_Init
=============
*/
static void GLExtsMenu_Init (void)
{
	static char *compression_names[] = {
		"off",
		"ARB",
		"DXT1",
		"DXT3",
		"DXT5",
		"S3TC",
		0
	};

	static char *yesno_names[] = {
		"no",
		"yes",
		0
	};

	static char *fastnice_names[] = {
		"fastest",
		"nicest",
		0
	};

	UI_StartFramework (&m_extensionsMenu.frameWork, FWF_CENTERHEIGHT);

	m_extensionsMenu.banner.generic.type		= UITYPE_IMAGE;
	m_extensionsMenu.banner.generic.flags		= UIF_NOSELECT|UIF_CENTERED;
	m_extensionsMenu.banner.generic.name		= NULL;
	m_extensionsMenu.banner.shader				= uiMedia.banners.video;

	m_extensionsMenu.header.generic.type		= UITYPE_ACTION;
	m_extensionsMenu.header.generic.flags		= UIF_NOSELECT|UIF_CENTERED|UIF_MEDIUM|UIF_SHADOW;
	m_extensionsMenu.header.generic.name		= "OpenGL Extensions";

	m_extensionsMenu.extensions_toggle.generic.type			= UITYPE_SPINCONTROL;
	m_extensionsMenu.extensions_toggle.generic.name			= "Extensions";
	m_extensionsMenu.extensions_toggle.itemNames			= yesno_names;
	m_extensionsMenu.extensions_toggle.generic.statusBar	= "Toggles the use of any of the below extensions entirely";

	m_extensionsMenu.multitexture_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.multitexture_toggle.generic.name		= "Multitexture";
	m_extensionsMenu.multitexture_toggle.itemNames			= yesno_names;
	m_extensionsMenu.multitexture_toggle.generic.statusBar	= "Multitexturing (increases performance)";

	m_extensionsMenu.mtexcombine_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.mtexcombine_toggle.generic.name		= "Multitexture Combine";
	m_extensionsMenu.mtexcombine_toggle.itemNames			= yesno_names;
	m_extensionsMenu.mtexcombine_toggle.generic.statusBar	= "Multitexture Combine Extension";

	m_extensionsMenu.cubemap_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.cubemap_toggle.generic.name		= "Cube Mapping";
	m_extensionsMenu.cubemap_toggle.itemNames			= yesno_names;
	m_extensionsMenu.cubemap_toggle.generic.statusBar	= "Enables the use of cubemapping";

	m_extensionsMenu.edgeclamp_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.edgeclamp_toggle.generic.name		= "Edge Clamping";
	m_extensionsMenu.edgeclamp_toggle.itemNames			= yesno_names;
	m_extensionsMenu.edgeclamp_toggle.generic.statusBar	= "Use an extension for clamping specified textures";

	m_extensionsMenu.genmipmap_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.genmipmap_toggle.generic.name		= "Mipmap Generation";
	m_extensionsMenu.genmipmap_toggle.itemNames			= yesno_names;
	m_extensionsMenu.genmipmap_toggle.generic.statusBar	= "Hardware mipmap generation";

	m_extensionsMenu.texcompress_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.texcompress_toggle.generic.name		= "Texture Compression";
	m_extensionsMenu.texcompress_toggle.itemNames			= compression_names;
	m_extensionsMenu.texcompress_toggle.generic.statusBar	= "Texture compression (quality sapping, performance increasing)";

	m_extensionsMenu.aniso_toggle.generic.type		= UITYPE_SPINCONTROL;
	m_extensionsMenu.aniso_toggle.generic.name		= "Anisotropic Filtering";
	m_extensionsMenu.aniso_toggle.itemNames			= yesno_names;
	m_extensionsMenu.aniso_toggle.generic.statusBar	= "Anisotropic mipmap filtering";

	m_extensionsMenu.anisoamount_slider.generic.type		= UITYPE_SLIDER;
	m_extensionsMenu.anisoamount_slider.generic.name		= "Maximum Anisotropy";
	m_extensionsMenu.anisoamount_slider.minValue			= 1;
	m_extensionsMenu.anisoamount_slider.maxValue			= cg.refConfig.maxAniso ? cg.refConfig.maxAniso : 16;
	m_extensionsMenu.anisoamount_slider.generic.statusBar	= "Maximum anisotropic filtering";

	m_extensionsMenu.apply_action.generic.type			= UITYPE_ACTION;
	m_extensionsMenu.apply_action.generic.flags			= UIF_CENTERED|UIF_MEDIUM|UIF_SHADOW;
	m_extensionsMenu.apply_action.generic.name			= "Apply";
	m_extensionsMenu.apply_action.generic.callBack		= GLExtsMenu_ApplyValues;
	m_extensionsMenu.apply_action.generic.statusBar		= "Apply Changes";

	m_extensionsMenu.reset_action.generic.type			= UITYPE_ACTION;
	m_extensionsMenu.reset_action.generic.flags			= UIF_CENTERED|UIF_MEDIUM|UIF_SHADOW;
	m_extensionsMenu.reset_action.generic.name			= "Reset";
	m_extensionsMenu.reset_action.generic.callBack		= ResetDefaults;
	m_extensionsMenu.reset_action.generic.statusBar		= "Reset Changes";

	m_extensionsMenu.cancel_action.generic.type			= UITYPE_ACTION;
	m_extensionsMenu.cancel_action.generic.flags		= UIF_CENTERED|UIF_MEDIUM|UIF_SHADOW;
	m_extensionsMenu.cancel_action.generic.name			= "Cancel";
	m_extensionsMenu.cancel_action.generic.callBack		= CancelChanges;
	m_extensionsMenu.cancel_action.generic.statusBar	= "Closes the Menu";

	GLExtsMenu_SetValues ();

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.banner);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.header);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.extensions_toggle);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.multitexture_toggle);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.mtexcombine_toggle);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.cubemap_toggle);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.edgeclamp_toggle);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.genmipmap_toggle);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.texcompress_toggle);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.aniso_toggle);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.anisoamount_slider);

	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.apply_action);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.reset_action);
	UI_AddItem (&m_extensionsMenu.frameWork,			&m_extensionsMenu.cancel_action);

	UI_FinishFramework (&m_extensionsMenu.frameWork, qTrue);
}


/*
=============
GLExtsMenu_Close
=============
*/
static struct sfx_s *GLExtsMenu_Close (void)
{
	return uiMedia.sounds.menuOut;
}


/*
=============
GLExtsMenu_Draw
=============
*/
static void GLExtsMenu_Draw (void)
{
	float	y;

	// Initialize if necessary
	if (!m_extensionsMenu.frameWork.initialized)
		GLExtsMenu_Init ();

	// Dynamically position
	m_extensionsMenu.frameWork.x			= cg.refConfig.vidWidth * 0.5f;
	m_extensionsMenu.frameWork.y			= 0;

	m_extensionsMenu.banner.generic.x		= 0;
	m_extensionsMenu.banner.generic.y		= 0;

	y = m_extensionsMenu.banner.height * UI_SCALE;

	m_extensionsMenu.header.generic.x					= 0;
	m_extensionsMenu.header.generic.y					= y += UIFT_SIZEINC;
	m_extensionsMenu.extensions_toggle.generic.x		= 0;
	m_extensionsMenu.extensions_toggle.generic.y		= y += UIFT_SIZEINC + UIFT_SIZEINCMED;
	m_extensionsMenu.multitexture_toggle.generic.x		= 0;
	m_extensionsMenu.multitexture_toggle.generic.y		= y += UIFT_SIZEINC*2;
	m_extensionsMenu.mtexcombine_toggle.generic.x		= 0;
	m_extensionsMenu.mtexcombine_toggle.generic.y		= y += UIFT_SIZEINC*2;
	m_extensionsMenu.cubemap_toggle.generic.x			= 0;
	m_extensionsMenu.cubemap_toggle.generic.y			= y += UIFT_SIZEINC;
	m_extensionsMenu.edgeclamp_toggle.generic.x			= 0;
	m_extensionsMenu.edgeclamp_toggle.generic.y			= y += UIFT_SIZEINC;
	m_extensionsMenu.genmipmap_toggle.generic.x			= 0;
	m_extensionsMenu.genmipmap_toggle.generic.y			= y += UIFT_SIZEINC;
	m_extensionsMenu.texcompress_toggle.generic.x		= 0;
	m_extensionsMenu.texcompress_toggle.generic.y		= y += UIFT_SIZEINC;
	m_extensionsMenu.aniso_toggle.generic.x				= 0;
	m_extensionsMenu.aniso_toggle.generic.y				= y += UIFT_SIZEINC*2;
	m_extensionsMenu.anisoamount_slider.generic.x		= 0;
	m_extensionsMenu.anisoamount_slider.generic.y		= y += UIFT_SIZEINC;
	m_extensionsMenu.apply_action.generic.x				= 0;
	m_extensionsMenu.apply_action.generic.y				= y += UIFT_SIZEINCMED*2;
	m_extensionsMenu.reset_action.generic.x				= 0;
	m_extensionsMenu.reset_action.generic.y				= y += UIFT_SIZEINCMED;
	m_extensionsMenu.cancel_action.generic.x			= 0;
	m_extensionsMenu.cancel_action.generic.y			= y += UIFT_SIZEINCMED;

	// Render
	UI_DrawInterface (&m_extensionsMenu.frameWork);
}


/*
=============
UI_GLExtsMenu_f
=============
*/
void UI_GLExtsMenu_f (void)
{
	GLExtsMenu_Init ();
	M_PushMenu (&m_extensionsMenu.frameWork, GLExtsMenu_Draw, GLExtsMenu_Close, M_KeyHandler);
}
