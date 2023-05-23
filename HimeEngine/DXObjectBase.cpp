#include "DXObjectBase.h"
#include "HimeCamera.h"

DXObjectBase::DXObjectBase()

{
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
}

DXObjectBase::~DXObjectBase()
{

}

void DXObjectBase::Update(HimeCamera* pCamera)
{

}

void DXObjectBase::Render()
{

}

void DXObjectBase::DecomposeLocalTM()
{

}
