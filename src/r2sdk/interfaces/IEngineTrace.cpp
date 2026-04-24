#include "../../pch.h"

#include "IEngineTrace.h"

void IEngineTrace::TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) noexcept
{
	return Memory::InvokeVTableFunction<void>(this, r2sdk::IEngineTrace::Index::TraceRay, ray, fMask, pTraceFilter, pTrace);
}