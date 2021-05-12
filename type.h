#include <windows.h>
#include <vector>
#include <map>
#define itertype(v) typeof((v).begin())
#include <string>

/*
int number(int from, int to)
{
    if (from > to)
    {
		int tmp = from;
		from = to;
		to = tmp;
    }
	int returnValue = 0;
	if ((to - from + 1) != 0)
		returnValue = ((rand() % (to - from + 1)) + from);
    return returnValue;
}
*/
inline bool str_to_number (bool& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (strtol(in, NULL, 10) != 0);
	return true;
}

inline bool str_to_number (char& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (char) strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number (unsigned char& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (unsigned char) strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number (short& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (short) strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number (unsigned short& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (unsigned short) strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number (int& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (int) strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number (unsigned int& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (unsigned int) strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number (long& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (long) strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number (unsigned long& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (unsigned long) strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number (long long& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (long long) strtoull(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned long long& out, const char *in)
{
	if (0 == in || 0 == in[0])	return false;

	out = strtoull(in, NULL, 10);
	return true;
}
inline bool str_to_number (float& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (float) strtof(in, NULL);
	return true;
}

inline bool str_to_number (double& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (double) strtod(in, NULL);
	return true;
}

inline bool str_to_number (long double& out, const char *in)
{
	if (0==in || 0==in[0])	return false;

	out = (long double) strtold(in, NULL);
	return true;
}


DWORD number_exxxxs(int from, int to)
{
    if (from > to)
    {
		int tmp = from;
		from = to;
		to = tmp;
    }

	DWORD returnValue = 0;
	if ((to - from + 1) != 0)
		returnValue = ((rand() % (to - from + 1)) + from);
    return returnValue;
}

class CSpecialItemGroup
{
	public:
		enum EGiveType
		{
			NONE,
			GOLD,
			EXP,
			MOB,
			SLOW,
			DRAIN_HP,
			POISON,
			BLEED,
			MOB_GROUP,
		};
		enum ESIGType { NORMAL, PCT, QUEST, SPECIAL };
		struct CSpecialItemInfo
		{
			DWORD vnum;
			int count;
			int rare;
			CSpecialItemInfo(DWORD _vnum, int _count, int _rare): vnum(_vnum), count(_count), rare(_rare){}
		};
		CSpecialItemGroup(DWORD vnum, BYTE type=0): m_dwVnum(vnum), m_bType(type){}
		void AddItem(DWORD vnum, int count, int prob, int rare)
		{
			if (!prob)
				return;
			if (!m_vecProbs.empty())
				prob += m_vecProbs.back();
			m_vecProbs.push_back(prob);
			m_vecItems.push_back(CSpecialItemInfo(vnum, count, rare));
		}
		bool IsEmpty() const{return m_vecProbs.empty();}

		int GetMultiIndex(std::vector <int> &idx_vec) const
		{
			idx_vec.clear();
			if (m_bType == PCT)
			{
				int count = 0;
				if (number_exxxxs(1,100) <= m_vecProbs[0])
				{
					idx_vec.push_back(0);
					count++;
				}
				for (DWORD i = 1; i < m_vecProbs.size(); i++)
				{
					if (number_exxxxs(1,100) <= m_vecProbs[i] - m_vecProbs[i-1])
					{
						idx_vec.push_back(i);
						count++;
					}
				}
				return count;
			}
			else
			{
				idx_vec.push_back(GetOneIndex());
				return 1;
			}
		}
		
		int GetOneIndex() const
		{
			int n = number_exxxxs(1, m_vecProbs.back());
			itertype(m_vecProbs) it = lower_bound(m_vecProbs.begin(), m_vecProbs.end(), n);
			return std::distance(m_vecProbs.begin(), it);
		}

		int GetVnum(int idx) const{return m_vecItems[idx].vnum;}
		int GetCount(int idx) const{return m_vecItems[idx].count;}
		int GetRarePct(int idx) const{return m_vecItems[idx].rare;}
		bool Contains(DWORD dwVnum) const
		{
			for (DWORD i = 0; i < m_vecItems.size(); i++)
			{
				if (m_vecItems[i].vnum == dwVnum)
					return true;
			}
			return false;
		}
		DWORD GetAttrVnum(DWORD dwVnum) const
		{
			if (CSpecialItemGroup::SPECIAL != m_bType)
				return 0;
			for (itertype(m_vecItems) it = m_vecItems.begin(); it != m_vecItems.end(); it++)
			{
				if (it->vnum == dwVnum)
				{
					return it->count;
				}
			}
			return 0;
		}
		int GetGroupSize() const{return m_vecProbs.size();}
		DWORD m_dwVnum;
		BYTE	m_bType;
		std::vector<int> m_vecProbs;
		std::vector<CSpecialItemInfo> m_vecItems;
};

class CLevelItemGroup
{
	struct SLevelItemGroupInfo
	{
		DWORD dwVNum;
		DWORD dwPct;
		int iCount;

		SLevelItemGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
			: dwVNum(dwVnum), dwPct(dwPct), iCount(iCount)
		{ }
	};

	private :
		DWORD m_dwLevelLimit;
		std::string m_stName;
		std::vector<SLevelItemGroupInfo> m_vec_items;

	public :
		CLevelItemGroup(DWORD dwLevelLimit)
			: m_dwLevelLimit(dwLevelLimit)
		{}

		DWORD GetLevelLimit() { return m_dwLevelLimit; }

		void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
		{
			m_vec_items.push_back(SLevelItemGroupInfo(dwItemVnum, dwPct, iCount));
		}

		const std::vector<SLevelItemGroupInfo> & GetVector()
		{
			return m_vec_items;
		}
};

class CSpecialAttrGroup
{
public:
	CSpecialAttrGroup(DWORD vnum)
		: m_dwVnum(vnum)
	{}
	struct CSpecialAttrInfo
	{
		CSpecialAttrInfo (DWORD _apply_type, DWORD _apply_value)
			: apply_type(_apply_type), apply_value(_apply_value)
		{}
		DWORD apply_type;
		DWORD apply_value;

	};
	DWORD m_dwVnum;
	std::string	m_stEffectFileName;
	std::vector<CSpecialAttrInfo> m_vecAttrs;
};

class CMobItemGroup
{
	public:
		struct SMobItemGroupInfo
		{
			DWORD dwItemVnum;
			int iCount;
			int iRarePct;

			SMobItemGroupInfo(DWORD dwItemVnum, int iCount, int iRarePct)
				: dwItemVnum(dwItemVnum),
			iCount(iCount),
			iRarePct(iRarePct)
			{
			}
		};

		CMobItemGroup(DWORD dwMobVnum, int iKillDrop, const std::string& r_stName)
			:
			m_dwMobVnum(dwMobVnum),
		m_iKillDrop(iKillDrop),
		m_stName(r_stName)
		{
		}

		int GetKillPerDrop() const
		{
			return m_iKillDrop;
		}

		void AddItem(DWORD dwItemVnum, int iCount, int iPartPct, int iRarePct)
		{
			if (!m_vecProbs.empty())
				iPartPct += m_vecProbs.back();
			m_vecProbs.push_back(iPartPct);
			m_vecItems.push_back(SMobItemGroupInfo(dwItemVnum, iCount, iRarePct));
		}

		// MOB_DROP_ITEM_BUG_FIX
		bool IsEmpty() const
		{
			return m_vecProbs.empty();
		}
		
		/*
		int GetOneIndex() const
		{
			int n = number(1, m_vecProbs.back());
			itertype(m_vecProbs) it = lower_bound(m_vecProbs.begin(), m_vecProbs.end(), n);
			return std::distance(m_vecProbs.begin(), it);
		}
		
		// END_OF_MOB_DROP_ITEM_BUG_FIX

		const SMobItemGroupInfo& GetOne() const
		{
			return m_vecItems[GetOneIndex()];
		}
		*/

	private:
		DWORD m_dwMobVnum;
		int m_iKillDrop;
		std::string m_stName;
		std::vector<int> m_vecProbs;
		std::vector<SMobItemGroupInfo> m_vecItems;
};

class CDropItemGroup
{
	struct SDropItemGroupInfo
	{
		DWORD	dwVnum;
		DWORD	dwPct;
		int	iCount;

		SDropItemGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
			: dwVnum(dwVnum), dwPct(dwPct), iCount(iCount)
			{}
	};

	public:
	CDropItemGroup(DWORD dwVnum, DWORD dwMobVnum, const std::string& r_stName)
		:
		m_dwVnum(dwVnum),
	m_dwMobVnum(dwMobVnum),
	m_stName(r_stName)
	{
	}

	const std::vector<SDropItemGroupInfo> & GetVector()
	{
		return m_vec_items;
	}

	void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
	{
		m_vec_items.push_back(SDropItemGroupInfo(dwItemVnum, dwPct, iCount));
	}

	private:
	DWORD m_dwVnum;
	DWORD m_dwMobVnum;
	std::string m_stName;
	std::vector<SDropItemGroupInfo> m_vec_items;
};

class CBuyerThiefGlovesItemGroup
{
	struct SThiefGroupInfo
	{
		DWORD	dwVnum;
		DWORD	dwPct;
		int	iCount;

		SThiefGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
			: dwVnum(dwVnum), dwPct(dwPct), iCount(iCount)
			{}
	};

	public:
	CBuyerThiefGlovesItemGroup(DWORD dwVnum, DWORD dwMobVnum, const std::string& r_stName)
		:
		m_dwVnum(dwVnum),
	m_dwMobVnum(dwMobVnum),
	m_stName(r_stName)
	{
	}

	const std::vector<SThiefGroupInfo> & GetVector()
	{
		return m_vec_items;
	}

	void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
	{
		m_vec_items.push_back(SThiefGroupInfo(dwItemVnum, dwPct, iCount));
	}

	private:
	DWORD m_dwVnum;
	DWORD m_dwMobVnum;
	std::string m_stName;
	std::vector<SThiefGroupInfo> m_vec_items;
};

class CManuelItemGroup
{
	struct SManuelItemGroupInfo
	{
		DWORD dwVNum;
		DWORD dwPct;
		int iCount;
		SManuelItemGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount): dwVNum(dwVnum),dwPct(dwPct), iCount(iCount)
		{}
	};

	private :
		DWORD m_dwManuelLimit;
		DWORD m_dwManuelLimit2;
		std::string m_stName;
		std::vector<SManuelItemGroupInfo> m_vec_items;

	public :
		CManuelItemGroup(DWORD dwManuelLimit,DWORD dwManuelLimit2):m_dwManuelLimit(dwManuelLimit), m_dwManuelLimit2(dwManuelLimit2)
		{}

		DWORD GetLevelLimitFirst() { return m_dwManuelLimit; }
		DWORD GetLevelLimitSecond() { return m_dwManuelLimit2; }

		void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
		{
			m_vec_items.push_back(SManuelItemGroupInfo(dwItemVnum, dwPct, iCount));
		}

		const std::vector<SManuelItemGroupInfo> & GetVector()
		{
			return m_vec_items;
		}
};


