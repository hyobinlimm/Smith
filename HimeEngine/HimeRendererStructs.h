#pragma once

#include "DirectXDefine.h"
#include "SimpleMath.h"


/// GameEngine�� �ְ� ���� �����͵��� ����ü

enum class eRenderState
{
	Wire,
	Solid, 
	end
};

enum class eObjType
{
	Helper,
	StaticMesh,
	Animation,
	Skinning,
	Bone, // bone�� ���� �ʿ��Ѱ� �ʹ�? 
	end
};

struct FilePath
{
	std::wstring ID_name; 
	std::wstring FileName;
	std::wstring TexName;
	//std::wstring NorName;
};

struct MeshBuildInfo
{
	std::wstring IDName;
	std::wstring NodeName;

	std::wstring ObjType;

	XMFLOAT4 Pos;
	XMFLOAT4 Rot;
	XMFLOAT4 Scale;

	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;

	float minX2;
	float minZ2;
	float maxX2;
	float maxZ2;

	// render�� ���� ���� �����ϴ� ��.
	bool isDraw; 
	bool isVolumeDraw = false;
	bool isCol = false;
	bool isInterAct = false;
};

struct CameraInfo
{
	XMFLOAT3 Pos;
	XMFLOAT3 Up;
	XMFLOAT3 Target;
	XMFLOAT3 Look;
	XMFLOAT3 Right;
};

// ���ڸ� ��� ���� ���� �־���� ����.
struct TextData
{
	std::wstring IDName;

	int x;
	int y; 

	float FontSize;

	std::wstring text;

	float value;
	float value2;
	float value3;

	std::wstring textValue;

	XMFLOAT4 color;

	bool isDraw;
};

// Texture2D�� ��� ���� ���� �־���� ����
struct UIData
{
	std::wstring Name;
	std::wstring UIType;
	std::wstring Data;
	std::wstring Type;
	std::wstring FileName;

	float MoveX;
	float MoveY;

	bool isMovable = false;
	bool isDraw;
	bool isTurn = false;

	float Left = 0;
	float Top = 0;
	float Right = 0;
	float Bottom = 0;

	int frame = 0;
	float rotate = 0;

	float width = 0;
	float height = 0;
};