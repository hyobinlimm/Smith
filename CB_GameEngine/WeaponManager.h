#pragma once

#include "WeaponTypeDataTable.h"
#include "MatDataTable.h"
#include "EnchantDataTable.h"
#include "HandleDataTable.h"

/// <summary>
/// json형식으로 저장된 Weapon과 관련된 모든 데이터를 불러와 가지고있을 곳.
/// </summary>

struct Weapon
{
	WeaponTypeData m_WeaponTypeData;
	MatData m_MatData;
	EnchantData m_EnchantData;
	HandleData m_HandleData;
};

class WeaponTypeDataTable;
class MatDataTable;
class EnchantDataTable;
class HandleDataTable;

class WeaponManager
{
public:
	WeaponManager() = default;

	~WeaponManager() = default;

	WeaponManager(const WeaponManager& other) = delete;

	WeaponManager& operator= (const WeaponManager& other) = delete;

	WeaponManager(WeaponManager&& other) = delete;

public:
	static shared_ptr<WeaponManager> GetInstance();

	void Initiallize();

	shared_ptr<WeaponTypeData> GetWeaponData(wstring ID);
	shared_ptr<MatData> GetMatData(wstring ID);
	shared_ptr<EnchantData> GetEnchantData(wstring ID);
	shared_ptr<HandleData> GetHandleData(wstring ID);

private:
	static shared_ptr<WeaponManager> g_WeaponInstance;

	shared_ptr<WeaponTypeDataTable> m_JsonWeaponTypeData;
	shared_ptr<MatDataTable> m_JsonMatData;
	shared_ptr<EnchantDataTable> m_JsonEnchantData;
	shared_ptr<HandleDataTable> m_JsonHandleData;
};

