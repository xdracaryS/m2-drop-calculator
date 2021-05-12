#ifndef __INC_METIN_II_GROUP_TEXT_PARSE_TREE_H__
#define __INC_METIN_II_GROUP_TEXT_PARSE_TREE_H__
#include "file_loader.h"
#include <sstream>
#include <map>
#ifndef itertype
#define itertype(v) typeof((v).begin())
#endif
typedef std::map<std::string, TTokenVector>	TTokenVectorMap;
typedef std::map<std::string, int> TMapNameToIndex;


inline void stl_lowers(std::string& rstRet)
{
	for (size_t i = 0; i < rstRet.length(); ++i)
		rstRet[i] = tolower(rstRet[i]);
}

class CGroupNode
{
public:
	class CGroupNodeRow
	{
	public:
		CGroupNodeRow(CGroupNode* pGroupNode, TTokenVector& vec_values);
		virtual ~CGroupNodeRow();

		template <typename T>
		bool GetValue(const std::string & stColKey, OUT T& value) const;
		template <typename T>
		bool GetValue(size_t idx, OUT T& value) const;

		int GetSize() const;

	private:
		CGroupNode*		m_pOwnerGroupNode;
		TTokenVector	m_vec_values;
	};
public:
	CGroupNode();
	virtual ~CGroupNode();

	bool Load(const char * c_szFileName);
	const char * GetFileName();

	DWORD GetChildNodeCount();
	bool SetChildNode(const char * c_szKey, CGroupNode* pChildNode);
	CGroupNode* GetChildNode(const std::string & c_rstrKey) const;
	std::string GetNodeName() const;

	bool IsToken(const std::string & c_rstrKey) const;

	size_t GetRowCount();
	
	template <typename T>
	bool GetValue(size_t i, const std::string & c_rstrColKey, T& tValue) const;	// n��°(map�� ����ִ� ������ ��, txt�� �����ʹ� ���� ����) row�� Ư�� �÷��� ���� ��ȯ�ϴ� �Լ�. 
																				// �������̱� ������, ���Ǹ� ���� �Լ�.
	template <typename T>
	bool GetValue(const std::string & c_rstrRowKey, const std::string & c_rstrColKey, T& tValue) const;
	template <typename T>
	bool GetValue(const std::string & c_rstrRowKey, int index, T& tValue) const;

	bool GetRow(const std::string & c_rstrKey, OUT const CGroupNodeRow ** ppRow) const;
	// �����, idx�� txt�� ������ ������ ���� ����.
	bool GetRow(size_t idx, OUT const CGroupNodeRow ** ppRow) const;
	bool GetGroupRow(const std::string& stGroupName, const std::string& stRow, OUT const CGroupNode::CGroupNodeRow ** ppRow) const;

	template <typename T>
	bool GetGroupValue(const std::string& stGroupName, const std::string& stRow, int iCol, OUT T& tValue) const;
	template <typename T>
	bool GetGroupValue(const std::string& stGroupName, const std::string& stRow, const std::string& stCol, OUT T& tValue) const;

	int	GetColumnIndexFromName(const std::string& stName) const;

private:
	typedef std::map <std::string, CGroupNode*> TMapGroup;
	typedef std::map <std::string, CGroupNode::CGroupNodeRow> TMapRow;
	TMapGroup				m_mapChildNodes;
	std::string strGroupName;

	TMapNameToIndex			m_map_columnNameToIndex;
	TMapRow					m_map_rows;
	friend class CGroupTextParseTreeLoader;
};

class CGroupTextParseTreeLoader
{
public:
	CGroupTextParseTreeLoader();
	virtual ~CGroupTextParseTreeLoader();

	bool Load(const char * c_szFileName);
	const char * GetFileName();

	CGroupNode*	GetGroup(const char * c_szGroupName);
private:
	bool LoadGroup(CGroupNode * pGroupNode);
	std::string					m_strFileName;
	DWORD						m_dwcurLineIndex;
	CGroupNode *				m_pRootGroupNode;
	CMemoryTextFileLoader		m_fileLoader;
};

template <typename T>
bool from_string(OUT T& t, IN const std::string& s)
{
	std::istringstream iss(s);
	return !(iss >> t).fail();
}

template <>
inline bool from_string <BYTE>(OUT BYTE& t, IN const std::string& s)
{
	std::istringstream iss(s);
	int temp;
	bool b = !(iss >> temp).fail();
	t = (BYTE)temp;
	return b;
}

template <typename T>
bool CGroupNode::GetValue(size_t i, const std::string & c_rstrColKey, T& tValue) const
{
	if (i > m_map_rows.size())
		return FALSE;
	
	TMapRow::const_iterator row_it = m_map_rows.begin();
	std::advance(row_it, i);
	
	itertype(m_map_columnNameToIndex) col_idx_it = m_map_columnNameToIndex.find(c_rstrColKey);
	if (m_map_columnNameToIndex.end() == col_idx_it)
	{
		return FALSE;
	}

	int index = col_idx_it->second;
	if (row_it->second.GetSize() <= index)
	{
		return FALSE;
	}
	
	return row_it->second.GetValue(index, tValue);
}

template <typename T>
bool CGroupNode::GetValue(const std::string & c_rstrRowKey, const std::string & c_rstrColKey, T& tValue) const
{
	TMapRow::const_iterator row_it = m_map_rows.find(c_rstrRowKey);
	if (m_map_rows.end() == row_it)
	{
		return FALSE;
	}
	itertype(m_map_columnNameToIndex) col_idx_it = m_map_columnNameToIndex.find(c_rstrColKey);
	if (m_map_columnNameToIndex.end() == col_idx_it)
	{
		return FALSE;
	}

	int index = col_idx_it->second;
	if (row_it->second.GetSize() <= index)
	{
		return FALSE;
	}
	
	return row_it->second.GetValue(index, tValue);
}

template <typename T>
bool CGroupNode::GetValue(const std::string & c_rstrRowKey, int index, T& tValue) const
{
	TMapRow::const_iterator row_it = m_map_rows.find(c_rstrRowKey);
	if (m_map_rows.end() == row_it)
	{
		return FALSE;
	}

	if (row_it->second.GetSize() <= index)
	{
		return FALSE;
	}
	return row_it->second.GetValue(index, tValue);
}

template <typename T>
bool CGroupNode::GetGroupValue(const std::string& stGroupName, const std::string& stRow, int iCol, OUT T& tValue) const
{
	CGroupNode* pChildGroup = GetChildNode(stGroupName);
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetValue(stRow, iCol, tValue))
			return true;
	}
	// default group�� ���캽.
	pChildGroup = GetChildNode("default");
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetValue(stRow, iCol, tValue))
			return true;
	}
	return false;
}

template <typename T>
bool CGroupNode::GetGroupValue(const std::string& stGroupName, const std::string& stRow, const std::string& stCol, OUT T& tValue) const
{
	CGroupNode* pChildGroup = GetChildNode(stGroupName);
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetValue(stRow, stCol, tValue))
			return true;
	}
	// default group�� ���캽.
	pChildGroup = GetChildNode("default");
	if (NULL != pChildGroup)
	{
		if (pChildGroup->GetValue(stRow, stCol, tValue))
			return true;
	}
	return false;
}

template <typename T>
bool CGroupNode::CGroupNodeRow::GetValue(const std::string & stColKey, OUT T& value) const
{
	size_t idx = m_pOwnerGroupNode->GetColumnIndexFromName(stColKey);
	if (idx < 0 || idx >= m_vec_values.size())
		return false;
	return from_string(value, m_vec_values[idx]);
}

#ifdef _WIN32


#pragma warning(push) 
#pragma warning(disable: 4018)
#endif // _WIN32

template <typename T>
bool CGroupNode::CGroupNodeRow::GetValue(size_t idx, OUT T& value) const
{
	if (idx < 0 || idx >= m_vec_values.size())
		return false;
	return from_string(value, m_vec_values[idx]);
}
#ifdef _WIN32
#pragma warning(push) 
#endif // _WIN32

#endif

