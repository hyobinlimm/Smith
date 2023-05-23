#include "CBpch.h"
#include "WeaponManager.h"

shared_ptr<WeaponManager> WeaponManager::g_WeaponInstance = nullptr;

std::shared_ptr<WeaponManager> WeaponManager::GetInstance()
{
	if (g_WeaponInstance == nullptr)
		g_WeaponInstance = make_shared<WeaponManager>();

	return g_WeaponInstance;
}

void WeaponManager::Initiallize()
{
	m_JsonWeaponTypeData = make_shared<WeaponTypeDataTable>();
	m_JsonMatData = make_shared<MatDataTable>();
	m_JsonEnchantData = make_shared<EnchantDataTable>();
	m_JsonHandleData = make_shared<HandleDataTable>();

	m_JsonWeaponTypeData->DeserializeFromFile("../Resource/DataTable/Smith_Weapon_Type_Data.json");
	m_JsonMatData->DeserializeFromFile("../Resource/DataTable/Smith_Weapon_Mat_Data.json");
	m_JsonEnchantData->DeserializeFromFile("../Resource/DataTable/Smith_Weapon_Enchant_Data.json");
	m_JsonHandleData->DeserializeFromFile("../Resource/DataTable/Smith_Weapon_Handle_Data.json");
}

shared_ptr<WeaponTypeData> WeaponManager::GetWeaponData(wstring ID)
{
	for (auto& weaponData : m_JsonWeaponTypeData->m_WeaponTypeList)
	{
		if (weaponData->m_WeaponTypeID.find(ID))
		{
			return weaponData;
		}
	}
}

std::shared_ptr<MatData> WeaponManager::GetMatData(wstring ID)
{
	for (auto& matData : m_JsonMatData->m_MatList)
	{
		if (matData->m_MatID.find(ID))
		{
			return matData;
		}
	}
}

std::shared_ptr<EnchantData> WeaponManager::GetEnchantData(wstring ID)
{
	for (auto& enchantData : m_JsonEnchantData->m_EnchantList)
	{
		if (enchantData->m_EnchantID.find(ID))
		{
			return enchantData;
		}
	}
}

std::shared_ptr<HandleData> WeaponManager::GetHandleData(wstring ID)
{
	for (auto& handleData : m_JsonHandleData->m_HandleList)
	{
		if (handleData->m_HandleID.find(ID))
		{
			return handleData;
		}
	}
}
