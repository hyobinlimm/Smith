#pragma once

#include "JSONSetting.h"

/// <summary>
/// UI �׸������� json �����͸� �޾ƿͼ� ������ �ִ´�.
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

