#include "socks.h"


#include <errno.h>
#include <iostream.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>

#include "Selector.h"
#include "socks.h"
#include "junk.h"

// #include "glarp.h"


Handler::Handler() 
{ 
  myfd = INVALID_FD; 
  mywait = MAXTIME;
  mask = HANDLE_NONE;
  tag=0;
}

Handler::Handler(const Handler &right)
{
  mask=right.mask;
  myfd=right.myfd;
  mywait=right.mywait;
  mylast=right.mylast;
  tag=right.tag;
}

Handler::~Handler() 
{
  myfd = INVALID_FD; 
  mywait = MAXTIME;
  mask = HANDLE_NONE;
  tag=0;
}

Handler & Handler::operator = (const Handler &right)
{
  mask=right.mask;
  myfd=right.myfd;
  mywait=right.mywait;
  mylast=right.mylast;
  tag=right.tag;
  return *this;
}
  


int Handler::GetFD() const { return myfd;}
void Handler::SetFD(const int fd) { myfd=fd; }

int Handler::GetTag() const { return tag;}

const TimeValue & Handler::GetWait() const { return mywait;}
void Handler::SetWait(const TimeValue &wait) {
  mywait=wait;
}

const TimeValue & Handler::GetLast() const { return mylast;}
void Handler::SetLast(const TimeValue &last) { 
  mylast=last;
}

void Handler::SetMask(const int mask) { this->mask=mask; } 
int  Handler::GetMask()const { return mask; }

bool Handler::HandlesRead() const { return GetMask() & HANDLE_READ;}
bool Handler::HandlesWrite() const { return GetMask() & HANDLE_WRITE;}
bool Handler::HandlesTimeout() const { return GetMask() & HANDLE_TIMEOUT;}
bool Handler::HandlesException() const { return GetMask() & HANDLE_EXCEPTION;}
void Handler::SetHandlesRead()  { SetMask(GetMask()|HANDLE_READ);}
void Handler::SetHandlesWrite()  { SetMask(GetMask()|HANDLE_WRITE);}
void Handler::SetHandlesTimeout()  { SetMask(GetMask()|HANDLE_TIMEOUT);}
void Handler::SetHandlesException()  { SetMask(GetMask()|HANDLE_EXCEPTION);}
void Handler::ClearHandlesRead()  { SetMask(GetMask()&~HANDLE_READ);}
void Handler::ClearHandlesWrite()  { SetMask(GetMask()&~HANDLE_WRITE);}
void Handler::ClearHandlesTimeout()  { SetMask(GetMask()&~HANDLE_TIMEOUT);}
void Handler::ClearHandlesException()  { SetMask(GetMask()&~HANDLE_EXCEPTION);}


int HandlerCompare::Compare(Handler &left, Handler &right) 
{
  return !((left.GetFD() == right.GetFD()) &&
	   ((left.GetWait())==(right.GetWait())) &&
	   ((left.GetTag())==(right.GetTag())));
}

int HandlerCompare::Compare(Handler *left, Handler *right) 
{
  return Compare(*left,*right);
}


Selector::Selector() : running(false) 
{}

Selector::~Selector()
{
  running=false;
  Fixup();

  Handler *h;
  while ((h=handlers.RemoveFromFront())) {
    delete h;
  }
}


Selector::Selector(const Selector &right) : running(false)
{
  DupeQueue((Queue<Handler> &)handlers,(const Queue<Handler> &)(right.handlers));
}


Selector & Selector::operator = (const Selector & right) 
{ 
  DupeQueue((Queue<Handler> &)handlers,(const Queue<Handler> &)(right.handlers));
  running=false;
  return *this;
}

extern "C" void JammedIgnore(int signum) {
  cerr << "Caught and ignored signal "<<signum<<"\n";
}

void PrintFDSet(fd_set &fds)
{
  int i;

  fprintf(stderr,"Set:");
  for (i=0;i<FD_SETSIZE;i++) {
    if (FD_ISSET(i,&fds)) { 
      fprintf(stderr," %d",i);
    }
  }
  fprintf(stderr,"\n");
}

int Selector::Run()
{
  fd_set read_fds;
  fd_set write_fds;
  fd_set except_fds;
  Handler *h;
  int maxfd=-1;
  int rc;
  TimeValue waittime, curtime,temp;
  struct timeval tv;
  
#if !defined(WIN32) || defined(__CYGWIN__)
  rc=IgnoreSignal(SIGPIPE);
#endif
  //rc = SetSignalHandler(SIGPIPE,&JammedIgnore);
  //rc=signal(SIGPIPE,&JammedIgnore);   
  // handlers will have to deal with
  // broken connections
  running=true;
  while (handlers.First()) {  // Fall through when no handlers are available.
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    maxfd=-1;
    waittime=MAXTIME;
    
    curtime.GetCurrentTime();
    
    FOREACH_BEGIN(h,handlers) {
      if (h->GetFD() != INVALID_FD) {
	if (h->HandlesRead()) {
	  FD_SET(h->GetFD(),&read_fds);
	}
	if (h->HandlesWrite()) {
	  FD_SET(h->GetFD(),&write_fds);
	}
	if (h->HandlesException()) {
	  FD_SET(h->GetFD(),&except_fds);
	}
	maxfd=MAX(maxfd,h->GetFD());
      }
      if (h->HandlesTimeout() && h->GetWait() < MAXTIME) {
	temp = h->GetLast();	
	temp += h->GetWait();	
	if (temp <= curtime) {	
	  waittime=MINTIME;	
	} else {	
	  temp -= curtime;
	  waittime=MIN(waittime,temp);
	}
      }
    } FOREACH_END(handlers)

    waittime.MakeTimeval(&tv);
    

#if 0
    fprintf(stderr,"maxfd+1=%d\n",maxfd+1);
    PrintFDSet(read_fds);
    PrintFDSet(write_fds);
    PrintFDSet(except_fds);
    fprintf(stderr,"waittime=%d.%d\n",tv.tv_sec,tv.tv_usec);
#endif
    rc=select(maxfd+1,&read_fds,&write_fds,&except_fds,waittime==MAXTIME ? 0: &tv);
#if 0
    if (rc<0) { 
      perror("select");
    }
#endif

    running=true;
    
    if (rc<0) {
      switch (errno) {
      case EBADF:
	cerr<<"Bad file descriptor in Selector::Run()\n";
	return -1;
	break;
      case EINVAL:
	cerr<<"Invalid parameter in Selector::Run()\n";
	return -1;
	break;
      case EINTR:;
	cerr<<"Interupted in Selector::Run()\n";
	break;
      default:
	break;
      }
    } else if (rc==0) {
      // time expired, run everyone that needs running
      FOREACH_BEGIN(h,handlers) {
	if (h->GetWait() != MAXTIME) {
	  curtime.GetCurrentTime();
	  temp = h->GetLast();
	  temp += h->GetWait();
	  if (curtime >= temp) {
	    if (h->HandlesTimeout()) {
	      h->HandleTimeout(*this);
	      h->SetLast(curtime);
	    }
	  }
	}
      } FOREACH_END(handlers)
    } else if (rc>0) {
      // First, exceptions
      FOREACH_BEGIN(h,handlers) {
	// Only makes sense if the handler is not being deleted
	if (!(deletequeue.IsInQueue(h))) {
	  if (h->GetFD() != INVALID_FD) {
	    if (FD_ISSET(h->GetFD(),&except_fds)) {	
	      if (h->HandlesException()) {
		curtime.GetCurrentTime();
		h->HandleException(h->GetFD(),*this);
		h->SetLast(curtime);
	      } else {
		// delete it
		CLOSE(h->GetFD());
		RemoveHandler(h);
	      }	
	    }	
	  }
	}
      } FOREACH_END(handlers)
      // reads
      FOREACH_BEGIN(h,handlers) {
	if (!(deletequeue.IsInQueue(h))) {
	  if (h->GetFD() != INVALID_FD) {
	    if (FD_ISSET(h->GetFD(),&read_fds)) {
	      if (h->HandlesRead()) {
		curtime.GetCurrentTime();
		h->HandleRead(h->GetFD(),*this);
		h->SetLast(curtime);
	      } 
	    }
	  }
	}
      } FOREACH_END(handlers)
      // writes
      FOREACH_BEGIN(h,handlers) {
	if (!(deletequeue.IsInQueue(h))) {
	  if (h->GetFD() != INVALID_FD) {
	    if (FD_ISSET(h->GetFD(),&write_fds)) {
	      if (h->HandlesWrite()) {
		curtime.GetCurrentTime();
		h->HandleWrite(h->GetFD(),*this);
		h->SetLast(curtime);
	      }
	    }
	  }
	}
      } FOREACH_END(handlers)
    } 
    running=false;
    Fixup();
    running=true;

  }
#if !defined(WIN32) || defined(__CYGWIN__)
  ListenToSignal(SIGPIPE);
#endif
  return 0;
}


void Selector::Fixup()
{
  assert(!running);

  Handler *h;
  while ((h=deletequeue.RemoveFromFront())) {
    delete RemoveHandler(h);
  }
  while ((h=addqueue.RemoveFromFront())) {
    AddHandler(h);
  }
}

int Selector::AddHandler(Handler *h) 
{
  if (running) {
    addqueue.AddAtBack(h);
  } else {
    handlers.AddAtBack(h);
  }
  return 0;
}



//
//  This should be cleaned up considerably so that
//  Remove semantics are a little bit more reasonable.
//

Handler *Selector::RemoveHandler(Handler *h)
{
  Handler *fh, *ret;

  if (running) {
    fh=handlers.Find(h);
    if (fh) {
      ret = fh->Clone();
      deletequeue.AddAtBack(fh);
    } else {
      ret=0;
    }
    return ret;
  } else {
    return handlers.FindRemove(h);
  }
}
      
Handler *Selector::RemoveHandler(int fd) 
{
  Handler *h;
  
  FOREACH_BEGIN(h,handlers) {
    if (h->GetFD()==fd) {
      break;
    }
  } FOREACH_END(handlers)
  if (h!=0) {
    return RemoveHandler(h);
  } else {
    return 0;
  }
}


//XXX
Queue<Handler> *Selector::FindMatchingHandlers(int fd)
{
  Queue<Handler> *hq = new Queue<Handler>;
  Handler *h;
  FOREACH_BEGIN(h,handlers) {
    if (h->GetFD()==fd) {
      hq->AddAtBack(h);
    }
  } FOREACH_END(handlers)
  return hq;
}
