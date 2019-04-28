#include "PacketManager.h"

PacketManager::PacketManager(ClientInfo *clientInfo)
{
	this->clientInfo = clientInfo;
}

void PacketManager::PacketProcess()
{
	// ���� ��Ŷ : clientInfo->messageBuffer
	// ���� ��Ŷ�� �м� �������Ƿ� messageBuffer �ȿ�
	// � ���������� ����������� ������,
	// �Ѱ��� Ȯ���� ���� [���� �տ��� ��ɾ�
	// - ProtoCommand�� �� ���� ��]�� �� �� �ִ�.
	// ���⼭ ProtoCommand�� ��������
	ProtoCommand *cmd = (ProtoCommand *)clientInfo->messageBuffer;

	// ���� ��Ŷ���� ProtoCommand�� �������Ƿ�
	// �� ��Ŷ�� � ��������� �� �� �ִ�.
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
	// Note : DB�� Ȯ���ؼ� ID�� �н����尡 �´��� Ȯ��

	//TODO : �̹� ���� id�� Ŭ���̾�Ʈ�� ������ ���ӺҰ�
	// ���� id�� �̹� ���� �ִ����� Ȯ���Ϸ��� clientList��
	// Ȯ���ؾ� �Ѵ�.
	// ����Ʈ ���ο��� �����ΰ��� ã�� ���ؼ��� algorithm�� find
	// �Լ��� ����� �� �ִ�.
	// find�� �ϳ��� ��ü�� �޾Ƶ鿩, == ������ ����� ����
	// ��ü�� ã�´�.  �׷��Ƿ� find�� ����ϱ� ���ؼ���
	// ==�����ڸ� �����ε��ؾ� �Ѵ�.
	// find_if�� �츮�� ã������ ��ü�� ������ ���ٽ����� �����
	// �ֹǷ� ==�����ڸ� �����ε��� �ʿ䰡 ����.

	mutexList.lock();
	list<ClientInfo *>::iterator fnd =
	find_if(clientList.begin(), clientList.end(),
		[loginREQ](ClientInfo *ci)// �����μ��� clientList���� ���ٽ����� �����ϴ� �μ�
		->
		bool
		{	//  ����Ʈ���� ���޵� ci�� �߿���
			// LoginREQ�� �� id�� ������ ���� ã�´�.
			// �� ci->id�� loginREQ->userID�� ��
			// �� �� string���� �ƴ϶� ���ڹ迭�̹Ƿ�
			// strcmp�� ����ؾ� �Ѵ�.
			int cmp = strncmp(ci->id, (char *)loginREQ->userid,
							maxUserIDLen);
			// ���� �� ���ڿ��� �����ϴٸ� 0,
			// �������� �ʴٸ� 0�� �ƴ� �ٸ� ���� ��ȯ�Ѵ�.
			return cmp == 0; //
			// cmp�� 0�̶�� ���� id�� loginREQ->userID��
			// ���ٴ� ���̸�, �츮�� ã������ ��
		}		
	);
	// find_if�� ����� fnd�� clientList.end()�� �� �� �ݺ����̱⿡
	// mutex�� Ǭ �� �ٸ� �����忡�� ����Ʈ�� �����ϸ� ������ ���� ���ɼ���
	// �ִ�.
	// �׷��Ƿ� �ݺ��ڿ� ���� �۾��� ���ؽ��� Ǯ���� ���� ������ ������ �Ѵ�.
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

		// �� ��� �ߺ����� ������ ClientInfo��
		// this->clientInfo�̹Ƿ� this->clientInfo��
		// ������ ���������� �˷��� �Ѵ�.
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
	// TODO : �α�Ȯ���� ������ �ʾ����� ����
	// TODO : ������ ������ �������� ����

	// this->clientInfo : ä���� �ϴ� ��
	if (!clientInfo->loginComplete) // �α��� �Ϸ���� �ʾҴٸ�
	{
		SendChatACK(ChatACKNotLogin);
		return;
	}


	// 0. ����� ũ���� �޸� Ȯ��
	char buffer[1024];

	// 1. Ŭ���̾�Ʈ�� ACK��Ŷ ������
	/*
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrChatACK *chatACK = (StrChatACK *)cmd->data;
	cmd->command = ComChatACK;
	chatACK->result = 0; // ����
	send(clientInfo->socket,
		buffer, sizeof(*cmd) + sizeof(*chatACK), 0);
	// Ŭ���̾�Ʈ�� ACK ��Ŷ ������ ����
	*/
	SendChatACK(ChatACKSuccess);


	// 2. ��� Ŭ���̾�Ʈ���� CMD ��Ŷ ������
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrChatCMD *chatCMD = (StrChatCMD *)cmd->data;
	cmd->command = ComChatCMD;
	strncpy_s((char *)chatCMD->userid, maxUserIDLen,
				clientInfo->id, maxUserIDLen);
	strncpy_s((char *)chatCMD->chat, maxChatSize,
				(char *)chatREQ->chat, maxChatSize);
	// ���⼭ ���� ���������� ��ü Ŭ���̾�Ʈ���� ������.
	mutexList.lock();
	for (list<ClientInfo *>::iterator client = clientList.begin();
		client != clientList.end(); ++client)
	{
		// client�� ���� send
		// ��, �α��� �Ϸ���� ���� Ŭ���̾�Ʈ���Դ� �������� �ȵȴ�.
		if ((*client)->loginComplete) // => �α��� �Ϸ�� Ŭ���̾�Ʈ���Ը�
			send((*client)->socket, buffer,
					sizeof(ProtoCommand)+sizeof(StrChatCMD), 0);
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
	// ActionLoginREQ������ �ش� id�� �ִ����� Ȯ���ؾ� �߱⿡
	// mutexList.unlock()�� �ϱ� ���� �ݺ��ڸ� Ȯ���ϴ� ���� 
	// ���´�.
	// ���������� ���⼭�� unlock()�� �ϸ� �ٸ� �����忡�� ����Ʈ��
	// �ǵ�� �ݺ��ڰ� ��ȿȭ�� �� �����Ƿ� unlock()�� �ϱ� ����
	// �ݺ��ڿ� ���� ó���� ������ �Ѵ�.
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
	char buffer[sizeof(ProtoCommand)+sizeof(StrFeelingCMD)];

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
		// client�� ���� send
		// ��, �α��� �Ϸ���� ���� Ŭ���̾�Ʈ���Դ� �������� �ȵȴ�.
		if ((*client)->loginComplete) // => �α��� �Ϸ�� Ŭ���̾�Ʈ���Ը�
			send((*client)->socket, buffer,
			sizeof(ProtoCommand)+sizeof(StrFeelingCMD), 0);
	}
	mutexList.unlock();
}