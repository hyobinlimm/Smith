#include "CBpch.h"
#include "EnchantDataTable.h"

EnchantDataTable::EnchantDataTable()
{

}

bool EnchantDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& _EnchantData = info->value;
		shared_ptr<EnchantData> _NewData = make_shared<EnchantData>();
		for (rapidjson::Value::ConstMemberIterator itr = _EnchantData.MemberBegin(); itr != _EnchantData.MemberEnd(); ++itr)
		{
			if (itr->name == "Enchant_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_EnchantID = _Name;
			}
			else if (itr->name == "Enchant_Model")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_EnchantModel = _Name;
			}
			else if (itr->name == "Enchant_Image")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_EnchantImage = _Name;
			}
		}
		m_EnchantList.emplace_back(_NewData);
	}

	return true;
}
