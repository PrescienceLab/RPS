#ifndef _xmlint
#define _xmlint

#include "Serializable.h"

class XmlInterface {
 public:
  virtual int GetXMLPackedSize() const = 0 ;
  virtual int GetMaxXMLPackedSize() const = 0;
  virtual int PackToXML(Buffer &buf) const = 0;
  virtual int UnpackFromXML(Buffer &buf) = 0 ;
};


template <class T> 
class XMLBasedSerializeableInfo : public T, public SerializeableInfo {
 public:
  int GetPackedSize() const {
    return GetXMLPackedSize();
  }
  int GetMaxPackedSize() const {
    return GetXMLMaxPackedSize();
  }
  int Pack(Buffer &buf) const {
    return PackToXML(buf);
  }
  int Unpack(Buffer &buf) {
    return UnpackFromXML(buf);
  }
};

#endif
