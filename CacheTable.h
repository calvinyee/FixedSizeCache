
#pragma once
#include <Windows.h>

//
// CacheTableEntry class
//
class CacheTableEntry
{
private:
	void* m_data;
	int   m_key;
	int   m_hitCount;
	int   m_dataSize;
	ULONGLONG m_timeStamp;

public:
	CacheTableEntry(int key, int dataSize, void* data);
	virtual ~CacheTableEntry();

	void* GetData()
	{
		return m_data;
	}

	void SetData(void* data)
	{
		if (m_data != NULL)
		{
			free(m_data);
			m_data = data;
		}

	}

	int GetKey()
	{
		return m_key;
	}

	int GetHitCount()
	{
		return m_hitCount;
	}

	int GetDataSize()
	{
		return m_dataSize;
	}

	void SetDataSize(int dataSize)
	{
		m_dataSize = dataSize;
	}

	void AddHitCount()
	{
		m_hitCount++;
	}

	ULONGLONG GetTimeStamp()
	{
		return m_timeStamp;
	}

	void SetTimeStamp(ULONGLONG timeStamp)
	{
		m_timeStamp = timeStamp;
	}
};

//
// CacheTableList class
//
class CacheTableList
{
private:
	CacheTableEntry* m_entry;

public:
	CacheTableList* m_next;

public:
	CacheTableList(CacheTableEntry* entry);
	virtual ~CacheTableList();

	CacheTableEntry* GetEntry()
	{
		return m_entry;
	}
};


typedef CacheTableList* PTABLELIST;

//
// CacheTable class
//
class CacheTable
{
private:
	PTABLELIST* m_table;
	int m_tableSize;
	bool m_bFixedSize;
	int m_count;

	CRITICAL_SECTION criticalCache;

public:
	CacheTable(int tableSize, bool fixedSize);
	virtual ~CacheTable();

	
	HRESULT AddAndUpdateCacheData(int key, const void* data, int dataSize);
	HRESULT GetCacheData(int key, CacheTableEntry*& cacheEntry);
	HRESULT DeleteCacheData(int key);
	HRESULT GetKeyWithMostHitCount(int& key);
	HRESULT GetKeyWithLeastHitCount(int& key);
	HRESULT ResizeTable(int newTableSize);
	HRESULT IsCacheDataExist(int key, bool& bExist);
	

	int GetTableSize()
	{
		return m_tableSize;
	}

	int GetCacheCount()
	{
		return m_count;
	}

private:
	void Clear();
	bool IsCacheDataExistInternal(int key);
	PTABLELIST DetachCacheList(int key);

};