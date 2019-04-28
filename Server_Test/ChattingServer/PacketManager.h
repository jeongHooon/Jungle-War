#pragma once
#include "../Chat.h"
#include "ClientInfo.h"

// ��Ŷó���� ������ Ŭ���� �����
class PacketManager
{
private:
	ClientInfo *clientInfo;

	// Ŭ���̾�Ʈ���� ���� ��Ŷ�� ó���ϴ� �޼���
	void ActionLoginREQ(StrLoginREQ *loginREQ);
	void ActionChatREQ(StrChatREQ *chatREQ);
	void ActionWisperREQ(StrWisperREQ *wisperREQ);
	void ActionFeelingREQ(StrFeelingREQ *feelingREQ);

	// Ŭ���̾�Ʈ�� Ư���� ��Ŷ�� ������ �޼���
	void SendLoginACK(EnumLoginACK result); // Ŭ���̾�Ʈ�� LoginACK ��Ŷ�� ������ �޼���
	void SendChatACK(EnumChatACK result);
	void SendWisperACK(EnumWisperACK result);
	void SendWisperCMD(ClientInfo *toWhom, StrWisperREQ *wisperREQ);
	void SendFeelingACK(EnumFeelingACK result);
	void SendFeelingCMD(ClientInfo *userID, StrFeelingREQ *feelingREQ);
public:
	PacketManager(ClientInfo *clientInfo);
	void PacketProcess();
};

