#ifndef _fileparamsets
#define _fileparamsets

#include "abstract.h"

#include <string>

class FileParameterSet : public ParameterSet {
 protected:
  string filename;
 public:
  FileParameterSet();
  FileParameterSet(const FileParameterSet &rhs);
  FileParameterSet(const string &s);
  FileParameterSet(const char *s);
  ~FileParameterSet();

  FileParameterSet & operator=(const FileParameterSet &rhs);

  void Get(string &s) const;
  void Set(const string &s);
  ParameterSet *Clone() const; 
  ParameterSetType GetType() const;

  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const FileParameterSet &p) {
  return p.operator<<(os);
}
#endif
