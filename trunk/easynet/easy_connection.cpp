#include "easy_connection.h"
#include "easy_acceptor.h"
#include "easy_context.h"

EasyConnection::EasyConnection(EasyAcceptor* pAcceptor) : acceptor_(pAcceptor)
{
	pConnection->recv_context_ = new EasyContext(OPERATION_RECV, this);
	pConnection->send_context_ = new EasyContext(OPERATION_SEND, this);
}

int EasyConnection::HandleMessage()
{
	bool bReadOK = false;
	int recvNum = 0;
	recv_context_->buffer_[0] = '\n';
	recv_context_->len_ = 0;
	while (true)
	{
		recvNum = recv(socket_, recv_context_->buffer_ + recvNum, MAXLINE, 0);
		if (recvNum < 0)
		{
			if (errno == EAGAIN)
			{
				bReadOK = true;
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		else if (recvNum == 0)
		{
			printf("close and leave\n");
			close(socket_);
			break;
		}

		recv_context_->len_ += recvNum;
		if (recvNum == MAXLINE)
		{
			continue;
		}
		else
		{
			bReadOK = true;
			break;
		}
	}

	if (bReadOK)
	{
		recv_context_->buffer_[recv_context_->len_] = '\0';
		printf("message=%s, len=%d\n", recv_context_->buffer_, recv_context_->len_);
	}

	return len_;
}

int EasyConnection::SendMessage()
{
	int writeNum = 0;
	int total = send_context_->len_;
	char* p = send_context_->buffer_;
	bool bWriteOK = false;
	while (true)
	{
		writeNum = send(socket_, p, total, 0);
		if (writeNum == -1)
		{
			if (errno == EAGAIN)
			{
				bWriteOK = true;
				break;
			}
			else if (errno = EINTR)
			{
				continue;
			}
			else
			{
				printf("other errors\n");
				break;
			}
		}
		else if (writeNum == 0)
		{
			printf("close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == total)
		{
			bWriteOK = true;
			break;
		}

		total -= writeNum;
		p += writeNum;
	}

	if (bWriteOK)
	{
		ev_.events = EPOLLIN | EPOLLET;
		epoll_ctl(acceptor_->epfd_, EPOLL_CTL_MOD, socket_, &ev_);
	}

	return 0;
}

int EasyConnection::SendMessage(char* buffer, int len)
{
	bool bWriteOK = false;
	int writeNum = 0;
	memcpy(send_context_->buffer, buffer, len);
	send_context_->len_ = len;

	while (true)
	{
		writeNum = send(socket_, send_context_->buffer_, send_context_->len_, 0);
		if (writeNum == -1)
		{
			if (errno == EAGAIN)
			{
				printf("sendmessage, eagain, go\n");
				ev_.events = EPOLLOUT | EPOLLET;
				epoll_ctl(acceptor_->epfd_, EPOLL_CTL_MOD, socket_, &ev_);
				break;
			}
		}
		else if (writeNum == 0)
		{
			printf("send close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == send_context_->len_)
		{
			bWriteOK = true;
			break;
		}
		send_context_->len_ -= writeNum;
		send_context_->buffer_ += writeNum;
	}

	return 0;
}
