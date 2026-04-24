#include "IViewRender.h"

#include "../../pch.h"

const Matrix4* IViewRender::GetWorldToScreenMatrix(bool bUnknown)
{
	return Memory::InvokeVTableFunction<Matrix4*>(this, r2sdk::IViewRender::Index::GetWorldToScreenMatrix, bUnknown);
}

bool IViewRender::HasWorldToScreenMatrix(bool bUnknown)
{
	return Memory::InvokeVTableFunction<bool>(this, r2sdk::IViewRender::Index::HasWorldToScreenMatrix, bUnknown);
}