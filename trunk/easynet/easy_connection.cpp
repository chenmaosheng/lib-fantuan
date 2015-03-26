#include "easy_connection.h"
#include "easy_acceptor.h"
#include "easy_context.h"

EasyConnection::EasyConnection(EasyAcceptor* pAcceptor) : acceptor_(pAcceptor)
{
	recv_context_ = new EasyContext;
	recv_context_->buffer_ = (char*)malloc(1024);
	send_context_ = new EasyContext;
	sem_init(&sem_, 0, 1);
}

int EasyConnection::HandleMessage()
{
	bool bReadOK = false;
	int recvNum = 0;
	printf("start to receve message\n");
	recv_context_->buffer_[0] = '\n';
	recv_context_->len_ = 0;
	while (true)
	{
		recvNum = recv(socket_, recv_context_->buffer_ + recvNum, MAXLINE, 0);
		if (recvNum < 0)
		{
			if (errno == EAGAIN)
			{
				printf("recv eagain\n");
				bReadOK = true;
				break;
			}
			else if (errno == EINTR)
			{
				printf("recv eintr\n");
				continue;
			}
			else
			{
				printf("other recv error\n");
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
			printf("hit maxline, continue\n");
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

	return recv_context_->len_;
}

int EasyConnection::SendMessage()
{
	if (send_context_->len_ == 0)
	{
		return 0;
	}
	printf("send the left\n");
	int writeNum = 0;
	int total = send_context_->len_;
	char* p = send_context_->buffer_;
	bool bWriteOK = false;
	while (true)
	{
		writeNum = send(socket_, p, total, 0);
		printf("send len=%d\n", total);
		if (writeNum == -1)
		{
			if (errno == EAGAIN)
			{
				printf("send eagain\n");
				bWriteOK = true;
				break;
			}
			else if (errno = EINTR)
			{
				printf("send eintr\n");
				continue;
			}
			else
			{
				printf("other send errors\n");
				break;
			}
		}
		else if (writeNum == 0)
		{
			printf("send close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == total)
		{
			printf("send all\n");
			bWriteOK = true;
			break;
		}

		printf("send partially\n");

		total -= writeNum;
		p += writeNum;
	}

	if (bWriteOK)
	{
		send_context_->buffer_[0] = '\0';
		send_context_->len_ = 0;
		ev_.events = EPOLLIN | EPOLLET;
		epoll_ctl(acceptor_->epfd_, EPOLL_CTL_MOD, socket_, &ev_);
	}

	sem_post(&sem_);

	return 0;
}

int EasyConnection::SendMessage(char* buffer, int len)
{
	sem_wait(&sem_);
	bool bWriteOK = false;
	int writeNum = 0;
	send_context_->buffer_ = buffer;
	send_context_->len_ = len;
	printf("sendmessage len=%d\n", send_context_->len_);

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
				return 0;
			}
		}
		else if (writeNum == 0)
		{
			printf("sendmessage close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == send_context_->len_)
		{
			printf("sendmessage all\n");
			bWriteOK = true;
			break;
		}
		printf("sendmessage partially\n");

		send_context_->len_ -= writeNum;
		send_context_->buffer_ += writeNum;
		printf("sendmessage len=%d\n", send_context_->len_);
	}

	if (bWriteOK)
	{
		send_context_->buffer_[0] = '\0';
		send_context_->len_ = 0;
	}

	sem_post(&sem_);

	return 0;
}

