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

struct EasyAcceptor
{
	int		socket_;
	int		epfd_;
	epoll_event ev_;
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

struct EasyConnection
{
	int socket_;
	sockaddr_in addr_;
	EasyAcceptor* acceptor_;
	epoll_event ev_;
};

EasyConnection* CreateConnection(EasyAcceptor* pAcceptor)
{
	EasyConnection* pConnection = new EasyConnection();
	pConnection->acceptor_ = pAcceptor;
	return pConnection;
}

int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
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
	pConnection->ev_.data.fd = pConnection->socket_;
	pConnection->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_ADD, pConnection->socket_, &pConnection->ev_);
}

int handle_message(int connfd)
{
	char buf[1024] = {0};
	int len;
	len = recv(connfd, buf, 1024, 0);
	if (len > 0)
	{
		printf("message=%s, len=%d\n", buf, len);
	}

	return len;
}

int main(int argc, char* argv[])
{
	struct epoll_event events[20];
	EasyAcceptor* pAcceptor = CreateAcceptor(0, 9001);
	for (;;)
	{
		int nfds = epoll_wait(pAcceptor->epfd_, events, 20, 500);
		for (int i = 0; i < nfds; ++i)
		{
			if (events[i].data.fd == pAcceptor->socket_)
			{
				AcceptConnection(pAcceptor);
			}
			else if (events[i].events & EPOLLIN)	
			{
				handle_message(events[i].data.fd);
			}
		}
	}
}
