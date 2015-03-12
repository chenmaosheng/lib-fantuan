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

EasyAcceptor* CreateAcceptor(sockaddr_in* addr)
{
	EasyAcceptor* pAcceptor = (EasyAcceptor*)malloc(sizeof(EasyAcceptor));
	pAcceptor->epfd_ = epoll_create(1);
	pAcceptor->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	pAcceptor->ev_.data.fd = pAcceptor->socket_;
	pAcceptor->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_ADD, pAcceptor->socket_, &pAcceptor->ev_);
	bind(pAcceptor->socket_, (sockaddr*)addr, sizeof(*addr));
	listen(pAcceptor->socket_, SOMAXCONN);
	return pAcceptor;
}

int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
	{
		return -1;
	}

	return 0;
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
	struct epoll_event ev, events[20];
	int epfd = epoll_create(256);
	sockaddr_in clientaddr, serveraddr;
	int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_aton("0.0.0.0", &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(9001);
	bind(listenfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
	listen(listenfd, 20);
	int nfds = 0, connfd = 0, sockfd=0;
	socklen_t clilen;
	char line[1024] = {0};
	for (;;)
	{
		nfds = epoll_wait(epfd, events, 20, 500);
		for (int i = 0; i < nfds; ++i)
		{
			if (events[i].data.fd == listenfd)
			{
				connfd = accept(listenfd, (sockaddr*)&clientaddr, &clilen);
				printf("connected, addr=%s\n", inet_ntoa(clientaddr.sin_addr));
				setnonblocking(connfd);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}
			else if (events[i].events & EPOLLIN)	
			{
				handle_message(events[i].data.fd);
			}
		}
	}
}
