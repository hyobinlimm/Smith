#include "CBpch.h"
#include "MatDataTable.h"

MatDataTable::MatDataTable()
{

}

bool MatDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& _MatData = info->value;
		shared_ptr<MatData> _NewData = make_shared<MatData>();
		for (rapidjson::Value::ConstMemberIterator itr = _MatData.MemberBegin(); itr != _MatData.MemberEnd(); ++itr)
		{
			if (itr->name == "Mat_ID")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_MatID = _Name;
			}
			else if (itr->name == "Mat_Category")
			{
				_NewData->m_MatCategory = itr->value.GetInt();
			}
			else if (itr->name == "Mat_Hardness")
			{
				_NewData->m_MatHardness = itr->value.GetInt();
			}
			else if (itr->name == "Mat_Model")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_MatModel = _Name;
			}
			else if (itr->name == "Mat_Image")
			{
				string _str = itr->value.GetString();
				wstring _Name;
				_Name.assign(_str.begin(), _str.end());

				_NewData->m_MatImage = _Name;
			}
		}
		m_MatList.emplace_back(_NewData);
	}

	return true;
}
