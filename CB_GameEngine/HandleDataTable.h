#pragma once

#include "JSONSetting.h"

struct HandleData
{
	wstring m_HandleID;
	wstring m_HandleModel;
	wstring m_HandleImage;
};

class HandleDataTable : public JSONSetting
{
public:
	HandleDataTable();
	virtual ~HandleDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<HandleData>> m_HandleList;
};

