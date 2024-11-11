#include "stdafx.h"
#include "toolnet.h"
#include "GeClientNet.h"
#include "geBCPacket.h"
#include "gePacketTable.h"
#include "GeSession.h"
#include "geIESModify.h"
#include "toolutil.h"

namespace net
{
	BOOL	Connect(const char * ip, unsigned short port, int & errCode);
	bool Process(int pktType, char* pktBuffer, int pktSize);


	CClientNet		s_clientNet;

	void Init()
	{
		s_clientNet.Init();
		geClientPacket::Init();
	}

	void	Exit()
	{
		s_clientNet.Exit();
	}

	BOOL	Connect(const char * ip, unsigned short port, int & errCode)
	{
		if (s_clientNet.IsConnected())
			return FALSE;

		return s_clientNet.Connect(ip, port, errCode);
	}

	BOOL	IsConnected()
	{
		return s_clientNet.IsConnected();
	}

	BOOL ReqLogin(const char * id, const char * passWord, const char * ip, unsigned short port)
	{
		int errorCode = 0;
		if (FALSE == Connect(ip, port, errorCode))
			return FALSE;

		
		PKT_DECL(CB_LOGIN)

		strcpy(packet.ID, id);
		// 암호화
		char md5Pass[512];
		memset(packet.PW, NULL, PWD_LEN);
		EncodePassword(passWord, packet.PW, md5Pass);

		packet.command	= PACKET_CB_LOGIN;
		packet.version	= 54;	// gecfg::GetVersion();

		s_clientNet.SendPacket(&packet, sizeof(packet));

		return TRUE;
	}

	void	ChangeIESProp(const char * idSpace, int classID, const char * propName, const char * propValue, const char * log)
	{
		char buf[NORMAL_PACKETSIZE];
		imc::CBinary bin(buf, NORMAL_PACKETSIZE);
		bin.Memset();
		PKS_CB_IES_MODIFY_INFO* out = (PKS_CB_IES_MODIFY_INFO*) bin.Allocate(sizeof(PKS_CB_IES_MODIFY_INFO));
		out->command = PACKET_CB_IES_MODIFY_INFO;

		IES_MODIFY_PKS_INFO* info = &out->info;
		strncpy(info->idSpace, idSpace, IDSPACE_NAME_LEN);
		info->classID = classID;
		WriteStringToBinary(propName, bin);
		WriteStringToBinary(propValue, bin);
		WriteStringToBinary(log, bin);
		out->len = bin.GetPosition();

		s_clientNet.SendPacket( buf, out->len );

	}

	void DeleteIESHis(int delDB, int delHistory, int rollbackValue, const INT_LIST& revisionList)
	{
		char buf[NORMAL_PACKETSIZE];
		imc::CBinary bin(buf, NORMAL_PACKETSIZE);
		bin.Memset();
		PKS_CB_IES_REVISION_DELETE* out = (PKS_CB_IES_REVISION_DELETE*) bin.Allocate(sizeof(PKS_CB_IES_REVISION_DELETE));
		out->command = PACKET_CB_IES_REVISION_DELETE;

		IES_HIS_DEL_INFO& info = out->info;
		info.deleteDB = delDB;
		info.deleteHistory = delHistory;
		info.rollbackValue = rollbackValue;

		WriteIntListToBinary(revisionList, bin);

		out->len = bin.GetPosition();
		s_clientNet.SendPacket( buf, out->len );

	}




	const int buf_len = 32767;
	static char pktBuffer[buf_len];
	static char saveBuffer[buf_len];

	void	Recv()
	{
		if (FALSE == s_clientNet.IsConnected())
			return;

		WORD pktType;
		int pktSize;
		BOOL bRet = FALSE;

		memset(pktBuffer, 0, buf_len);

		int processingPacketCount = 0;
		while (true) {
				if (false == s_clientNet.GetPacket(pktBuffer, &pktSize, &pktType)) {
					break;
				}

			WORD command = *(WORD*)(pktBuffer);
			if (!Process(command, pktBuffer, pktSize)) {

			}
			bRet = TRUE;

		}

	}

	void OnIESModifyInfo(const char * packetBuffer);
	void OnLogout(const char * packetBuffer);
	void OnIESModifyList(const char * packetBuffer);
	void OnIESDelete(const char * packetBuffer);

	bool Process(int pktType, char* pktBuffer, int pktSize)
	{
		switch(pktType)
		{
			case PACKET_BC_LOGOUTOK:
				OnLogout(pktBuffer);
				break;
			case PACKET_BC_IES_MODIFY_INFO:
				OnIESModifyInfo(pktBuffer);
				break;
			case PACKET_BC_IES_MODIFY_LIST:
				OnIESModifyList(pktBuffer);
				break;
			case PACKET_BC_IES_REVISION_DELETE:
				OnIESDelete(pktBuffer);
				break;
			default:
			break;
		}

		return true;
	}


	void OnIESModifyInfo(const char * packetBuffer)
	{
		PKS_BC_IES_MODIFY_INFO* packet = (PKS_BC_IES_MODIFY_INFO*) packetBuffer;
		packetBuffer += sizeof(PKS_BC_IES_MODIFY_INFO);

		IES_MANAGE_INFO info;
		iesman::ReadIESChangeFromBuffer(&packetBuffer, &info);

		IES_HISTORY& history = Session().M_IES_HISTORY;
		iesman::ApplyIESHistory(info, history);
		iesman::ChangeIESEntry(info.idSpace, info.classID, info.propName, info.propValue);

		util::UpdateAllHistory();
	}

	void OnIESModifyList(const char * packetBuffer)
	{
		PKS_BC_IES_MODIFY_LIST* packet = (PKS_BC_IES_MODIFY_LIST*) packetBuffer;
		packetBuffer += sizeof(packetBuffer);

		IES_HISTORY& history = Session().M_IES_HISTORY;
		iesman::ApplyHistoryFromBuffer(packetBuffer, history);
		iesman::UpdateEntryByHistory(history);

		util::UpdateAllHistory();
	}

	void OnIESDelete(const char * packetBuffer)
	{
		PKS_BC_IES_REVISION_DELETE* packet = (PKS_BC_IES_REVISION_DELETE*) packetBuffer;
		packetBuffer += sizeof(PKS_BC_IES_REVISION_DELETE);

		const IES_HIS_DEL_INFO& info = packet->info;
		INT_LIST list;
		ReadIntListFromBuffer(&packetBuffer, &list);

		IES_HISTORY& history = Session().M_IES_HISTORY;
		if (info.deleteHistory){
			iesman::DeleteRevisionList(list, info.rollbackValue, history);
		}
		util::UpdateAllHistory();
	}

	void OnLogout(const char * packetBuffer)
	{
		s_clientNet.Disconnect();
	}
};