#include "PacketManager.h"

PacketManager::PacketManager(ClientInfo *clientInfo)
{
	this->clientInfo = clientInfo;
}

void PacketManager::PacketProcess()
{
	// 받은 패킷 : clientInfo->messageBuffer
	// 아직 패킷을 분석 못했으므로 messageBuffer 안에
	// 어떤 프로토콜이 들어있을지는 모르지만,
	// 한가지 확실한 것은 [가장 앞에는 명령어
	// - ProtoCommand가 들어가 있을 것]을 알 수 있다.
	// 여기서 ProtoCommand를 꺼내려면
	ProtoCommand *cmd = (ProtoCommand *)clientInfo->messageBuffer;

	// 받은 패킷에서 ProtoCommand를 꺼냈으므로
	// 이 패킷이 어떤 명령인지를 알 수 있다.
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
	// Note : DB를 확인해서 ID와 패스워드가 맞는지 확인

	//TODO : 이미 같은 id의 클라이언트가 있으면 접속불가
	// 같은 id가 이미 들어와 있는지를 확인하려면 clientList를
	// 확인해야 한다.
	// 리스트 내부에서 무엇인가를 찾기 위해서는 algorithm의 find
	// 함수를 사용할 수 있다.
	// find는 하나의 객체를 받아들여, == 연산의 결과가 참인
	// 객체를 찾는다.  그러므로 find를 사용하기 위해서는
	// ==연산자를 오버로드해야 한다.
	// find_if는 우리가 찾으려는 객체의 조건을 람다식으로 명시해
	// 주므로 ==연산자를 오버로드할 필요가 없다.

	mutexList.lock();
	list<ClientInfo *>::iterator fnd =
	find_if(clientList.begin(), clientList.end(),
		[loginREQ](ClientInfo *ci)// 전달인수는 clientList에서 람다식으로 전달하는 인수
		->
		bool
		{	//  리스트에서 전달된 ci들 중에서
			// LoginREQ로 온 id와 동일한 것을 찾는다.
			// 즉 ci->id와 loginREQ->userID와 비교
			// 둘 다 string형이 아니라 문자배열이므로
			// strcmp를 사용해야 한다.
			int cmp = strncmp(ci->id, (char *)loginREQ->userid,
							maxUserIDLen);
			// 만약 두 문자열이 동일하다면 0,
			// 동일하지 않다면 0이 아닌 다른 수를 반환한다.
			return cmp == 0; //
			// cmp가 0이라는 것은 id가 loginREQ->userID와
			// 같다는 뜻이며, 우리가 찾으려는 것
		}		
	);
	// find_if의 결과인 fnd나 clientList.end()는 둘 다 반복자이기에
	// mutex를 푼 후 다른 스레드에서 리스트를 수정하면 문제가 생길 가능성이
	// 있다.
	// 그러므로 반복자에 대한 작업은 뮤텍스가 풀리기 전에 완전히 끝내야 한다.
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

		// 이 경우 중복으로 접속한 ClientInfo가
		// this->clientInfo이므로 this->clientInfo의
		// 인증이 실패했음을 알려야 한다.
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
	// TODO : 로긴확인이 끝나지 않았으면 실패
	// TODO : 지금은 무조건 성공으로 간주

	// this->clientInfo : 채팅을 하는 쪽
	if (!clientInfo->loginComplete) // 로긴이 완료되지 않았다면
	{
		SendChatACK(ChatACKNotLogin);
		return;
	}


	// 0. 충분한 크기의 메모리 확보
	char buffer[1024];

	// 1. 클라이언트로 ACK패킷 보내기
	/*
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrChatACK *chatACK = (StrChatACK *)cmd->data;
	cmd->command = ComChatACK;
	chatACK->result = 0; // 성공
	send(clientInfo->socket,
		buffer, sizeof(*cmd) + sizeof(*chatACK), 0);
	// 클라이언트로 ACK 패킷 보내기 성공
	*/
	SendChatACK(ChatACKSuccess);


	// 2. 모든 클라이언트에게 CMD 패킷 보내기
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrChatCMD *chatCMD = (StrChatCMD *)cmd->data;
	cmd->command = ComChatCMD;
	strncpy_s((char *)chatCMD->userid, maxUserIDLen,
				clientInfo->id, maxUserIDLen);
	strncpy_s((char *)chatCMD->chat, maxChatSize,
				(char *)chatREQ->chat, maxChatSize);
	// 여기서 만든 프로토콜을 전체 클라이언트에게 보낸다.
	mutexList.lock();
	for (list<ClientInfo *>::iterator client = clientList.begin();
		client != clientList.end(); ++client)
	{
		// client에 대해 send
		// 단, 로긴이 완료되지 못한 클라이언트에게는 보내서는 안된다.
		if ((*client)->loginComplete) // => 로긴이 완료된 클라이언트에게만
			send((*client)->socket, buffer,
					sizeof(ProtoCommand)+sizeof(StrChatCMD), 0);
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
	// ActionLoginREQ에서는 해당 id가 있는지만 확인해야 했기에
	// mutexList.unlock()을 하기 전에 반복자를 확인하는 일을 
	// 끝냈다.
	// 마찬가지로 여기서도 unlock()을 하면 다른 스레드에서 리스트를
	// 건드려 반복자가 무효화될 수 있으므로 unlock()을 하기 전에
	// 반복자에 대한 처리를 끝내야 한다.
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
		// client에 대해 send
		// 단, 로긴이 완료되지 못한 클라이언트에게는 보내서는 안된다.
		if ((*client)->loginComplete) // => 로긴이 완료된 클라이언트에게만
			send((*client)->socket, buffer,
			sizeof(ProtoCommand)+sizeof(StrFeelingCMD), 0);
	}
	mutexList.unlock();
}