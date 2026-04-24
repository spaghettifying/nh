#pragma once

#include "../interfaces/ITraceFilter.h"
#include "../../pch.h"

class CTraceFilter : public ITraceFilter
{
public:

	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int contentsMask) override
	{
		UNREFERENCED_PARAMETER(contentsMask);
		return reinterpret_cast<const void*>(pEntity) != pSkipEntity;
	}

	virtual TraceType_t	GetTraceType() const override
	{
		return TRACE_EVERYTHING;
	}

	const void* pSkipEntity = nullptr;
};