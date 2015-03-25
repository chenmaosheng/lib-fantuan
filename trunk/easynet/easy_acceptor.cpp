#include "easy_acceptor.h"

EasyAcceptor::EasyAcceptor(unsigned int ip, unsigned short port)
{
	epfd_ = epoll_create(256);
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ev_.data.fd = socket_;
	ev_.events = EPOLLIN | EPOLLET | EPOLLOUT;
	epoll_ctl(pAcceptor->epfd_, EPOLL_CTL_ADD, socket_, &ev_);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	bind(socket_, (sockaddr*)&addr, sizeof(addr));
	listen(socket_, SOMAXCONN);
}

void EasyAcceptor::AcceptConnection()
{
	EasyConnection* pConnection = new EasyConnection(this);
	socklen_t cilen;
	pConnection->socket_ = accept(socket_, (sockaddr*)&pConnection->addr_, &clilen);
	fcntl(pConnection->socket_, F_SETFL, O_NONBLOCK);
	pConnection->ev_.data.ptr = pConnection;
	pConnection->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, pConnection->socket_, &pConnection->ev_);
}