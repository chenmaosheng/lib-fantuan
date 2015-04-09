#include "simple_connection.h"
#include "simple_context.h"

SimpleConnection::SimpleConnection()
{
	recv_context_ = new SimpleContext;
	recv_context_->buffer_ = (char*)_aligned_malloc(MAX_INPUT_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
	send_context_ = new SimpleContext;
}

SimpleConnection::~SimpleConnection()
{
	SAFE_DELETE(send_context_);
	_aligned_free(recv_context_->buffer_);
	SAFE_DELETE(recv_context_);
}

int32 SimpleConnection::Connect(const char* ip, uint16 port, SimpleHandler* pHandler)
{
	socket_ = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	handler_ = *pHandler;
	//unsigned long nNoBlock = 1;
	//ioctlsocket(socket_, FIONBIO, &nNoBlock);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	int32 result = connect(socket_, (sockaddr*)&addr, sizeof(addr) );
	if (result == SOCKET_ERROR)
	{
		//if (GetLastError() != WSAEWOULDBLOCK)
		{
			LOG_ERR(_T("connect failed"));
			Close();
			return -1;
		}	
	}

	handler_.OnConnection((ConnID)this);

	return 0;
}

int32 SimpleConnection::RecvData()
{
	recv_context_->len_ = recv(socket_, recv_context_->buffer_, MAX_INPUT_BUFFER, 0);
	if (recv_context_->len_ == 0)
	{
		LOG_DBG(_T("socket was closed"));
		return -1;
	}

	handler_.OnData((ConnID)this, (uint32)recv_context_->len_, recv_context_->buffer_);

	return 0;
}

int32 SimpleConnection::SendData(uint32 len, char* buf)
{
	int32 writeNum = 0;
	bool bWriteOK = false;
	send_context_->buffer_ = buf;
	send_context_->len_ = len;
	while (true)
	{
		writeNum = send(socket_, send_context_->buffer_, send_context_->len_, 0);
		if (writeNum == SOCKET_ERROR)
		{
			LOG_ERR(_T("SendData failed"));
			Close();
			break;
		}
		else if (writeNum == 0)
		{
			LOG_DBG(_T("sendmessage close and leave"));
			Close();
			break;
		}

		if (writeNum == send_context_->len_)
		{
			LOG_DBG(_T("sendmessage all"));
			bWriteOK = true;
			break;
		}
		LOG_DBG(_T("sendmessage partially"));

		send_context_->len_ -= writeNum;
		send_context_->buffer_ += writeNum;
		LOG_DBG(_T("sendmessage len=%d"), send_context_->len_);
	}

	if (!bWriteOK)
	{
		return -1;
	}
	
	send_context_->buffer_[0] = '\0';
	send_context_->len_ = 0;
	return 0;
}

void SimpleConnection::Close()
{
	closesocket(socket_);
	handler_.OnDisconnect((ConnID)this);
}

void SimpleConnection::SetClient(void* pClient)
{
	client_ = pClient;
}

void* SimpleConnection::GetClient()
{
	return client_;
}