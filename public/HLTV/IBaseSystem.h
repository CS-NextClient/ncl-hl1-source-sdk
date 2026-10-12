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

#pragma once

#if defined(_WIN32)
	#define LIBRARY_PREFIX "dll"
#elif defined(OSX)
	#define LIBRARY_PREFIX "dylib"
#else
	#define LIBRARY_PREFIX "so"
#endif

#include "ISystemModule.h"
#include "IVGuiModule.h"

class Panel;
class ObjectList;
class IFileSystem;

class IBaseSystem: virtual public ISystemModule {
public:
	virtual ~IBaseSystem() {}

	virtual double GetTime() = 0;
	virtual unsigned int GetTick() = 0;
	virtual void SetFPS(float fps) = 0;

	virtual void Printf(const char *fmt, ...) = 0;
	virtual void DPrintf(const char *fmt, ...) = 0;

	virtual void RedirectOutput(char *buffer = nullptr, int maxSize = 0) = 0;

	virtual IFileSystem *GetFileSystem() = 0;
	virtual unsigned char *LoadFile(const char *name, int *length = nullptr) = 0;
	virtual void FreeFile(unsigned char *fileHandle) = 0;

	virtual void SetTitle(const char *text) = 0;
	virtual void SetStatusLine(const char *text) = 0;

	virtual void ShowConsole(bool visible) = 0;
	virtual void LogConsole(const char *filename) = 0;

	virtual bool InitVGUI(IVGuiModule *module) = 0;

#ifdef _WIN32
	virtual Panel *GetPanel() = 0;
#endif // _WIN32

	virtual bool RegisterCommand(const char *name, ISystemModule *module, int commandID) = 0;
	virtual void GetCommandMatches(const char *string, ObjectList *pMatchList) = 0;
	virtual void ExecuteString(const char *commands) = 0;
	virtual void ExecuteFile(const char *filename) = 0;
	virtual void Errorf(const char *fmt, ...) = 0;

	virtual char *CheckParam(const char *param) = 0;

	virtual bool AddModule(ISystemModule *module, const char *name) = 0;
	virtual ISystemModule *GetModule(const char *interfacename, const char *library, const char *instancename = nullptr) = 0;
	virtual bool RemoveModule(ISystemModule *module) = 0;

	virtual void Stop() = 0;
	virtual char *GetBaseDir() = 0;
};

#define BASESYSTEM_INTERFACE_VERSION "basesystem002"
