#pragma once
#include "../../pch.h"

class VMatrix
{
public:
	float m[4][4]{};

	const float* operator[](int i) const {
		return m[i];
	}

	float* operator[](int i) {
		return m[i];
	}

	const matrix3x4_t& As3x4() const {
		return *((const matrix3x4_t*)this);
	}
};