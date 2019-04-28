#include "PacketManager.h"

void PacketManager::SendLoginACK(EnumLoginACK result)
{
	// ProtoCommand와 StrLoginACK를 붙여서 보내는 메서드
	// 두 개의 구조체를 담을 만한 메모리를 확보한다.
	char buffer[sizeof(ProtoCommand)+sizeof(StrLoginACK)];
	// buffer의 앞을 ProtoCommand로 설정
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd의 바로 뒤에 loginACK를 이어서 만든다.
	StrLoginACK *loginACK = (StrLoginACK *)cmd->data;

	cmd->command = ComLoginACK;
	loginACK->result = result;

	// 이 두 패킷을 클라이언트로 전달
	send(clientInfo->socket,	//패킷이 들어온 소켓
		buffer,		// 보낼 패킷이 시작되는 위치
		sizeof(ProtoCommand)+sizeof(StrLoginACK), // 패킷 크기
		0);
}

void PacketManager::SendChatACK(EnumChatACK result)
{
	// 충분한 메모리를 확보한다.
	char buffer[sizeof(ProtoCommand)+sizeof(StrChatACK)];

	// 버퍼 앞부분을 ProtoCommand로 세팅
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd 바로 뒤에 StrChatAck
	StrChatACK *chatACK = (StrChatACK *)cmd->data;// 

	// 데이터 세팅
	cmd->command = ComChatACK;
	chatACK->result = result;

	// 패킷보내기
	send(clientInfo->socket, buffer,
		sizeof(ProtoCommand)+sizeof(StrChatACK),
		0);
}

void PacketManager::SendWisperACK(EnumWisperACK result)
{
	char buffer[sizeof(ProtoCommand)+sizeof(StrWisperACK)];
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrWisperACK *wisperAck = (StrWisperACK *)cmd->data;

	cmd->command = ComWisperACK;
	wisperAck->result = result;

	send(clientInfo->socket, buffer, sizeof(*cmd) + sizeof(*wisperAck), 0);
}

void PacketManager::SendWisperCMD(ClientInfo *toWhom, StrWisperREQ *wisperREQ)
{
	char buffer[sizeof(ProtoCommand)+sizeof(StrWisperCMD)];
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrWisperCMD *wisperCMD = (StrWisperCMD *)cmd->data;

	cmd->command = ComWisperCMD;
	strncpy_s((char *)wisperCMD->userID, maxUserIDLen,
				(char *)clientInfo->id, maxUserIDLen);
	// wisperCMD->userID에는 보낸 사람 ID가 들어가야 하므로
	// 보낸 사람인 clientInfo의 id를 복사한다.
	wisperCMD->userID[maxUserIDLen - 1] = '\0';
	strncpy_s((char *)wisperCMD->chat, maxChatSize,
				(char *)wisperREQ->chat, maxChatSize);
	wisperCMD->chat[maxChatSize - 1] = '\0';

	send(toWhom->socket, buffer, sizeof(*cmd) + sizeof(*wisperCMD), 0);
}

void PacketManager::SendFeelingACK(EnumFeelingACK result)
{
	char buffer[sizeof(ProtoCommand)+sizeof(StrFeelingACK)];
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrFeelingACK *feelingACK = (StrFeelingACK *)cmd->data;

	cmd->command = ComFeelinACK;
	feelingACK->result = result;

	send(clientInfo->socket, buffer, sizeof(ProtoCommand)+sizeof(StrFeelingACK), 0);
}

void PacketManager::SendFeelingCMD(ClientInfo *user, StrFeelingREQ *feelingREQ)
{
	char buffer[sizeof(ProtoCommand)+sizeof(StrFeelingCMD)];
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrFeelingCMD *feelingCMD = (StrFeelingCMD *)cmd->data;

	cmd->command = ComFeelingCMD;
	strncpy_s((char *)feelingCMD->userid, maxUserIDLen,
		(char *)clientInfo->id, maxUserIDLen);
	// wisperCMD->userID에는 보낸 사람 ID가 들어가야 하므로
	// 보낸 사람인 clientInfo의 id를 복사한다.
	feelingCMD->userid[maxUserIDLen - 1] = '\0';
	strncpy_s((char *)feelingCMD->chat, maxChatSize,
		(char *)feelingREQ->chat, maxChatSize);
	feelingCMD->chat[maxChatSize - 1] = '\0';

	send(user->socket, buffer, sizeof(*cmd) + sizeof(StrFeelingCMD), 0);
}