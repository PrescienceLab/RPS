#ifndef _Selector
#define _Selector

#include "Queues.h"
#include "Timers.h"

class Selector;




#define INVALID_FD -1

#define HANDLE_NONE      0
#define HANDLE_READ      1
#define HANDLE_WRITE     2
#define HANDLE_TIMEOUT   4
#define HANDLE_EXCEPTION 8

class Handler {
 protected:
  int tag;
  int mask;
  int myfd;
  TimeValue mywait;
  TimeValue mylast;
 public:
  Handler();
  Handler(const Handler &right);
  virtual ~Handler();

  Handler & operator = (const Handler &right);

  virtual Handler *Clone()=0;
  virtual int HandleRead(int fd, Selector &s)=0;
  virtual int HandleWrite(int fd, Selector &s)=0;
  virtual int HandleException(int fd, Selector &s)=0;
  virtual int HandleTimeout(Selector &s)=0;

  int GetFD() const ;
  void SetFD(const int fd) ;

  int GetTag() const ;

  const TimeValue & GetWait() const ;
  void SetWait(const TimeValue &wait) ;

  const TimeValue & GetLast() const ;
  void SetLast(const TimeValue &last) ;

  void SetMask(const int mask) ;
  int  GetMask() const ;

  bool HandlesRead() const ;
  bool HandlesWrite() const ;
  bool HandlesTimeout() const ;
  bool HandlesException() const ;
  void SetHandlesRead()  ;
  void SetHandlesWrite()  ;
  void SetHandlesTimeout()  ;
  void SetHandlesException() ;
  void ClearHandlesRead()  ;
  void ClearHandlesWrite()  ;
  void ClearHandlesTimeout()  ;
  void ClearHandlesException() ;
};


class HandlerCompare {
 public:
  static int Compare(Handler &left, Handler &right) ;
  static int Compare(Handler *left, Handler *right) ;
};


class Selector {
 private:
  bool running;
  SearchableQueue<Handler,HandlerCompare> handlers;
  SearchableQueue<Handler,HandlerCompare> deletequeue;
  SearchableQueue<Handler,HandlerCompare> addqueue;
 protected:
  void Fixup();
 public:
  Selector();
  Selector(const Selector &right);
  virtual ~Selector();

  Selector & operator = (const Selector &right);

  virtual int AddHandler(Handler *h);
  virtual Handler *RemoveHandler(Handler *h);
  virtual Handler *RemoveHandler(int fd);
  Queue<Handler> *FindMatchingHandlers(int fd);
  virtual int Run();
};


#endif
