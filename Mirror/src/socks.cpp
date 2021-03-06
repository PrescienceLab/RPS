#include "socks.h"
#include "junk.h"
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__sparc__)
#include <sys/filio.h>
#endif

#if defined(__sparc__) || (defined(WIN32) && !defined(__CYGWIN__))
#define SOCKOPT_TYPE char*
#else
#define SOCKOPT_TYPE void*
#endif

#if defined(linux)
#define SOCKOPT_LEN_TYPE unsigned
#else
#define SOCKOPT_LEN_TYPE int
#endif


int GetSockType(const int fd)
{
  int type;
  SOCKOPT_LEN_TYPE len=sizeof(int);
  
  if (getsockopt(fd,SOL_SOCKET,SO_TYPE,(SOCKOPT_TYPE)&type,&len)) {
    return -1;
  } else {
    return type;
  }
}


int IsSocket(const int fd)
{
  return GetSockType(fd)>=0;
}

int IsStreamSocket(const int fd)
{
  return GetSockType(fd)==SOCK_STREAM;
}

int IsDatagramSocket(const int fd)
{
  return GetSockType(fd)==SOCK_DGRAM;
}


int IsValidIPMulticastAddress(const unsigned adx)
{
  
  //int x=(ntohl(port)>>24)&0xff;
  int x=(adx>>24)&0xff;

  if (x<224 || x>239) {
    return 0;
  } else {
    return 1;
  }
}

void IPToHostname(const unsigned ip, char *name, const int namesize) 
{ 
  struct in_addr ia; ia.s_addr=ip;
  struct hostent *he=gethostbyaddr((const char *)&(ia),
				       sizeof(ia),
				       AF_INET);
  strncpy(name,he ? he->h_name: "UNKNOWN HOST",namesize-1);
}


void PrintIPAddress(const unsigned adx, FILE *out)
{
  fprintf(out,"%3d.%3d.%3d.%3d", (adx>>24)&0xff,
	  (adx>>16)&0xff, (adx>>8)&0xff, (adx)&0xff);
}

unsigned ToIPAddress(const char *hostname)
{
    unsigned x;

    if ((x=inet_addr(hostname))!=INADDR_NONE) {
	return ntohl(x);
    } else {
	struct hostent *he;

	if ((he=gethostbyname(hostname))==NULL) {
	    return INADDR_NONE;
	} else {
	    memcpy(&x,he->h_addr,4);
	    x=ntohl(x);
	    return x;
	}
    }
}



#if 0
#define WELL_KNOWN_HOST ((char*)"www.cnn.com")
#define WELL_KNOWN_PORT 80

unsigned GetMyIPAddress()
{
  static unsigned adx=0;
  static bool setup=false;
  char *host;
  short port;

  host = getenv("RPS_WELL_KNOWN_HOST") ? getenv("RPS_WELL_KNOWN_HOST") : WELL_KNOWN_HOST;
  port = getenv("RPS_WELL_KNOWN_PORT") ? atoi(getenv("RPS_WELL_KNOWN_PORT")) : WELL_KNOWN_PORT;


  if (setup) {
    return adx;
  } else {
    // Connect to a well known machine and check out our socket's address
    int fd = CreateAndSetupTcpSocket();
    if (fd) {
      if (ConnectToHost(fd,host,port)) {
	CLOSE(fd);
	setup=true;
	return adx;
      }
      struct sockaddr_in glarpy_sa;
      SOCKOPT_LEN_TYPE len = sizeof(glarpy_sa);
      if (getsockname(fd,(struct sockaddr*)&glarpy_sa,&len)) {
	CLOSE(fd);
	setup=true;
	return adx;
      }
      assert(glarpy_sa.sin_family == AF_INET);
      adx = ntohl(glarpy_sa.sin_addr.s_addr);
      setup=true;
      //PrintIPAddress(adx);
      return adx;
    } else {
      return adx;
    }
  }
}

#else 

unsigned GetMyIPAddress()
{
  static unsigned adx=0;
  static bool setup=false;

  if (setup) { 
    return adx;
  } else {
    char host[1024];
    if (gethostname(host,1024)) { 
      return 0;
    }
    adx=ToIPAddress(host);
    setup=true;
    return adx;
  }
}

#endif

int CreateAndSetupUdpSocket(const int bufsize, const bool nonblocking)
{
  int mysocket;
  int val;

  // create socket for connections
  if ((mysocket=socket(AF_INET,SOCK_DGRAM,0))<0) {
    return -1;
  }
    
  // set reuseaddr to avoid binding problems
  if (setsockopt(mysocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&val,sizeof(int))) {
    return -1;
  }
  
  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_SNDBUF,
                (const char*) &val, sizeof(val))<0) {
      CLOSE(mysocket);
      return -1;
  }

  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_RCVBUF,
                 (const char*)&val, sizeof(val))<0) {
    CLOSE(mysocket);
    return -1;
  }

  if (nonblocking) {
    val=1;
    if (IOCTL(mysocket,FIONBIO,&val)) {
      CLOSE(mysocket);
      return -1;
    }
  }

  return mysocket;
}


int CreateAndSetupTcpSocket(const int bufsize, const bool nodelay, const bool nonblocking)
{
  int mysocket;
  int val;

  // create socket for connections
  if ((mysocket=socket(AF_INET,SOCK_STREAM,0))<0) {
    return -1;
  }
    
  // set reuseaddr to avoid binding problems
  if (setsockopt(mysocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&val,sizeof(int))) {
    return -1;
  }
  
  // Set nodelay so that our messages get
  if (nodelay) {
    val=1;
    if (setsockopt(mysocket,IPPROTO_TCP,TCP_NODELAY,(const char *)&val,sizeof(int))) {
      CLOSE(mysocket);
   return -1;
    }
  }

  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_SNDBUF,
                (const char*) &val, sizeof(val))<0) {
      CLOSE(mysocket);
      return -1;
  }

  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_RCVBUF,
                 (const char*)&val, sizeof(val))<0) {
    CLOSE(mysocket);
    return -1;
  }

  if (nonblocking) {
    val=1;
    if (IOCTL(mysocket,FIONBIO,&val)) {
      CLOSE(mysocket);
      return -1;
    }
  }

  return mysocket;
}


int CreateAndSetupUnixDomainSocket(const int bufsize, const bool nonblocking)
{
  int mysocket;
  int val;

  // create socket for connections
  if ((mysocket=socket(AF_UNIX,SOCK_STREAM,0))<0) {
    return -1;
  }
    
  // set reuseaddr to avoid binding problems
  if (setsockopt(mysocket,SOL_SOCKET,SO_REUSEADDR,(const char *)&val,sizeof(int))) {
    return -1;
  }
  

  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_SNDBUF,
                (const char*) &val, sizeof(val))<0) {
      CLOSE(mysocket);
      return -1;
  }

  val = bufsize;

  if (setsockopt(mysocket,SOL_SOCKET,SO_RCVBUF,
                 (const char*)&val, sizeof(val))<0) {
    CLOSE(mysocket);
    return -1;
  }

  if (nonblocking) {
    val=1;
    if (IOCTL(mysocket,FIONBIO,&val)) {
      CLOSE(mysocket);
      return -1;
    }
  }

  return mysocket;
}


int SetNoDelaySocket(const int fd, const bool nodelay)
{
  int val = nodelay == true;

  // Set nodelay so that our messages get
  return setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(const char *)&val,sizeof(int));
}


int BindSocket(const int mysocket, const unsigned adx, const int myport)
{  
  struct sockaddr_in my_sa;

  memset(&my_sa,0,sizeof my_sa);
  my_sa.sin_port=htons(myport);
  my_sa.sin_addr.s_addr=htonl(adx);
  my_sa.sin_family=AF_INET;

  if (bind(mysocket,(struct sockaddr *)&my_sa,sizeof my_sa)) {
    return -1;
  }
  return 0;
}

int BindSocket(const int mysocket, const int myport)
{
  return BindSocket(mysocket,(unsigned) INADDR_ANY,myport);
}  

int BindSocket(const int mysocket, const char *host_or_ip, const int myport)
{
  return BindSocket(mysocket, ToIPAddress(host_or_ip), myport);
}

int BindSocket(const int mysocket, const char *pathname)
{
#if defined(WIN32) && !defined(__CYGWIN__)
  return -1;
#else 
  struct sockaddr_un my_sa;
  int len;

  memset(&my_sa,0,sizeof my_sa);
  my_sa.sun_family=AF_UNIX;
  strcpy(my_sa.sun_path,pathname);
  len = strlen(my_sa.sun_path) + sizeof(my_sa.sun_family);

  if (bind(mysocket,(struct sockaddr *)&my_sa,len)) {
    return -1;
  }
  return 0;
#endif
}  


int ListenSocket(const int mysocket, const int maxc)
{
  int maxcon = MIN(maxc,SOMAXCONN);
  return listen(mysocket,maxcon);
}

int ConnectToHost(const int mysocket, const int hostip, const int port)
{
  struct sockaddr_in sa;

  memset(&sa,0,sizeof sa);
  sa.sin_port=htons(port);
  sa.sin_addr.s_addr = htonl(hostip);
  sa.sin_family=AF_INET;

  return connect(mysocket,(struct sockaddr *)&sa,sizeof sa);
}
  

int ConnectToHost(const int mysocket, const char *host, const  int port)
{
  return ConnectToHost(mysocket,ToIPAddress(host),port);
}


int ConnectToPath(const int mysocket, const char *pathname)
{
#if defined(WIN32) && !defined(__CYGWIN__)
  return -1;
#else 
  struct sockaddr_un my_sa;
  int len;

  memset(&my_sa,0,sizeof my_sa);
  my_sa.sun_family=AF_UNIX;
  strcpy(my_sa.sun_path,pathname);
  len = strlen(my_sa.sun_path) + sizeof(my_sa.sun_family);

  if (connect(mysocket,(struct sockaddr *)&my_sa,len)) {
    return -1;
  }
  return 0;
#endif
}
  


int JoinMulticastGroup(const int mysocket, const unsigned adx)
{
  if (!IsValidIPMulticastAddress(adx)) {
    return -1;
  }

  struct ip_mreq req;

  memset(&req,0,sizeof req);
  
  req.imr_multiaddr.s_addr=htonl(adx);
  req.imr_interface.s_addr=htonl(INADDR_ANY);
  
  if (setsockopt(mysocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,
		 (const char*) &req,sizeof req)<0) {
    return -1;
  }
  return 0;
}

int JoinMulticastGroup(const int mysocket, const char *IP)
{
  return JoinMulticastGroup(mysocket,ToIPAddress(IP));
}


int LeaveMulticastGroup(const int mysocket, const unsigned adx)
{
  if (!IsValidIPMulticastAddress(adx)) {
    return -1;
  }

  struct ip_mreq req;

  memset(&req,0,sizeof req);
  
  req.imr_multiaddr.s_addr=htonl(adx);
  req.imr_interface.s_addr=htonl(INADDR_ANY);
  
  if (setsockopt(mysocket,IPPROTO_IP,IP_DROP_MEMBERSHIP,
		 (const char *) &req,sizeof req)<0) {
    return -1;
  }
  return 0;
}


int LeaveMulticastGroup(const int mysocket, const char *IP)
{
  return LeaveMulticastGroup(mysocket,ToIPAddress(IP));
}



int SetMulticastTimeToLive(const int mysocket, const unsigned char ttl)
{
  if (setsockopt(mysocket,IPPROTO_IP,IP_MULTICAST_TTL,
		 (SOCKOPT_TYPE)&ttl,(SOCKOPT_LEN_TYPE)sizeof(ttl))<0) {
    return -1;
  }
  return 0;
}

int SendTo(int const mysocket, 
	   const unsigned ip, const int port, 
	   const char *buf, const int len, const bool sendall)
{
  struct sockaddr_in sa;
  
  memset(&sa,0,sizeof(sockaddr_in));
  sa.sin_family=AF_INET;
  sa.sin_addr.s_addr=htonl(ip);
  sa.sin_port=htons(port);
  
  if (!sendall) {
    return sendto(mysocket,buf,len,0,(struct sockaddr *) &sa,
		  sizeof(sockaddr_in));
  } else {
    int left=len;
    int sent;
    while (left>0) {
      sent=sendto(mysocket,&(buf[len-left]),left,0,(struct sockaddr *)
		  &sa, sizeof(sockaddr_in));
      if (sent<0) {	
	return -1;
      } else {
	left-=sent;
      }
    }
    return len;
  }
}


int ReceiveFrom(const int mysocket, 
		const unsigned ip, const int port, 
		char *buf, const int len,
		const bool recvall)
{
  struct sockaddr_in sa;
  SOCKOPT_LEN_TYPE size=sizeof(sockaddr_in);
  
  memset(&sa,0,sizeof(sockaddr_in));
  sa.sin_family=AF_INET;
  sa.sin_addr.s_addr=htonl(ip);
  sa.sin_port=htons(port);
  
  if (!recvall) {
    return recvfrom(mysocket,buf,len,0,(struct sockaddr *) &sa, &size);
  } else {
    int left=len;	
    int received;
    while (left>0) {
      received=recvfrom(mysocket,&(buf[len-left]),left,0,(struct sockaddr *) &sa, &size);
      if (received<0) {
	return -1;
      } else if (received==0) {
	break;
      } else {	
	left-=received;
      }
    }
    return len-left;
  }
}

int SendTo(const int mysocket, 
	   const char *host_or_ip, const int port, 
	   const char *buf, const int len, const bool sendall)
{
  return SendTo(mysocket,ToIPAddress(host_or_ip),port,buf,len,sendall);
}

int ReceiveFrom(const int mysocket, 
		const char *host_or_ip, const int port, 
		char *buf, const int len, const bool recvall)
{
  return ReceiveFrom(mysocket,ToIPAddress(host_or_ip),port,buf,len,recvall);
}


int Send(const int fd, const char *buf, const int len, const bool sendall)
{
  if (!sendall) {
    return WRITE(fd,buf,len);
  } else {
    int left=len;
    int sent;
    while (left>0) {
      sent=WRITE(fd,&(buf[len-left]),left);
      if (sent<0) {	
	return -1;
      } else if (sent==0) {
	break;
      } else {
	left-=sent;
      }
    }
    return len-left;
  }
}
    

int Receive(const int fd, char *buf, const int len, const bool recvall)
{
  if (!recvall) {
    return READ(fd,buf,len);
  } else {
    int left=len;	
    int received;
    while (left>0) {
      received=READ(fd,&(buf[len-left]),left);
      if (received<0) {
	return -1;
      } else if (received==0) {
	return 0;
      } else {	
	left-=received;
      }
    }
    return len-left;
  }
}


int SetSignalHandler(const int signum, void (*handler)(int), const bool oneshot)
{
#if defined(WIN32) || defined(CYGWIN) // cygwin does not appear to have sigaction, so...
  signal(signum,handler);  //notice that this is oneshot
  return 0;
#else
  struct sigaction sa;

#if defined(__sparc__)
  sa.sa_handler= (void (*)(...)) handler;  // SUN FREAKS
#else
  sa.sa_handler=handler;
#endif  

  sigemptyset(&(sa.sa_mask));
#if defined(linux) 
#define SIGHAND_ONESHOT SA_ONESHOT
#endif
#if defined(__osf__) || defined(__FreeBSD__) || defined(__sparc__)
#define SIGHAND_ONESHOT SA_RESETHAND
#endif

  sa.sa_flags=oneshot == true ? SIGHAND_ONESHOT : 0;
#if defined(linux)
  sa.sa_restorer=0;
#endif

  return sigaction(signum,&sa,0);
#endif
}


int IgnoreSignal(const int signum)
{
  return SetSignalHandler(signum,SIG_IGN);
}

int ListenToSignal(const int signum)
{
  return SetSignalHandler(signum,SIG_DFL);
}

#if defined(WIN32) && !defined(__CYGWIN__)
class SockInit {
public:
  SockInit() { 
    WSADATA foo;
    WSAStartup(MAKEWORD(2,0),&foo);
  }
  ~SockInit() { 
    if (WSAIsBlocking()) {
	    WSACancelBlockingCall();
    }
    WSACleanup();
  }
};

SockInit thesockinit; // constructor should get called on startup.
#endif


