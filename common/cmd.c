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
// cmd.c
//

#include "common.h"

#define MAX_CMD_HASH	128

cmdFunc_t		*com_commandList;
static cmdFunc_t *com_cmdHashTree[MAX_CMD_HASH];

qBool			com_cmdWait;

static int		com_cmdArgc;
static char		*com_cmdArgv[MAX_STRING_TOKENS];
static char		com_cmdArgs[MAX_STRING_CHARS];

// ==========================================================================

/*
============
Cmd_Exists
============
*/
void *Cmd_Exists (char *cmdName)
{
	cmdFunc_t	*cmd;
	uint32		hash;

	if (!cmdName)
		return NULL;

	hash = Com_HashGeneric (cmdName, MAX_CMD_HASH);
	for (cmd=com_cmdHashTree[hash] ; cmd ; cmd=cmd->hashNext) {
		if (!Q_stricmp (cmdName, cmd->name))
			return cmd;
	}

	return NULL;
}


/*
============
Cmd_CallBack
============
*/
void Cmd_CallBack (void (*callBack) (const char *name))
{
	cmdFunc_t	*cmd;

	for (cmd=com_commandList ; cmd ; cmd=cmd->next) {
		callBack (cmd->name);
	}
}

/*
=============================================================================

	COMMAND EXECUTION

=============================================================================
*/

/*
============
Cmd_Argc
============
*/
int Cmd_Argc (void)
{
	return com_cmdArgc;
}


/*
============
Cmd_Argv
============
*/
char *Cmd_Argv (int arg)
{
	if (arg >= com_cmdArgc)
		return "";
	return com_cmdArgv[arg];	
}


/*
============
Cmd_Args

Returns a single string containing argv(1) to argv(argc()-1)
============
*/
char *Cmd_Args (void)
{
	return com_cmdArgs;
}


/*
======================
Cmd_MacroExpandString
======================
*/
char *Cmd_MacroExpandString (char *text)
{
	static	char	expanded[MAX_STRING_CHARS];
	int		i, j, count, len;
	qBool	inquote;
	char	*scan;
	char	temporary[MAX_STRING_CHARS];
	char	*token, *start;
	parse_t	*ps;

	inquote = qFalse;
	scan = text;

	len = strlen (scan);
	if (len >= MAX_STRING_CHARS) {
		Com_Printf (PRNT_WARNING, "Line exceeded %i chars, discarded.\n", MAX_STRING_CHARS);
		return NULL;
	}

	count = 0;
	for (i=0 ; i<len ; i++) {
		if (scan[i] == '"')
			inquote ^= 1;
		if (inquote)
			continue;	// Don't expand inside quotes
		if (scan[i] != '$')
			continue;

		// Scan out the complete macro
		start = scan+i+1;
		ps = Com_BeginParseSession (&start);
		token = Com_Parse (ps);
		if (!start)
			continue;
	
		token = Cvar_GetStringValue (token);
		Com_EndParseSession (ps);

		j = strlen (token);
		len += j;
		if (len >= MAX_STRING_CHARS) {
			Com_Printf (PRNT_WARNING, "Expanded line exceeded %i chars, discarded.\n", MAX_STRING_CHARS);
			return NULL;
		}

		strncpy (temporary, scan, i);
		strcpy (temporary+i, token);
		strcpy (temporary+i+j, start);

		strcpy (expanded, temporary);
		scan = expanded;
		i--;

		if (++count == 100) {
			Com_Printf (PRNT_WARNING, "Macro expansion loop, discarded.\n");
			return NULL;
		}
	}

	if (inquote) {
		Com_Printf (PRNT_WARNING, "Line has unmatched quote, discarded.\n");
		return NULL;
	}

	return scan;
}


/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
$Cvars will be expanded unless they are in a quoted token
============
*/
void Cmd_TokenizeString (char *text, qBool macroExpand)
{
	parse_t	*ps;
	char	*token;
	int		i;

	// Clear the args from the last string
	for (i=0 ; i<com_cmdArgc ; i++)
		Mem_Free (com_cmdArgv[i]);

	com_cmdArgc = 0;
	com_cmdArgs[0] = 0;
	
	// Macro expand the text
	if (macroExpand)
		text = Cmd_MacroExpandString (text);

	if (!text)
		return;

	for ( ; ; ) {
		// Skip whitespace up to an \n
		while (*text && *text <= ' ' && *text != '\n')
			text++;
		
		if (*text == '\n') {
			// A newline seperates commands in the buffer
			text++;
			break;
		}

		if (!*text)
			return;

		// Set com_cmdArgs to everything after the first arg
		if (com_cmdArgc == 1) {
			int		l;

			Q_strcatz (com_cmdArgs, text, sizeof (com_cmdArgs));

			// Strip off any trailing whitespace
			l = strlen (com_cmdArgs) - 1;
			for ( ; l>=0 ; l--)
				if (com_cmdArgs[l] <= ' ')
					com_cmdArgs[l] = 0;
				else
					break;
		}

		ps = Com_BeginParseSession (&text);
		token = Com_Parse (ps);
		if (!text) {
			Com_EndParseSession (ps);
			return;
		}

		if (com_cmdArgc < MAX_STRING_TOKENS) {
			com_cmdArgv[com_cmdArgc] = Mem_StrDup (token);
			com_cmdArgc++;
		}
		Com_EndParseSession (ps);
	}
}


/*
============
Cmd_ExecuteString

A complete command line has been parsed, so try to execute it
FIXME: lookupnoadd the token to speed search?
============
*/
void Cmd_ExecuteString (char *text)
{	
	cmdFunc_t	*cmd;
	aliasCmd_t	*alias;

	Cmd_TokenizeString (text, qTrue);
			
	// Execute the command line
	if (!Cmd_Argc ())
		return;		// no tokens

	// Check functions
	cmd = Cmd_Exists (com_cmdArgv[0]);
	if (cmd) {
		if (!cmd->function) {
			// forward to server command
			Cmd_ExecuteString (Q_VarArgs ("cmd %s", text));
		}
		else
			cmd->function ();
		return;
	}

	// Check alias
	alias = Alias_Exists (com_cmdArgv[0]);
	if (alias) {
		if (++com_aliasCount == MAX_ALIAS_LOOPS) {
			Com_Printf (PRNT_WARNING, "Cmd_ExecuteString: MAX_ALIAS_LOOPS hit\n");
			return;
		}

		Cbuf_InsertText (Q_VarArgs ("%s\n", alias->value));
		return;
	}
	
	// Check cvars
	if (Cvar_Command ())
		return;

#ifndef DEDICATED_ONLY
	// Send it as a server command if we are connected
	if (!dedicated->integer && CL_ForwardCmdToServer ())
		return;
#endif

	// Command unknown
	Com_Printf (0, "Unknown command \"%s" S_STYLE_RETURN "\"\n", Cmd_Argv (0));
}


/*
============
Cmd_AddCommand
============
*/
void *Cmd_AddCommand (char *cmdName, void (*function) (void), char *description)
{
	cmdFunc_t	*cmd;

	// Check if it's already a cmd
	if (Cmd_Exists (cmdName)) {
		Com_Printf (PRNT_WARNING, "Cmd_AddCommand: \"%s\" already defined as a command\n", cmdName);
		return NULL;
	}
	
	// Check if it's already a cvar
	if (Cvar_Exists (cmdName)) {
		Com_Printf (PRNT_WARNING, "Cmd_AddCommand: \"%s\" already defined as a cvar\n", cmdName);
		return NULL;
	}

	// Overwrite aliases
	if (Alias_Exists (cmdName)) {
		Com_Printf (PRNT_WARNING, "Cmd_AddCommand: overwriting alias \"%s\" with a command\n", cmdName);
		Alias_RemoveAlias (cmdName);
	}

	// Allocate
	cmd = Mem_Alloc (sizeof (cmdFunc_t));
	cmd->hashValue = Com_HashGeneric (cmdName, MAX_CMD_HASH);

	// Fill it in
	cmd->name = Mem_StrDup (cmdName);
	cmd->function = function;
	cmd->description = description;

	// Link it in
	cmd->next = com_commandList;
	com_commandList = cmd;

	// Link it into the hash tree
	cmd->hashNext = com_cmdHashTree[cmd->hashValue];
	com_cmdHashTree[cmd->hashValue] = cmd;

	return cmd;
}


/*
============
Cmd_RemoveCommand
============
*/
void Cmd_RemoveCommand (char *cmdName, void *cmdPtr)
{
	cmdFunc_t	*cmd, **prev;

	if (!cmdName && !cmdPtr) {
		Com_Printf (PRNT_WARNING, "Cmd_RemoveCommand: cmdName and cmdPtr NULL\n");
		return;
	}

	if (cmdPtr) {
		// De-link it from command list
		prev = &com_commandList;
		for ( ; ; ) {
			cmd = *prev;
			if (!cmd) {
				Com_Printf (PRNT_WARNING, "Cmd_RemoveCommand: %s not added\n", cmdName);
				return;
			}

			if (cmd == cmdPtr) {
				*prev = cmd->next;
				break;
			}
			prev = &cmd->next;
		}

		// De-link it from hash list
		prev = &com_cmdHashTree[cmd->hashValue];
		for ( ; ; ) {
			cmd = *prev;
			if (!cmd) {
				Com_Printf (PRNT_WARNING, "Cmd_RemoveCommand: %s not added to hash list\n", cmdName);
				return;
			}

			if (cmd == cmdPtr) {
				*prev = cmd->hashNext;
				Mem_Free (cmd->name);
				Mem_Free (cmd);
				return;
			}
			prev = &cmd->hashNext;
		}
	}
	else {
		// De-link it from command list
		prev = &com_commandList;
		for ( ; ; ) {
			cmd = *prev;
			if (!cmd) {
				Com_Printf (PRNT_WARNING, "Cmd_RemoveCommand: %s not added\n", cmdName);
				return;
			}

			if (!Q_stricmp (cmdName, cmd->name)) {
				*prev = cmd->next;
				break;
			}
			prev = &cmd->next;
		}

		// De-link it from hash list
		prev = &com_cmdHashTree[cmd->hashValue];
		for ( ; ; ) {
			cmd = *prev;
			if (!cmd) {
				Com_Printf (PRNT_WARNING, "Cmd_RemoveCommand: %s not added\n", cmdName);
				return;
			}

			if (!Q_stricmp (cmdName, cmd->name)) {
				*prev = cmd->hashNext;
				Mem_Free (cmd->name);
				Mem_Free (cmd);
				return;
			}
			prev = &cmd->hashNext;
		}
	}
}

/*
==============================================================================

	CONSOLE COMMANDS

==============================================================================
*/

/*
===============
Cmd_Exec_f
===============
*/
static void Cmd_Exec_f (void)
{
	char	*f, *f2;
	int		len;

	if (Cmd_Argc () != 2) {
		Com_Printf (0, "syntax: exec <filename> : execute a script file\n");
		return;
	}

	len = FS_LoadFile (Cmd_Argv (1), (void **)&f);
	if (!f) {
		Com_Printf (PRNT_WARNING, "couldn't exec %s\n", Cmd_Argv (1));
		return;
	}
	Com_Printf (0, "executing %s\n", Cmd_Argv (1));

	// The file doesn't have a trailing \n\0, so we need to put it there
	f2 = Mem_Alloc (len + 2);
	memcpy (f2, f, len);
	f2[len] = '\n';
	f2[len+1] = '\0';

	Cbuf_InsertText (f2);

	Mem_Free (f2);
	FS_FreeFile (f);
}


/*
===============
Cmd_Echo_f

Just prints the rest of the line to the console
===============
*/
static void Cmd_Echo_f (void)
{
	int		i;
	
	for (i=1 ; i<Cmd_Argc () ; i++) {
		Com_Printf (0, "%s ", Cmd_Argv (i));
	}

	Com_Printf (0, "\n");
}


/*
============
Cmd_List_f
============
*/
static int alphaSortCmp (const void *_a, const void *_b)
{
	const cmdFunc_t *a = (const cmdFunc_t *) _a;
	const cmdFunc_t *b = (const cmdFunc_t *) _b;

	return Q_stricmp (a->name, b->name);
}
static void Cmd_List_f (void) {
	cmdFunc_t	*cmd, *sortedList;
	int			i, j, total;
	int			matching, longest;
	char		*wildCard;

	if ((Cmd_Argc () != 1) && (Cmd_Argc () != 2)) {
		Com_Printf (0, "usage: cmdlist [wildcard]\n");
		return;
	}

	if (Cmd_Argc () == 2)
		wildCard = Cmd_Argv (1);
	else
		wildCard = "*";

	// Create a list and get longest cmd length
	for (matching=0, total=0, cmd=com_commandList ; cmd ; cmd=cmd->next, total++) {
		if (!Q_WildcardMatch (wildCard, cmd->name, 1))
			continue;

		matching++;
	}

	if (!matching) {
		Com_Printf (0, "%i commands total, %i matching\n", total, matching);
		return;
	}

	sortedList = Mem_Alloc (matching * sizeof (cmdFunc_t));
	for (matching=0, longest=0, cmd=com_commandList ; cmd ; cmd=cmd->next) {
		if (!Q_WildcardMatch (wildCard, cmd->name, 1))
			continue;

		if ((int)strlen (cmd->name) > longest)
			longest = strlen (cmd->name);

		sortedList[matching++] = *cmd;
	}

	// Sort it
	qsort (sortedList, matching, sizeof (sortedList[0]), alphaSortCmp);

	// Print it
	longest++;
	for (j=0 ; j<matching ;  j++) {
		cmd = &sortedList[j];

		Com_Printf (0, "%s ", cmd->name);
		for (i=0 ; i<longest-(int)strlen(cmd->name) ; i++)
			Com_Printf (0, " ");
		Com_Printf (0, "%s\n", cmd->description);
	}

	if (matching)
		Mem_Free (sortedList);
	Com_Printf (0, "%i commands total, %i matching\n", total, matching);
}


/*
============
Cmd_Wait_f

Causes execution of the remainder of the command buffer to be delayed until next frame.
This allows commands like: bind g "impulse 5 ; +attack ; wait ; -attack ; impulse 2"
============
*/
static void Cmd_Wait_f (void)
{
	com_cmdWait = qTrue;
}

/*
=============================================================================

	INITIALIZATION

=============================================================================
*/

/*
============
Cmd_Init
============
*/
void Cmd_Init (void)
{
	memset (com_cmdHashTree, 0, sizeof (com_cmdHashTree));

	Cmd_AddCommand ("cmdlist",		Cmd_List_f,		"Prints out a list of commands");
	Cmd_AddCommand ("echo",			Cmd_Echo_f,		"Echos text to the console");
	Cmd_AddCommand ("exec",			Cmd_Exec_f,		"Execute a cfg file");
	Cmd_AddCommand ("wait",			Cmd_Wait_f,		"Forces remainder of command buffer to be delayed until next frame");
}
