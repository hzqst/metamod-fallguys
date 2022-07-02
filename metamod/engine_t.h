// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// engine_t.h - The engine_t type

/*
 * Copyright (c) 2001-2006 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#ifndef MM_ENGINE_T_H
#define MM_ENGINE_T_H

#include "eiface.h"             // engfuncs_t, globalvars_t
#include "engineinfo.h"         // EngineInfo
#include "comp_dep.h"
#include "osdep.h"	//unlikely, OPEN_ARGS


#include "studio.h"		// me
#include "r_studioint.h"		// me


// Our structure for storing engine references.
struct engine_t {
	engine_t() DLLINTERNAL;
	engine_t(const engine_t&) DLLINTERNAL;
	engine_t& operator=(const engine_t&) DLLINTERNAL;

	enginefuncs_t	*funcs;			// engine funcs
	globalvars_t	*globals;		// engine globals
	enginefuncs_t	*pl_funcs;		// "modified" eng funcs we give to plugins
	sv_blending_interface_t *engine_studioblend;
	server_studio_api_t *engine_studioapi;
	float(*engine_rotationmatrix)[3][4];
	float(*engine_bonetransform)[MAXSTUDIOBONES][3][4];
	EngineInfo       info;          // some special info elements
};

inline engine_t::engine_t() 
    : funcs(NULL), globals(NULL), pl_funcs(NULL), 
	engine_studioblend(NULL), engine_studioapi(NULL), engine_rotationmatrix(NULL), engine_bonetransform(NULL), info()
{
}


inline engine_t::engine_t(const engine_t& _rhs) 
    : funcs(_rhs.funcs), globals(_rhs.globals), pl_funcs(_rhs.pl_funcs), 
	engine_studioblend(_rhs.engine_studioblend), engine_studioapi(_rhs.engine_studioapi), engine_rotationmatrix(_rhs.engine_rotationmatrix), engine_bonetransform(_rhs.engine_bonetransform), info(_rhs.info)
{
}


inline engine_t& engine_t::operator=(const engine_t& _rhs) 
{
    funcs = _rhs.funcs;
    globals = _rhs.globals;
    pl_funcs = _rhs.pl_funcs;
	engine_studioblend = _rhs.engine_studioblend;
	engine_studioapi = _rhs.engine_studioapi;
	engine_rotationmatrix = _rhs.engine_rotationmatrix;
	engine_bonetransform = _rhs.engine_bonetransform;
    info = _rhs.info;
    return *this;
}


extern engine_t Engine DLLHIDDEN;

#endif /* MM_ENGINE_T_H */

