#include "stdafx.h"
#include "imcStr.h"
#include "FunctionExtracter.h"
#include "io.h"
#include "imcXml.h"
#include <map>
#include <algorithm>
#include "imcTokenizer.h"


imc::CVector<std::string>	effectcmds;

namespace imc
{

	imc::CVector<std::string>* GetCmdsList()
	{
		return &effectcmds;
	}

	void RegisterEffectCmds(imc::CVector<std::string>& list)
	{
		if (list.Count() == 0)
		{
			effectcmds.AddToTail("forkptcl");
			effectcmds.AddToTail("particle");
		} else {
			for (int j = 0 ; j < list.Count() ; j ++)
			{
				effectcmds.AddToTail(list.Element(j));
			}
		}
	}

	bool IsEffectCmds(const char * cmd)
	{
		for (int j = 0 ; j < effectcmds.Count() ; j ++)
		{
			if (effectcmds.Element(j) == cmd)
				return true;
		}

		return false;
	}

	bool IsEffectEvent(TiXmlElement* event)
	{
		for (int j = 0 ; j < effectcmds.Count() ; j ++)
		{
			if (event->Attribute(effectcmds.Element(j).c_str())){
				return true;
			}
		}

		return false;
	}	

	struct EFFECT_INFO
	{
		double	time;
		TiXmlElement*	elmt;
	};

	typedef imc::CVector<EFFECT_INFO> EFFECT_LIST;

	struct CHANNEL
	{
		std::string		name;
		EFFECT_LIST*	Effect;
	};

	typedef imc::CVector<CHANNEL*>	CHANNEL_LIST;

	struct ACTION
	{
		std::string		name;
		CHANNEL_LIST*	ChannelList;
	};


	typedef imc::CVector<ACTION*> ACTION_LIST;
	
	void ParseEffectInfo(const char * pFileNamePath, ACTION_LIST& ActionList)
	{
		imcfile::IFileMem* pFileMem = imcfile::Load(pFileNamePath, true);
		if (!pFileMem)
		{
			LOG_LINE("");
			return;
		}

		TiXmlDocument userInfoDoc;
		char* pData = pFileMem->GetData();
		if (userInfoDoc.Parse(pData) && userInfoDoc.Error() == FALSE)
		{

			TiXmlHandle hDoc(&userInfoDoc);
			TiXmlElement* pElem;
			TiXmlHandle hRoot(0);

			pElem=hDoc.FirstChildElement().Element();
			if (!pElem) 
				return;

			hRoot=TiXmlHandle(pElem);

			pElem=hRoot.FirstChild("ActionList").FirstChild().Element();
			for (pElem; pElem; pElem=pElem->NextSiblingElement()) {

				const char *pActionName = pElem->Attribute("name");

				ACTION* pAction = new ACTION;
				pAction->name = pActionName;
				pAction->ChannelList = new CHANNEL_LIST;
				ActionList.AddToTail(pAction);

				

				TiXmlElement* channel;
				for (channel = pElem->FirstChildElement("Channel"); channel; channel=channel->NextSiblingElement()) {

					const char *pName=channel->Attribute("name");

					CHANNEL* pChannel = new CHANNEL;
					pChannel->Effect = new EFFECT_LIST;
					pChannel->name = pName;
					pAction->ChannelList->AddToTail(pChannel);

					TiXmlElement* event;
					for (event = channel->FirstChildElement("Event"); event; event=event->NextSiblingElement("Event")) {
						double time;
						event->QueryDoubleAttribute("time", &time);

						if (IsEffectEvent(event)){
							EFFECT_INFO	info;
							info.elmt = (TiXmlElement*) event->Clone();
							info.time = time;
							pChannel->Effect->AddToTail(info);							
						}

					}
				}
			}

		}

	}

	

	EFFECT_INFO* GetEffectElement(const char * pActionName, const char * pChannelName, double starttime, double endtime, ACTION_LIST& ActionList)
	{
		int j;
		int count = ActionList.Count();
		for (j = 0 ; j < count ; j ++)
		{
			ACTION* pAction = ActionList.Element(j);
			if (pAction->name != pActionName)
				continue;

			int i;
			int channelcnt = pAction->ChannelList->Count();
			for (i = 0 ; i < channelcnt ; i ++)
			{
				CHANNEL* pChannal = pAction->ChannelList->Element(i);
				if (pChannal->name != pChannelName)
					continue;

				int k;
				int EventCnt = pChannal->Effect->Count();
				for (k = 0 ; k < EventCnt ; k++)
				{
					EFFECT_INFO* pInfo = &pChannal->Effect->Element(k);
					double beforetime = pInfo->time;

					if (beforetime >= starttime && beforetime < endtime){
						EFFECT_INFO* ret = new EFFECT_INFO;
						ret->time = pInfo->time;
						ret->elmt = pInfo->elmt;
						pChannal->Effect->Remove(k);
						return ret;
					}

				}
			}
		}

		return NULL;
	}

	static char actfilename[1024];

	void TestMergeEffect(const char * pFileNamePath, const char * pFileName)
	{
		ACTION_LIST ActionList;
		
		ParseEffectInfo(pFileNamePath, ActionList);

		strcpy(actfilename, pFileNamePath);
		int len = strlen(actfilename);
		strcpy(actfilename + len - 3, "act");
		
		imcfile::IFileMem* pFileMem = imcfile::Load(actfilename, true);
		if (!pFileMem)
		{
			LOG_LINE("");
			return;
		}

		TiXmlDocument userInfoDoc;
		char* pData = pFileMem->GetData();
		if (userInfoDoc.Parse(pData) && userInfoDoc.Error() == FALSE)
		{

			TiXmlHandle hDoc(&userInfoDoc);
			TiXmlElement* pElem;
			TiXmlHandle hRoot(0);

			pElem=hDoc.FirstChildElement().Element();
			if (!pElem) 
				return;

			// save this for later
			hRoot=TiXmlHandle(pElem);		

			pElem=hRoot.FirstChild("ActionList").FirstChild().Element();
			for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
				
				const char *pActionName = pElem->Attribute("name");
				
				TiXmlElement* channel;
				for (channel = pElem->FirstChildElement("Channel"); channel; channel=channel->NextSiblingElement()) {
					
					TiXmlElement* event = channel->FirstChildElement("Event");
					while(true)
					{
						if (event == NULL)
							break;

						if (IsEffectEvent(event))
						{
							channel->RemoveChild(event);
							event = channel->FirstChildElement("Event");
							continue;
						}

						event=event->NextSiblingElement("Event");
					}

					const char * pChannelName =channel->Attribute("name");
					
					double beforetime = 0.0f;
					event = channel->FirstChildElement("Event");
					while(true)
					{
						if (event == NULL)
							break;

						double time;
						event->QueryDoubleAttribute("time", &time);

						if (NULL == event->NextSiblingElement("Event"))
							time = 9999999999;
						
						EFFECT_INFO* pEffectInfo = GetEffectElement(pActionName, pChannelName, beforetime, time, ActionList);
						if (pEffectInfo)
						{
							channel->InsertBeforeChild(event, *pEffectInfo->elmt);
							delete pEffectInfo->elmt;
							delete pEffectInfo;
							event = channel->FirstChildElement("Event");
							continue;
						}

						beforetime = time;
						event=event->NextSiblingElement("Event");
					}
					
				}
			}

			BOOL saveresult = userInfoDoc.SaveFile(actfilename);

		}

	}

};