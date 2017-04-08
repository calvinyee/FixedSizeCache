#include "stdafx.h"
#include "CacheTable.h"


//
// CacheTableEntry class
//
CacheTableEntry::CacheTableEntry(int key, int dataSize, void* data)
{
	m_key = key;
	m_dataSize = dataSize;
	m_data = data;
	m_hitCount = 0;
	m_timeStamp = GetTickCount64();
}

CacheTableEntry::~CacheTableEntry()
{
	if (m_data != NULL)
	{
		free(m_data);
		m_data = NULL;
	}
}


//
// CacheTableList class
//
CacheTableList::CacheTableList(CacheTableEntry* entry)
{
	m_entry = entry;
	m_next = NULL;
}

CacheTableList::~CacheTableList()
{
	if (m_entry != NULL)
	{
		delete m_entry;
	}
}

//
// CacheTable class
//
CacheTable::CacheTable(int tableSize, bool fixedSize)
{
	m_tableSize = tableSize;
	m_bFixedSize = fixedSize;
	m_count = 0;

	if (tableSize > 0)
	{
		m_table = new PTABLELIST[tableSize];
		for (int i = 0; i < tableSize; i++)
		{
			m_table[i] = NULL;
		}

	}
	else
	{
		m_table = NULL;
	}

	InitializeCriticalSection(&criticalCache);
}


CacheTable::~CacheTable()
{
	Clear();

	DeleteCriticalSection(&criticalCache);
}

//
// public methods
//
HRESULT CacheTable::AddAndUpdateCacheData(int key, const void* data, int dataSize)
{
	HRESULT hr = S_OK;

	if (data == NULL || dataSize <= 0 || key < 0)
	{
		hr = E_INVALIDARG;
		return hr;
	}

	if (m_table == NULL)
	{
		hr = E_POINTER;
		return hr;
	}

	int tableIndex = key % m_tableSize;

	void* dataCopy = malloc(dataSize);
	if (dataCopy == NULL)
	{
		hr = E_OUTOFMEMORY;
		return hr;
	}

	memcpy(dataCopy, data, dataSize);

	EnterCriticalSection(&criticalCache);

	if (m_bFixedSize)
	{
		if (m_count == m_tableSize && !IsCacheDataExistInternal(key))
		{
			// need to delete an entry
			int leastKey = -1;
			HRESULT hre = GetKeyWithLeastHitCount(leastKey);
			if (hre == S_OK && leastKey >= 0)
			{
				DeleteCacheData(leastKey);
			}
		}
	}

	PTABLELIST pTableList = m_table[tableIndex];
	if (pTableList == NULL)
	{
		CacheTableEntry* pEntry = new CacheTableEntry(key, dataSize, dataCopy);
		pTableList = new CacheTableList(pEntry);

		m_table[tableIndex] = pTableList;
		m_count++;
	}
	else
	{
		bool entryFound = false;
		CacheTableEntry* pEntry = NULL;
		PTABLELIST pTempList = pTableList;
		while (pTempList != NULL)
		{
			pEntry = pTempList->GetEntry();
			if (pEntry && pEntry->GetKey() == key)
			{
				entryFound = true;
				break;
			}

			pTempList = pTempList->m_next;
		}

		if (entryFound)
		{
			// update entry
			pEntry->SetData(dataCopy);
			ULONGLONG timeStamp = GetTickCount64();

			pEntry->SetTimeStamp(timeStamp);
			pEntry->SetDataSize(dataSize);
		}
		else
		{
			CacheTableEntry* pEntry = new CacheTableEntry(key, dataSize, dataCopy);
			PTABLELIST pNewNode = new CacheTableList(pEntry);

			pNewNode->m_next = pTableList;

			pTableList = pNewNode;

			m_table[tableIndex] = pTableList;
			m_count++;
		}

	}

	LeaveCriticalSection(&criticalCache);

	return hr;
}



HRESULT CacheTable::GetCacheData(int key, CacheTableEntry*& cacheEntry)
{
	HRESULT hr = S_OK;

	if (key < 0)
	{
		hr = E_INVALIDARG;
		return hr;
	}

	if (m_table == NULL)
	{
		hr = E_POINTER;
		return hr;
	}

	cacheEntry = NULL;

	EnterCriticalSection(&criticalCache);

	int tableIndex = key % m_tableSize;

	PTABLELIST pTableList = m_table[tableIndex];
	if (pTableList == NULL)
	{		
		LeaveCriticalSection(&criticalCache);
		return hr;
	}

	bool entryFound = false;
	CacheTableEntry* pEntry = NULL;
	PTABLELIST pTempList = pTableList;
	while (pTempList != NULL)
	{
		pEntry = pTempList->GetEntry();
		if (pEntry && pEntry->GetKey() == key)
		{
			entryFound = true;
			break;
		}

		pTempList = pTempList->m_next;
	}

	if (entryFound)
	{
		pEntry->AddHitCount();
		ULONGLONG timeStamp = GetTickCount64();
		pEntry->SetTimeStamp(timeStamp);

		cacheEntry = pEntry;
	}

	LeaveCriticalSection(&criticalCache);

	return hr;
}


HRESULT CacheTable::DeleteCacheData(int key)
{
	HRESULT hr = S_OK;

	if (key < 0)
	{
		hr = E_INVALIDARG;
		return hr;
	}

	if (m_table == NULL)
	{
		hr = E_POINTER;
		return hr;
	}

	EnterCriticalSection(&criticalCache);

	int tableIndex = key % m_tableSize;
	PTABLELIST pTableList = m_table[tableIndex];
	if (pTableList == NULL)
	{
		LeaveCriticalSection(&criticalCache);
		return hr;
	}

	bool entryFound = false;
	CacheTableEntry* pEntry = NULL;
	PTABLELIST pPreviousList = NULL;
	PTABLELIST pCurrentList = pTableList;
	while (pCurrentList != NULL)
	{
		pEntry = pCurrentList->GetEntry();
		if (pEntry && pEntry->GetKey() == key)
		{
			entryFound = true;
			break;
		}

		pPreviousList = pCurrentList;
		pCurrentList = pCurrentList->m_next;
	}

	if (entryFound)
	{
		if (pPreviousList != NULL)
		{
			pPreviousList->m_next = pCurrentList->m_next;
			delete pCurrentList;
		}
		else
		{
			pTableList = pCurrentList->m_next;
			delete pCurrentList;
			m_table[tableIndex] = pTableList;
		}

		m_count--;
	}

	LeaveCriticalSection(&criticalCache);

	return hr;
}


HRESULT CacheTable::GetKeyWithMostHitCount(int& key)
{
	HRESULT hr = S_OK;

	key = -1;

	if (m_table == NULL)
	{
		hr = E_POINTER;
		return hr;
	}

	EnterCriticalSection(&criticalCache);

	ULONGLONG timeStamp = 0;
	int nHitCount = 0;
	for (int i = 0; i < m_tableSize; i++)
	{
		PTABLELIST pTableList = m_table[i];
		if (pTableList == NULL)
		{
			continue;
		}

		while (pTableList != NULL)
		{
			CacheTableEntry* pEntry = pTableList->GetEntry();
			if (pEntry->GetHitCount() > nHitCount)
			{
				nHitCount = pEntry->GetHitCount();
				key = pEntry->GetKey();
				timeStamp = pEntry->GetTimeStamp();
			}
			else if (pEntry->GetHitCount() == nHitCount)
			{
				if (pEntry->GetTimeStamp() > timeStamp)
				{
					timeStamp = pEntry->GetTimeStamp();
					key = pEntry->GetKey();
				}
			}

			pTableList = pTableList->m_next;
		}

	}

	LeaveCriticalSection(&criticalCache);

	return hr;
}



HRESULT CacheTable::GetKeyWithLeastHitCount(int& key)
{
	HRESULT hr = S_OK;

	key = -1;

	if (m_table == NULL)
	{
		hr = E_POINTER;
		return hr;
	}

	EnterCriticalSection(&criticalCache);

	ULONGLONG timeStamp = 0;
	int nHitCount = INT_MAX;
	for (int i = 0; i < m_tableSize; i++)
	{
		PTABLELIST pTableList = m_table[i];
		if (pTableList == NULL)
		{
			continue;
		}

		while (pTableList != NULL)
		{
			CacheTableEntry* pEntry = pTableList->GetEntry();
			if (pEntry->GetHitCount() < nHitCount)
			{
				nHitCount = pEntry->GetHitCount();
				key = pEntry->GetKey();
				timeStamp = pEntry->GetTimeStamp();
			}
			else if (pEntry->GetHitCount() == nHitCount)
			{
				if (pEntry->GetTimeStamp() < timeStamp)
				{
					timeStamp = pEntry->GetTimeStamp();
					key = pEntry->GetKey();
				}
			}

			pTableList = pTableList->m_next;
		}

	}

	LeaveCriticalSection(&criticalCache);

	return hr;
}


HRESULT CacheTable::ResizeTable(int newTableSize)
{
	HRESULT hr = S_OK;

	if (newTableSize <= 0)
	{
		hr = E_INVALIDARG;
		return hr;
	}

	if (newTableSize == m_tableSize)
	{
		return hr;
	}

	PTABLELIST* newTable = new PTABLELIST[newTableSize];
	for (int i = 0; i < newTableSize; i++)
	{
		newTable[i] = NULL;
	}

	int newCount = 0;

	EnterCriticalSection(&criticalCache);

	if (m_bFixedSize && newTableSize < m_tableSize)
	{
		// need to pick newTableSize most used entries from m_table
		int key = -1;
		for (int i = 0; i < newTableSize; i++)
		{
			HRESULT hre = GetKeyWithMostHitCount(key);
			if (hre == S_OK && key >= 0)
			{
				PTABLELIST pCacheList = DetachCacheList(key);

				if (pCacheList != NULL)
				{
					int nNewIndex = key % newTableSize;
					PTABLELIST pNewTableList = newTable[nNewIndex];
					if (pNewTableList == NULL)
					{
						newTable[nNewIndex] = pCacheList;
					}
					else
					{
						pCacheList->m_next = pNewTableList;
						pNewTableList = pCacheList;
						newTable[nNewIndex] = pNewTableList;
					}

					newCount++;
				}
			}
		}

		Clear();
	}
	else
	{
		if (m_table != NULL)
		{
			for (int i = 0; i < m_tableSize; i++)
			{
				PTABLELIST pTableList = m_table[i];

				while (pTableList != NULL)
				{
					PTABLELIST pTempList = pTableList;
					pTableList = pTableList->m_next;

					pTempList->m_next = NULL;

					// add pTempList to newTable
					CacheTableEntry* pEntry = pTempList->GetEntry();

					int nNewIndex = pEntry->GetKey() % newTableSize;

					PTABLELIST pNewTableList = newTable[nNewIndex];
					if (pNewTableList == NULL)
					{
						newTable[nNewIndex] = pTempList;
					}
					else
					{
						pTempList->m_next = pNewTableList;
						pNewTableList = pTempList;
						newTable[nNewIndex] = pNewTableList;
					}

					newCount++;
				}
			}

			delete[] m_table;
		}
	}

	m_table = newTable;
	m_tableSize = newTableSize;
	m_count = newCount;

	LeaveCriticalSection(&criticalCache);

	return hr;
}


HRESULT CacheTable::IsCacheDataExist(int key, bool& bExist)
{
	bExist = IsCacheDataExistInternal(key);
	return S_OK;
}

//
// private methods
//
void CacheTable::Clear()
{
	EnterCriticalSection(&criticalCache);

	if (m_table != NULL)
	{
		for (int i = 0; i < m_tableSize; i++)
		{
			PTABLELIST pTableList = m_table[i];

			PTABLELIST tempList = pTableList;
			while (tempList != NULL)
			{
				pTableList = tempList->m_next;
				delete tempList;
				tempList = pTableList;
			}
		}

		delete[] m_table;
	}

	LeaveCriticalSection(&criticalCache);
}


PTABLELIST CacheTable::DetachCacheList(int key)
{
	if (key < 0)
	{
		return NULL;
	}

	EnterCriticalSection(&criticalCache);

	int tableIndex = key % m_tableSize;
	PTABLELIST pTableList = m_table[tableIndex];
	if (pTableList == NULL)
	{
		LeaveCriticalSection(&criticalCache);
		return NULL;
	}

	bool entryFound = false;
	CacheTableEntry* pEntry = NULL;
	PTABLELIST pPreviousList = NULL;
	PTABLELIST pCurrentList = pTableList;
	while (pCurrentList != NULL)
	{
		pEntry = pCurrentList->GetEntry();
		if (pEntry && pEntry->GetKey() == key)
		{
			entryFound = true;
			break;
		}

		pPreviousList = pCurrentList;
		pCurrentList = pCurrentList->m_next;
	}

	if (entryFound)
	{
		if (pPreviousList != NULL)
		{
			pPreviousList->m_next = pCurrentList->m_next;
		}
		else
		{
			pTableList = pCurrentList->m_next;
			m_table[tableIndex] = pTableList;
		}

		m_count--;

		pCurrentList->m_next = NULL;

		LeaveCriticalSection(&criticalCache);

		return pCurrentList;
	}

	LeaveCriticalSection(&criticalCache);

	return NULL;
}



bool CacheTable::IsCacheDataExistInternal(int key)
{
	if (key < 0 || m_table == NULL)
	{
		return false;
	}

	EnterCriticalSection(&criticalCache);

	int tableIndex = key % m_tableSize;
	PTABLELIST pTableList = m_table[tableIndex];

	if (pTableList == NULL)
	{
		LeaveCriticalSection(&criticalCache);
		return false;
	}

	bool entryFound = false;
	CacheTableEntry* pEntry = NULL;
	PTABLELIST pTempList = pTableList;
	while (pTempList != NULL)
	{
		pEntry = pTempList->GetEntry();
		if (pEntry && pEntry->GetKey() == key)
		{
			entryFound = true;
			break;
		}

		pTempList = pTempList->m_next;
	}

	LeaveCriticalSection(&criticalCache);

	return entryFound;
}
