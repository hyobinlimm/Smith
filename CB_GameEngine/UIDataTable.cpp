#include "CBpch.h"
#include "UIDataTable.h"

UIDataTable::UIDataTable()
	:m_DataCount(0)
{
}

bool UIDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& UIObjects = info->value;
		for (rapidjson::Value::ConstMemberIterator itr = UIObjects.MemberBegin(); itr != UIObjects.MemberEnd(); ++itr)
		{
			if (itr->name == "Name")
			{
				string _str = itr->value.GetString();
				wstring _name;
				_name.assign(_str.begin(), _str.end());
				m_Name.push_back(_name);
			}
			else if (itr->name == "UIType")
			{
				string _str = itr->value.GetString();
				wstring _type;
				_type.assign(_str.begin(), _str.end());
				m_UIType.push_back(_type);
			}
			else if (itr->name == "Data")
			{
				string _str = itr->value.GetString();
				wstring _type;
				_type.assign(_str.begin(), _str.end());
				m_Data.push_back(_type);
			}
			else if (itr->name == "Type")
			{
				string _str = itr->value.GetString();
				wstring _type;
				_type.assign(_str.begin(), _str.end());
				m_Type.push_back(_type);
			}
			else if (itr->name == "FileName")
			{
				string _str = itr->value.GetString();
				wstring _texName;
				_texName.assign(_str.begin(), _str.end());
				m_FileName.push_back(_texName);
				m_DataCount++;
			}
		}
	}

	return true;
}
