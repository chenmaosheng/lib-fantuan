#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

struct EasyAcceptor
{
	int		socket_;
	int		epfd_;
	epoll_event ev_;
	epoll_event events_[20];
};

struct EasyConnection
{
	int socket_;
	sockaddr_in addr_;
	EasyAcceptor* acceptor_;
	epoll_event ev_;
	char buffer_[1024];
	int len;
};

EasyAcceptor* CreateAcceptor(unsigned int ip, unsigned short port)
{
	EasyAcceptor* pAcceptor = new EasyAcceptor();
	pAcceptor->epfd_ = epoll_create(256);
	pAcceptor->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	pAcceptor->ev_.data.fd = pAcceptor->socket_;
	pAcceptor->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_ADD, pAcceptor->socket_, &pAcceptor->ev_);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	bind(pAcceptor->socket_, (sockaddr*)&addr, sizeof(addr));
	listen(pAcceptor->socket_, SOMAXCONN);
	return pAcceptor;
}

EasyConnection* CreateConnection(EasyAcceptor* pAcceptor)
{
	EasyConnection* pConnection = new EasyConnection();
	pConnection->acceptor_ = pAcceptor;
	return pConnection;
}

int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1)
	{
		return -1;
	}

	return 0;
}

void AcceptConnection(EasyAcceptor* pAcceptor)
{
	socklen_t clilen;
	EasyConnection* pConnection = CreateConnection(pAcceptor);
	pConnection->socket_ = accept(pAcceptor->socket_, (sockaddr*)&pConnection->addr_, &clilen);
	printf("connected, addr=%s\n", inet_ntoa(pConnection->addr_.sin_addr));
	setnonblocking(pConnection->socket_);
	pConnection->ev_.data.ptr = pConnection;
	pConnection->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_ADD, pConnection->socket_, &pConnection->ev_);
}

int handle_message(void* ptr)
{
	EasyConnection* pConnection = (EasyConnection*)(ptr);
	EasyAcceptor* pAcceptor = pConnection->acceptor_;
	printf("ready to receive\n");
	pConnection->len = recv(pConnection->socket_, pConnection->buffer_, 1024, 0);
	printf("receive something, len=%d\n", pConnection->len);
	bool bReadOK = false;
	if (pConnection->len < 0)
	{
		if (errno == EAGAIN)
		{
			printf("no data to receive\n");
		}
	}
	else if (pConnection->len > 0)
	{
		printf("message=%s, len=%d\n", pConnection->buffer_, pConnection->len, pConnection->acceptor_);
		//send(pConnection->socket_, pConnection->buffer_, len, 0);
	}
	else
	{
		printf("client left\n");
		close(pConnection->socket_);
	}

	return pConnection->len;
}

int send_packet(EasyConnection* pConnection, char* buffer, int len)
{
	int sentlen = send(pConnection->socket_, buffer, len, 0);
	if (len == -1)
	{
		if (errno == EAGAIN)
		{
			
		}
	}
	
	if (sentlen < len)
	{

	}
}

int send_message(void* ptr)
{
	EasyConnection* pConnection = (EasyConnection*)(ptr);
	EasyAcceptor* pAcceptor = pConnection->acceptor_;
	printf("message=%s, len=%d\n", pConnection->buffer_, pConnection->len);
	int len = send(pConnection->socket_, pConnection->buffer_, pConnection->len, 0);
	printf("len=%d\n", len);
	if (len == -1)
	{
		if (errno == EAGAIN)
		{
			pConnection->ev_.events = EPOLLIN | EPOLLET;
			epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_MOD, pConnection->socket_, &pConnection->ev_);
		}
	}
	else if (len > 0)
	{
		pConnection->ev_.events = EPOLLIN | EPOLLET;
		epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_MOD, pConnection->socket_, &pConnection->ev_);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	EasyAcceptor* pAcceptor = CreateAcceptor(0, 9001);
	for (;;)
	{
		int nfds = epoll_wait(pAcceptor->epfd_, pAcceptor->events_, 20, 500);
		for (int i = 0; i < nfds; ++i)
		{
			if (pAcceptor->events_[i].data.fd == pAcceptor->socket_)
			{
				AcceptConnection(pAcceptor);
			}
			else if (pAcceptor->events_[i].events & EPOLLIN)	
			{
				handle_message(pAcceptor->events_[i].data.ptr);
			}
			else if (pAcceptor->events_[i].events & EPOLLOUT)
			{
				send_message(pAcceptor->events_[i].data.ptr);
			}
		}
	}
}
