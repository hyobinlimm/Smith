#pragma once

#include "JSONSetting.h"

struct SoundData
{
	string m_Scene;
	string m_Name;
	string m_Type;
	string m_FileName;
	string m_Loop;
};

class SoundDataTable : public JSONSetting
{
public:
	SoundDataTable();
	virtual ~SoundDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<SoundData>> m_SoundList;
};

