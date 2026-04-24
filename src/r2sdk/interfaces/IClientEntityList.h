#pragma once

class IClientEntity;
class CBaseHandle;

class IClientEntityList
{
public:
	void *			GetClientNetworkable(int entnum);
	virtual void*		GetClientNetworkableFromHandle(CBaseHandle hEnt) = 0;
	virtual void*			GetClientUnknownFromHandle(CBaseHandle hEnt) = 0;
	IClientEntity*					GetClientEntity(int entnum);
	virtual IClientEntity*			GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	int								NumberOfEntities(bool bIncludeNonNetworkable);
	int								GetHighestEntityIndex(void);
	void							SetMaxEntities(int maxents);
	int								GetMaxEntities();
};