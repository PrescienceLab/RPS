#ifndef _socks
#define _socks

#if defined(WIN32) && !defined(__CYGWIN__)
#warning "including windows"
#include <windows.h>
#include <winsock.h>
#else
extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
}
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define SND_RCV_SOCKET_BUF_SIZE 65536
#ifndef INADDR_NONE
#define INADDR_NONE             0xffffffff
#endif


#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#define WRITE(fd,buf,len) send(fd,buf,len,0)
#define READ(fd,buf,len) recv(fd,buf,len,0)
#define CLOSE(x) closesocket(x)
#define IOCTL(x,y,z) ioctlsocket((SOCKET)x,(long)y,(unsigned long *)z)
#else
#define WRITE(fd,buf,len) write(fd,buf,len)
#define READ(fd,buf,len) read(fd,buf,len)
#define CLOSE(x) close(x)
#define IOCTL(x,y,z) ioctl(x,y,z)
#endif


int CreateAndSetupTcpSocket(int bufsize=SND_RCV_SOCKET_BUF_SIZE, 
			    bool nodelay=true,
			    bool nonblocking=false);

int CreateAndSetupUdpSocket(int bufsize=SND_RCV_SOCKET_BUF_SIZE, 
			    bool nonblocking=false);

int CreateAndSetupUnixDomainSocket(int bufsize=SND_RCV_SOCKET_BUF_SIZE,
				   bool nonblocking=false);

int SetNoDelaySocket(const int fd, bool nodelay=true);

int IsSocket(const int fd);
int IsStreamSocket(const int fd);
int IsDatagramSocket(const int fd);

int BindSocket(int mysocket, int myport);
int BindSocket(int mysocket, unsigned adx, int myport);
int BindSocket(int mysocket, char *host_or_ip, int myport);
int BindSocket(int mysocket, char *pathname);

int ListenSocket(int mysocket, int max=SOMAXCONN);

int ConnectToHost(int mysocket, int hostip, int port);
int ConnectToHost(int mysocket, char *host, int port);
int ConnectToPath(int mysocket, char *pathname);


int Send(const int fd, const char *buf, const int len, bool sendall=true);
int Receive(const int fd, char *buf, const int len, bool recvall=true);

int SendTo(int mysocket, 
	   unsigned ip, int port, 
	   const char *buf, const int len, bool sendall=true);
int ReceiveFrom(int mysocket, 
		unsigned ip, int port, 
		char *buf, const int len, bool recvall=true);
int SendTo(int mysocket, 
	   char *host_or_ip, int port, 
	   const char *buf, const int len, bool sendall=true);
int ReceiveFrom(int mysocket, 
		char *host_or_ip, int port, 
		char *buf, const int len, bool recvall=true);


int JoinMulticastGroup(int mysocket, char *IP);
int JoinMulticastGroup(int mysocket, unsigned adx);
int LeaveMulticastGroup(int mysocket, char *IP);
int LeaveMulticastGroup(int mysocket, unsigned adx);
int SetMulticastTimeToLive(int mysocket, const unsigned char ttl);


unsigned GetMyIPAddress();
unsigned ToIPAddress(char *hostname);
void     PrintIPAddress(unsigned adx, FILE *out=stderr);
void     IPToHostname(unsigned ip, char *name, int namesize);
int      IsValidIPMulticastAddress(unsigned ipadx);


int SetSignalHandler(int signum, void (*handler)(int), bool oneshot=false);
int IgnoreSignal(int signum);
int ListenToSignal(int signum);



#endif
