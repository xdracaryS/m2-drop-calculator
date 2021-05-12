#include "stdafx.h"
#define cout std::cout
bool IsReadMonster = false;
bool IsReadChest = false;


int MINMAX2(int min, int value, int max)
{
    register int tv;

    tv = (min > value ? min : value);
    return (max < tv) ? max : tv;
}


std::map<DWORD, CDropItemGroup*> m_map_pkDropItemGroup;
std::map<DWORD, CSpecialItemGroup*> m_map_pkSpecialItemGroup;
std::map<DWORD, CSpecialItemGroup*> m_map_pkQuestItemGroup;
std::map<DWORD, CSpecialAttrGroup*> m_map_pkSpecialAttrGroup;
std::map<DWORD, CMobItemGroup*> m_map_pkMobItemGroup;
std::map<DWORD, CLevelItemGroup*> m_map_pkLevelItemGroup;
std::map<DWORD, CManuelItemGroup*> m_map_pkManuelItemGroup;
std::map<DWORD, CBuyerThiefGlovesItemGroup*> m_map_pkGloveItemGroup;

bool ReadChestDrop()
{
	CTextFileLoader loader;

	if (!loader.Load("special_item_group.txt"))
		return false;

	std::string stName;

	for (DWORD i = 0; i < loader.GetChildNodeCount(); ++i)
	{
		loader.SetChildNode(i);
		loader.GetCurrentNodeName(&stName);
		int iVnum;
		if (!loader.GetTokenInteger("vnum", &iVnum))
		{
			
			cout <<"ReadSpecialDropItemFile : Syntax error"<< "special_item_group.txt" <<" : no vnum, node "<<stName.c_str();
			loader.SetParentNode();
			return false;
		}

		//sys_log(0,"DROP_ITEM_GROUP %s %d", stName.c_str(), iVnum);
		TTokenVector * pTok;
		std::string stType;
		int type = CSpecialItemGroup::NORMAL;
		if (loader.GetTokenString("type", &stType))
		{
			stl_lowers(stType);
			if (stType == "pct")
			{
				type = CSpecialItemGroup::PCT;
			}
			else if (stType == "quest")
			{
				type = CSpecialItemGroup::QUEST;
				//quest::CQuestManager::instance().RegisterNPCVnum(iVnum);
			}
			else if (stType == "special")
			{
				type = CSpecialItemGroup::SPECIAL;
			}
		}

		if ("attr" == stType)
		{
			CSpecialAttrGroup * pkGroup = new CSpecialAttrGroup(iVnum);
			for (int k = 1; k < 256; ++k)
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);
				if (loader.GetTokenVector(buf, &pTok))
				{
					/*
					DWORD apply_type = 0;
					int	apply_value = 0;
					str_to_number(apply_type, pTok->at(0).c_str());
					if (0 == apply_type)
					{
						apply_type = FN_get_apply_type(pTok->at(0).c_str());
						if (0 == apply_type)
						{
							cout<<"Invalid APPLY_TYPE "<< pTok->at(0).c_str()<<" in Special Item Group Vnum "<<iVnum;
							return false;
						}
					}
					str_to_number(apply_value, pTok->at(1).c_str());
					if (apply_type > MAX_APPLY_NUM)
					{
						cout<<"Invalid APPLY_TYPE"<<apply_type<<" in Special Item Group Vnum "<<iVnum;
						return false;
					}
					pkGroup->m_vecAttrs.push_back(CSpecialAttrGroup::CSpecialAttrInfo(apply_type, apply_value));
					*/
				}
				else
				{
					break;
				}
			}
			if (loader.GetTokenVector("effect", &pTok))
			{
				pkGroup->m_stEffectFileName = pTok->at(0);
			}
			loader.SetParentNode();
			m_map_pkSpecialAttrGroup.insert(std::make_pair(iVnum, pkGroup));
		}
		else
		{
			CSpecialItemGroup * pkGroup = new CSpecialItemGroup(iVnum, type);
			for (int k = 1; k < 256; ++k)
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if (loader.GetTokenVector(buf, &pTok))
				{
					const std::string& name = pTok->at(0);
					DWORD dwVnum = 0;

					/*
					if (!GetVnumByOriginalName(name.c_str(), dwVnum))
					{
						
						if (name == "???" || name == "exp")
						{
							dwVnum = CSpecialItemGroup::EXP;
						}
						else if (name == "gold")
						{
							dwVnum = CSpecialItemGroup::GOLD;
						}
						else if (name == "mob")
						{
							dwVnum = CSpecialItemGroup::MOB;
						}
						else if (name == "slow")
						{
							dwVnum = CSpecialItemGroup::SLOW;
						}
						else if (name == "drain_hp")
						{
							dwVnum = CSpecialItemGroup::DRAIN_HP;
						}
						else if (name == "poison")
						{
							dwVnum = CSpecialItemGroup::POISON;
						}
						else if (name == "group")
						{
							dwVnum = CSpecialItemGroup::MOB_GROUP;
						}
						else
						{
							cout << "ReadSpecialDropItemFile : there is no item:"<<name.c_str()<<" node "<<  stName.c_str();
							return false;
						}
					}
					*/
					str_to_number(dwVnum, name.c_str());


					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());
					int iProb = 0;
					str_to_number(iProb, pTok->at(2).c_str());

					int iRarePct = 0;
					if (pTok->size() > 3)
					{
						str_to_number(iRarePct, pTok->at(3).c_str());
					}

					//sys_log(0,"        name %s count %d prob %d rare %d", name.c_str(), iCount, iProb, iRarePct);
					pkGroup->AddItem(dwVnum, iCount, iProb, iRarePct);
					continue;
				}

				break;
			}
			loader.SetParentNode();
			if (CSpecialItemGroup::QUEST == type)
			{
				m_map_pkQuestItemGroup.insert(std::make_pair(iVnum, pkGroup));
			}
			else
			{
				m_map_pkSpecialItemGroup.insert(std::make_pair(iVnum, pkGroup));
			}
		}
	}
	return true;
}

bool ReadMonsterDrop()
{
	CTextFileLoader loader;
	bool isReloading = false;
	if (!loader.Load("mob_drop_item.txt"))
		return false;

	for (DWORD i = 0; i < loader.GetChildNodeCount(); ++i)
	{
		std::string stName("");
		loader.GetCurrentNodeName(&stName);
		loader.SetChildNode(i);

		int iMobVnum = 0;
		int iKillDrop = 0;
		int iLevelLimit = 0;
		int iManuelLimit = 0;
		int iManuelLimit2 = 0;

		std::string strType("");

		if (!loader.GetTokenString("type", &strType))
		{
			cout << "ReadMonsterDropItemGroup : Syntax error mob_drop_item.txt : no type (kill|drop), node "<<stName.c_str();
			loader.SetParentNode();
			return false;
		}

		if (!loader.GetTokenInteger("mob", &iMobVnum))
		{
			cout <<"ReadMonsterDropItemGroup : Syntax error mob_drop_item.txt : no mob vnum, node "<<stName.c_str();
			loader.SetParentNode();
			return false;
		}

		if (strType == "kill")
		{
			if (!loader.GetTokenInteger("kill_drop", &iKillDrop))
			{
				cout <<"ReadMonsterDropItemGroup : Syntax error mob_drop_item.txt : no kill drop count, node "<<stName.c_str();
				loader.SetParentNode();
				return false;
			}
		}
		else
		{
			iKillDrop = 1;
		}

		if (strType == "limit")
		{
			if (!loader.GetTokenInteger("level_limit", &iLevelLimit))
			{
				cout <<"ReadmonsterDropItemGroup : Syntax error mob_drop_item.txt : no level_limit, node "<<stName.c_str();
				loader.SetParentNode();
				return false;
			}
		}
		else
		{
			iLevelLimit = 0;
		}
		
		TTokenVector* pTok = NULL;
		if (strType == "manuel")
		{
			if (loader.GetTokenVector("level_manuel", &pTok) )
			{
				str_to_number(iManuelLimit, pTok->at(0).c_str());
				str_to_number(iManuelLimit2, pTok->at(1).c_str());
			}
			else
			{
				cout << "ReadmonsterDropItemGroup : Syntax error mob_drop_item.txt : no level_limit, node "<<stName.c_str();
				loader.SetParentNode();
				return false;
			}
		}
		else
		{
			iManuelLimit = 0;
			iManuelLimit2 = 0;
		}

		//sys_log(0,"MOB_ITEM_GROUP %s [%s] %d %d", stName.c_str(), strType.c_str(), iMobVnum, iKillDrop);

		if (iKillDrop == 0)
		{
			loader.SetParentNode();
			continue;
		}
		if (strType == "kill")
		{
			CMobItemGroup * pkGroup = new CMobItemGroup(iMobVnum, iKillDrop, stName);

			for (int k = 1; k < 256; ++k)
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if (loader.GetTokenVector(buf, &pTok))
				{
					//sys_log(1, "               %s %s", pTok->at(0).c_str(), pTok->at(1).c_str());
					std::string& name = pTok->at(0);
					DWORD dwVnum = 0;

					str_to_number(dwVnum, name.c_str());

					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());

					if (iCount<1)
					{
						cout <<"ReadMonsterDropItemGroup : there is no count for item "<<name.c_str()<<" : node "<<stName.c_str()<<" : vnum "<<dwVnum<<", count "<<iCount;
						return false;
					}

					int iPartPct = 0;
					str_to_number(iPartPct, pTok->at(2).c_str());

					if (iPartPct == 0)
					{
						cout << "ReadMonsterDropItemGroup : there is no drop percent for item "<<name.c_str()<<": node "<<stName.c_str()<<" : vnum "<<dwVnum<<", count "<<iCount<<", pct 0";
						return false;
					}

					int iRarePct = 0;
					str_to_number(iRarePct, pTok->at(3).c_str());
					iRarePct = MINMAX2(0, iRarePct, 100);

					//sys_log(0,"        %s count %d rare %d", name.c_str(), iCount, iRarePct);
					pkGroup->AddItem(dwVnum, iCount, iPartPct, iRarePct);
					continue;
				}

				break;
			}
			m_map_pkMobItemGroup.insert(std::map<DWORD, CMobItemGroup*>::value_type(iMobVnum, pkGroup));

		}
		else if (strType == "drop")
		{
			CDropItemGroup* pkGroup = new CDropItemGroup(0, iMobVnum, stName);
			for (int k = 1; k < 256; ++k)
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if (loader.GetTokenVector(buf, &pTok))
				{
					std::string& name = pTok->at(0);
					DWORD dwVnum = 0;
					str_to_number(dwVnum, name.c_str());
					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());

					if (iCount < 1)
					{
						cout <<"ReadMonsterDropItemGroup : there is no count for item "<< name.c_str()<<" : node "<<stName.c_str();
						delete pkGroup;
						return false;
					}

					float fPercent = atof(pTok->at(2).c_str());

					DWORD dwPct = (DWORD)(10000.0f * fPercent);

					//sys_log(0,"        name %s pct %d count %d", name.c_str(), dwPct, iCount);
					pkGroup->AddItem(dwVnum, dwPct, iCount);

					continue;
				}

				break;
			}
			m_map_pkDropItemGroup.insert(std::map<DWORD, CDropItemGroup*>::value_type(iMobVnum, pkGroup));
		}
		else if ( strType == "limit" )
		{
			CLevelItemGroup* pkLevelItemGroup = new CLevelItemGroup(iLevelLimit);

			for ( int k=1; k < 256; k++ )
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if ( loader.GetTokenVector(buf, &pTok) )
				{
					std::string& name = pTok->at(0);
					DWORD dwItemVnum = 0;
					str_to_number(dwItemVnum, name.c_str());

					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());

					if (iCount < 1)
					{
						delete pkLevelItemGroup;
						return false;
					}

					float fPct = atof(pTok->at(2).c_str());
					DWORD dwPct = (DWORD)(10000.0f * fPct);

					pkLevelItemGroup->AddItem(dwItemVnum, dwPct, iCount);

					continue;
				}

				break;
			}
			m_map_pkLevelItemGroup.insert(std::map<DWORD, CLevelItemGroup*>::value_type(iMobVnum, pkLevelItemGroup));
		}
		else if (strType == "manuel")
		{
			CManuelItemGroup* pkManuelItemGroup = new CManuelItemGroup(iManuelLimit,iManuelLimit2);
			for ( int k=1; k < 256; k++ )
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if ( loader.GetTokenVector(buf, &pTok) )
				{
					std::string& name = pTok->at(0);
					DWORD dwItemVnum = 0;
					str_to_number(dwItemVnum, name.c_str());
					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());
					if (iCount < 1)
					{
						delete pkManuelItemGroup;
						return false;
					}
					float fPct = atof(pTok->at(2).c_str());
					DWORD dwPct = (DWORD)(10000.0f * fPct);
					pkManuelItemGroup->AddItem(dwItemVnum, dwPct, iCount);
					continue;
				}
				break;
			}
			m_map_pkManuelItemGroup.insert(std::map<DWORD, CManuelItemGroup*>::value_type(iMobVnum, pkManuelItemGroup));
		}
		else if (strType == "thiefgloves")
		{
			CBuyerThiefGlovesItemGroup* pkGroup = new CBuyerThiefGlovesItemGroup(0, iMobVnum, stName);
			for (int k = 1; k < 256; ++k)
			{
				char buf[4];
				snprintf(buf, sizeof(buf), "%d", k);

				if (loader.GetTokenVector(buf, &pTok))
				{
					std::string& name = pTok->at(0);
					DWORD dwVnum = 0;

					str_to_number(dwVnum, name.c_str());

					int iCount = 0;
					str_to_number(iCount, pTok->at(1).c_str());
					if (iCount < 1)
					{
						cout << "ReadMonsterDropItemGroup : there is no count for item"<<name.c_str()<<" : node"<<stName.c_str();
						delete pkGroup;
						//M2_DELETE(pkGroup);
						return false;
					}

					float fPercent = atof(pTok->at(2).c_str());

					DWORD dwPct = (DWORD)(10000.0f * fPercent);

					//sys_log(0,"        name %s pct %d count %d", name.c_str(), dwPct, iCount);
					pkGroup->AddItem(dwVnum, dwPct, iCount);

					continue;
				}

				break;
			}

			m_map_pkGloveItemGroup.insert(std::map<DWORD, CBuyerThiefGlovesItemGroup*>::value_type(iMobVnum, pkGroup));
		}
		else
		{
			cout << "ReadMonsterDropItemGroup : Syntax error mob_drop_item.txt : invalid type "<<strType.c_str()<<" (kill|drop), node "<<stName.c_str();
			loader.SetParentNode();
			return false;
		}

		loader.SetParentNode();
	}
	return true;
}

DWORD number_ex(int from, int to)
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

std::map<DWORD,DWORD> MonsterLoop(DWORD mobVnum, DWORD loopCount)
{
	std::map<DWORD,DWORD> vec_item;
	vec_item.clear();
	cout <<"\n Hesaplama basliyor.";
	for(DWORD j=0;j<loopCount;j++)
	{
			itertype(m_map_pkManuelItemGroup) it;
			it = m_map_pkManuelItemGroup.find(mobVnum);
			if (it != m_map_pkManuelItemGroup.end())
			{
				//if (iLevel >= it->second->GetLevelLimitFirst() && iLevel <= it->second->GetLevelLimitSecond())
				{
					typeof(it->second->GetVector()) v = it->second->GetVector();
					for (int i=0; i < v.size(); i++)
					{
						if (v[i].dwPct >= (DWORD)(number_ex(1, 1000000)) && number_ex(1,1000) >= 400)
						{
							DWORD dwVnum = v[i].dwVNum;
							if(vec_item.find(dwVnum) != vec_item.end())
							{
								
								vec_item.find(dwVnum)->second += 1;
							}
							else
							{
								vec_item.insert(std::pair<DWORD, DWORD>(dwVnum, 1));
							}
						}
					}
				}
			}

			itertype(m_map_pkDropItemGroup) it2;
			it2 = m_map_pkDropItemGroup.find(mobVnum);
			if (it2 != m_map_pkDropItemGroup.end())
			{
				typeof(it2->second->GetVector()) v = it2->second->GetVector();
				for (int i = 0; i < v.size(); ++i)
				{
					if (v[i].dwPct >= (DWORD)(number_ex(1, 1000000)) && number_ex(1,1000) >= 400)
					{
						DWORD dwVnum = v[i].dwVnum;
						if(vec_item.find(dwVnum) != vec_item.end())
						{
							vec_item.find(dwVnum)->second += 1;
						}
						else
						{
							vec_item.insert(std::pair<DWORD, DWORD>(dwVnum, 1));
						}
					}
				}
			}
			
			itertype(m_map_pkLevelItemGroup) it3;
			it3 = m_map_pkLevelItemGroup.find(mobVnum);
			if ( it3 != m_map_pkLevelItemGroup.end() )
			{
				//if ( it->second->GetLevelLimit() <= (DWORD)iLevel )
				{
					typeof(it3->second->GetVector()) v = it3->second->GetVector();
					for ( int i=0; i < v.size(); i++ )
					{
						if (v[i].dwPct >= (DWORD)(number_ex(1, 1000000)) && number_ex(1,1000) >= 400)
						{
							DWORD dwVnum = v[i].dwVNum;
							if(vec_item.find(dwVnum) != vec_item.end())
							{
								vec_item.find(dwVnum)->second += 1;
							}
							else
							{
								vec_item.insert(std::pair<DWORD, DWORD>(dwVnum, 1));
							}
						}
					}
				}
			}
	cout <<".";
	usleep(2000);
	}
	cout <<"\n Hesaplama bitti.\n";
	return vec_item;
}

std::map<DWORD,DWORD> ChestLoop(DWORD chestVnum, DWORD loopCount)
{
	std::map<DWORD,DWORD> vec_item;
	vec_item.clear();
	CSpecialItemGroup* pGroup;
	itertype(m_map_pkSpecialItemGroup) it = m_map_pkSpecialItemGroup.find(chestVnum);
	if (it != m_map_pkSpecialItemGroup.end())
	{
		pGroup=it->second;
	}
	else
	{
		cout << "None item vnum!";
		return vec_item;
	}
	
	
	for(DWORD j=0;j<loopCount;j++)
	{
		std::vector <int> idxes;
		int n = pGroup->GetMultiIndex(idxes);
		for (int i = 0; i < n; i++)
		{
			int idx = idxes[i];
			DWORD dwVnum = pGroup->GetVnum(idx);
			//DWORD dwCount = pGroup->GetCount(idx);
			//int	iRarePct = pGroup->GetRarePct(idx);
			if(vec_item.find(dwVnum) != vec_item.end())
			{
				vec_item.find(dwVnum)->second += 1;
			}
			else
			{
				vec_item.insert(std::pair<DWORD, DWORD>(dwVnum, 1));
			}
		}
	}
	return vec_item;
}
		
bool Loop()
{
	cout << "Hello World - dracaryS - date:(29/12/2019-04:35)\n";
	cout << "version 0.1 - Any bug: dracaryS#9089\n";
	cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";
	cout << "reading mob_drop_item....\n";

	if(ReadMonsterDrop())
	{
		cout << "mob_drop_item successfully read!\n ";
		IsReadMonster = true;
	}
	else
	{
		cout << "mob_drop_item fail!\n ";
	}
	cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";
	cout << "reading special_item_group....\n";
	if(ReadChestDrop())
	{
		cout << "special_item_group successfully read!\n ";
		IsReadChest = true;
	}
	else
	{
		cout << "special_item_group fail!\n ";
	}
	cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";
	cout << "\nWrite Type:\n";
	cout << "1:Monster\n";
	cout << "2:Chest\n";
	int type=0;
	int vnum=0;
	int loop=0;
	std::cin>>type;
	if(type == 1)
	{
		if(!IsReadMonster)
		{
			cout << "mob_drop_item fail...";
			return false;
		}
		cout << "Write MobVnum: \n";
		std::cin>>vnum;
		if(vnum > 0)
		{
			cout << "Write LoopCount: \n";
			std::cin>>loop;
			if(loop > 0)
			{
				std::map<DWORD,DWORD> loopResult = MonsterLoop(vnum,loop);
				std::map<DWORD,DWORD>::iterator it;
				cout << "MOB RESULT:\n";
		       	for (it = loopResult.begin(); it != loopResult.end(); it++)
		       	{
		            cout << "ItemVnum:"<<it->first<<" Count:"<<it->second<<"\n";
		       	}
			}
			else
			{
				system("cls");
				cout << "Wrong Argument! \n";
				return false;
			}
		}
		else
		{
			system("cls");
			cout << "Wrong Argument! \n";
			return false;
		}
	}
	else if(type == 2)
	{
		if(!IsReadChest)
		{
			cout << "special_item_group fail...";
			return false;
		}
		cout << "Write ChestVnum: \n";
		std::cin>>vnum;
		if(vnum > 0)
		{
			cout << "Write LoopCount: \n";
			std::cin>>loop;
			if(loop > 0)
			{
				std::map<DWORD,DWORD> loopResult = ChestLoop(vnum,loop);
				std::map<DWORD,DWORD>::iterator it;
				cout << "CHEST RESULT:\n";
		       	for (it = loopResult.begin(); it != loopResult.end(); it++)
		       	{
		            cout << "ItemVnum:"<<it->first<<" Count:"<<it->second<<"\n";
		       	}
			}
			else
			{
				system("cls");
				cout << "Wrong Argument! \n";
				return false;
			}
		}
		else
		{
			system("cls");
			cout << "Wrong Argument! \n";
			return false;
		}
	}
	return true;
}

void Destroy()
{
	for (std::map<DWORD, CDropItemGroup*>::iterator it = m_map_pkDropItemGroup.begin(); it != m_map_pkDropItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CSpecialItemGroup*>::iterator it = m_map_pkSpecialItemGroup.begin(); it != m_map_pkSpecialItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CSpecialItemGroup*>::iterator it = m_map_pkQuestItemGroup.begin(); it != m_map_pkQuestItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CSpecialAttrGroup*>::iterator it = m_map_pkSpecialAttrGroup.begin(); it != m_map_pkSpecialAttrGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CMobItemGroup*>::iterator it = m_map_pkMobItemGroup.begin(); it != m_map_pkMobItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CLevelItemGroup*>::iterator it = m_map_pkLevelItemGroup.begin(); it != m_map_pkLevelItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CManuelItemGroup*>::iterator it = m_map_pkManuelItemGroup.begin(); it != m_map_pkManuelItemGroup.end(); it++)
	{
		delete it->second;
	}
	for (std::map<DWORD, CBuyerThiefGlovesItemGroup*>::iterator it = m_map_pkGloveItemGroup.begin(); it != m_map_pkGloveItemGroup.end(); it++)
	{
		delete it->second;
	}
}

int main()
{
	if(!Loop())
	{
		// xD 
	}
	Destroy();
	//cout << " good bye.. dracaryS\n";
	system("pause");
	return 0;
}



