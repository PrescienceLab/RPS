#ifndef _client
#define _client

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "socks.h"
#include "Serializable.h"
#include "Mirror.h"
#include "EndPoint.h"

#include "junk.h"

enum ReferenceConnectionType {
CONNECT_UNCONNECTED, 
CONNECT_FD, 
CONNECT_STDIO, 
CONNECT_FILE,
CONNECT_REMOTE, 
CONNECT_REMOTE_MULTICAST, 
CONNECT_LOCAL};



template <class SERREQ, class SERRESP> 
class Reference {
 private:
  ReferenceConnectionType ctype;
  int fd;
  unsigned adx;
  unsigned port;
  bool     udp;
  FILE    *file;
  LocalSource *localsource;
  char    *pathname;
 public:
  Reference() { ctype=CONNECT_UNCONNECTED; fd=0; adx=0; port=0; udp=false; pathname=0;}
  virtual ~Reference() { Disconnect(); CHK_DEL_MAT(pathname);}
  
  virtual int ConnectTo(int fd) {
    Disconnect();
    ctype=CONNECT_FD;
    this->fd=fd;
    return 0;
  }

  virtual int ConnectTo(FILE *in) {
    Disconnect();
    ctype=CONNECT_STDIO;
    this->file=in;
    return 0;
  }

  virtual int ConnectTo(LocalSource *ls) {
    Disconnect();
    ctype=CONNECT_LOCAL;
    localsource=ls;
    return 0;
  }
  
  virtual int ConnectTo(EndPoint &ep) {
    switch (ep.atype) {
    case EndPoint::EP_SOURCE:
    case EndPoint::EP_SERVER:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return ConnectTo(ep.adx,ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return ConnectTo(ep.adx,ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return ConnectTo(ep.pathname,true);
	break;
      case EndPoint::COMM_FILE:
	return ConnectTo(ep.pathname,false);
	break; 
      case EndPoint::COMM_STDIO:
	return ConnectTo(ep.file);
	break;
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
      break;
    case EndPoint::EP_TARGET:
    case EndPoint::EP_CONNECT:
    case EndPoint::EP_UNKNOWN:
      return -1;
      break;
    }
    return -1;
  }
    

  virtual int ConnectTo(char *hostorip, char *port, bool udp=false) {
    return ConnectTo(ToIPAddress(hostorip),atoi(port),udp);
  }
  virtual int ConnectTo(char *hostorip, unsigned port, bool udp=false) {
    return ConnectTo(ToIPAddress(hostorip),port,udp);
  }
  virtual int ConnectTo(unsigned adx, char *port, bool udp=false) {
    return ConnectTo(adx,atoi(port),udp);
  }
  virtual int ConnectTo(unsigned adx, unsigned port,bool udp=false) {
    Disconnect();
    if (IsValidIPMulticastAddress(adx)) {
      assert(udp==true);
      if ((fd=CreateAndSetupUdpSocket())<0) {
	perror("Client::ConnectTo can't create socket");
	Disconnect();
	return -1;
      }
      if (BindSocket(fd,(unsigned int)INADDR_ANY,port)) {
	perror("Client::ConnectTo can't bind");
	Disconnect();
	return -1;
      }
    
      if (JoinMulticastGroup(fd,adx)) {
	perror("Client::ConnectTo can't join group");
	Disconnect();
	return -1;
      }
      ctype=CONNECT_REMOTE_MULTICAST;
    } else {
      if (!udp) {
	if ((fd=CreateAndSetupTcpSocket())<0) {
	  perror("Client::ConnectTo can't create socket");
	  Disconnect();
	  return -1;
	}
      } else {
	if ((fd=CreateAndSetupUdpSocket())<0) {
	  perror("Client::ConnectTo can't create socket");
	  Disconnect();
	  return -1;
	}
      }
      if (ConnectToHost(fd,adx,port)) {
	perror("Client::ConnectTo can't connect to server");
	Disconnect();
	return -1;
      }
      ctype=CONNECT_REMOTE;
    }
    this->adx=adx;
    this->port=port;
    this->udp=udp;
    return 0;
  }

  virtual int ConnectTo(char *pathname, bool isunixdom=true) {
    Disconnect();
    if (isunixdom) {
      if ((fd = CreateAndSetupUnixDomainSocket())<0) {
	perror("Client::ConnectTo can't create socket");
	Disconnect();
        return -1;
      }
      if (ConnectToPath(fd,pathname)) {
	perror("Client::ConnectTo can't connect to path");
	Disconnect();
	return -1;
      }
      ctype=CONNECT_REMOTE;
    } else {
      if ((fd=open(pathname,O_RDWR|O_CREAT|O_TRUNC,0644))<0) {
	perror("Client::ConnectTo can't open file");
	Disconnect();
	return -1;
      }
      ctype=CONNECT_FILE;
      CHK_DEL_MAT(this->pathname);
      this->pathname = new char [strlen(pathname)+1];
      strcpy(this->pathname,pathname);
      return 0;
    }
  }


  virtual int Disconnect() { 
    switch (ctype) { 
    case CONNECT_UNCONNECTED:
    case CONNECT_FD:
    case CONNECT_STDIO:
      ctype=CONNECT_UNCONNECTED;
      return 0;
      break;
    case CONNECT_LOCAL:
      localsource=0;
      ctype=CONNECT_UNCONNECTED;
      return 0;
      break;
    case CONNECT_FILE:
    case CONNECT_REMOTE:
      close(fd);
      ctype=CONNECT_UNCONNECTED;
      return 0;
      break;
    case CONNECT_REMOTE_MULTICAST:
      LeaveMulticastGroup(fd,adx);
      close(fd);
      ctype=CONNECT_UNCONNECTED;
      return 0;
      break;
    }
    return -1;
  }

  virtual int SendRequest(SERREQ &r) {
    switch (ctype) { 
    case CONNECT_FD:
    case CONNECT_REMOTE:
    case CONNECT_REMOTE_MULTICAST:
    case CONNECT_FILE:
      return r.Serialize(fd);
      break;
    case CONNECT_STDIO:
      return r.Serialize(fileno(file));
      break;
    case CONNECT_LOCAL: 
      return 0;
      break;
    case CONNECT_UNCONNECTED:
      return -1;
      break;
    }
    return -1;
  }
    
  virtual int RecvResponse(SERRESP &s) {
    assert(ctype!=CONNECT_UNCONNECTED);
    switch (ctype) { 
    case CONNECT_FD:
    case CONNECT_REMOTE:
    case CONNECT_REMOTE_MULTICAST:
    case CONNECT_FILE:
      return s.Unserialize(fd);
      break;
    case CONNECT_STDIO:
      return s.Unserialize(fileno(file));
      break;
      break;
    case CONNECT_LOCAL: {
      Buffer buf;
      int rc;
      localsource->GetData(buf);
      rc = s.Unserialize(buf);
      return rc;
    }
    break;
    case CONNECT_UNCONNECTED: 
      return -1;
      break;
    }
    return -1;
  }

  virtual int Call(SERREQ &req, SERRESP &resp) {
    int rc;
    rc = SendRequest(req);
    if (!rc) {
      return RecvResponse(resp);
    } else {
      return rc;
    }
  }
};
      

class Empty : public SerializeableInfo {
 public:
  virtual int GetPackedSize() const { return 0;} 
  virtual int GetMaxPackedSize() const { return 0;}
  virtual int Pack(Buffer &buf) const { return 0;}
  virtual int Unpack(Buffer &buf) { return 0;}
  virtual int Serialize(Buffer &buf) const {
    return 0;
  }
  virtual int Unserialize(Buffer &buf) {
    return 0;
  }
  virtual int Serialize(const int fd) const {
    return 0;
  }
  virtual int Unserialize(const int fd) {
    return 0;
  }
};

template <class SERINFO> 
class StreamingInputReference : public Reference<Empty, SERINFO> {
 public:
  virtual int GetNextItem(SERINFO &s) { return RecvResponse(s);}
};

template <class SERINFO> 
class StreamingOutputReference : public Reference<SERINFO,Empty> {
 public:
  virtual int PutNextItem(SERINFO &s) { return SendRequest(s);}
};

#endif
