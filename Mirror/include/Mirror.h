#ifndef _Mirror
#define _Mirror

#include "Selector.h"
#include "Queues.h"
#include "Serializable.h"
#include "socks.h"
#include "EndPoint.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#ifdef WIN32
#include <io.h>
#endif

#include "junk.h"

#define QUEUED_WRITES

class LocalSource {
 public:
  virtual ~LocalSource() {}
  virtual void GetData(Buffer &buf)=0;
  virtual void GetInterval(TimeValue &tv)=0;
};

class LocalTarget {
 public:
  virtual ~LocalTarget() {}
  virtual void ProcessData(Buffer &buf)=0;
};


class AbstractMirror {
 public:
  virtual ~AbstractMirror() {} ;

  virtual int AddEndPoint(EndPoint &ep)=0;
  virtual int DeleteEndPoint(EndPoint &ep)=0;

  virtual int AddFDSource(int fd)=0;
  virtual int DeleteFDSource(int fd)=0;

  virtual int AddLocalSource(LocalSource *ls)=0;
  virtual int DeleteLocalSource(LocalSource *ls)=0;

  virtual int AddRemoteSource(char *iporhost, int port, bool udp=false)=0;
  virtual int AddRemoteSource(unsigned adx, int port, bool udp=false)=0;
  virtual int AddRemoteSource(char *pathname, bool isunixdom=true)=0;
  virtual int DeleteRemoteSource(char *iporhost, int port, bool udp=false)=0;
  virtual int DeleteRemoteSource(unsigned adx, int port, bool udp=false)=0;
  virtual int DeleteRemoteSource(char *pathname, bool isunixdom=true)=0;

  virtual int AddFDConnect(int fd)=0;
  virtual int DeleteFDConnect(int fd)=0;

  virtual int AddLocalConnect(int port, bool udp=false)=0;
  virtual int AddLocalConnect(char *pathname)=0;
  virtual int DeleteLocalConnect(int port, bool udp=false)=0;
  virtual int DeleteLocalConnect(char *pathname)=0;

  virtual int AddFDRequestResponseConnect(int fd)=0;
  virtual int DeleteFDRequestResponseConnect(int fd)=0;

  virtual int AddLocalRequestResponseConnect(int port, bool udp=false)=0;
  virtual int AddLocalRequestResponseConnect(char *pathname)=0;
  virtual int DeleteLocalRequestResponseConnect(int port, bool udp=false)=0;
  virtual int DeleteLocalRequestResponseConnect(char *pathname)=0;

  virtual int AddFDRequestResponse(int fd)=0;
  virtual int DeleteFDRequestResponse(int fd)=0;              

  virtual int AddFDTarget(int fd)=0;
  virtual int DeleteFDTarget(int fd)=0;              

  virtual int AddLocalTarget(LocalTarget *lt)=0;
  virtual int DeleteLocalTarget(LocalTarget *lt)=0;

  virtual int AddRemoteTarget(char *ip, int port, bool udp=false, int ttl=1)=0;
  virtual int AddRemoteTarget(unsigned, int port, bool udp=false, int ttl=1)=0;
  virtual int AddRemoteTarget(char *pathname, bool isunixdom=true)=0;
  virtual int DeleteRemoteTarget(char *iporhost, int port, bool udp=false)=0;  
  virtual int DeleteRemoteTarget(unsigned adx, int port, bool udp=false)=0;  
  virtual int DeleteRemoteTarget(char *pathname, bool isunixdom=true)=0;

  virtual int DeleteMatching(int fd)=0;

//  virtual int PushStreamInput(void *data, const int len)=0;

  virtual int Forward(char *buf, const int len)=0;
  virtual int Forward(Buffer &buf)=0;

#ifdef QUEUED_WRITES
  virtual int QueueWrite(int fd, Buffer &buf)=0;
#endif

  virtual int Run()=0;
};  




// These comparisons return nonzero if true
struct ComparableMirrorData {
  enum ComparableMirrorDataType {FDS,FDC,FDT,FDRR,FDRRC,LS,LC,LT,LRRC,RS,RT} datatype;
  AbstractMirror *mirror;
  ComparableMirrorData(ComparableMirrorDataType t, 
		       AbstractMirror *mir) : datatype(t), mirror(mir) {}
  virtual int Compare(ComparableMirrorData &other)=0;
  virtual int Forward(Buffer &buf, bool sendall=true)=0;
};

class ComparableMirrorDataCompare {
 public:
  static int Compare(ComparableMirrorData *left, ComparableMirrorData *right) {
    return !(left->Compare(*right));
  }
  static int Compare(ComparableMirrorData &left, ComparableMirrorData &right) {
    return !(left.Compare(right));
  }
};

typedef SearchableQueue<ComparableMirrorData,ComparableMirrorDataCompare> ComparableMirrorDataQueue;

#ifdef QUEUED_WRITES
#define SEND_DATA(mir,fd,buf,sa) ((mir)->QueueWrite(fd,buf))  
#else
#define SEND_DATA(mir,fd,buf,sa) (::Send(fd,(buf).Data(),(buf).Size(),sa))  
#endif

#define FD_BASED(name)						\
struct name : public ComparableMirrorData {			\
  int fd;							\
  name(int thefd, AbstractMirror *mir) :			\
     ComparableMirrorData(ComparableMirrorData::##name,mir),	\
			  fd(thefd) {}				\
  virtual int Compare(ComparableMirrorData &other) {		\
    if (datatype==other.datatype) {				\
      return this->Compare((struct name &)other);		\
    } else {							\
      return 0;							\
    }								\
  }								\
  virtual int Compare(struct name &other) {			\
    return fd==other.fd;					\
  }								\
  virtual int Forward(Buffer &buf, bool sendall=true) {		\
    return SEND_DATA(mirror,fd,buf,sendall);			\
  }								\
};

//XXX
#define PTR_BASED(name,ptrtype,ptrname,fwd)					\
struct name : public ComparableMirrorData {					\
  ptrtype *ptrname;								\
  Handler *handler;								\
  name(ptrtype *theptrname, Handler *h,AbstractMirror *mir)  :			\
    ComparableMirrorData(ComparableMirrorData::##name,mir),			\
    ptrname(theptrname), handler(h) {}						\
  virtual int Compare(ComparableMirrorData &other) {				\
    if (datatype==other.datatype) {						\
        return this->Compare((struct name &)other);				\
    } else {									\
        return 0;								\
    }										\
  }										\
  virtual int Compare(struct name &other) {					\
    return ptrname==other.ptrname;						\
  }										\
  virtual int Forward(Buffer &buf, bool sendall=true) {				\
    ((LocalTarget*)ptrname)->ProcessData(buf); sendall=true; return buf.Size();	\
  }										\
};

#define ADX_BASED(name)							     \
struct name : public ComparableMirrorData {				     \
  enum AddressType {TCP, UDP, AFILE, UNIXDOM};				     \
  AddressType addresstype;						     \
  unsigned adx;								     \
  int port;								     \
  char *pathname;							     \
  int fd;								     \
  virtual ~name() {							     \
    CHK_DEL_MAT(pathname);						     \
  }									     \
  name(char *path, bool isunixdom,AbstractMirror *mir) :		     \
	  ComparableMirrorData(ComparableMirrorData::##name,mir),	     \
          adx(0), port(0), fd(-1) {					     \
          if (isunixdom) {addresstype=UNIXDOM;} else {addresstype=AFILE;}    \
	  pathname=new char [strlen(path)+1];				     \
  }									     \
  name(unsigned theadx, int theport, bool isudp,AbstractMirror *mir):	     \
	  ComparableMirrorData(ComparableMirrorData::##name,mir),	     \
          adx(theadx), port(theport), fd(-1),				     \
          pathname(0) {if (isudp) {addresstype=UDP;} else {addresstype=TCP;} \
  }									     \
  virtual int Compare(ComparableMirrorData &other) {			     \
    if (datatype==other.datatype) {					     \
        return this->Compare((struct name &)other);			     \
    } else {								     \
        return 0;							     \
    }									     \
  }									     \
  virtual int Compare(struct name &other) {				     \
    switch (addresstype) {						     \
    case TCP:								     \
    case UDP:								     \
      return adx==other.adx && port==other.port				     \
          && addresstype==other.addresstype;				     \
      break;								     \
    case UNIXDOM:							     \
    case AFILE:								     \
      return addresstype==other.addresstype				     \
	  && !strcmp(pathname,other.pathname);				     \
      break;								     \
    }									     \
  }									     \
  virtual int Forward(Buffer &buf, bool sendall=true) {			     \
    return SEND_DATA(mirror,fd,buf,sendall);				     \
  }                                                                          \
};									     




FD_BASED(FDS)
FD_BASED(FDC)
FD_BASED(FDT)
FD_BASED(FDRRC)
FD_BASED(FDRR)

PTR_BASED(LS,LocalSource,ls,0)
PTR_BASED(LT,LocalTarget,lt,1)


ADX_BASED(RS)
ADX_BASED(RT)
ADX_BASED(LC)
ADX_BASED(LRRC)

// Simple mirror.  Accept connections on one fd,
// data on another, and then forward the data to all
// open connections
// send them out to a multicast address and
// to a set of connections
// accept new connections




class MirrorHandler : public Handler {
 protected:
  AbstractMirror *mymirror;
 public:
  MirrorHandler(AbstractMirror *mirror) { mymirror=mirror;}
  virtual ~MirrorHandler() { mymirror=0;}
  virtual Handler *Clone()=0;
};


class NullHandler : public MirrorHandler {
public:
  NullHandler(AbstractMirror *m) :  MirrorHandler(m) {
     fprintf(stderr, "Warning: Instantiating NullHandler\n");
  }
  Handler *Clone() { 
    return new NullHandler(*this);
  }
  int HandleRead(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};

template <class SERIN, class SEROUT>
class NullCompute {
 public:
  static int Compute(const SERIN &in, SEROUT &out) { out=in; return 0;}
};


class GenericMirrorInputHandler : public MirrorHandler {
 public:
  GenericMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() { 
    return new GenericMirrorInputHandler(*this);
  }
  int HandleRead(int fd, Selector &s) {
    char buf[65536];
    int len=Receive(fd,buf,65536,false);
    if (len<1) {
      mymirror->DeleteFDSource(fd);
      return -1;
    } else {
      mymirror->Forward(buf,len);
      return 0;
    }
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};

class GenericMirrorNewConnectionHandler : public MirrorHandler {
public:
  GenericMirrorNewConnectionHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() { 
    return new GenericMirrorNewConnectionHandler(*this);
  }
  
  // Accept new connection
  int HandleRead(int fd, Selector &s) {
    int acc = accept(fd,0,0);
    if (acc>=0) {
      SetNoDelaySocket(acc);
      mymirror->AddFDTarget(acc);
      return 0;
    } else {
      return -1;
    }
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};



class GenericMirrorNewRequestResponseConnectionHandler : public MirrorHandler {
public:
  GenericMirrorNewRequestResponseConnectionHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() { 
    return new GenericMirrorNewRequestResponseConnectionHandler(*this);
  }
  
  // Accept new connection
  int HandleRead(int fd, Selector &s) {
    int acc = accept(fd,0,0);
    if (acc>=0) { 
      SetNoDelaySocket(acc);
      mymirror->AddFDRequestResponse(acc);
      return 0;
    } else {
      return -1;
    }
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};



template <class SERIALIZEABLEINFO>
class SerializeableMirrorInputHandler : public MirrorHandler {
public:
  SerializeableMirrorInputHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() { 
    return new SerializeableMirrorInputHandler(*this);
  }
  
  int HandleRead(int fd, Selector &s) {
    SERIALIZEABLEINFO t;
    Buffer buf;
    if (t.Unserialize(fd)) { 
      mymirror->DeleteFDSource(fd);
      return -1;
    } else {
      t.Serialize(buf);
      mymirror->Forward(buf);
      return 0;
    }
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};


template <class SERIN, class COMPUTE, class SEROUT>
class SerializeableMirrorRequestResponseHandler : public MirrorHandler {
public:
  SerializeableMirrorRequestResponseHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() { 
    return new SerializeableMirrorRequestResponseHandler(*this);
  }
  
  int HandleRead(int fd, Selector &s) {
    SERIN t;
    Buffer buf;
    if (t.Unserialize(fd)) { 
      mymirror->DeleteFDRequestResponse(fd);
      return -1;
    } else {
      SEROUT out;
      int rc = COMPUTE::Compute(t,out);
      out.Serialize(fd);
      return 0;
    }
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};


class LocalSourceHandler : public MirrorHandler {
 private:
  LocalSource *source;
 public:
  LocalSourceHandler(AbstractMirror *mir, LocalSource *ls) :
    MirrorHandler(mir) {
    source=ls;
    TimeValue tv;
    ls->GetInterval(tv);
    SetWait(tv);
    SetHandlesTimeout();
  }
  Handler *Clone() { 
    return new LocalSourceHandler(*this);
  }

  int HandleRead(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    Buffer buf;
    source->GetData(buf);
    mymirror->Forward(buf);
    return -1;
  }
  LocalSource *GetSource() { return source;}
};


class QueuedBuffer : public Buffer {
 public:
  int    left;
  QueuedBuffer(const Buffer &thebuf) : Buffer(thebuf) { left=Size(); }
  virtual ~QueuedBuffer() { left=0; }
};
    

#define QUEUED_WRITE_HANDLER_TAG 0xbeefdead

class QueuedWriteHandler : public MirrorHandler {
 private:
  Queue<QueuedBuffer> bufs;
  int  curoff;
 public:
  QueuedWriteHandler(AbstractMirror *mir, int fd) :
    MirrorHandler(mir) {
    curoff=0;
    tag = QUEUED_WRITE_HANDLER_TAG; 
  }
  virtual ~QueuedWriteHandler() { 
    bufs.Clear();
  }
  Handler *Clone() { 
    return new QueuedWriteHandler(*this);
  }
  int QueueWrite(const Buffer &thebuf) {
    QueuedBuffer *buf = new QueuedBuffer(thebuf);
    if (buf==0) { 
      return 0;
    }
    bufs.AddAtBack(buf);
    SetHandlesWrite();
    return buf->Size();
  }
  int HandleRead(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(int fd, Selector &s) {
    QueuedBuffer *buf;
    int bytessent;
    while (buf=bufs.RemoveFromFront()) {
      bytessent = 
	::Send(fd,&((buf->Data())[buf->Size()-buf->left]),buf->left,false);
      if (bytessent<0) { 
	// OK, we can't write any more to this fd, so we'll just
	// kill ourselves off
	perror("QueuedWrite");
	delete buf;
	ClearHandlesWrite();
	s.RemoveHandler(this);
	mymirror->DeleteFDTarget(fd);
	return -1;
      }
      buf->left -= bytessent;
      if (buf->left>0) { 
	// OK, we cant finish the current buffer - requeue it and leave
	bufs.AddAtFront(buf);
	//Leave HandlesWrite enabled, obviously
        return 0;
      }
      // OK, if we got here, we have finished the buffer and go to the next
      delete buf;
    }
    // if we got here, we're out of bufs, so disable handles write
    ClearHandlesWrite();
    return 0;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};



template <class SOURCEHANDLER, 
          class SOURCECONNECTHANDLER, 
          class REQUESTRESPONSEHANDLER, 
          class REQUESTRESPONSECONNECTHANDLER>
class Mirror : public AbstractMirror {
 protected:
  bool forwarding;
  bool sharedselector;
  Selector *sel;
  ComparableMirrorDataQueue sources, connects, targets, rrconnects, rrsources;
  ComparableMirrorDataQueue adds, deletes;
 protected:
  int Add(ComparableMirrorData *x) {
    switch (x->datatype) {
    case ComparableMirrorData::FDS:
      return AddFDSource(((FDS*)x)->fd);
      break;
    case ComparableMirrorData::FDC:
      return AddFDConnect(((FDC*)x)->fd);
      break;
    case ComparableMirrorData::FDT:
      return AddFDTarget(((FDT*)x)->fd);
      break;
    case ComparableMirrorData::FDRRC:
      return AddFDRequestResponseConnect(((FDRRC*)x)->fd);
      break;
    case ComparableMirrorData::FDRR:
      return AddFDRequestResponse(((FDRR*)x)->fd);
      break;
    case ComparableMirrorData::LS:
      return AddLocalSource(((LS*)x)->ls);
      break;
    case ComparableMirrorData::LC: {
      LC *y = (LC*)x;
      switch (y->addresstype) {
      case LC::AFILE:
        return -1;
        break;
      case LC::UNIXDOM:
        return AddLocalConnect(y->pathname);
        break;
      case LC::TCP:
        return AddLocalConnect(y->port, false);
        break;
      case LC::UDP:
        return AddLocalConnect(y->port, true);
        break;
      }
           }
      break;
    case ComparableMirrorData::LRRC: {
      LRRC *y = (LRRC*)x;
      switch (y->addresstype) {
      case LRRC::AFILE:
        return -1;
        break;
      case LRRC::UNIXDOM:
        return AddLocalRequestResponseConnect(y->pathname);
        break;
      case LRRC::TCP:
        return AddLocalRequestResponseConnect(y->port, false);
        break;
      case LRRC::UDP:
        return AddLocalRequestResponseConnect(y->port, true);
        break;
      }
      }
      break;
    case ComparableMirrorData::LT:
      return AddLocalTarget(((LT*)x)->lt);
      break;
    case ComparableMirrorData::RS: {
      RS *y = (RS*)x;
      switch (y->addresstype) {
      case RS::AFILE:
        return AddRemoteSource(y->pathname, false);
        break;
      case RS::UNIXDOM:
        return AddRemoteSource(y->pathname, true);
        break;
      case RS::TCP:
        return AddRemoteSource(y->adx, y->port, false);
        break;
      case RS::UDP:
        return AddRemoteSource(y->adx, y->port, true);
        break;
      }
      }
      break;
    case ComparableMirrorData::RT: {
      RT *y = (RT*)x;
      switch (y->addresstype) {
      case RT::AFILE:
        return AddRemoteTarget(y->pathname, false);
        break;
      case RT::UNIXDOM:
        return AddRemoteTarget(y->pathname, true);
        break;
      case RT::TCP:
        return AddRemoteTarget(y->adx, y->port, false);
        break;
      case RT::UDP:
        return AddRemoteTarget(y->adx, y->port, true);
        break;
      }
      }
      break;
    }
    return 0;
  }
  
  int Delete(ComparableMirrorData *x) {
    switch (x->datatype) {
    case ComparableMirrorData::FDS:
      return DeleteFDSource(((FDS*)x)->fd);
      break;
    case ComparableMirrorData::FDC:
      return DeleteFDConnect(((FDC*)x)->fd);
      break;
    case ComparableMirrorData::FDT:
      return DeleteFDTarget(((FDT*)x)->fd);
      break;
    case ComparableMirrorData::FDRRC:
      return DeleteFDRequestResponseConnect(((FDRRC*)x)->fd);
      break;
    case ComparableMirrorData::FDRR:
      return DeleteFDRequestResponse(((FDRR*)x)->fd);
      break;
    case ComparableMirrorData::LS:
      return DeleteLocalSource(((LS*)x)->ls);
      break;
    case ComparableMirrorData::LC: {
      LC *y = (LC*)x;
      switch (y->addresstype) {
      case LC::AFILE:
        return -1;
        break;
      case LC::UNIXDOM:
        return DeleteLocalConnect(y->pathname);
        break;
      case LC::TCP:
        return DeleteLocalConnect(y->port, false);
        break;
      case LC::UDP:
        return DeleteLocalConnect(y->port, true);
        break;
      } 
         }
      break;
    case ComparableMirrorData::LRRC: {
      LRRC *y = (LRRC*)x;
      switch (y->addresstype) {
      case LRRC::AFILE:
	return -1;
        break;
      case LRRC::UNIXDOM:
        return DeleteLocalRequestResponseConnect(y->pathname);
        break;
      case LRRC::TCP:
        return DeleteLocalRequestResponseConnect(y->port, false);
        break;
      case LRRC::UDP:
        return DeleteLocalRequestResponseConnect(y->port, true);
        break;
      }
           }
      break;
    case ComparableMirrorData::LT:
      return DeleteLocalTarget(((LT*)x)->lt);
      break;
    case ComparableMirrorData::RS: {
      RS *y = (RS*)x;
      switch (y->addresstype) {
      case RS::AFILE:
        return DeleteRemoteSource(y->pathname, false);
        break;
      case RS::UNIXDOM:
        return DeleteRemoteSource(y->pathname, true);
        break;
      case RS::TCP:
        return DeleteRemoteSource(y->adx, y->port, false);
        break;
      case RS::UDP:
        return DeleteRemoteSource(y->adx, y->port, true);
        break;
      }
      }
      break;
    case ComparableMirrorData::RT: {
      RT *y = (RT*)x;
      switch (y->addresstype) {
      case RT::AFILE:
        return DeleteRemoteTarget(y->pathname, false);
        break;
      case RT::UNIXDOM:
        return DeleteRemoteTarget(y->pathname, true);
        break;
      case RT::TCP:
        return DeleteRemoteTarget(y->adx, y->port, false);
        break;
      case RT::UDP:
        return DeleteRemoteTarget(y->adx, y->port, true);
        break;
      }
      }
      break;
    }
    return 0;
  }
  
  void Fixup() {
    assert(!forwarding);
    ComparableMirrorData *x;
    while ((x=deletes.RemoveFromFront())) {
      Delete(x);
      delete x;
    }
    while ((x=adds.RemoveFromFront())) {
      Add(x);
      delete x;
    }
  }
  
  
 public:
  

  Mirror(Selector *sharedsel=0) {
    forwarding=false;
    if (sharedsel) { 
       sel=sharedsel;
       sharedselector=true;
    } else {
       sel = new Selector;
       sharedselector=false;
    }
  }
 
  
  virtual ~Mirror() {
    // Now the only items are on the actual queuss
    ComparableMirrorData *x;
    forwarding=true;
    FOREACH_BEGIN(x,sources) {
      Delete(x);
    } FOREACH_END(sources)
    FOREACH_BEGIN(x,connects) {
      Delete(x);
    } FOREACH_END(connects)
    FOREACH_BEGIN(x,targets) {
      Delete(x);
    } FOREACH_END(targets)
    FOREACH_BEGIN(x,rrconnects) {
      Delete(x);
    } FOREACH_END(rrconnects)
    FOREACH_BEGIN(x,rrsources) {
      Delete(x);
    } FOREACH_END(rrsources)
    forwarding=false;
    Fixup();
    if (!sharedselector && sel) { 
      delete sel;
    }
  }

  int AddEndPoint(EndPoint &ep) {
    switch (ep.atype) {
    case EndPoint::EP_SOURCE:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return AddRemoteSource(ep.adx,ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return AddRemoteSource(ep.adx,ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return AddRemoteSource(ep.pathname,true);
	break;
      case EndPoint::COMM_FILE:
	return AddRemoteSource(ep.pathname,false);
	break;
      case EndPoint::COMM_STDIO:
	return AddFDSource(fileno(ep.file));
	break;
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
      break;
    case EndPoint::EP_TARGET:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return AddRemoteTarget(ep.adx,ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return AddRemoteTarget(ep.adx,ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return AddRemoteTarget(ep.pathname,true);
	break;
      case EndPoint::COMM_FILE:
	return AddRemoteTarget(ep.pathname,false);
	break;
      case EndPoint::COMM_STDIO:
	return AddFDTarget(fileno(ep.file));
	break;
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      break;
      }
    case EndPoint::EP_CONNECT:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return AddLocalConnect(ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return AddLocalConnect(ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return AddLocalConnect(ep.pathname);
	break;
      case EndPoint::COMM_FILE:
      case EndPoint::COMM_STDIO:
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
    case EndPoint::EP_SERVER:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return AddLocalRequestResponseConnect(ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return AddLocalRequestResponseConnect(ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return AddLocalRequestResponseConnect(ep.pathname);
	break;
      case EndPoint::COMM_FILE:
      case EndPoint::COMM_STDIO:
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
      break;
    case EndPoint::EP_UNKNOWN:
      return -1;
      break;
    }
    return -1;
  }
  
  int DeleteEndPoint(EndPoint &ep) {
    switch (ep.atype) {
    case EndPoint::EP_SOURCE:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return DeleteRemoteSource(ep.adx,ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return DeleteRemoteSource(ep.adx,ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return DeleteRemoteSource(ep.pathname,true);
	break;
      case EndPoint::COMM_FILE:
	return DeleteRemoteSource(ep.pathname,false);
	break;
      case EndPoint::COMM_STDIO:
	return DeleteFDSource(fileno(ep.file));
	break;
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
      break;
    case EndPoint::EP_TARGET:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return DeleteRemoteTarget(ep.adx,ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return DeleteRemoteTarget(ep.adx,ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return DeleteRemoteTarget(ep.pathname,true);
	break;
      case EndPoint::COMM_FILE:
	return DeleteRemoteTarget(ep.pathname,false);
	break;
      case EndPoint::COMM_STDIO:
	return DeleteFDTarget(fileno(ep.file));
	break;
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      break;
      }
    case EndPoint::EP_CONNECT:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return DeleteLocalConnect(ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return DeleteLocalConnect(ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return DeleteLocalConnect(ep.pathname);
	break;
      case EndPoint::COMM_FILE:
      case EndPoint::COMM_STDIO:
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
    case EndPoint::EP_SERVER:
      switch (ep.ctype) {
      case EndPoint::COMM_TCP:
	return DeleteLocalRequestResponseConnect(ep.port,false);
	break;
      case EndPoint::COMM_UDP:
	return DeleteLocalRequestResponseConnect(ep.port,true);
	break;
      case EndPoint::COMM_UNIXDOM:
	return DeleteLocalRequestResponseConnect(ep.pathname);
	break;
      case EndPoint::COMM_FILE:
      case EndPoint::COMM_STDIO:
      case EndPoint::COMM_UNKNOWN:
	return -1;
	break;
      }
      break;
    case EndPoint::EP_UNKNOWN:
      return -1;
      break;
    }
    return -1;
  }

  int AddFDSource(int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDS(fd,this));
      return 0;
    } else {
      Handler *h = new SOURCEHANDLER(this);
      h->SetFD(fd);
      sources.AddAtBack(new FDS(fd,this));
      sel->AddHandler(h);
    }
    return 0;
  }
  
  int DeleteFDSource(int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDS(fd,this));
      return 0;
    } else {
      FDS thingtofind(fd,this);
      FDS *temp=(FDS*)sources.FindRemove(&thingtofind);
      if (temp) {
	Handler *h=sel->RemoveHandler(fd);
	if (h) { delete h;}
	close(fd);
	delete temp;
      }
      return 0;
    }
  }	
  
  int AddLocalSource(LocalSource *source) {
    if (forwarding) {
      adds.AddAtBack(new LS(source,0,this));
      return 0;
    } else {
      LS *ls = new LS(source,0,this);
      ls->handler = new LocalSourceHandler(this,source);
      sources.AddAtBack(ls);
      sel->AddHandler(ls->handler);
      return 0;
    }
  }
  
  int DeleteLocalSource(LocalSource *source) {
    if (forwarding) {
      deletes.AddAtBack(new LS(source,0,this));
      return 0;
    } else {
      LS thingtofind(source,0,this);
      LS *temp = (LS*)sources.FindRemove(&thingtofind);
      if (temp) {
	Handler *h=sel->RemoveHandler(temp->handler);
	if (h) { delete h;}
	delete temp;
      }
      return 0;
    }
  }
  
  int AddRemoteSource(char *iporhost, int port, bool udp=false) {
    return AddRemoteSource(ToIPAddress(iporhost),port,udp);
  }
  
  int AddRemoteSource(unsigned adx, int port, bool udp=false) {
    if (forwarding) {
      adds.AddAtBack(new RS(adx,port,udp,this));
      return 0;
    } else {
      int fd;
      if (IsValidIPMulticastAddress(adx)) {
	assert(udp==true);
	if ((fd=CreateAndSetupUdpSocket())<0) {
	  perror("Mirror:AddRemoteSource can't create socket");
	  return -1;
	}
	if (BindSocket(fd,(unsigned int)INADDR_ANY,port)) {
	  perror("Mirror:AddRemoteSource can't bind");
	  close(fd);
	  return -1;
	}
	if (JoinMulticastGroup(fd,adx)) {
	  perror("Mirror:AddRemoteSource can't join group");
	  close(fd);
	  return -1;
	}
      } else {
	if (!udp) {
	  if ((fd=CreateAndSetupTcpSocket())<0) {
	    perror("Mirror:AddRemoteSourc can't create socket");
	    return -1;
	  }
	} else {
	  if ((fd=CreateAndSetupUdpSocket())<0) {
	    perror("Mirror:AddRemoteSourc can't create socket");
	    return -1;
	  }
	}
	if (ConnectToHost(fd,adx,port)) {
	  perror("Mirror:AddRemoteSourc can't connect to server");
	  close(fd);
	  return -1;
	}
      }
      RS *rs = new RS(adx,port,udp,this);
      rs->fd=fd;
      sources.AddAtBack(rs);
      Handler *h = new SOURCEHANDLER(this);
      h->SetFD(fd);
      sel->AddHandler(h);
      return 0;
    }
  }
  
  int AddRemoteSource(char *pathname, bool isunixdom=true) {
    if (forwarding) {
      adds.AddAtBack(new RS(pathname,isunixdom,this));
      return 0;
    } else {
      int fd;
      if (isunixdom) {
         if ((fd=CreateAndSetupUnixDomainSocket())<0) {
            perror("Mirror:AddRemoteSource can't create socket");
            return -1;
         } 
         if (ConnectToPath(fd,pathname)) {
	    perror("Mirror:AddRemoteSourc can't connect to server");
	    close(fd);
	    return -1;
	}
      } else {
         if ((fd=open(pathname,O_RDONLY,0))) {
            perror("Mirror:AddRemoteSource can't open file");
            return -1;
         }
      }
      RS *rs = new RS(pathname,isunixdom,this);
      rs->fd=fd;
      sources.AddAtBack(rs);
      Handler *h = new SOURCEHANDLER(this);
      h->SetFD(fd);
      sel->AddHandler(h);
      return 0;
    }
  }
  int DeleteRemoteSource(char *iporhost, int port, bool udp=false) {
    return DeleteRemoteSource(ToIPAddress(iporhost),port,udp);
  }
  
  int DeleteRemoteSource(unsigned adx, int port, bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new RS(adx,port,udp,this));
      return 0;
    } else {
      RS thingtofind(adx,port,udp,this);
      RS *rs = (RS *)sources.FindRemove(&thingtofind);
      if (rs) {
	if (IsValidIPMulticastAddress(adx)) {
	  LeaveMulticastGroup(rs->fd,adx);
	}
	Handler *h = sel->RemoveHandler(rs->fd);
	if (h) { delete h;}
	close(rs->fd);
	delete rs;
	delete h;
      }
      return 0;
    }
  }

  int DeleteRemoteSource(char *pathname, bool isunixdom=true) {
    if (forwarding) {
      deletes.AddAtBack(new RS(pathname,isunixdom,this));
      return 0;
    } else {
      RS thingtofind(pathname, isunixdom,this);
      RS *rs = (RS *)sources.FindRemove(&thingtofind);
      if (rs) {
	Handler *h = sel->RemoveHandler(rs->fd);
	if (h) { delete h;}
	close(rs->fd);
	delete rs;
	delete h;
      }
      return 0;
    }
  }
  
  int AddFDConnect(int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDC(fd,this));
      return 0;
    } else {
      Handler *h = new SOURCECONNECTHANDLER(this);
      h->SetFD(fd);
      connects.AddAtBack(new FDC(fd,this));
      sel->AddHandler(h);
      return 0;
    }
  }
  
  int DeleteFDConnect(int fd) {
    if (forwarding) {
      deletes.AddAtBack(new FDC(fd,this));
      return 0;
    } else {
      FDC thingtofind(fd,this);
      FDC *fdc = (FDC*) connects.FindRemove(&thingtofind);
      if (fdc) {
	close(fd);
	Handler *h = sel->RemoveHandler(fd);
	delete fdc;
	delete h;
      }
      return 0;
    }
  }
  
  int AddLocalConnect(int port, bool udp=false) {
    if (forwarding) {
      adds.AddAtBack(new LC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      int sock;
      if (udp) {
	sock=CreateAndSetupUdpSocket();
      } else {
	sock=CreateAndSetupTcpSocket();
      }
      if (!sock) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      if (BindSocket(sock,port)) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      if (ListenSocket(sock)) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      Handler *h = new SOURCECONNECTHANDLER(this);
      h->SetFD(sock);
      sel->AddHandler(h);
      LC *lc = new LC(GetMyIPAddress(),port,udp,this);
      lc->fd=sock;
      connects.AddAtBack(lc);
      return 0;
    }
  }

  int AddLocalConnect(char *pathname) {
    if (forwarding) {
      adds.AddAtBack(new LC(pathname,true,this));
      return 0;
    } else {
      int sock;
      sock=CreateAndSetupUnixDomainSocket();
      if (!sock) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      if (BindSocket(sock,pathname)) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      if (ListenSocket(sock)) {
	perror("Mirror::AddLocalConnet");
	return -1;
      }
      Handler *h = new SOURCECONNECTHANDLER(this);
      h->SetFD(sock);
      sel->AddHandler(h);
      LC *lc = new LC(pathname,true,this);
      lc->fd=sock;
      connects.AddAtBack(lc);
      return 0;
    }
  }

  int DeleteLocalConnect(int port, bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new LC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      LC thingtofind(GetMyIPAddress(),port,udp,this);
      LC *lc = (LC*) connects.FindRemove(&thingtofind);
      if (lc) {
	Handler *h = sel->RemoveHandler(lc->fd);
	if (h) { delete h;}
	close(lc->fd);
	delete lc;
      }
      return 0;
    }
  }
  
  int DeleteLocalConnect(char *pathname) {
    if (forwarding) {
      deletes.AddAtBack(new LC(pathname,true,this));
      return 0;
    } else {
      LC thingtofind(pathname,true,this);
      LC *lc = (LC*) connects.FindRemove(&thingtofind);
      if (lc) {
	Handler *h = sel->RemoveHandler(lc->fd);
	if (h) { delete h;}
	close(lc->fd);
	delete lc;
      }
      return 0;
    }
  }

  int AddFDRequestResponse(int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      Handler *h = new REQUESTRESPONSEHANDLER(this);
      h->SetFD(fd);
      rrsources.AddAtBack(new FDRR(fd,this));
      sel->AddHandler(h);
    }
    return 0;
  }
  
  int DeleteFDRequestResponse(int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      FDRR thingtofind(fd,this);
      FDRR *temp=(FDRR*)rrsources.FindRemove(&thingtofind);
      if (temp) {
	Handler *h=sel->RemoveHandler(fd);
	if (h) { delete h;}
	close(fd);
	delete temp;
      }
      return 0;
    }
  }	

  int AddFDRequestResponseConnect(int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      Handler *h = new REQUESTRESPONSECONNECTHANDLER(this);
      h->SetFD(fd);
      rrconnects.AddAtBack(new FDRR(fd,this));
      sel->AddHandler(h);
      return 0;
    }
  }
  
  int DeleteFDRequestResponseConnect(int fd) {
    if (forwarding) {
      deletes.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      FDRR thingtofind(fd,this);
      FDRR *fdrr = (FDRR*) rrconnects.FindRemove(&thingtofind);
      if (fdrr) {
	close(fd);
	Handler *h = sel->RemoveHandler(fd);
	delete fdrr;
	delete h;
      }
      return 0;
    }
  }
  
  int AddLocalRequestResponseConnect(int port, bool udp=false) {
    if (forwarding) {
      adds.AddAtBack(new LRRC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      int sock;
      if (udp) {
	sock=CreateAndSetupUdpSocket();
      } else {
	sock=CreateAndSetupTcpSocket();
      }
      if (!sock) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      if (BindSocket(sock,port)) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      if (ListenSocket(sock)) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      Handler *h = new REQUESTRESPONSECONNECTHANDLER(this);
      h->SetFD(sock);
      sel->AddHandler(h);
      LRRC *lrr = new LRRC(GetMyIPAddress(),port,udp,this);
      lrr->fd=sock;
      rrconnects.AddAtBack(lrr);
      return 0;
    }
  }

  int AddLocalRequestResponseConnect(char *pathname) {
    if (forwarding) {
      adds.AddAtBack(new LRRC(pathname,true,this));
      return 0;
    } else {
      int sock;
      sock=CreateAndSetupUnixDomainSocket();
      if (!sock) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      if (BindSocket(sock,pathname)) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      if (ListenSocket(sock)) {
	perror("Mirror::AddLocalRequestResponse");
	return -1;
      }
      Handler *h = new REQUESTRESPONSECONNECTHANDLER(this);
      h->SetFD(sock);
      sel->AddHandler(h);
      LRRC *lrr = new LRRC(pathname,true,this);
      lrr->fd=sock;
      rrconnects.AddAtBack(lrr);
      return 0;
    }
  }

  int DeleteLocalRequestResponseConnect(int port, bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new LRRC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      LRRC thingtofind(GetMyIPAddress(),port,udp,this);
      LRRC *lrrc = (LRRC*) rrconnects.FindRemove(&thingtofind);
      if (lrrc) {
	Handler *h = sel->RemoveHandler(lrrc->fd);
	if (h) { delete h;}
	close(lrrc->fd);
	delete lrrc;
      }
      return 0;
    }
  }
  
  int DeleteLocalRequestResponseConnect(char *pathname) {
    if (forwarding) {
      deletes.AddAtBack(new LRRC(pathname,true,this));
      return 0;
    } else {
      LRRC thingtofind(pathname,true,this);
      LRRC *lrrc = (LRRC*) rrconnects.FindRemove(&thingtofind);
      if (lrrc) {
	Handler *h = sel->RemoveHandler(lrrc->fd);
	if (h) { delete h;}
	close(lrrc->fd);
	delete lrrc;
      }
      return 0;
    }
  }

  int AddFDTarget(int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDT(fd,this));
      return 0;
    } else {
      targets.AddAtBack(new FDT(fd,this));
      return 0;
    }
  }
  
  int DeleteFDTarget(int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDT(fd,this));
      return 0;
    } else {
      FDT thingtofind(fd,this);
      FDT *temp=(FDT*)targets.FindRemove(&thingtofind);
      if (temp) {
	close(fd);
	delete temp;
      }
      return 0;
    }
  }	
  
  int AddLocalTarget(LocalTarget *target) {
    if (forwarding) {
      adds.AddAtBack(new LT(target,0,this));
      return 0;
    } else {
      LT *lt = new LT(target,0,this);
      targets.AddAtBack(lt);
      return 0;
    }
  }
  
  int DeleteLocalTarget(LocalTarget *target) {
    if (forwarding) {
      deletes.AddAtBack(new LT(target,0,this));
      return 0;
    } else {
      LT thingtofind(target,0,this);
      LT *temp = (LT*)targets.FindRemove(&thingtofind);
      if (temp) {
	delete temp;
      }
      return 0;
    }
  }
  
  int AddRemoteTarget(char *iporhost, int port, bool udp=false, int ttl=1) {
    return AddRemoteTarget(ToIPAddress(iporhost),port,udp,ttl);
  }
  
  int AddRemoteTarget(unsigned adx, int port, bool udp=false, int ttl=1) {
    if (forwarding) {
      adds.AddAtBack(new RT(adx,port,udp,this));
      return 0;
    } else {
      int fd;
      if (udp) {
	fd = CreateAndSetupUdpSocket();
      } else {
	fd = CreateAndSetupTcpSocket();
      }
      if (udp) {
	BindSocket(fd,ntohl(INADDR_ANY),port);
      }
      if (IsValidIPMulticastAddress(adx)) {
	//JoinMulticastGroup(fd,adx);
	SetMulticastTimeToLive(fd,ttl);
      }
      ConnectToHost(fd,adx,port);
      RT *rt = new RT(adx,port,udp,this);
      rt->fd=fd;
      targets.AddAtBack(rt);
      return 0;
    }
  }
  
  int AddRemoteTarget(char *pathname, bool isunixdom=true) {
    if (forwarding) {
      adds.AddAtBack(new RT(pathname,isunixdom,this));
      return 0;
    } else {
      int fd;
      if (isunixdom) {
        if ((fd=CreateAndSetupUnixDomainSocket())<0) { 
          perror("Mirror::AddRemoteTarget");
          return -1;
        }
        if (ConnectToPath(fd,pathname)) {
          perror("Mirror::AddRemoteTarget");
          close(fd);
          return -1;
        }
      } else {
	if ((fd = open(pathname,O_CREAT | O_WRONLY | O_TRUNC, 0644))<0) {
          perror("Mirror::AddRemoteTarget");
          return -1;
        }
      }
      RT *rt = new RT(pathname,isunixdom,this);
      rt->fd=fd;
      targets.AddAtBack(rt);
      return 0;
    }
  }

  int DeleteRemoteTarget(char *iporhost, int port, bool udp=false) {
    return DeleteRemoteTarget(ToIPAddress(iporhost),port,udp);
  }
  
  int DeleteRemoteTarget(unsigned adx, int port, bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new RT(adx,port,udp,this));
      return 0;
    } else {
      RT thingtofind(adx,port,udp,this);
      RT *rt = (RT*)targets.FindRemove(&thingtofind);
      if (rt) {
	if (IsValidIPMulticastAddress(adx)) {
	  LeaveMulticastGroup(rt->fd,adx);
	}
	close(rt->fd);
	delete rt;
      }
      return 0;
    }
  }
  
  int DeleteRemoteTarget(char *pathname, bool isunixdom=true) {
    if (forwarding) {
      deletes.AddAtBack(new RT(pathname,isunixdom,this));
      return 0;
    } else {
      RT thingtofind(pathname,isunixdom,this);
      RT *rt = (RT*)targets.FindRemove(&thingtofind);
      if (rt) {
        close(rt->fd);
	delete rt;
      }
      return 0;
    }
  }

  virtual int DeleteMatching(int fd) {
    DeleteFDSource(fd);
    DeleteFDConnect(fd);
    DeleteFDTarget(fd);
    DeleteFDRequestResponse(fd);
    DeleteFDRequestResponseConnect(fd);
    return 0;
  }


  virtual int Forward(Buffer &buf) {
    ComparableMirrorData *x;
    
    assert(forwarding==false);
    forwarding=true;
    FOREACH_BEGIN(x,targets) {
      if ((x->Forward(buf))<buf.Size()) {
        Delete(x);
      }
    } FOREACH_END(targets)
    forwarding=false;
    Fixup();
    return 0;
  }
  
  virtual int Forward(char *buf, const int len) {
    Buffer b;
    b.Resize(len);
    memcpy(b.Data(),buf,len);
    return Forward(b);
  }
  
  virtual int QueueWrite(int fd, Buffer &buf) {
    Queue<Handler> *hw = sel->FindMatchingHandlers(fd);
    Handler *h;
    QueuedWriteHandler *qwh;
    int len;
    FOREACH_BEGIN(h,*hw) {
      if (h->GetTag() == QUEUED_WRITE_HANDLER_TAG) {
	// Should only be one matching one
	qwh = (QueuedWriteHandler*)h;
	delete hw;
	return qwh->QueueWrite(buf);
      }
    } FOREACH_END(*hw)
    // No write handler exists.  create one
    qwh = new QueuedWriteHandler(this,fd);
    qwh->SetFD(fd);
    sel->AddHandler(qwh);
    return qwh->QueueWrite(buf);
  }


  virtual int Run() {
    assert(forwarding==false);
    return sel->Run();
  }
};




template <class SERIALIZEABLEINFO, 
          class SOURCEHANDLER, 
          class SOURCECONNECTHANDLER,
          class REQUESTRESPONSEHANDLER,
          class REQUESTRESPONSECONNECTHANDLER>
class SerializeableMirror : 
  public Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER, REQUESTRESPONSEHANDLER, REQUESTRESPONSECONNECTHANDLER>
{
 public:
  virtual int ForwardSer(const SERIALIZEABLEINFO &s) {
    Buffer buf;
    s.Serialize(buf);
    return Forward(buf);
  }
};


template <class INSER, class COMPUTE, class OUTSER> 
class SerializeableMirrorInputComputeOutputHandler : 
public SerializeableMirrorInputHandler<INSER> {
public:
  SerializeableMirrorInputComputeOutputHandler(AbstractMirror *m) 
    : SerializeableMirrorInputHandler<INSER>(m) { SetHandlesRead(); }
  Handler *Clone() { 
    return new SerializeableMirrorInputComputeOutputHandler(*this);
  }
  int HandleRead(int fd, Selector &s) {
    int rc;
    INSER in;
    OUTSER out;
    if (in.Unserialize(fd)) {
      mymirror->DeleteMatching(fd);
      return -1;
    } else {
      rc = COMPUTE::Compute(in,out);
      Buffer buf;
      out.Serialize(buf);
      return mymirror->Forward(buf);
    }
  }
};


template <class INSER, class COMPUTE, class OUTSER> 
class SerializeableRequestResponseHandler : public MirrorHandler {
public:
  SerializeableRequestResponseHandler(AbstractMirror *m) 
    : MirrorHandler(m) { SetHandlesRead(); }
  Handler *Clone() { 
    return new SerializeableRequestResponseHandler(*this);
  }
  int HandleRead(int fd, Selector &s) {
    int rc;
    INSER in;
    OUTSER out;
    if (in.Unserialize(fd)) {
      mymirror->DeleteMatching(fd);
      return -1;
    } else {
      rc = COMPUTE::Compute(in,out);
      if (!rc) {
	return out.Serialize(fd);
      } else {
	return rc;
      }
    }
  }
  int HandleWrite(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};



// Generic behavior is simply to forward from sources->targets
// And to forward from requests -> targets

class GenericMirror : public Mirror<GenericMirrorInputHandler,
		                    GenericMirrorNewConnectionHandler,
		      NullHandler, NullHandler> {};




template <class SERIN, class COMPUTE, class SEROUT> 
class GenericSerializeableInputComputeOutputMirror :
public SerializeableMirror<SERIN,
                           SerializeableMirrorInputComputeOutputHandler<SERIN,COMPUTE,SEROUT>,
			   GenericMirrorNewConnectionHandler, 
                           NullHandler, NullHandler> {};


template <class SERIN, class COMPUTE, class SEROUT> 
class SerializeableRequestResponseMirror :
public SerializeableMirror<SERIN,
                           NullHandler,
                           NullHandler,
                           SerializeableRequestResponseHandler<SERIN,COMPUTE,SEROUT>,
			   GenericMirrorNewRequestResponseConnectionHandler> {};


template <class DATASERIN, class DATACOMPUTE, class DATASEROUT, 
          class CTRLSERIN, class CTRLCOMPUTE, class CTRLSEROUT> 
class GenericSerializeableInputComputeOutputWithRequestResponseControlMirror :
public SerializeableMirror<DATASERIN,
                           SerializeableMirrorInputComputeOutputHandler<DATASERIN,DATACOMPUTE,DATASEROUT>,
			   GenericMirrorNewConnectionHandler, 
                           SerializeableRequestResponseHandler<CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>,
			   GenericMirrorNewRequestResponseConnectionHandler> {};


template <class SERIALIZEABLEINFO>
class GenericSerializeableMirror : 
public GenericSerializeableInputComputeOutputMirror<SERIALIZEABLEINFO,
         NullCompute<SERIALIZEABLEINFO,SERIALIZEABLEINFO>,
         SERIALIZEABLEINFO> {};

/*
template <class DATAIN, class COMPUTE, class DATAOUT>
class Filter : public GenericSerializeableInputComputeOutputMirror<DATAIN,COMPUTE,DATAOUT> {};
*/

template <class DATAIN, class DATACOMPUTE, class DATAOUT, 
          class CTRLIN, class CTRLCOMPUTE, class CTRLOUT> 
class FilterWithControl :
public 
GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<
DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT
>
{};






#endif
