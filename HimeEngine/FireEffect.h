#pragma once

#include "DXObjectBase.h"
#include "Effects.h"

class ResourceManager;
class HimeCamera;
class d3dHelper;

class FireEffect : public DXObjectBase
{
public:
	FireEffect();
	FireEffect(XMVECTOR pos);
	~FireEffect();

private:
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	HimeCamera* mCamera;

	// ��ȯ ����
	//XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)
	XMFLOAT4X4 m_View;	// �þ� ��ȯ ��� (ī�޶� ��)
	XMFLOAT4X4 m_Proj;	// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

public: //get set �Լ� ����
	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	

	// Time elapsed since the system was reset.
	float GetAge()const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Reset();
	
	void SetTexResource();
	void Initialize();

	void Update(float detalTime, HimeCamera* pCamera);
	virtual void Render() override;
	
	// ������� �ʴ� �Լ���
	virtual void SetTexResource(std::wstring name) override {};
	virtual void UpdateAnimation(int m_framecount) override {};
	virtual void SetPos(XMFLOAT4 Pos) override {};
	virtual void SetRot(XMFLOAT4 Rot) override {};
	virtual void DecomposeLocalTM() override {};
	virtual void CalcLocalTM() override {};


private:
	FireEffect(const FireEffect& rhs);
	FireEffect& operator=(const FireEffect& rhs);

private:
	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	FireEffect* mFX;
	d3dHelper* mD3DHelper;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	/// <summary>
	/// ///////////////////////////////////////
	/// </summary>
	UINT m_IndexCount;
	Material m_Material;

	/// ���� �ܺο��� �޾ƿ����� ����
	DirectionalLight m_DirLights[3];
	XMFLOAT3 m_EyePosW;

	ID3D11ShaderResourceView* m_pNormalMapSRV;

	XMFLOAT4X4 m_TexTransform;

public:
	bool m_IsSkinningObj;
};

