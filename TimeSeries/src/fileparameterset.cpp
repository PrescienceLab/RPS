#include <new>
#include "fileparameterset.h"

FileParameterSet::FileParameterSet() 
{}

FileParameterSet::FileParameterSet(const FileParameterSet &rhs) : filename(rhs.filename)
{}

FileParameterSet::FileParameterSet(const string &n) : filename(n)
{}

FileParameterSet::FileParameterSet(const char *n) : filename(n)
{}

FileParameterSet::~FileParameterSet() 
{}

FileParameterSet & FileParameterSet::operator=(const FileParameterSet &rhs)
{
  this->~FileParameterSet();
  return *(new (this) FileParameterSet(rhs));
}

void FileParameterSet::Get(string &s) const 
{
  s=filename;
}

void FileParameterSet::Set(const string &s) 
{
  filename=s;
}

ParameterSet *FileParameterSet::Clone() const 
{ 
  return new FileParameterSet(*this);
}

ParameterSetType FileParameterSet::GetType() const 
{
  return File;
}

void FileParameterSet::Print(FILE *out) const 
{
  fprintf(out,"FileParameterSet: filename=%s\n",filename.c_str());
}

void FileParameterSet::Dump(FILE *out) const 
{
  Print(out);
}

ostream & FileParameterSet::operator<<(ostream &os) const 
{
  os << "FileParameterSet(filename="<<filename<<")";
  return os;
}

