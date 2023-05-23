#include "CBpch.h"
#include "ObjectDataTable.h"

ObjectDataTable::ObjectDataTable()
	: m_DataCount(0)
{

}

bool ObjectDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	XMFLOAT4 value;
	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& soundObjects = info->value;
		for (rapidjson::Value::ConstMemberIterator itr = soundObjects.MemberBegin(); itr != soundObjects.MemberEnd(); ++itr)
		{
			if (itr->name == "Name")
			{
				string _str = itr->value.GetString(); 
				wstring _name;
				_name.assign(_str.begin(), _str.end());

				m_Name.push_back(_name);
			}
			else if (itr->name == "Type")
			{
				string _str = itr->value.GetString();
				wstring _type;
				_type.assign(_str.begin(), _str.end());

				m_Type.push_back(_type);
			}
			else if (itr->name == "Position.x")
			{
				value.x = itr->value.GetFloat();
			}
			else if (itr->name == "Position.y")
			{
				value.y = itr->value.GetFloat();
			}
			else if (itr->name == "Position.z")
			{
				value.z = itr->value.GetFloat();
				value.w = 0;
				XMFLOAT4 pos = value;
				m_Position.push_back(pos);
			}
			else if (itr->name == "Rotation.x")
			{
				value.x = itr->value.GetFloat();
			}
			else if (itr->name == "Rotation.y")
			{
				value.y = itr->value.GetFloat();
			}
			else if (itr->name == "Rotation.z")
			{
				value.z = itr->value.GetFloat();
				value.w = 0;
				XMFLOAT4 rot = value;
				m_Rotation.push_back(rot);
			}
			else if (itr->name == "Scale.x")
			{
				value.x = itr->value.GetFloat();
			}
			else if (itr->name == "Scale.y")
			{
				value.y = itr->value.GetFloat();
			}
			else if (itr->name == "Scale.z")
			{
				value.z = itr->value.GetFloat();
				value.w = 0;
				XMFLOAT4 scale = value;
				m_Scale.push_back(scale);
			}
			else if (itr->name == "FileName")
			{
				string _str = itr->value.GetString();
				wstring _fileName;
				_fileName.assign(_str.begin(), _str.end());

				m_FileName.push_back(_fileName);
			}
			else if (itr->name == "TexName")
			{
				string _str = itr->value.GetString();
				wstring _texName;
				_texName.assign(_str.begin(), _str.end());

				m_TexName.push_back(_texName);
				m_DataCount++;
			}
		}
	}

	return true;
}
