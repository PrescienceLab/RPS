#ifndef _MirrorTemplate
#define _MirrorTemplate

#include <iostream>
#include <new>
#include <typeinfo>

using namespace std;

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

#include "junk.h"

#define QUEUED_WRITES

#define MAKE_EQUAL(T)          \
T & operator=(const T &rhs) {  \
  this->~T();                  \
  return *(new(this)T(rhs));   \
}

#define MAKE_OUTPUT(T) inline ostream & operator<<(ostream &os, const T &rhs) { return rhs.operator<<(os);}

#define MAKE_OUTPUT_TEMPLATE(X) template <class T> inline ostream & operator<<(ostream &os, const X<T> &rhs) { return rhs.operator<<(os);};

class LocalSource {
 public:
  LocalSource() {}
  LocalSource(const LocalSource &rhs) {}
  virtual LocalSource & operator=(const LocalSource &rhs) { return *this;}
  virtual ~LocalSource() {}
  virtual void GetData(Buffer &buf)=0;
  virtual void GetInterval(TimeValue &tv)=0;
  virtual ostream & operator<<(ostream &os) const {
    return (os<<"LocalSource()");
  }
};

MAKE_OUTPUT(LocalSource)


class LocalTarget {
 public:
  LocalTarget() {}
  LocalTarget(const LocalTarget &rhs) {}
  virtual ~LocalTarget() {}
  virtual LocalTarget & operator=(const LocalTarget & rhs) { return *this;}
  virtual void ProcessData(Buffer &buf)=0;
  virtual ostream & operator<<(ostream &os) const {
    return (os<<"LocalTarget()");
  }
};

MAKE_OUTPUT(LocalTarget)


class AbstractMirror {
 public:
  AbstractMirror() {}
  AbstractMirror(const AbstractMirror &rhs) {}
  virtual AbstractMirror & operator=(const AbstractMirror &rhs) {return *this;}
  virtual ~AbstractMirror() {} ;

  virtual int AddEndPoint(const EndPoint &ep)=0;
  virtual int DeleteEndPoint(const EndPoint &ep)=0;

  virtual int AddFDSource(const int fd)=0;
  virtual int DeleteFDSource(const int fd)=0;

  virtual int AddLocalSource(LocalSource *ls)=0;
  virtual int DeleteLocalSource(LocalSource *ls)=0;

  virtual int AddRemoteSource(const char *iporhost, const int port, const bool udp=false)=0;
  virtual int AddRemoteSource(const unsigned adx, const int port, const bool udp=false)=0;
  virtual int AddRemoteSource(const char *pathname, const bool isunixdom=true)=0;
  virtual int DeleteRemoteSource(const char *iporhost, const int port, const bool udp=false)=0;
  virtual int DeleteRemoteSource(const unsigned adx, const int port, const bool udp=false)=0;
  virtual int DeleteRemoteSource(const char *pathname, const bool isunixdom=true)=0;

  virtual int AddFDConnect(const int fd)=0;
  virtual int DeleteFDConnect(const int fd)=0;

  virtual int AddLocalConnect(const int port, const bool udp=false)=0;
  virtual int AddLocalConnect(const char *pathname)=0;
  virtual int DeleteLocalConnect(const int port, const bool udp=false)=0;
  virtual int DeleteLocalConnect(const char *pathname)=0;

  virtual int AddFDRequestResponseConnect(const int fd)=0;
  virtual int DeleteFDRequestResponseConnect(const int fd)=0;

  virtual int AddLocalRequestResponseConnect(const int port, const bool udp=false)=0;
  virtual int AddLocalRequestResponseConnect(const char *pathname)=0;
  virtual int DeleteLocalRequestResponseConnect(const int port, const bool udp=false)=0;
  virtual int DeleteLocalRequestResponseConnect(const char *pathname)=0;

  virtual int AddFDRequestResponse(const int fd)=0;
  virtual int DeleteFDRequestResponse(const int fd)=0;              

  virtual int AddFDTarget(const int fd)=0;
  virtual int DeleteFDTarget(const int fd)=0;              

  virtual int AddLocalTarget(LocalTarget *lt)=0;
  virtual int DeleteLocalTarget(LocalTarget *lt)=0;

  virtual int AddRemoteTarget(const char *ip, const int port, const bool udp=false, const int ttl=1)=0;
  virtual int AddRemoteTarget(const unsigned, const int port, const bool udp=false, const int ttl=1)=0;
  virtual int AddRemoteTarget(const char *pathname, const bool isunixdom=true)=0;
  virtual int DeleteRemoteTarget(const char *iporhost, const int port, const bool udp=false)=0;  
  virtual int DeleteRemoteTarget(const unsigned adx, const int port, const bool udp=false)=0;  
  virtual int DeleteRemoteTarget(const char *pathname, const bool isunixdom=true)=0;

  virtual int DeleteMatching(const int fd)=0;

//  virtual int PushStreamInput(void *data, const int len)=0;

  virtual int Forward(const char *buf, const int len)=0;
  virtual int Forward(Buffer &buf)=0;

#ifdef QUEUED_WRITES
  virtual int QueueWrite(const int fd, const Buffer &buf)=0;
#endif

  virtual int Run()=0;

  virtual ostream & operator<<(ostream &os) const {
    return (os<<"AbstractMirror()");
  }
};  

MAKE_OUTPUT(AbstractMirror)


// These comparisons return nonzero if true
struct ComparableMirrorData {
  enum ComparableMirrorDataType {FDS,FDC,FDT,FDRR,FDRRC,LS,LC,LT,LRRC,RS,RT} datatype;
  AbstractMirror *mirror;
  ComparableMirrorData(const ComparableMirrorDataType t, 
		       AbstractMirror *mir) : datatype(t), mirror(mir) {}
  virtual int Compare(const ComparableMirrorData &other) const =0;
  virtual int Forward(Buffer &buf, const bool sendall=true)=0;
};

class ComparableMirrorDataCompare {
 public:
  static int Compare(const ComparableMirrorData *left, const ComparableMirrorData *right) {
    return !(left->Compare(*right));
  }
  static int Compare(const ComparableMirrorData &left, const ComparableMirrorData &right) {
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
  name(const int thefd, AbstractMirror *mir) :		        \
     ComparableMirrorData(ComparableMirrorData::name,mir),	\
			  fd(thefd) {}				\
  virtual int Compare(const ComparableMirrorData &other)  const {           \
    if (datatype==other.datatype) {				\
      return this->Compare((const struct name &)other);		\
    } else {							\
      return 0;							\
    }								\
  }								\
  virtual int Compare(const struct name &other) const {               \
    return fd==other.fd;					\
  }								\
  virtual int Forward(Buffer &buf, const bool sendall=true) {		\
    return SEND_DATA(mirror,fd,buf,sendall);			\
  }								\
};


//XXX
#define PTR_BASED(name,ptrtype,ptrname,fwd)					\
struct name : public ComparableMirrorData {					\
  ptrtype *ptrname;								\
  Handler *handler;								\
  name(ptrtype *theptrname, Handler *h, AbstractMirror *mir)  :		\
    ComparableMirrorData(ComparableMirrorData::name,mir),			\
    ptrname(theptrname), handler(h) {}						\
  virtual int Compare(const ComparableMirrorData &other) const {				\
    if (datatype==other.datatype) {						\
        return this->Compare((const struct name &)other);				\
    } else {									\
        return 0;								\
    }										\
  }										\
  virtual int Compare(const struct name &other) const {					\
    return ptrname==other.ptrname;						\
  }										\
  virtual int Forward(Buffer &buf, const bool sendall=true) {				\
    ((LocalTarget*)ptrname)->ProcessData(buf); return buf.Size();	\
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
  name(const char *path, const bool isunixdom, AbstractMirror *mir) :		     \
	  ComparableMirrorData(ComparableMirrorData::name,mir),	     \
          adx(0), port(0), fd(-1) {					     \
          if (isunixdom) {addresstype=UNIXDOM;} else {addresstype=AFILE;}    \
	  pathname=new char [strlen(path)+1];				     \
          strcpy(pathname,path);                                             \
  }									     \
  name(const unsigned theadx, const int theport, const bool isudp, AbstractMirror *mir):	     \
	  ComparableMirrorData(ComparableMirrorData::name,mir),	     \
          adx(theadx), port(theport), pathname(0), fd(-1) {                  \
          if (isudp) {addresstype=UDP;} else {addresstype=TCP;}              \
  }									     \
  virtual int Compare(const ComparableMirrorData &other) const {			     \
    if (datatype==other.datatype) {					     \
        return this->Compare((const struct name &)other);			     \
    } else {								     \
        return 0;							     \
    }									     \
  }									     \
  virtual int Compare(const struct name &other) const {				     \
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
    default:                                                                 \
      return false;                                                          \
      break;                                                                 \
    }									     \
  }									     \
  virtual int Forward(Buffer &buf, const bool sendall=true) {			     \
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
  MirrorHandler() : Handler(), mymirror(0) {}
  MirrorHandler(const MirrorHandler &rhs) : Handler(rhs), mymirror(rhs.mymirror) {}
  virtual MirrorHandler & operator=(const MirrorHandler &rhs) { mymirror=rhs.mymirror; return *this; }
  MirrorHandler(AbstractMirror *mirror) { mymirror=mirror;}
  virtual ~MirrorHandler() { mymirror=0;}
  virtual Handler *Clone() const =0;
  ostream & operator<<(ostream &os) const {
    return (os<<"MirrorHandler(mymirror="<<*mymirror<<")");
  }
};

MAKE_OUTPUT(MirrorHandler)


class NullHandler : public MirrorHandler {
public:
  NullHandler() : MirrorHandler() {}
  NullHandler(const NullHandler &rhs) : MirrorHandler(rhs) {}
  virtual ~NullHandler() {};
  NullHandler(AbstractMirror *m) :  MirrorHandler(m) {
     fprintf(stderr, "Warning: Instantiating NullHandler\n");
  }
  virtual MAKE_EQUAL(NullHandler)
  Handler *Clone() const { 
    return new NullHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"NullHandler(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

MAKE_OUTPUT(NullHandler)

template <class SERIN, class SEROUT>
class NullCompute {
 public:
  NullCompute() {}
  NullCompute(const NullCompute &rhs) {}
  virtual ~NullCompute() {}
  virtual NullCompute<SERIN,SEROUT> & operator=(const NullCompute<SERIN,SEROUT> &rhs) {
    this->~NullCompute();
    return *(new(this)NullCompute<SERIN,SEROUT>(rhs));
  }
  static int Compute(const SERIN &in, SEROUT &out) { out=in; return 0;}
  ostream & operator<<(ostream &os) const {
    os<<"NullCompute<"<<typeid(SERIN).name()<<", "<<typeid(SEROUT).name()<<">()";
    return os;
  }
};

template <class SERIN, class SEROUT> 
inline ostream & operator<<(ostream &os, const NullCompute<SERIN,SEROUT> &rhs) {
  return rhs.operator<<(os);
}

class GenericMirrorInputHandler : public MirrorHandler {
 public:
  GenericMirrorInputHandler() : MirrorHandler() {}
  GenericMirrorInputHandler(const GenericMirrorInputHandler &rhs) : MirrorHandler(rhs) {}
  virtual ~GenericMirrorInputHandler() {}
  virtual MAKE_EQUAL(GenericMirrorInputHandler)
  GenericMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new GenericMirrorInputHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
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
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"GenericMirrorInputHandler(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

MAKE_OUTPUT(GenericMirrorInputHandler)

class GenericMirrorNewConnectionHandler : public MirrorHandler {
public:
  GenericMirrorNewConnectionHandler() : MirrorHandler() {}
  GenericMirrorNewConnectionHandler(const GenericMirrorNewConnectionHandler &rhs) : MirrorHandler(rhs) {}
  virtual ~GenericMirrorNewConnectionHandler() {}
  virtual MAKE_EQUAL(GenericMirrorNewConnectionHandler)
  GenericMirrorNewConnectionHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new GenericMirrorNewConnectionHandler(*this);
  }
  
  // Accept new connection
  int HandleRead(const int fd, Selector &s) {
    int acc = accept(fd,0,0);
    if (acc>=0) {
      SetNoDelaySocket(acc);
      mymirror->AddFDTarget(acc);
      return 0;
    } else {
      return -1;
    }
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"GenericMirrorNewConnectionHandler(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

MAKE_OUTPUT(GenericMirrorNewConnectionHandler)

class GenericMirrorNewRequestResponseConnectionHandler : public MirrorHandler {
public:
  GenericMirrorNewRequestResponseConnectionHandler() : MirrorHandler() {}
  GenericMirrorNewRequestResponseConnectionHandler(const GenericMirrorNewRequestResponseConnectionHandler &rhs) : MirrorHandler(rhs) {}
  virtual ~GenericMirrorNewRequestResponseConnectionHandler() {};
  virtual MAKE_EQUAL(GenericMirrorNewRequestResponseConnectionHandler)
  GenericMirrorNewRequestResponseConnectionHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new GenericMirrorNewRequestResponseConnectionHandler(*this);
  }
  
  // Accept new connection
  int HandleRead(const int fd, Selector &s) {
    int acc = accept(fd,0,0);
    if (acc>=0) { 
      SetNoDelaySocket(acc);
      mymirror->AddFDRequestResponse(acc);
      return 0;
    } else {
      return -1;
    }
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"GenericMirrorNewRequestResponseConnectionHandler(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

MAKE_OUTPUT(GenericMirrorNewRequestResponseConnectionHandler)

template <class SERIALIZEABLEINFO>
class SerializeableMirrorInputHandler : public MirrorHandler {
public:
  SerializeableMirrorInputHandler() : MirrorHandler() {}
  SerializeableMirrorInputHandler(const SerializeableMirrorInputHandler<SERIALIZEABLEINFO> & rhs) : MirrorHandler(rhs) {}
  virtual ~SerializeableMirrorInputHandler() {}
  SerializeableMirrorInputHandler<SERIALIZEABLEINFO> & operator=(const SerializeableMirrorInputHandler<SERIALIZEABLEINFO> &rhs) {
    this->~SerializeableMirrorInputHandler();
    return *(new(this)SerializeableMirrorInputHandler<SERIALIZEABLEINFO>(rhs));
  }
  SerializeableMirrorInputHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new SerializeableMirrorInputHandler(*this);
  }
  
  int HandleRead(const int fd, Selector &s) {
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
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"GenericMirrorInputHandler<"<<typeid(SERIALIZEABLEINFO).name()<<">(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

template <class SERIALIZEABLEINFO>
inline ostream & operator<<(ostream &os, const SerializeableMirrorInputHandler<SERIALIZEABLEINFO> &rhs) {
  return rhs.operator<<(os);
}


template <class SERIN, class COMPUTE, class SEROUT>
class SerializeableMirrorRequestResponseHandler : public MirrorHandler {
public:
  SerializeableMirrorRequestResponseHandler() : MirrorHandler() {}
  SerializeableMirrorRequestResponseHandler(const SerializeableMirrorRequestResponseHandler<SERIN,COMPUTE,SEROUT> &rhs) : MirrorHandler(rhs) {}
  virtual ~SerializeableMirrorRequestResponseHandler() {}
  virtual SerializeableMirrorRequestResponseHandler<SERIN,COMPUTE,SEROUT> & operator=(const SerializeableMirrorRequestResponseHandler<SERIN,COMPUTE,SEROUT> & rhs) {
    this->~SerializeableMirrorRequestResponseHandler();
    return *(new(this)SerializeableMirrorRequestResponseHandler<SERIN,COMPUTE,SEROUT>(rhs));
  }
  SerializeableMirrorRequestResponseHandler(AbstractMirror *mirror) : MirrorHandler(mirror) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new SerializeableMirrorRequestResponseHandler(*this);
  }
  
  int HandleRead(const int fd, Selector &s) {
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
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
  ostream & operator<<(ostream &os) const {
    os<<"SerializeableMirrorRequestResponseHandler<"<<typeid(SERIN).name()
      <<", "<<typeid(COMPUTE).name()<<", "<<typeid(SEROUT).name()<<">(";
    MirrorHandler::operator<<(os);
    os<<")";
    return os;
  }
};

template <class SERIN, class COMPUTE, class SEROUT>
inline ostream & operator<<(ostream &os, const SerializeableMirrorRequestResponseHandler<SERIN,COMPUTE,SEROUT> &rhs) {
  return rhs.operator<<(os);
}

class LocalSourceHandler : public MirrorHandler {
 private:
  LocalSource *source;
 public:
  LocalSourceHandler() : MirrorHandler(), source(0) {}
  LocalSourceHandler(const LocalSourceHandler &rhs) : MirrorHandler(rhs), source(rhs.source) {}
  virtual ~LocalSourceHandler() {}
  virtual MAKE_EQUAL(LocalSourceHandler)
  LocalSourceHandler(AbstractMirror *mir, LocalSource *ls) :
    MirrorHandler(mir) {
    source=ls;
    TimeValue tv;
    ls->GetInterval(tv);
    SetWait(tv);
    SetHandlesTimeout();
  }
  Handler *Clone() const { 
    return new LocalSourceHandler(*this);
  }

  int HandleRead(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    Buffer buf;
    source->GetData(buf);
    mymirror->Forward(buf);
    return -1;
  }
  LocalSource *GetSource() const { return source;}
  ostream & operator<<(ostream &os) const {
    os << "LocalSourceHandler(";
    MirrorHandler::operator<<(os);
    os << ", source="<<*source<<")";
    return os;
  }
};

MAKE_OUTPUT(LocalSourceHandler)

class QueuedBuffer : public Buffer {
 public:
  int    left;
  QueuedBuffer() : Buffer() {}
  QueuedBuffer(const QueuedBuffer &rhs) : Buffer(rhs), left(rhs.left) {}
  virtual MAKE_EQUAL(QueuedBuffer)
  QueuedBuffer(const Buffer &thebuf) : Buffer(thebuf) { left=Size(); }
  virtual ~QueuedBuffer() { left=0; }
  ostream & operator<<(ostream &os) const {
    os <<"QueuedBuffer(";
    Buffer::operator<<(os);
    os <<", left="<<left<<")";
    return os;
  }
};
   
MAKE_OUTPUT(QueuedBuffer) 

const int QUEUED_WRITE_HANDLER_TAG=0xbeefdead;

class QueuedWriteHandler : public MirrorHandler {
 private:
  Queue<QueuedBuffer> bufs;
  int  curoff;
 public:
  QueuedWriteHandler() : MirrorHandler() {}
  QueuedWriteHandler(const QueuedWriteHandler &rhs) : MirrorHandler(rhs) {
    DupeQueue(bufs,rhs.bufs);
    curoff=rhs.curoff;
  }
  virtual MAKE_EQUAL(QueuedWriteHandler)
  QueuedWriteHandler(AbstractMirror *mir, int fd) :
    MirrorHandler(mir) {
    curoff=0;
    tag = QUEUED_WRITE_HANDLER_TAG; 
  }
  virtual ~QueuedWriteHandler() { 
    bufs.Clear();
  }
  Handler *Clone() const { 
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
  int HandleRead(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    QueuedBuffer *buf;
    int bytessent;
    while ((buf=bufs.RemoveFromFront())) {
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
  ostream & operator<<(ostream &os) const {
    os <<"QueuedWriteHandler(";
    MirrorHandler::operator<<(os);
    os << ", curoff="<<curoff<<"bufs=(";
    bool next=false;
    QueuedBuffer *b;
    FOREACH_BEGIN(b,const_cast<Queue<QueuedBuffer>&>(bufs)) {
      if (!next) {
	os <<", ";
      }
      os <<*b;
      next=true;
    } FOREACH_END(const_cast<Queue<QueuedBuffer>&>(bufs))
    os <<"))";
    return os;
  }
};

MAKE_OUTPUT(QueuedWriteHandler)

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
  int Add(const ComparableMirrorData *x) {
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
  
  int Delete(const ComparableMirrorData *x) {
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
  
  Mirror(const Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> &rhs) {
    DupeQueue(sources,rhs.sources);
    DupeQueue(connects,rhs.connects);
    DupeQueue(targets,rhs.targets);
    DupeQueue(rrconnects, rhs.rrconnects);
    DupeQueue(rrsources,rhs.rrsources);
    DupeQueue(adds,rhs.adds);
    DupeQueue(deletes,rhs.deletes);
    forwarding=rhs.forwarding;
    sharedselector=true;
    sel=rhs.sel;
  }


  Mirror(Selector *sharedsel=0) : AbstractMirror() {
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

  virtual Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> & operator= (const Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> &rhs) {
    this->~Mirror();
    return *(new(this)Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER>(rhs));
  }

  int AddEndPoint(const EndPoint &ep) {
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
  
  int DeleteEndPoint(const EndPoint &ep) {
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

  int AddFDSource(const int fd) {
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
  
  int DeleteFDSource(const int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDS(fd,this));
      return 0;
    } else {
      FDS thingtofind(fd,this);
      FDS *temp=(FDS*)sources.FindRemove(&thingtofind);
      if (temp) {
	Handler *h=sel->RemoveHandler(fd);
	if (h) { delete h;}
	CLOSE(fd);
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
  
  int AddRemoteSource(const char *iporhost, const int port, const bool udp=false) {
    return AddRemoteSource(ToIPAddress(iporhost),port,udp);
  }
  
  int AddRemoteSource(const unsigned adx, const int port, const bool udp=false) {
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
	  CLOSE(fd);
	  return -1;
	}
	if (JoinMulticastGroup(fd,adx)) {
	  perror("Mirror:AddRemoteSource can't join group");
	  CLOSE(fd);
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
	  CLOSE(fd);
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
  
  int AddRemoteSource(const char *pathname, const bool isunixdom=true) {
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
	    CLOSE(fd);
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
  int DeleteRemoteSource(const char *iporhost, const int port, const bool udp=false) {
    return DeleteRemoteSource(ToIPAddress(iporhost),port,udp);
  }
  
  int DeleteRemoteSource(const unsigned adx, const int port, const bool udp=false) {
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
	CLOSE(rs->fd);
	delete rs;
	delete h;
      }
      return 0;
    }
  }

  int DeleteRemoteSource(const char *pathname, const bool isunixdom=true) {
    if (forwarding) {
      deletes.AddAtBack(new RS(pathname,isunixdom,this));
      return 0;
    } else {
      RS thingtofind(pathname, isunixdom,this);
      RS *rs = (RS *)sources.FindRemove(&thingtofind);
      if (rs) {
	Handler *h = sel->RemoveHandler(rs->fd);
	if (h) { delete h;}
	CLOSE(rs->fd);
	delete rs;
	delete h;
      }
      return 0;
    }
  }
  
  int AddFDConnect(const int fd) {
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
  
  int DeleteFDConnect(const int fd) {
    if (forwarding) {
      deletes.AddAtBack(new FDC(fd,this));
      return 0;
    } else {
      FDC thingtofind(fd,this);
      FDC *fdc = (FDC*) connects.FindRemove(&thingtofind);
      if (fdc) {
	CLOSE(fd);
	Handler *h = sel->RemoveHandler(fd);
	delete fdc;
	delete h;
      }
      return 0;
    }
  }
  
  int AddLocalConnect(const int port, const bool udp=false) {
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

  int AddLocalConnect(const char *pathname) {
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

  int DeleteLocalConnect(const int port, const bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new LC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      LC thingtofind(GetMyIPAddress(),port,udp,this);
      LC *lc = (LC*) connects.FindRemove(&thingtofind);
      if (lc) {
	Handler *h = sel->RemoveHandler(lc->fd);
	if (h) { delete h;}
	CLOSE(lc->fd);
	delete lc;
      }
      return 0;
    }
  }
  
  int DeleteLocalConnect(const char *pathname) {
    if (forwarding) {
      deletes.AddAtBack(new LC(pathname,true,this));
      return 0;
    } else {
      LC thingtofind(pathname,true,this);
      LC *lc = (LC*) connects.FindRemove(&thingtofind);
      if (lc) {
	Handler *h = sel->RemoveHandler(lc->fd);
	if (h) { delete h;}
	CLOSE(lc->fd);
	delete lc;
      }
      return 0;
    }
  }

  int AddFDRequestResponse(const int fd) {
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
  
  int DeleteFDRequestResponse(const int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      FDRR thingtofind(fd,this);
      FDRR *temp=(FDRR*)rrsources.FindRemove(&thingtofind);
      if (temp) {
	Handler *h=sel->RemoveHandler(fd);
	if (h) { delete h;}
	CLOSE(fd);
	delete temp;
      }
      return 0;
    }
  }	

  int AddFDRequestResponseConnect(const int fd) {
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
  
  int DeleteFDRequestResponseConnect(const int fd) {
    if (forwarding) {
      deletes.AddAtBack(new FDRR(fd,this));
      return 0;
    } else {
      FDRR thingtofind(fd,this);
      FDRR *fdrr = (FDRR*) rrconnects.FindRemove(&thingtofind);
      if (fdrr) {
	CLOSE(fd);
	Handler *h = sel->RemoveHandler(fd);
	delete fdrr;
	delete h;
      }
      return 0;
    }
  }
  
  int AddLocalRequestResponseConnect(const int port, const bool udp=false) {
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

  int AddLocalRequestResponseConnect(const char *pathname) {
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

  int DeleteLocalRequestResponseConnect(const int port, const bool udp=false) {
    if (forwarding) {
      deletes.AddAtBack(new LRRC(GetMyIPAddress(),port,udp,this));
      return 0;
    } else {
      LRRC thingtofind(GetMyIPAddress(),port,udp,this);
      LRRC *lrrc = (LRRC*) rrconnects.FindRemove(&thingtofind);
      if (lrrc) {
	Handler *h = sel->RemoveHandler(lrrc->fd);
	if (h) { delete h;}
	CLOSE(lrrc->fd);
	delete lrrc;
      }
      return 0;
    }
  }
  
  int DeleteLocalRequestResponseConnect(const char *pathname) {
    if (forwarding) {
      deletes.AddAtBack(new LRRC(pathname,true,this));
      return 0;
    } else {
      LRRC thingtofind(pathname,true,this);
      LRRC *lrrc = (LRRC*) rrconnects.FindRemove(&thingtofind);
      if (lrrc) {
	Handler *h = sel->RemoveHandler(lrrc->fd);
	if (h) { delete h;}
	CLOSE(lrrc->fd);
	delete lrrc;
      }
      return 0;
    }
  }

  int AddFDTarget(const int fd) {
    if (forwarding) {
      adds.AddAtBack(new FDT(fd,this));
      return 0;
    } else {
      targets.AddAtBack(new FDT(fd,this));
      return 0;
    }
  }
  
  int DeleteFDTarget(const int fd) {
    if (forwarding) { 
      deletes.AddAtBack(new FDT(fd,this));
      return 0;
    } else {
      FDT thingtofind(fd,this);
      FDT *temp=(FDT*)targets.FindRemove(&thingtofind);
      if (temp) {
	CLOSE(fd);
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
  
  int AddRemoteTarget(const char *iporhost, const int port, const bool udp=false, const int ttl=1) {
    return AddRemoteTarget(ToIPAddress(iporhost),port,udp,ttl);
  }
  
  int AddRemoteTarget(const unsigned adx, const int port, const bool udp=false, const int ttl=1) {
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
  
  int AddRemoteTarget(const char *pathname, const bool isunixdom=true) {
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
          CLOSE(fd);
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

  int DeleteRemoteTarget(const char *iporhost, const int port, const bool udp=false) {
    return DeleteRemoteTarget(ToIPAddress(iporhost),port,udp);
  }
  
  int DeleteRemoteTarget(const unsigned adx, const int port, const bool udp=false) {
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
	CLOSE(rt->fd);
	delete rt;
      }
      return 0;
    }
  }
  
  int DeleteRemoteTarget(const char *pathname, const bool isunixdom=true) {
    if (forwarding) {
      deletes.AddAtBack(new RT(pathname,isunixdom,this));
      return 0;
    } else {
      RT thingtofind(pathname,isunixdom,this);
      RT *rt = (RT*)targets.FindRemove(&thingtofind);
      if (rt) {
        CLOSE(rt->fd);
	delete rt;
      }
      return 0;
    }
  }
  
  virtual int DeleteMatching(const int fd) {
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
  
  virtual int Forward(const char *buf, const int len) {
    Buffer b;
    b.Resize(len);
    memcpy(b.Data(),buf,len);
    return Forward(b);
  }
  
  virtual int QueueWrite(const int fd, const Buffer &buf) {
    Queue<Handler> *hw = sel->FindMatchingHandlers(fd);
    Handler *h;
    QueuedWriteHandler *qwh;
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


template <class SOURCEHANDLER, 
          class SOURCECONNECTHANDLER, 
          class REQUESTRESPONSEHANDLER, 
          class REQUESTRESPONSECONNECTHANDLER>
inline ostream & operator<<(ostream &os, const Mirror<SOURCEHANDLER, SOURCECONNECTHANDLER, REQUESTRESPONSEHANDLER, REQUESTRESPONSECONNECTHANDLER> &rhs) {
  return rhs.operator<<(os);
}



template <class SERIALIZEABLEINFO, 
          class SOURCEHANDLER, 
          class SOURCECONNECTHANDLER,
          class REQUESTRESPONSEHANDLER,
          class REQUESTRESPONSECONNECTHANDLER>
		      class SerializeableMirror : 
  public Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER, REQUESTRESPONSEHANDLER, REQUESTRESPONSECONNECTHANDLER>
{
 public:
  SerializeableMirror() : Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER>() {}
  SerializeableMirror(const SerializeableMirror<SERIALIZEABLEINFO,SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> &rhs) : Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER>(rhs) {}
  virtual ~SerializeableMirror() {};
  virtual SerializeableMirror<SERIALIZEABLEINFO,SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> & operator=(const SerializeableMirror<SERIALIZEABLEINFO,SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER> &rhs) {
    this->~SerializeableMirror();
    return *(new(this) SerializeableMirror<SERIALIZEABLEINFO,SOURCEHANDLER,SOURCECONNECTHANDLER,REQUESTRESPONSEHANDLER,REQUESTRESPONSECONNECTHANDLER>(rhs));
  }
  
  virtual int ForwardSer(const SERIALIZEABLEINFO &s) {
    Buffer buf;
    s.Serialize(buf);
    return Forward(buf);
  }
  
  ostream & operator<<(ostream &os) const {
    os << "SerializeableMirror<"<<typeid(SERIALIZEABLEINFO).name()<<","<<typeid(SOURCEHANDLER).name()<<","
       << typeid(SOURCECONNECTHANDLER).name()<<","<<typeid(REQUESTRESPONSEHANDLER).name()
       << ","<<typeid(REQUESTRESPONSECONNECTHANDLER).name()<<">(";
    Mirror<SOURCEHANDLER,SOURCECONNECTHANDLER, REQUESTRESPONSEHANDLER, REQUESTRESPONSECONNECTHANDLER>::operator<<(os);
    os << ")";
    return os;
  }
};

template <class SERIALIZEABLEINFO, 
          class SOURCEHANDLER, 
          class SOURCECONNECTHANDLER,
          class REQUESTRESPONSEHANDLER,
          class REQUESTRESPONSECONNECTHANDLER>
inline ostream & operator<<(ostream &os, const SerializeableMirror<SERIALIZEABLEINFO, SOURCEHANDLER, SOURCECONNECTHANDLER, REQUESTRESPONSEHANDLER, REQUESTRESPONSECONNECTHANDLER> &rhs) {
return rhs.operator<<(os);
}


template <class INSER, class COMPUTE, class OUTSER> 
class SerializeableMirrorInputComputeOutputHandler : 
public SerializeableMirrorInputHandler<INSER> {
public:
  SerializeableMirrorInputComputeOutputHandler() : SerializeableMirrorInputHandler<INSER>() {}
  SerializeableMirrorInputComputeOutputHandler(AbstractMirror *m) 
    : SerializeableMirrorInputHandler<INSER>(m) { SetHandlesRead(); }
  SerializeableMirrorInputComputeOutputHandler(const SerializeableMirrorInputComputeOutputHandler<INSER,COMPUTE,OUTSER> &rhs) 
    : SerializeableMirrorInputHandler<INSER>(rhs) { SetHandlesRead(); }
  virtual ~SerializeableMirrorInputComputeOutputHandler() {}
  virtual SerializeableMirrorInputComputeOutputHandler<INSER,COMPUTE,OUTSER> & operator = (const SerializeableMirrorInputComputeOutputHandler<INSER,COMPUTE,OUTSER> &rhs) {
    this->~SerializeableMirrorInputComputeOutputHandler();
    return *(new(this)SerializeableMirrorInputComputeOutputHandler<INSER,COMPUTE,OUTSER>(rhs));
  }
  
  Handler *Clone() const { 
    return new SerializeableMirrorInputComputeOutputHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
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
  ostream & operator <<(ostream & os) const {
    os << "SerializeableMirrorInputComputeOutputHandler<"<<typeid(INSER).name()<<","
       << typeid(COMPUTE).name()<<","<<typeid(OUTSER).name()<<">(";
    SerializeableMirrorInputHandler<INSER>::operator<<(os);
    os << ")";
    return os;
  }
};

template <class INSER, class COMPUTE, class OUTSER> 
inline ostream & operator<<(ostream &os, const SerializeableMirrorInputComputeOutputHandler<INSER,COMPUTE,OUTSER> &rhs) {
return rhs.operator<<(os);
};


template <class INSER, class COMPUTE, class OUTSER> 
	  class SerializeableRequestResponseHandler : public MirrorHandler {
public:
   SerializeableRequestResponseHandler() : MirrorHandler() {}
   SerializeableRequestResponseHandler(const SerializeableRequestResponseHandler<INSER,COMPUTE,OUTSER> &rhs) :
    MirrorHandler(rhs) {}
   SerializeableRequestResponseHandler(AbstractMirror *m) 
    : MirrorHandler(m) { SetHandlesRead(); }
   virtual ~SerializeableRequestResponseHandler() {}
   virtual SerializeableRequestResponseHandler<INSER,COMPUTE,OUTSER> & operator=(const SerializeableRequestResponseHandler<INSER,COMPUTE,OUTSER> &rhs) { 
      this->~SerializeableRequestResponseHandler();
      return *(new(this)SerializeableRequestResponseHandler<INSER,COMPUTE,OUTSER>(rhs));
   }
  Handler *Clone() const { 
    return new SerializeableRequestResponseHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
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
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }

  ostream & operator <<(ostream & os) const {
    os << "SerializeableRequestResponseHandler<"<<typeid(INSER).name()<<","
       << typeid(COMPUTE).name()<<","<<typeid(OUTSER).name()<<">(";
    MirrorHandler::operator<<(os);
    os << ")";
    return os;
  }

};

template <class INSER, class COMPUTE, class OUTSER> 
inline ostream & operator<<(ostream &os, const SerializeableRequestResponseHandler<INSER,COMPUTE,OUTSER> &rhs) {
  return rhs.operator<<(os);
}

// Generic behavior is simply to forward from sources->targets
// And to forward from requests -> targets

class GenericMirror : public Mirror<GenericMirrorInputHandler,
		                    GenericMirrorNewConnectionHandler,
		                    NullHandler, 
                                    NullHandler> {
public:
  GenericMirror() : Mirror<GenericMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler>() {}
  GenericMirror(const GenericMirror &rhs) : Mirror<GenericMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler>(rhs) {}
  virtual ~GenericMirror() {}
  virtual MAKE_EQUAL(GenericMirror)
   
  ostream & operator <<(ostream & os) const {
    os << "GenericMirror(";
    Mirror<GenericMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler>::operator<<(os);
    os << ")";
    return os;
  }

};

MAKE_OUTPUT(GenericMirror)



template <class SERIN, class COMPUTE, class SEROUT> 
class GenericSerializeableInputComputeOutputMirror :
public SerializeableMirror<SERIN,
                           SerializeableMirrorInputComputeOutputHandler<SERIN,COMPUTE,SEROUT>,
			   GenericMirrorNewConnectionHandler, 
                           NullHandler, NullHandler> {
public:
  GenericSerializeableInputComputeOutputMirror() : 
    SerializeableMirror<SERIN, SerializeableMirrorInputComputeOutputHandler<SERIN,COMPUTE,SEROUT>,
    GenericMirrorNewConnectionHandler, NullHandler, NullHandler>() {}
  GenericSerializeableInputComputeOutputMirror(const GenericSerializeableInputComputeOutputMirror<SERIN,COMPUTE,SEROUT> &rhs) : SerializeableMirror<SERIN, SerializeableMirrorInputComputeOutputHandler<SERIN,COMPUTE,SEROUT>,								 GenericMirrorNewConnectionHandler, NullHandler, NullHandler>(rhs) {}
  virtual ~GenericSerializeableInputComputeOutputMirror() {}
  virtual GenericSerializeableInputComputeOutputMirror<SERIN,COMPUTE,SEROUT> & operator=(const GenericSerializeableInputComputeOutputMirror<SERIN,COMPUTE,SEROUT> &rhs) {
     this->~GenericSerializeableInputComputeOutputMirror();  
     return *(new(this)GenericSerializeableInputComputeOutputMirror<SERIN,COMPUTE,SEROUT>(rhs));
  }

  ostream &operator<<(ostream &os) const {
    os << "GenericSerializeableInputComputeOutputMirror<"<<typeid(SERIN).name()<<","
       << typeid(COMPUTE).name()<<","<<typeid(SEROUT).name()<<">(";
    SerializeableMirror<SERIN, SerializeableMirrorInputComputeOutputHandler<SERIN,COMPUTE,SEROUT>,
    GenericMirrorNewConnectionHandler, NullHandler, NullHandler>::operator<<(os);
    os << ")";
    return os;
  }
};

template <class SERIN, class COMPUTE, class SEROUT> 
inline ostream & operator<<(ostream &os, const GenericSerializeableInputComputeOutputMirror<SERIN,COMPUTE,SEROUT> &rhs)
{
  return rhs.operator<<(os);
};

template <class SERIN, class COMPUTE, class SEROUT> 
class SerializeableRequestResponseMirror :
public SerializeableMirror<SERIN,
                           NullHandler,
                           NullHandler,
                           SerializeableRequestResponseHandler<SERIN,COMPUTE,SEROUT>,
			   GenericMirrorNewRequestResponseConnectionHandler> {
public:
  SerializeableRequestResponseMirror() : 
    SerializeableMirror<SERIN, NullHandler,NullHandler,
      SerializeableRequestResponseHandler<SERIN,COMPUTE,SEROUT>, GenericMirrorNewRequestResponseConnectionHandler>() {}
  SerializeableRequestResponseMirror(const SerializeableRequestResponseMirror<SERIN,COMPUTE,SEROUT> &rhs) : 
    SerializeableMirror<SERIN, NullHandler,NullHandler,
      SerializeableRequestResponseHandler<SERIN,COMPUTE,SEROUT>, GenericMirrorNewRequestResponseConnectionHandler>(rhs) {}
  virtual ~SerializeableRequestResponseMirror() {}
  virtual SerializeableRequestResponseMirror<SERIN,COMPUTE,SEROUT> & operator=(const SerializeableRequestResponseMirror<SERIN,COMPUTE,SEROUT> &rhs) {
    this->~SerializeableRequestResponseMirror();
    return *(new(this)SerializeableRequestResponseMirror<SERIN,COMPUTE,SEROUT>(rhs));
  }
      
  ostream &operator<<(ostream &os) const {
    os << "SerializeableRequestResponseMirror<"<<typeid(SERIN).name()<<","
       << typeid(COMPUTE).name()<<","<<typeid(SEROUT).name()<<">(";
     SerializeableMirror<SERIN, NullHandler,NullHandler,SerializeableRequestResponseHandler<SERIN,COMPUTE,SEROUT>, GenericMirrorNewRequestResponseConnectionHandler>::operator<<(os);
    os << ")";
    return os;
  }
};


template <class SERIN, class COMPUTE, class SEROUT> 
inline ostream & operator<<(ostream &os, const SerializeableRequestResponseMirror<SERIN,COMPUTE,SEROUT> &rhs) {
  return rhs.operator<<(os);
};


template <class DATASERIN, class DATACOMPUTE, class DATASEROUT, 
          class CTRLSERIN, class CTRLCOMPUTE, class CTRLSEROUT> 
class GenericSerializeableInputComputeOutputWithRequestResponseControlMirror :
public SerializeableMirror<DATASERIN,
                           SerializeableMirrorInputComputeOutputHandler<DATASERIN,DATACOMPUTE,DATASEROUT>,
			   GenericMirrorNewConnectionHandler, 
                           SerializeableRequestResponseHandler<CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>,
			   GenericMirrorNewRequestResponseConnectionHandler> {


public:
  GenericSerializeableInputComputeOutputWithRequestResponseControlMirror() : 
    SerializeableMirror<DATASERIN, SerializeableMirrorInputComputeOutputHandler<DATASERIN,DATACOMPUTE,DATASEROUT>,
    GenericMirrorNewConnectionHandler, SerializeableRequestResponseHandler<CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>,
    GenericMirrorNewRequestResponseConnectionHandler>() {}
  GenericSerializeableInputComputeOutputWithRequestResponseControlMirror(const GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATASERIN,DATACOMPUTE,DATASEROUT,CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT> &rhs) : 
    SerializeableMirror<DATASERIN, SerializeableMirrorInputComputeOutputHandler<DATASERIN,DATACOMPUTE,DATASEROUT>,
    GenericMirrorNewConnectionHandler, SerializeableRequestResponseHandler<CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>,
    GenericMirrorNewRequestResponseConnectionHandler>(rhs) {}
  virtual ~GenericSerializeableInputComputeOutputWithRequestResponseControlMirror() {}
  virtual GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATASERIN,DATACOMPUTE,DATASEROUT,CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT> & operator=(const GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATASERIN,DATACOMPUTE,DATASEROUT,CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT> &rhs) {
    this->~GenericSerializeableInputComputeOutputWithRequestResponseControlMirror();
    return *(new(this)GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATASERIN,DATACOMPUTE,DATASEROUT,CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>(rhs));
  }
  ostream &operator<<(ostream &os) const {
    os << "GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<";
    os <<typeid(DATASERIN).name()<<","<< typeid(DATACOMPUTE).name()<<","<<typeid(DATASEROUT).name()<<",";
    os <<typeid(CTRLSERIN).name()<<","<< typeid(CTRLCOMPUTE).name()<<","<<typeid(CTRLSEROUT).name()<<">(";
    SerializeableMirror<DATASERIN, SerializeableMirrorInputComputeOutputHandler<DATASERIN,DATACOMPUTE,DATASEROUT>,
    GenericMirrorNewConnectionHandler, SerializeableRequestResponseHandler<CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT>,
    GenericMirrorNewRequestResponseConnectionHandler>::operator<<(os);
    os << ")";
    return os;
  }
};

template <class DATASERIN, class DATACOMPUTE, class DATASEROUT, class CTRLSERIN, class CTRLCOMPUTE, class CTRLSEROUT> 
inline ostream &operator<<(ostream &os, const GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATASERIN,DATACOMPUTE,DATASEROUT,CTRLSERIN,CTRLCOMPUTE,CTRLSEROUT> &rhs) {
  return rhs.operator<<(os);
};
  
template <class SERIALIZEABLEINFO>
class GenericSerializeableMirror : 
public GenericSerializeableInputComputeOutputMirror<SERIALIZEABLEINFO,
         NullCompute<SERIALIZEABLEINFO,SERIALIZEABLEINFO>,
         SERIALIZEABLEINFO> {
public:
  GenericSerializeableMirror() : GenericSerializeableInputComputeOutputMirror<SERIALIZEABLEINFO, NullCompute<SERIALIZEABLEINFO,SERIALIZEABLEINFO>, SERIALIZEABLEINFO>() {}
  GenericSerializeableMirror(const GenericSerializeableMirror<SERIALIZEABLEINFO> &rhs) : GenericSerializeableInputComputeOutputMirror<SERIALIZEABLEINFO, NullCompute<SERIALIZEABLEINFO,SERIALIZEABLEINFO>, SERIALIZEABLEINFO>(rhs) {}
  virtual ~GenericSerializeableMirror() {}
  virtual GenericSerializeableMirror<SERIALIZEABLEINFO> & operator= (const GenericSerializeableMirror<SERIALIZEABLEINFO> &rhs) {
    this->~GenericSerializeableMirror();
    return *(new(this) GenericSerializeableMirror<SERIALIZEABLEINFO>(rhs));
  }

  ostream &operator<<(ostream &os) const {
    os << "GenericSerializeableMirror<"<<typeid(SERIALIZEABLEINFO).name()<<">(";
    GenericSerializeableInputComputeOutputMirror<SERIALIZEABLEINFO, NullCompute<SERIALIZEABLEINFO,SERIALIZEABLEINFO>, SERIALIZEABLEINFO>::operator<<(os);
    os << ")";
    return os;
  }

};

template <class SERIALIZEABLEINFO> 
inline ostream & operator<<(ostream &os, const GenericSerializeableMirror<SERIALIZEABLEINFO> &rhs) {
  return rhs.operator<<(os);
};

template <class DATAIN, class DATACOMPUTE, class DATAOUT, 
          class CTRLIN, class CTRLCOMPUTE, class CTRLOUT> 
class FilterWithControl :
public 
GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<
DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT
>
{
public:
  FilterWithControl() : 
    GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATAIN, DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT>() {}
  FilterWithControl(const FilterWithControl<DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT> &rhs) : 
    GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATAIN, DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT>(rhs) {}
  virtual ~FilterWithControl() {}
  virtual FilterWithControl<DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT> & operator=(const FilterWithControl<DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT> &rhs) {
    this->~FilterWithControl();
    return *(new(this)FilterWithControl<DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT>(rhs));
  }
    
  ostream &operator<<(ostream &os) const {
    os << "FilterWithControl<"
       <<typeid(DATAIN).name()<<","<< typeid(DATACOMPUTE).name()<<","<<typeid(DATAOUT).name()<<","
       <<typeid(CTRLIN).name()<<","<< typeid(CTRLCOMPUTE).name()<<","<<typeid(CTRLOUT).name()<<">(";
    GenericSerializeableInputComputeOutputWithRequestResponseControlMirror<DATAIN, DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT>::operator<<(os);
    os << ")";
    return os;
  }
};

template <class DATAIN, class DATACOMPUTE, class DATAOUT, class CTRLIN, class CTRLCOMPUTE, class CTRLOUT>
inline ostream & operator<<(ostream &os, const FilterWithControl<DATAIN,DATACOMPUTE,DATAOUT,CTRLIN,CTRLCOMPUTE,CTRLOUT> &rhs) {
  return rhs.operator<<(os);
};





#endif
