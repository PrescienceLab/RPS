#ifndef _Buffer
#define _Buffer

#include "MirrorTemplate.h"
#include "Queues.h"
#include "TimeStamp.h"


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
   BufferingSerializeableMirror() {
     numitems=0;
     bufdepth=0;
   }
   BufferingSerializeableMirror(const BufferingSerializeableMirror<SERIN> &rhs) {
     numitems=rhs.numitems;
     bufdepth=rhs.bufdepth;
     for (SERIN *i=rhs.dataitems.First(); i!=0; i=rhs.dataitems.Next()) {
       dataitems.AddAtBack(new SERIN(*i));
     }
   }
   BufferingSerializeableMirror(unsigned bufdepth=DEFAULT_BUF_DEPTH) {
     numitems=0;
     this->bufdepth=bufdepth;
   }
   virtual ~BufferingSerializeableMirror() {
     dataitems.Clear();
     numitems=0;
   }
   BufferingSerializeableMirror & operator=(const BufferingSerializeableMirror<SERIN> &rhs) {
     this->~BufferingSerializableMirror();
     return *(new(this)BufferingSerializeableMirror<SERIN>(rhs));
   }

   virtual int Enqueue(const SERIN &obj) {
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

   virtual unsigned Request(SERIN *p, const unsigned n) {
     unsigned num = MIN(n,numitems);
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

   ostream & operator<<(ostream &os) const {
     os <<"BufferSerializeableMirror<"<<typeid(SERIN).name()<<">(numitems="<<numitems
	<<", bufdepth="<<bufdepth<<"dataitems=(";
     bool f=false;
     
     for (SERIN *i=const_cast<Queue<SERIN>&>(dataitems).First(); i!=0; i=const_cast<Queue<SERIN>&>(dataitems).Next(),f=true) { 
       if (!f) {
	 os<<", ";
       }
       os << *i;
     }
     os << "))";
     return os;
   }
};

template <class T>
inline ostream & operator<<(ostream &os, const BufferingSerializeableMirror<T> &rhs) 
{  return rhs.operator<<(os); };


struct BufferDataRequest : public SerializeableInfo {
  int num;

  BufferDataRequest() : SerializeableInfo(), num(0) {}
  BufferDataRequest(const BufferDataRequest &rhs) :
    SerializeableInfo(rhs), num(rhs.num) {}
  ~BufferDataRequest() {}
  BufferDataRequest & operator=(const BufferDataRequest &rhs) {
    this->~BufferDataRequest();
    return *(new(this)BufferDataRequest(rhs));
  }

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
  
  ostream & operator<<(ostream &os) const {
    os <<"BufferDataRequest(";
    SerializeableInfo::operator<<(os);
    os <<", num="<<num<<")";
    return os;
  }
};


inline ostream & operator<<(ostream &os, const BufferDataRequest &rhs) 
{  return rhs.operator<<(os); };


template <class SERINFO> 
struct BufferDataReply : public SerializeableInfo {
  TimeStamp timestamp;
  int num;
  SERINFO *data;

  BufferDataReply(const BufferDataReply &rhs) :
    SerializeableInfo(rhs), num(rhs.num) {
    data = new SERINFO[num];
    for (int i=0;i<num;i++) {
      data[i]=rhs.data[i];
    }
  }
  BufferDataReply & operator=(const BufferDataReply &rhs) {
    this->~BufferDataReply();
    return *(new(this)BufferDataReply<SERINFO>(rhs));
  }

  
  BufferDataReply(unsigned size=0)  : SerializeableInfo(), num(0), data(0) {
    num=size;
    data=0;
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

   ostream & operator<<(ostream &os) const {
     os <<"BufferDataReply<"<<typeid(SERINFO).name()<<">(timestamp="<<timestamp
	<<", num="<<num<<"data=(";
     for (int i=0; i<num; i++) {
       if (i>0) {
	 os<<", ";
       }
       os << data[i];
     }
     os << "))";
     return os;
   }

};

template <class SERINFO>
inline ostream & operator<<(ostream &os, const BufferDataReply<SERINFO> &rhs) 
{  return rhs.operator<<(os); };


template <class SERIN> 
class BufferDataRequestResponseHandler : public MirrorHandler {
 private:
  BufferingSerializeableMirror<SERIN>  *queuemirror;
  
 public:
  BufferDataRequestResponseHandler() : MirrorHandler() { assert(0); }
  BufferDataRequestResponseHandler(const BufferDataRequestResponseHandler<SERIN> &rhs) :
    MirrorHandler(rhs), queuemirror(rhs.queuemirror)
  {}
    
  BufferDataRequestResponseHandler(AbstractMirror *m) : MirrorHandler(m) {
    queuemirror = (BufferingSerializeableMirror<SERIN> *) m;
    SetHandlesRead();
  }

  virtual ~BufferDataRequestResponseHandler() {}
  
  Handler *Clone() const { 
    return new BufferDataRequestResponseHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
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
     os <<"BufferDataRequestResponseHandler<"<<typeid(SERIN).name()<<">(";
     MirrorHandler::operator<<(os);
     os <<", queuemirror="<<*queuemirror<<")";
     return os;
   }

};

template <class SERIN>
inline ostream & operator<<(ostream &os, const BufferDataRequestResponseHandler<SERIN> &rhs) 
{  return rhs.operator<<(os); };


#endif
