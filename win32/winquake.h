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
// winquake.h
// Win32-specific Quake header file
//

#pragma warning (disable : 4229)  // mgraph gets this

#include <windows.h>
#include <dsound.h>

#define WINDOW_APP_NAME		"EGL v"EGL_VERSTR
#define	WINDOW_CLASS_NAME	"EGL"

extern HINSTANCE	globalhInst;
extern HWND			globalhWnd;

extern qBool		sysWindows;
extern uInt			sysMsgTime;

extern qBool		sysActiveApp;
extern qBool		sysMinimized;

//
// input
//

void	IN_Activate (qBool active);
void	IN_MouseEvent (int mstate);

//
// qhost
//

void	ConProcInit (int argc, char **argv);
void	ConProcShutdown (void);

//
// console window
//

qBool	SysConsole_Init (void);
void	SysConsole_Shutdown (void);
