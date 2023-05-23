#include "CBpch.h"
#include "QuestDataTable.h"

QuestDataTable::QuestDataTable()
{

}

bool QuestDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& _QuestData = info->value;
		shared_ptr<QuestData> _NewData = make_shared<QuestData>();
		for (rapidjson::Value::ConstMemberIterator itr = _QuestData.MemberBegin(); itr != _QuestData.MemberEnd(); ++itr)
		{
			if (itr->name == "Quest_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QuestID = _Name;
			}
			else if (itr->name == "Quest_Date")
			{
				_NewData->m_QuestDate = itr->value.GetInt();
			}
			else if (itr->name == "Quest_Model_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QuestModel = _Name;
			}
			else if (itr->name == "Quest_UI_Image")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QuestUIImage = _Name;
			}
			else if (itr->name == "Quest_Weapon_Type")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QWeaponType = _Name;
			}
			else if (itr->name == "Quest_Weapon_Category")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QWeaponCategory = _Name;
			}
			else if (itr->name == "Quest_Mat_1")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QMat1 = _Name;
			}
			else if (itr->name == "Quest_Mat_2")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QMat2 = _Name;
			}
			else if (itr->name == "Quest_Hardness_min")
			{
				_NewData->m_QHardnessMin = itr->value.GetFloat();
			}
			else if (itr->name == "Quest_Hardness_max")
			{
				_NewData->m_QHardnessMax = itr->value.GetFloat();
			}
			else if (itr->name == "Quest_Enchant")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QEnchant = _Name;
			}
			else if (itr->name == "Quest_Handle")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QHandle = _Name;
			}
			else if (itr->name == "Quest_Review_Image_1")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QReview1 = _Name;
			}
			else if (itr->name == "Quest_Review_Image_2")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QReview2 = _Name;
			}
			else if (itr->name == "Quest_Review_Image_3")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QReview3 = _Name;
			}
			else if (itr->name == "Quest_Review_Image_4")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_QReview4 = _Name;
			}
			else if (itr->name == "Quest_IsUsed")
			{
				_NewData->m_IsUsed = itr->value.GetInt();
			}
		}
		if (_NewData->m_QuestDate == 1)
		{
			m_Quest1List.emplace_back(_NewData);
		}
		else if (_NewData->m_QuestDate == 2)
		{
			m_Quest2List.emplace_back(_NewData);
		}
		else
		{
			m_Quest3List.emplace_back(_NewData);
		}
	}

	return true;
}
