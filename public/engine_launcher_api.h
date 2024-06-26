/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
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

#ifndef ENGINE_LAUNCHER_API_H
#define ENGINE_LAUNCHER_API_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

#ifdef _WIN32
	#define ENGINE_CLIENT_LIB "hw.dll"
	#define ENGINE_CLIENT_SOFT_LIB "sw.dll"
#else
	#define ENGINE_CLIENT_LIB "hw.so"
#endif // _WIN32

enum EngineRunResult
{
    ENGRUN_QUITTING = 0,
    ENGRUN_CHANGED_VIDEOMODE = 1,
    ENGRUN_UNSUPPORTED_VIDEOMODE = 2
};

class IEngineAPI : public IBaseInterface
{
public:
	virtual EngineRunResult Run(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) = 0;
};

#define VENGINE_LAUNCHER_API_VERSION "VENGINE_LAUNCHER_API_VERSION002"

#endif // ENGINE_LAUNCHER_API_H
