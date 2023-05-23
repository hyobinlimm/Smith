#pragma once

#include "JSONSetting.h"

class ObjectDataTable : public JSONSetting
{
public:
	ObjectDataTable();
	virtual ~ObjectDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<wstring> m_Name;
	vector<wstring> m_Type;
	vector<XMFLOAT4> m_Position;
	vector<XMFLOAT4> m_Rotation;
	vector<XMFLOAT4> m_Scale;
	vector<wstring> m_FileName;
	vector<wstring> m_TexName;
	//string m_Name[100];
	//string m_Type[100];
	//XMFLOAT4 m_Position[100];
	//XMFLOAT4 m_Rotation[100];
	//XMFLOAT4 m_Scale[100];
	//string m_FileName[100];
	//string m_TexName[100];

	int m_DataCount;
};

