#ifndef _EndPoint
#define _EndPoint

// source:udp:host:port
// source:tcp:host:port
// source:file:stdin|stdout|stderr|file
// and same for target...
// and 
// connect:udp:port
// connect:tcp:port

struct EndPoint {
  enum EndpointType { EP_UNKNOWN, EP_SOURCE, EP_CONNECT, EP_TARGET, EP_SERVER };
  enum EndpointCommType { COMM_UNKNOWN, COMM_TCP, COMM_UDP, COMM_UNIXDOM, COMM_FILE, COMM_STDIO};

  EndpointType  atype;
  EndpointCommType ctype;
  unsigned adx;
  unsigned port;
  FILE     *file;
  char     *pathname;
  
  EndPoint();
  EndPoint(const EndPoint &right);
  virtual ~EndPoint();
  EndPoint & operator = (const EndPoint &right);

  int Parse(const char *s);
};
  

#endif
