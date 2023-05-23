#pragma once

#include "JSONSetting.h"

/// <summary>
/// UI 그리기위해 json 데이터를 받아와서 가지고 있는다.
/// </summary>

class UIDataTable : public JSONSetting
{
public:
	UIDataTable();
	virtual ~UIDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<wstring> m_Name;
	vector<wstring> m_UIType;
	vector<wstring> m_Data;
	vector<wstring> m_Type;
	vector<wstring> m_FileName;

	int m_DataCount;
};

