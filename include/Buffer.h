#ifndef _Buffer
#define _Buffer

#include "Mirror.h"
#include "Queues.h"
#include "TimeStamp.h"

#include "junk.h"

// Buffer a stream of data records and handle requests for some number of records



#define DEFAULT_BUF_DEPTH 1000
#define MAX_REPLY_DEPTH DEFAULT_BUF_DEPTH

template <class SERIN> class BufferInputHandler;

struct BufferDataRequest;
template <class SERIN> struct BufferDataReply;
template <class SERIN> class BufferDataRequestResponseHandler;

template <class SERIN>
class BufferingSerializeableMirror :  public SerializeableMirror<
                                         SERIN,
					 SerializeableMirrorInputHandler<SERIN>,
			                 GenericMirrorNewConnectionHandler, 
					 BufferDataRequestResponseHandler<SERIN>,
			                 GenericMirrorNewRequestResponseConnectionHandler> {

private:
   Queue<SERIN> dataitems;
   unsigned numitems;
   unsigned bufdepth;

public:
   BufferingSerializeableMirror(unsigned bufdepth=DEFAULT_BUF_DEPTH) {
     numitems=0;
     this->bufdepth=bufdepth;
   }

   virtual ~BufferingSerializeableMirror() {
     dataitems.Clear();
     numitems=0;
   }

   virtual int Enqueue(SERIN &obj) {
     SERIN *clone = new SERIN(obj);
     dataitems.AddAtFront(clone);
     numitems++;
     while (numitems>bufdepth) {
       delete dataitems.RemoveFromBack();
       numitems--;
     }
     return 0;
   }

   virtual int Enqueue(Buffer &buf) {
     SERIN in;
     in.Unserialize(buf);
     return Enqueue(in);
   }

   virtual unsigned Request(SERIN *p, unsigned num) {
     num = MIN(num,numitems);
     unsigned i;
     SERIN *x;

     for (i=0,x=dataitems.First();i<num && x!=0 ;i++, x=dataitems.Next()) {
       p[num-i-1] = *x;
     }
     return num;
   }


   virtual int Forward(Buffer &buf) {
     int rc;

     rc=Enqueue(buf);

     return SerializeableMirror<SERIN,
                            SerializeableMirrorInputHandler<SERIN>,
                            GenericMirrorNewConnectionHandler, 
                            BufferDataRequestResponseHandler<SERIN>,
                            GenericMirrorNewRequestResponseConnectionHandler>::Forward(buf);
   }
     
};



struct BufferDataRequest : public SerializeableInfo {
  int num;

  int GetPackedSize() const { return 4; }
  int GetMaxPackedSize() const { return 4;}
  int Pack(Buffer &buf) const {
    buf.Pack(num);
    return 0;
  }
  int Unpack(Buffer &buf) {
    buf.Unpack(num);
    return 0;
  }
};


template <class SERINFO> 
struct BufferDataReply : public SerializeableInfo {
  TimeStamp timestamp;
  int num;
  SERINFO *data;
  
  BufferDataReply(unsigned size=0) { 
    num=size;
    if (num>0) { 
      data=new SERINFO [num];
    }
  }
      
  virtual ~BufferDataReply() { 
    if (data) {
      delete [] data;
    }
    data=0;
  }

  int GetPackedSize() const { 
    int sum=timestamp.GetPackedSize()+4;
    int i;
    for (i=0;i<num;i++) { 
     sum+=data[i].GetPackedSize();
    }
    return sum;
  }

  int GetMaxPackedSize() const { 
    if (num==0) {
      return timestamp.GetMaxPackedSize()+4;
    } else {
      return timestamp.GetMaxPackedSize()+4 + num * data[0].GetMaxPackedSize();
    }
  }
  int Pack(Buffer &buf) const {
    timestamp.Pack(buf);
    buf.Pack(num);
    int i;
    for (i=0;i<num;i++) {
      data[i].Pack(buf);
    }
    return 0;
  }
  int Unpack(Buffer &buf) {
    timestamp.Unpack(buf);
    buf.Unpack(num);
    data = new SERINFO [num];
    int i;
    for (i=0;i<num;i++ ) {
      data[i].Unpack(buf);
    }
    return 0;
  }
};


template <class SERIN> 
class BufferDataRequestResponseHandler : public MirrorHandler {
 private:
  BufferingSerializeableMirror<SERIN>  *queuemirror;
  
 public:
  BufferDataRequestResponseHandler(AbstractMirror *m) : MirrorHandler(m) {
    queuemirror = (BufferingSerializeableMirror<SERIN> *) m;
    SetHandlesRead();
  }
  
  Handler *Clone() { 
    return new BufferDataRequestResponseHandler(*this);
  }
  int HandleRead(int fd, Selector &s) {
    BufferDataRequest in;
    if (in.Unserialize(fd)) {
      mymirror->DeleteMatching(fd);
      return -1;
    } else {
      BufferDataReply<SERIN> repl(in.num);
      unsigned numitems =  queuemirror->Request(repl.data,repl.num);
      repl.num = numitems;
      return repl.Serialize(fd);
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

   /*

template <class SERIN>
class BufferInputHandler : public MirrorHandler {
 public:
  BufferInputHandler(AbstractMirror *m) : MirrorHandler(m) {}
  
  int HandleRead(int fd, Selector &s) {
    int rc;
    SERIN in;
    if (in.Unserialize(fd)) {
      mymirror->DeleteMatching(fd);
      return -1;
    } else {
      Buffer buf;
      in.Serialize(buf);
      return mymirror->Forward(in)
    }
  }
};

   */


#endif
