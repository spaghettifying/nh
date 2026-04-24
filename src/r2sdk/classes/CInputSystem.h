#pragma once

#include "../ButtonCode.h"
#include "../../utilities/Memory.h"
#include "CGlobalVarsBase.h"

class CInputSystem
{
public:
	bool IsButtonDown(ButtonCode_t Button)
	{
		return Memory::CallVirtual<13, bool>(this, Button);
	}

	bool KeyPressed(ButtonCode_t Code, int Time = 9)
	{
		if (!g_pGlobalVars)
			return IsButtonDown(Code);

		static int PreviousTick = 0;
		int CurrentTick = g_pGlobalVars->tickcount;

		if ((CurrentTick - PreviousTick) < 0)	
			PreviousTick = 0;
		
		if (IsButtonDown(Code) && (CurrentTick - PreviousTick) >= Time) //arbitrary number
		{
			PreviousTick = CurrentTick;
			return true;
		}
		return false;
	}
};

inline CInputSystem* InputSystem;