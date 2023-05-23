
#include "Effects.h"
#include "HimeDevice.h"
#include <fstream>
#include <vector>

#pragma region Effect
Effect::Effect(const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	ID3D11Device* _device = HimeDevice::GetInst()->GetD3dDevice();
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, _device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region PosColorEffect
PosColorEffect::PosColorEffect(const std::wstring& filename)
	: Effect(filename)
{
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Tech = mFX->GetTechniqueByName("ColorTech");
}

PosColorEffect::~PosColorEffect()
{

}
#pragma endregion

#pragma region PosColorEffect
PosNormalEffect::PosNormalEffect(const std::wstring& filename)
	: Effect(filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");
}

PosNormalEffect::~PosNormalEffect()
{
}

#pragma endregion

#pragma region PosNorTexEffect
PosNorTexEffect::PosNorTexEffect(const std::wstring& filename)
	:Effect(filename)
{
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");

	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	/// 텍스쳐 추가
	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");
	/// RimLight 추가
	Light1TexRimTech = mFX->GetTechniqueByName("Light1TexRim");
	Light2TexRimTech = mFX->GetTechniqueByName("Light2TexRim");
}

PosNorTexEffect::~PosNorTexEffect()
{

}

#pragma endregion

PosNorTexSkinEffect::PosNorTexSkinEffect(const std::wstring& filename)
	: Effect(filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");

	/// 텍스쳐
	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

	/// 스키닝
	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();

	Light0TexSkinTech = mFX->GetTechniqueByName("Light0TexSkin");
	Light1TexSkinTech = mFX->GetTechniqueByName("Light1TexSkin");
	Light2TexSkinTech = mFX->GetTechniqueByName("Light2TexSkin");
	Light3TexSkinTech = mFX->GetTechniqueByName("Light3TexSkin");
}

PosNorTexSkinEffect::~PosNorTexSkinEffect()
{
}

#pragma region ParticleEffect
ParticleEffect::ParticleEffect(const std::wstring& filename)
	: Effect(filename)
{
	StreamOutTech = mFX->GetTechniqueByName("StreamOutTech");
	DrawTech = mFX->GetTechniqueByName("DrawTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	GameTime = mFX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = mFX->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = mFX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = mFX->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = mFX->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion

#pragma region Effects

PosColorEffect* Effects::PosColorFX = 0;
PosNormalEffect* Effects::PosNormalFX = 0;
PosNorTexEffect* Effects::PosNorTexFX = 0;
PosNorTexSkinEffect* Effects::PosNorTexSkinFX = 0;
ParticleEffect* Effects::FireFX = 0; 

void Effects::InitAll()
{
	PosColorFX = new PosColorEffect(L"../Resource/FX/color.cso");
	PosNormalFX = new PosNormalEffect(L"../Resource/FX/Basic.cso");
	PosNorTexFX = new PosNorTexEffect(L"../Resource/FX/BasicTex.cso");
	PosNorTexSkinFX = new PosNorTexSkinEffect(L"../Resource/FX/BasicTexSkin.cso");
	FireFX = new ParticleEffect(L"../Resource/FX/particle.cso");
}

void Effects::DestroyAll()
{
	SafeDelete(PosNorTexSkinFX);
	SafeDelete(PosNormalFX);
	SafeDelete(PosNorTexFX);
	SafeDelete(PosColorFX);
	SafeDelete(FireFX);
}
#pragma endregion

