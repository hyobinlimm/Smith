#pragma once

#include "JSONSetting.h"

struct EnchantData
{
	wstring m_EnchantID;
	wstring m_EnchantModel;
	wstring m_EnchantImage;
};

class EnchantDataTable: public JSONSetting
{
public:
	EnchantDataTable();
	virtual ~EnchantDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<EnchantData>> m_EnchantList;
};

