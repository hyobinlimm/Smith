#include "CBpch.h"
#include "WeaponTypeDataTable.h"

WeaponTypeDataTable::WeaponTypeDataTable()
{

}

bool WeaponTypeDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& TypeData = info->value;
		shared_ptr<WeaponTypeData> _NewData = make_shared<WeaponTypeData>();
		for (rapidjson::Value::ConstMemberIterator itr = TypeData.MemberBegin(); itr != TypeData.MemberEnd(); ++itr)
		{
			if (itr->name == "Weapon_Type_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_WeaponTypeID = _Name;
			}
			else if (itr->name == "Weapon_Category")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_WeaponCategory = _Name;
			}
			else if (itr->name == "Weapon_Model_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_WeaponModelID = _Name;
			}
			else if (itr->name == "Weapon_Image")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_WeaponImage = _Name;
			}
		}
		m_WeaponTypeList.emplace_back(_NewData);
	}

	return true;
}
