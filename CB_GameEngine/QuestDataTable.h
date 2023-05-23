#pragma once

#include "JSONSetting.h"

struct QuestData
{
	wstring m_QuestID;
	int m_QuestDate;
	wstring m_QuestModel;
	wstring m_QuestUIImage;
	wstring m_QWeaponCategory;
	wstring m_QWeaponType;
	wstring m_QMat1;
	wstring m_QMat2;
	float m_QHardnessMin;
	float m_QHardnessMax;
	wstring m_QEnchant;
	wstring m_QHandle;
	wstring m_QReview1;
	wstring m_QReview2;
	wstring m_QReview3;
	wstring m_QReview4;
	int m_IsUsed;
};

class QuestDataTable : public JSONSetting
{
public:
	QuestDataTable();
	virtual ~QuestDataTable() {};

public:
	virtual bool Deserialize(const std::string& s);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };
	virtual bool DataSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const { return false; };

public:
	vector<shared_ptr<QuestData>> m_Quest1List;
	vector<shared_ptr<QuestData>> m_Quest2List;
	vector<shared_ptr<QuestData>> m_Quest3List;
};

