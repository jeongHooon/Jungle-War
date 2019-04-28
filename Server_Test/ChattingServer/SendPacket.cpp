#include "PacketManager.h"

void PacketManager::SendLoginACK(EnumLoginACK result)
{
	// ProtoCommand�� StrLoginACK�� �ٿ��� ������ �޼���
	// �� ���� ����ü�� ���� ���� �޸𸮸� Ȯ���Ѵ�.
	char buffer[sizeof(ProtoCommand)+sizeof(StrLoginACK)];
	// buffer�� ���� ProtoCommand�� ����
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd�� �ٷ� �ڿ� loginACK�� �̾ �����.
	StrLoginACK *loginACK = (StrLoginACK *)cmd->data;

	cmd->command = ComLoginACK;
	loginACK->result = result;

	// �� �� ��Ŷ�� Ŭ���̾�Ʈ�� ����
	send(clientInfo->socket,	//��Ŷ�� ���� ����
		buffer,		// ���� ��Ŷ�� ���۵Ǵ� ��ġ
		sizeof(ProtoCommand)+sizeof(StrLoginACK), // ��Ŷ ũ��
		0);
}

void PacketManager::SendChatACK(EnumChatACK result)
{
	// ����� �޸𸮸� Ȯ���Ѵ�.
	char buffer[sizeof(ProtoCommand)+sizeof(StrChatACK)];

	// ���� �պκ��� ProtoCommand�� ����
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd �ٷ� �ڿ� StrChatAck
	StrChatACK *chatACK = (StrChatACK *)cmd->data;// 

	// ������ ����
	cmd->command = ComChatACK;
	chatACK->result = result;

	// ��Ŷ������
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
	// wisperCMD->userID���� ���� ��� ID�� ���� �ϹǷ�
	// ���� ����� clientInfo�� id�� �����Ѵ�.
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
	// wisperCMD->userID���� ���� ��� ID�� ���� �ϹǷ�
	// ���� ����� clientInfo�� id�� �����Ѵ�.
	feelingCMD->userid[maxUserIDLen - 1] = '\0';
	strncpy_s((char *)feelingCMD->chat, maxChatSize,
		(char *)feelingREQ->chat, maxChatSize);
	feelingCMD->chat[maxChatSize - 1] = '\0';

	send(user->socket, buffer, sizeof(*cmd) + sizeof(StrFeelingCMD), 0);
}