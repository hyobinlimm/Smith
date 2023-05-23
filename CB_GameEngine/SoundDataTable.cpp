#include "CBpch.h"
#include "SoundDataTable.h"

SoundDataTable::SoundDataTable()
{

}

bool SoundDataTable::Deserialize(const std::string& s)
{
	rapidjson::Document doc;
	if (!InitDocument(s, doc))
		return false;

	for (auto info = doc.MemberBegin(); info != doc.MemberEnd(); info++)
	{
		const rapidjson::Value& soundObjects = info->value;
		shared_ptr<SoundData> _NewData = make_shared<SoundData>();
		for (rapidjson::Value::ConstMemberIterator itr = soundObjects.MemberBegin(); itr != soundObjects.MemberEnd(); ++itr)
		{
			if (itr->name == "Scene")
			{
				_NewData->m_Scene = itr->value.GetString();
			}
			else if (itr->name == "Name")
			{
				_NewData->m_Name = itr->value.GetString();
			}
			else if (itr->name == "Type")
			{
				_NewData->m_Type = itr->value.GetString();
			}
			else if (itr->name == "FileName")
			{
				_NewData->m_FileName = itr->value.GetString();
			}
			else if (itr->name == "Loop")
			{
				_NewData->m_Loop = itr->value.GetString();
			}
		}
		m_SoundList.emplace_back(_NewData);
	}

	return true;
}
