#include <string.h>
#include <assert.h>
#include <iostream.h>

#include "Serializable.h"

#include "socks.h"

#include "junk.h"

#include "XDRpack.h"

#include "debug.h"


#ifdef WIN32
int SerializeableInfo::BOUNDARY=0xdeadbeef;
#endif


#if DEBUG
#define DEBUGPRT(func,type,num) fprintf(stderr,"%s(%s,%d)\n",func,STRINGIZE(type),num)
#define DEBUGPRTPACK(type,num) DEBUGPRT("Pack",type,num)
#define DEBUGPRTUNPACK(type,num) DEBUGPRT("Unpack",type,num)
#else
#define DEBUGPRT(func,type,num)
#define DEBUGPRTPACK(type,num) 
#define DEBUGPRTUNPACK(type,num) 
#endif


#define MAKE_PACK_UNPACK(T)				\
  void Buffer::Pack(const T *x, int num) {	\
    int i;						\
    DEBUGPRTPACK(T,num);				\
    ResizeFor(sizeof(T),num);				\
    for (i=0;i<num;i++) {				\
      ::Pack(data+curwrite,x[i]);			\
      curwrite+=sizeof(T);				\
    }							\
  }							\
  void Buffer::Unpack(T *x, int num) {		\
    int i;						\
    DEBUGPRTUNPACK(T,num);				\
    assert((curread+num*(int)sizeof(T))<=datalen);		\
    for (i=0;i<num;i++) {				\
      ::Unpack(data+curread,&(x[i]));			\
      curread+=sizeof(T);				\
    }							\
  }							\
  void Buffer::Pack(const T &x) {			\
    Pack(&x,1);						\
  }							\
  void Buffer::Unpack(T &x) {				\
    Unpack(&x,1);					\
  }



Buffer::Buffer(bool check) 
{ 
  datalen=0; curread=0; curwrite=0; data=0; typecheck=check;
}
 
Buffer::Buffer(const Buffer &right) {
  typecheck=right.typecheck;
  curwrite=right.curwrite;
  curread=right.curread;
  data=0;
  datalen=0;
  Resize(right.datalen,false);
  memcpy(this->data,right.data,right.datalen);
}

Buffer::~Buffer() 
{ 
  datalen=0; curread=0; curwrite=0; 
  CHK_DEL_MAT(data);
}

Buffer & Buffer::operator = (const Buffer & right) 
{
  typecheck=right.typecheck;
  curwrite=right.curwrite;
  curread=right.curread;
  CHK_DEL_MAT(data);
  datalen=0;
  Resize(right.datalen,false);
  memcpy(this->data,right.data,right.datalen);
  return *this;
}


void Buffer::ResetRead() 
{
  curread=0;
}

void Buffer::ResetWrite() 
{ 
  curwrite=0;
}
  
MAKE_PACK_UNPACK(bool)
MAKE_PACK_UNPACK(char)
MAKE_PACK_UNPACK(unsigned char)
MAKE_PACK_UNPACK(short)
MAKE_PACK_UNPACK(unsigned short)
MAKE_PACK_UNPACK(int)
MAKE_PACK_UNPACK(unsigned int)
MAKE_PACK_UNPACK(long)
MAKE_PACK_UNPACK(unsigned long)
MAKE_PACK_UNPACK(float)
MAKE_PACK_UNPACK(double)


char * Buffer::Data() const 
{
  return data;
}

int Buffer::Size() const 
{
  return curwrite;
}

void Buffer::Resize(int len, bool copy) 
{
  if (len>datalen) {
    char * newdata = new char [len];
    assert(newdata);
    if (data) {
      if (copy) {
	memcpy(newdata,data,MIN(len,curwrite));
      }
      delete [] data;
    }
    data = newdata;
    datalen = len;
  }
}

void Buffer::ResizeFor(unsigned typesize, int num, bool copy) 
{
  int needed = curwrite+typesize*num;
  Resize(needed,copy);
}


SerializeableInfo::~SerializeableInfo()
{}

SerializeableInfo & SerializeableInfo::operator = (const SerializeableInfo &right)
{
  return *this;                 // default behavior - no fields 
}

int SerializeableInfo::Serialize(Buffer &buf) const 
{
  int temp=BOUNDARY;
  buf.Pack(temp);
  temp = GetPackedSize();
  buf.Pack(temp);
  Pack(buf); 
  return 0; 
}


int SerializeableInfo::Unserialize(Buffer &buf, int boundary, int len) 
{
  if (!boundary) {
    buf.Unpack(&boundary,1);
  }
  if (!len) {
    buf.Unpack(&len,1);
  }
  assert(boundary==BOUNDARY); 
  return Unpack(buf);
}


int SerializeableInfo::Serialize(const int fd) const 
{
  int len;
  Buffer buf;
  len=Serialize(buf);
  if (len) { 
    return len;
  }
  if (IsDatagramSocket(fd)) {
    len=Send(fd,buf.Data(),buf.Size(),false) ;
  } else {
    len=Send(fd,buf.Data(),buf.Size());
  }
  if (len!=buf.Size()) {
    return -1;
  } else {
    return 0;
  }
}


int SerializeableInfo::Unserialize(const int fd) {
  int recvlen;
  int header[2];
  Buffer buf;
  if (IsDatagramSocket(fd)) {
    // Receive in one fell swoop
    int maxlen = 2*sizeof(int)+GetMaxPackedSize();
    buf.Resize(maxlen);
    recvlen = Receive(fd,buf.Data(),maxlen,false);
    if (recvlen<2*(int)sizeof(int)) { 
      return -1;
    } else {
      return Unserialize(buf);
    }
  } else {
    buf.Resize(2*sizeof(int));
    recvlen = Receive(fd,buf.Data(),2*sizeof(int));
    if (recvlen<2*(int)sizeof(int)) {
      return -1;
    }
    buf.Unpack(header,2);
    assert(header[0]==BOUNDARY);
    buf.Resize(header[1],false);
    buf.ResetRead();
    buf.ResetWrite();
    recvlen = Receive(fd,buf.Data(),header[1]);
    if (recvlen<header[1]) { 
      return -1;
    } else {
      return Unserialize(buf,header[0],header[1]);
    }
  }
}


