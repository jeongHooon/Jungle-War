#include "PacketManager.h"

PacketManager::PacketManager(ClientInfo *clientInfo)
{
	this->clientInfo = clientInfo;
}

void PacketManager::PacketProcess()
{

	ProtoCommand *cmd = (ProtoCommand *)clientInfo->messageBuffer;

	switch (cmd->command) // 패킷의 명령어
	{
	case ComLoginREQ://이 뒤에는 StrLoginREQ
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
			[loginREQ](ClientInfo *ci)// 전달인수는 clientList에서 람다식으로 전달하는 인수
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
	{// 같은 아이디가 없다.
		cout << loginREQ->userid << " 접속 완료" << endl;
		this->clientInfo->loginComplete = true;
	}
	else
	{// 같은 아이디로 중복접속
		cout << loginREQ->userid << " 중복접속" << endl;
		// TODO : 클라이언트로 중복접속되었다는 메시지를 보낸다.
		SendLoginACK(LoginACKDuplicateConnect);

		
		this->clientInfo->loginComplete = false;
		return;
	}




	// clientInfo에 id를 설정
	mutexList.lock();
	strncpy_s(
		clientInfo->id,			// 복사될 위치
		maxUserIDLen,			// 복사될 위치의 크기
		(char *)loginREQ->userid,// 어디에서 복사할 것이냐
		maxUserIDLen);			// 몇개나 복사할 것이냐
	mutexList.unlock();

	// 클라이언트로 접속 허락한다는 패킷 보내기
	SendLoginACK(LoginACKConnectAllow);// 접속 허용
}

void PacketManager::ActionChatREQ(StrChatREQ *chatREQ)
{

	if (!clientInfo->loginComplete) // 로긴이 완료되지 않았다면
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

		if ((*client)->loginComplete) // => 로긴이 완료된 클라이언트에게만
			send((*client)->socket, buffer,
				sizeof(ProtoCommand) + sizeof(StrChatCMD), 0);
	}
	mutexList.unlock();
}

void PacketManager::ActionWisperREQ(StrWisperREQ *wisperREQ)
{
	// 1. 귓말을 보낼 수 있는 상황인지 확인
	if (!clientInfo->loginComplete) // 로긴이 아직 안되어 있다면 
	{
		SendWisperACK(WisperACKNotLogin);
		return; // 로긴이 안되어 있으므로 더이상 wisper처리를 할 필요 없음
	}

	// 2. 귓말을 보낼 대상을 찾는다. - 리스트에서 해당 id를 찾는다.

	mutexList.lock();
	list<ClientInfo *>::iterator fnd =
		find_if(clientList.begin(), clientList.end(),
			[wisperREQ](ClientInfo *ci)//인수는 clientList에서 보내는 객체
			-> bool
	{
		int cmp = strncmp(ci->id, (char *)wisperREQ->toWhom, maxUserIDLen);
		return cmp == 0;// strcmp의 결과가 0이라는 것은 같다는 뜻
	}
	);

	if (fnd != clientList.end())
	{
		// WisperAck
		SendWisperACK(WisperACKSuccess); // 대상을 찾아서 귓말을 보내므로

		// fnd를 향해 채팅을 보낸다.
		SendWisperCMD(*fnd, wisperREQ);
		// fnd가 반복자이므로 *fnd는 반복자가 가리키는 객체->ClientInfo의 포인터
	}
	else
		SendWisperACK(WisperACKNotFound); // 대상을 찾지 못했음

	mutexList.unlock();
}

void PacketManager::ActionFeelingREQ(StrFeelingREQ *feelingREQ)
{

	if (!clientInfo->loginComplete) // 로긴이 완료되지 않았다면
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

		if ((*client)->loginComplete) // => 로긴이 완료된 클라이언트에게만
			send((*client)->socket, buffer,
				sizeof(ProtoCommand) + sizeof(StrFeelingCMD), 0);
	}
	mutexList.unlock();
}