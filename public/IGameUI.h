#ifndef IGAMEUI_GS_H
#define IGAMEUI_GS_H
#include <interface.h>
#include <vmodes.h>
#include <quakedef.h>
#include <APIProxy.h>

class IGameUI : public IBaseInterface
{
public:
	virtual void Initialize(CreateInterfaceFn *factories, int count) = 0;
	virtual void Start(cl_enginefunc_t *engineFuncs, int interfaceVersion, void *system) = 0;
	virtual void Shutdown(void) = 0;
	virtual int ActivateGameUI(void) = 0;
	virtual int ActivateDemoUI(void) = 0;
	virtual int HasExclusiveInput(void) = 0;
	virtual void RunFrame(void) = 0;
	virtual void ConnectToServer(const char *game, int IP, int port) = 0;
	virtual void DisconnectFromServer(void) = 0;
	virtual void HideGameUI(void) = 0;
	virtual bool IsGameUIActive(void) = 0;
	virtual void LoadingStarted(const char *resourceType, const char *resourceName) = 0;
	virtual void LoadingFinished(const char *resourceType, const char *resourceName) = 0;
	virtual void StartProgressBar(const char *progressType, int progressSteps) = 0;
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) = 0;
	virtual void StopProgressBar(bool bError, const char *failureReason, const char *extendedReason = NULL) = 0;
	virtual int SetProgressBarStatusText(const char *statusText) = 0;
	virtual void SetSecondaryProgressBar(float progress) = 0;
	virtual void SetSecondaryProgressBarText(const char *statusText) = 0;
	virtual void ValidateCDKey(bool, bool) = 0;
	virtual void OnDisconnectFromServer(int eSteamLoginFailure, const char *username) = 0;
};

#define GAMEUI_INTERFACE_VERSION_GS "GameUI007"

#endif