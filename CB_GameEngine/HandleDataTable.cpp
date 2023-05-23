#include "CBpch.h"
#include "HandleDataTable.h"

HandleDataTable::HandleDataTable()
{

}

bool HandleDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& _HandleData = info->value;
		shared_ptr<HandleData> _NewData = make_shared<HandleData>();
		for (rapidjson::Value::ConstMemberIterator itr = _HandleData.MemberBegin(); itr != _HandleData.MemberEnd(); ++itr)
		{
			if (itr->name == "Handle_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_HandleID = _Name;
			}
			else if (itr->name == "Handle_Model")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_HandleModel = _Name;
			}
			else if (itr->name == "Handle_Image")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_HandleImage = _Name;
			}
		}
		m_HandleList.emplace_back(_NewData);
	}

	return true;
}
