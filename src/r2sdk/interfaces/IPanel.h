#pragma once

#include "../../utilities/Memory.h"

class IPanel {
public:
	const char *GetName(unsigned int panel) {
		return Memory::CallVirtual<36, const char *>(this, panel);
	}
};

inline IPanel *VGuiPanel;
