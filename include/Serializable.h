#ifndef _Serializeable
#define _Serializeable

#define CHECKING_DEFAULT false
#define DEBUG 0


#define MAKE_PACK_UNPACK_PROTO(T)	\
  void Pack(const T *x, int num=1);	\
  void Unpack(T *x, int num=1);         \
  void Pack(const T &x); 		\
  void Unpack(T &x); 

class Buffer {
 private:
  bool typecheck;
  int  curwrite;
  int  curread;
  int  datalen;
  char *data;


 public:

  Buffer(bool check=CHECKING_DEFAULT);
  Buffer(const Buffer &right);
  virtual ~Buffer();

  virtual Buffer & operator = (const Buffer & right);

  void ResetRead();
  void ResetWrite();
  
  MAKE_PACK_UNPACK_PROTO(bool)
  MAKE_PACK_UNPACK_PROTO(char)
  MAKE_PACK_UNPACK_PROTO(unsigned char)
  MAKE_PACK_UNPACK_PROTO(short)
  MAKE_PACK_UNPACK_PROTO(unsigned short);
  MAKE_PACK_UNPACK_PROTO(int);
  MAKE_PACK_UNPACK_PROTO(unsigned int);
  MAKE_PACK_UNPACK_PROTO(long);
  MAKE_PACK_UNPACK_PROTO(unsigned long);
  MAKE_PACK_UNPACK_PROTO(float);
  MAKE_PACK_UNPACK_PROTO(double);


  char * Data() const;
  int Size() const ;

  void Resize(int len, bool copy=true);
  void ResizeFor(unsigned typesize, int num, bool copy=true);
};

// Baseclass for information that can be shipped.
// Subclasses should pack their data into buffer using

class SerializeableInfo {
 private:
#ifdef WIN32
  static int BOUNDARY;
#else
  static const int BOUNDARY=0xdeadbeef;
#endif
 public:
  // default and default copy constructor ok
  virtual ~SerializeableInfo();
  virtual SerializeableInfo & operator = (const SerializeableInfo &right);

  // User must override these four

  virtual int GetPackedSize() const =0;
  virtual int GetMaxPackedSize() const = 0;
  virtual int Pack(Buffer &buf) const =0;
  virtual int Unpack(Buffer &buf)=0 ;

  virtual int Serialize(Buffer &buf) const;
  virtual int Unserialize(Buffer &buf, int boundary=0, int len=0) ;
  virtual int Serialize(const int fd) const ;
  virtual int Unserialize(const int fd) ;
};
#endif
