#pragma once

#include <string>
#include <wchar.h>
/// <summary>
/// MeshObject가 만들어지기 위해 필요한
/// .ASE, .dds 경로, 이름
/// 등을 들고 있는 객체 
/// </summary>

class BuildInfo
{
private:
	std::wstring m_Name;
	std::wstring m_PathMap;
	std::wstring m_PathASE;
	//std::wstring m_PathNormalMap;
	//const wchar_t* m_TexFilePath;

public:
	std::wstring GetName() const { return m_Name; }
	void SetName(std::wstring val) { m_Name = val; }

	const std::wstring GetPathMap() const {
		if (m_PathMap == L"")
			return L"Fail";
		std::wstring _texturePath(L"../Resource/Texture/" + m_PathMap + L".dds");
		return _texturePath;
	}
	
	void SetPathMap(std::wstring val) { m_PathMap = val; }
	
	std::wstring GetPathASE() const {	
		std::wstring _ASEPath(L"../Resource/ASE/" + m_PathASE + L".ASE");
		return _ASEPath; }

	void SetPathASE(std::wstring val) { m_PathASE = val; }
};

