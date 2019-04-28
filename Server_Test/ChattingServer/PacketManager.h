#pragma once
#include "../Chat.h"
#include "ClientInfo.h"

// 패킷처리를 전담할 클래스 만들기
class PacketManager
{
private:
	ClientInfo *clientInfo;

	// 클라이언트에서 들어온 패킷을 처리하는 메서드
	void ActionLoginREQ(StrLoginREQ *loginREQ);
	void ActionChatREQ(StrChatREQ *chatREQ);
	void ActionWisperREQ(StrWisperREQ *wisperREQ);
	void ActionFeelingREQ(StrFeelingREQ *feelingREQ);

	// 클라이언트로 특정한 패킷을 보내는 메서드
	void SendLoginACK(EnumLoginACK result); // 클라이언트로 LoginACK 패킷을 보내는 메서드
	void SendChatACK(EnumChatACK result);
	void SendWisperACK(EnumWisperACK result);
	void SendWisperCMD(ClientInfo *toWhom, StrWisperREQ *wisperREQ);
	void SendFeelingACK(EnumFeelingACK result);
	void SendFeelingCMD(ClientInfo *userID, StrFeelingREQ *feelingREQ);
public:
	PacketManager(ClientInfo *clientInfo);
	void PacketProcess();
};

