#ifndef _Server
#define _Server

#include "Queues.h"
#include "junk.h"
#include "Selector.h"
#include <assert.h>

template <class CONNECTIONHANDLER> class SimpleTCPServer;

template <class CONNECTIONHANDLER>
class SimpleTCPServerNewConnectionHandler : public Handler {
 private:
  class SimpleTCPServer<CONNECTIONHANDLER> *myserver;
 public:
  SimpleTCPServerNewConnectionHandler(class SimpleTCPServer<CONNECTIONHANDLER> *server) { 
    myserver=server; 
    SetHandlesRead();
    SetHandlesException();
  }
  virtual ~SimpleTCPServerNewConnectionHandler() { 
    myserver=0; 
  }
  virtual int HandleRead(int fd, Selector &s) {
    CONNECTIONHANDLER *h = new CONNECTIONHANDLER;
    h->SetFD(accept(fd,0,0));
    return s.AddHandler(h);
  }
  virtual int HandleWrite(int fd, Selector &s) {
    assert(0);
  }
  virtual int HandleException(int fd, Selector &s) {
    close(fd);
    s.RemoveHandler(this);
    return 0;
  }
  virtual int HandleTimeout(Selector &s) {
    assert(0);
  }
};
	     
template <class CONNECTIONHANDLER>
class SimpleTCPServer  {
 private:
  Selector sel;
 public:
  SimpleTCPServer() {};
  virtual ~SimpleTCPServer() {};
  void Run(int port) {
    SimpleTCPServerNewConnectionHandler<CONNECTIONHANDLER> *h = 
      new SimpleTCPServerNewConnectionHandler<CONNECTIONHANDLER>(this);
    int sock=CreateAndSetupTcpSocket();
    if (BindSocket(sock,port)) {
      perror("SimpleTCPServer::BindSocket");
      return;
    }
    if (ListenSocket(sock)) {
      perror("SimpleTCPServer::ListenSocket");
      return;
    }
    h->SetFD(sock);
    sel.AddHandler(h);
    sel.Run();
  }
};
  

#endif
