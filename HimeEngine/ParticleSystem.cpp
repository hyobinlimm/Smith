#include "HimeDevice.h"
#include "InputLayouts.h"
#include "Effects.h"
#include "HimeCamera.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
	: mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

ParticleSystem::~ParticleSystem()
{
	ReleaseCOM(mInitVB);
	ReleaseCOM(mDrawVB);
	ReleaseCOM(mStreamOutVB);
}

void ParticleSystem::Init( ID3D11ShaderResourceView* texArraySRV,
	ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	mMaxParticles = maxParticles;
	
	Effects::InitAll();
	mFX = Effects::FireFX;

	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;

	ID3D11Device* _Device3D = HimeDevice::GetInst()->GetD3dDevice();

	BuildVB(_Device3D);
}

float ParticleSystem::GetAge()const
{
	return mAge;
}

void ParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void ParticleSystem::Update(float dt, float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	mAge += dt;
}

void ParticleSystem::Draw(const HimeCamera* cam)
{
	XMMATRIX VP = cam->ViewProj();

	//
	// Set constants.
	mFX->SetViewProj(VP);
	mFX->SetGameTime(mGameTime);
	mFX->SetTimeStep(mTimeStep);
	mFX->SetEyePosW(mEyePosW);
	mFX->SetEmitPosW(mEmitPosW);
	mFX->SetEmitDirW(mEmitDirW);
	mFX->SetTexArray(mTexArraySRV);
	mFX->SetRandomTex(mRandomTexSRV);

	//
	// Set IA stage.
	ID3D11DeviceContext* _deviceContext = HimeDevice::GetInst()->GetDeviceContext();
	_deviceContext->IASetInputLayout(InputLayouts::Particle);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(VertexStruct::Particle);
	UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if (mFirstRun)
		_deviceContext->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
	else
		_deviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	_deviceContext->SOSetTargets(1, &mStreamOutVB, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	mFX->StreamOutTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mFX->StreamOutTech->GetPassByIndex(p)->Apply(0, _deviceContext);

		if (mFirstRun)
		{
			_deviceContext->Draw(1, 0);
			mFirstRun = false;
		}
		else
		{
			_deviceContext->DrawAuto();
		}
	}

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	_deviceContext->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	_deviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	mFX->DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mFX->DrawTech->GetPassByIndex(p)->Apply(0, _deviceContext);

		_deviceContext->DrawAuto();
	}
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{
	// Create the buffer to kick-off the particle system.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexStruct::Particle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	VertexStruct::Particle p;
	ZeroMemory(&p, sizeof(VertexStruct::Particle));
	p.Age = 0.0f;
	p.Type = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));

	// Create the ping-pong buffers for stream-out and drawing.
	vbd.ByteWidth = sizeof(VertexStruct::Particle) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(device->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(device->CreateBuffer(&vbd, 0, &mStreamOutVB));
}