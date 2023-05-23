#pragma once

#include "JSONSetting.h"

struct MatData
{
	wstring m_MatID;
	int m_MatCategory;
	int m_MatHardness;
	wstring m_MatModel;
	wstring m_MatImage;
};

class MatDataTable : public JSONSetting
{
public:
	MatDataTable();
	virtual ~MatDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<MatData>> m_MatList;
};

