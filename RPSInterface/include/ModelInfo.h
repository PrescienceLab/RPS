#ifndef _ModelInfo
#define _ModelInfo

#include "fit.h"
#include "abstract.h"
#include "shared.h"
#include "Serializable.h"
#include <stdio.h>


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

#if 0
  static void MakePDQModel(ModelInfo &mi, 
			   const ModelType modelclass, 
			   const unsigned modelattr,
			   const int p, 
			   const int d, 
			   const int q);

  static void GetPDQModel(const ModelInfo &mi,
			  ModelType &modelclass, 
			  unsigned &modelattr,
			  int &p, 
			  int &d, 
			  int &q);

  static void MakeRefittingPDQModel(ModelInfo &mi, 
				    const ModelType modelclass, 
				    const unsigned modelattr,
				    const int p, 
				    const int d, 
				    const int q,
				    const int refitint);

  static void GetRefittingPDQModel(const ModelInfo &mi,
				   ModelType &modelclass, 
				   unsigned &modelattr,
				   int &p, 
				   int &d, 
				   int &q,
				   int &refitint);

  static void MakeModelInfo(ModelInfo &mi, const ModelTemplate &mt);
  static void MakeModelTemplate(const ModelInfo &mi, ModelTemplate &mt);
#endif
};

#endif

