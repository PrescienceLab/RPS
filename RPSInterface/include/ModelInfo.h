#ifndef _ModelInfo
#define _ModelInfo

#include "fit.h"
#include "abstract.h"
#include "shared.h"
#include "Serializable.h"
#include <stdio.h>
#include <iostream>


using namespace std;



struct ModelInfo : public ModelTemplate, public SerializeableInfo {
  ModelInfo();
  ModelInfo(const ModelTemplate &mt);
  ModelInfo(const ModelInfo &right);

  virtual ~ModelInfo();
  
  ModelInfo & operator = (const ModelInfo &mi) ;

  ModelInfo *Clone() const ;

  int GetPackedSize() const ; 
  int GetMaxPackedSize() const ;

  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const  ;
  ostream &Print(ostream &os) const  ;
  ostream & operator<<(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const ModelInfo &rhs) { return rhs.operator<<(os);}

#endif

