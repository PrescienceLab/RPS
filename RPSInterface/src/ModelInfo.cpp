#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "socks.h"

#include "ModelInfo.h"

#include "pdqparamsets.h"

#include "junk.h"
#include "fit.h"


ModelInfo::ModelInfo() : ModelTemplate()
{ 
}

ModelInfo::ModelInfo(const ModelTemplate &mt) : ModelTemplate(mt)
{
}
  

ModelInfo::ModelInfo(const ModelInfo &right) : ModelTemplate(right)
{}

ModelInfo::~ModelInfo() 
{ 
}


ModelInfo & ModelInfo::operator = (const ModelInfo &right) 
{
  *((ModelTemplate*)this) = (const ModelTemplate &)right;
  return *this;
}

ModelInfo *ModelInfo::Clone() const
{
  return new ModelInfo(*this);
}

int ModelInfo::GetPackedSize() const 
{
  return _GetPackedSize();
}

int ModelInfo::GetMaxPackedSize() const 
{
  return _GetMaxPackedSize();
}

class BufferByteStreamWrapper : public ByteStream {
private:
  Buffer &buf;
public:
  BufferByteStreamWrapper(Buffer &b) : buf(b) {}
  int Put(const char *b, const int len) { buf.Pack(b,len); return len;}
  int Get(char *b, const int len) { buf.Unpack(b,len); return len;}
};


int ModelInfo::Pack(Buffer &buf) const 
{
  BufferByteStreamWrapper bsw(buf);
  return _Pack(bsw);
}
  
int ModelInfo::Unpack(Buffer &buf) 
{
  BufferByteStreamWrapper bsw(buf);
  return _Unpack(bsw);
}

void ModelInfo::Print(FILE *out) const
{
  fprintf(out,"ModelInfo: ");
  ModelTemplate::Print(out);
}

ostream &ModelInfo::Print(ostream &os) const
{
  os<<"ModelInfo(";
  ModelTemplate::Print(os);
  os<<")";
  return os;
}


ostream &ModelInfo::operator<<(ostream &os) const
{
  return Print(os);
}

