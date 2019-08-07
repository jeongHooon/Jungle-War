#include "PacketManager.h"

PacketManager::PacketManager(ClientInfo *clientInfo)
{
	this->clientInfo = clientInfo;
}

void PacketManager::PacketProcess()
{

	ProtoCommand *cmd = (ProtoCommand *)clientInfo->messageBuffer;

	switch (cmd->command) // ��Ŷ�� ��ɾ�
	{
	case ComLoginREQ://�� �ڿ��� StrLoginREQ
		ActionLoginREQ((StrLoginREQ *)cmd->data);
		break;
	case ComChatREQ:
		ActionChatREQ((StrChatREQ *)cmd->data);
		break;
	case ComWisperREQ:
		ActionWisperREQ((StrWisperREQ *)cmd->data);
		break;
	case ComFeelingREQ:
		ActionFeelingREQ((StrFeelingREQ *)cmd->data);
		break;
	}
}

void PacketManager::ActionLoginREQ(StrLoginREQ *loginREQ)
{

	mutexList.lock();
	list<ClientInfo *>::iterator fnd =
		find_if(clientList.begin(), clientList.end(),
			[loginREQ](ClientInfo *ci)// �����μ��� clientList���� ���ٽ����� �����ϴ� �μ�
			->
			bool
	{	
		int cmp = strncmp(ci->id, (char *)loginREQ->userid,
			maxUserIDLen);
		return cmp == 0; //
		
	}
	);

	bool duplicateConnect = (fnd != clientList.end());
	mutexList.unlock();
	if (!duplicateConnect)
	{// ���� ���̵� ����.
		cout << loginREQ->userid << " ���� �Ϸ�" << endl;
		this->clientInfo->loginComplete = true;
	}
	else
	{// ���� ���̵�� �ߺ�����
		cout << loginREQ->userid << " �ߺ�����" << endl;
		// TODO : Ŭ���̾�Ʈ�� �ߺ����ӵǾ��ٴ� �޽����� ������.
		SendLoginACK(LoginACKDuplicateConnect);

		
		this->clientInfo->loginComplete = false;
		return;
	}




	// clientInfo�� id�� ����
	mutexList.lock();
	strncpy_s(
		clientInfo->id,			// ����� ��ġ
		maxUserIDLen,			// ����� ��ġ�� ũ��
		(char *)loginREQ->userid,// ��𿡼� ������ ���̳�
		maxUserIDLen);			// ��� ������ ���̳�
	mutexList.unlock();

	// Ŭ���̾�Ʈ�� ���� ����Ѵٴ� ��Ŷ ������
	SendLoginACK(LoginACKConnectAllow);// ���� ���
}

void PacketManager::ActionChatREQ(StrChatREQ *chatREQ)
{

	if (!clientInfo->loginComplete) // �α��� �Ϸ���� �ʾҴٸ�
	{
		SendChatACK(ChatACKNotLogin);
		return;
	}


	char buffer[1024];

	SendChatACK(ChatACKSuccess);


	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrChatCMD *chatCMD = (StrChatCMD *)cmd->data;
	cmd->command = ComChatCMD;
	strncpy_s((char *)chatCMD->userid, maxUserIDLen,
		clientInfo->id, maxUserIDLen);
	strncpy_s((char *)chatCMD->chat, maxChatSize,
		(char *)chatREQ->chat, maxChatSize);

	mutexList.lock();
	for (list<ClientInfo *>::iterator client = clientList.begin();
		client != clientList.end(); ++client)
	{

		if ((*client)->loginComplete) // => �α��� �Ϸ�� Ŭ���̾�Ʈ���Ը�
			send((*client)->socket, buffer,
				sizeof(ProtoCommand) + sizeof(StrChatCMD), 0);
	}
	mutexList.unlock();
}

void PacketManager::ActionWisperREQ(StrWisperREQ *wisperREQ)
{
	// 1. �Ӹ��� ���� �� �ִ� ��Ȳ���� Ȯ��
	if (!clientInfo->loginComplete) // �α��� ���� �ȵǾ� �ִٸ� 
	{
		SendWisperACK(WisperACKNotLogin);
		return; // �α��� �ȵǾ� �����Ƿ� ���̻� wisperó���� �� �ʿ� ����
	}

	// 2. �Ӹ��� ���� ����� ã�´�. - ����Ʈ���� �ش� id�� ã�´�.

	mutexList.lock();
	list<ClientInfo *>::iterator fnd =
		find_if(clientList.begin(), clientList.end(),
			[wisperREQ](ClientInfo *ci)//�μ��� clientList���� ������ ��ü
			-> bool
	{
		int cmp = strncmp(ci->id, (char *)wisperREQ->toWhom, maxUserIDLen);
		return cmp == 0;// strcmp�� ����� 0�̶�� ���� ���ٴ� ��
	}
	);

	if (fnd != clientList.end())
	{
		// WisperAck
		SendWisperACK(WisperACKSuccess); // ����� ã�Ƽ� �Ӹ��� �����Ƿ�

		// fnd�� ���� ä���� ������.
		SendWisperCMD(*fnd, wisperREQ);
		// fnd�� �ݺ����̹Ƿ� *fnd�� �ݺ��ڰ� ����Ű�� ��ü->ClientInfo�� ������
	}
	else
		SendWisperACK(WisperACKNotFound); // ����� ã�� ������

	mutexList.unlock();
}

void PacketManager::ActionFeelingREQ(StrFeelingREQ *feelingREQ)
{

	if (!clientInfo->loginComplete) // �α��� �Ϸ���� �ʾҴٸ�
	{
		SendFeelingACK(FeelingACKNotLogin);
		return;
	}

	SendFeelingACK(FeelingACKSuccess);
	char buffer[sizeof(ProtoCommand) + sizeof(StrFeelingCMD)];

	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrFeelingCMD *feelingCMD = (StrFeelingCMD *)cmd->data;

	cmd->command = ComFeelingCMD;

	strncpy_s((char *)feelingCMD->userid, maxUserIDLen,
		clientInfo->id, maxUserIDLen);
	strncpy_s((char *)feelingCMD->chat, maxChatSize,
		(char *)feelingREQ->chat, maxChatSize);

	mutexList.lock();
	for (list<ClientInfo *>::iterator client = clientList.begin();
		client != clientList.end(); ++client)
	{

		if ((*client)->loginComplete) // => �α��� �Ϸ�� Ŭ���̾�Ʈ���Ը�
			send((*client)->socket, buffer,
				sizeof(ProtoCommand) + sizeof(StrFeelingCMD), 0);
	}
	mutexList.unlock();
}