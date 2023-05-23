#pragma once

#include "JSONSetting.h"

struct WeaponTypeData
{
	wstring m_WeaponTypeID;
	wstring m_WeaponCategory;
	wstring m_WeaponModelID;
	wstring m_WeaponImage;
};

class WeaponTypeDataTable : public JSONSetting
{
public:
	WeaponTypeDataTable();
	virtual ~WeaponTypeDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<WeaponTypeData>> m_WeaponTypeList;
};

