#include "LineGraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Buffer.h"
#include "EndPoint.h" 
#include "Reference.h"
#include "Measurement.h"
#include "LoadMeasurement.h"
#include "PredictionRequestResponse.h"
#include "glarp.h" 


#define DEBUG_PRINT 0
#define DEBUG_PRINT_START if (DEBUG_PRINT) {
#define DEBUG_PRINT_END   }

#define DPS DEBUG_PRINT_START
#define DPE DEBUG_PRINT_END

#define MIN(x,y) ((x)<(y) ? (x) : (y))



JNIEXPORT void JNICALL Java_LineGraph_callLoadBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr, jint num, jstring host) {
  
  DPS fprintf(stderr,"Hi, in callLoadBufferClient\n"); DPE
  // Acquire endpoint string
  const char* endpointstr = env->GetStringUTFChars(host, 0);

  DPS fprintf(stderr,"EndPoint: %s\n",endpointstr); DPE

  EndPoint ep;						     

  if (ep.Parse(endpointstr) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"Invalid Endpoint\n");
    return;
  }

  env->ReleaseStringUTFChars(host, endpointstr);


  Reference<BufferDataRequest,BufferDataReply<LoadMeasurement> > ref;

  BufferDataRequest req;
  BufferDataReply<LoadMeasurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"loadbufferclient: Can't Connect!!\n");
    return;
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"loadbufferclient: Call failed\n");
    ref.Disconnect();
    return;
  }

  ref.Disconnect();

  long int i;

  DPS
  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stderr);
  }
  DPE

  // Trasfer data from the jdouble array to the jdoubleArray
    for (i=0;i<MIN(repl.num,num);i++) {
      env->SetDoubleArrayRegion(jdArr,i,1,&(repl.data[i].avgs[0]));
    }

  return;
}

JNIEXPORT void JNICALL Java_LineGraph_callPredBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr1, jdoubleArray jdArr2, jint num, jstring host) {
  
  
  DPS fprintf(stderr,"Hi, in callPredBufferClient\n"); DPE
  // Acquire endpoint string
  const char* endpointstr = env->GetStringUTFChars(host, 0);
  DPS fprintf(stderr,"EndPoint: %s\n",endpointstr); DPE

  EndPoint ep;  

  if (ep.Parse(endpointstr) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"Invalid Endpoint\n");
    return;
  }

  env->ReleaseStringUTFChars(host, endpointstr);

  Reference<BufferDataRequest,BufferDataReply<PredictionResponse> > ref;
  
  BufferDataRequest req;
  BufferDataReply<PredictionResponse> repl;
        
  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"predbufferclient: Can't Connect!!\n");
    return;
  }

  req.num=1;
  
  if (ref.Call(req,repl)) { 
    fprintf(stderr,"predbufferclient: Call failed\n");
    ref.Disconnect();
    return;
  }

  ref.Disconnect();
  
  DPS
  int i;
  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stderr);
  }
  DPE

  
  // Trasfer data from the jdouble arrays to the jdoubleArrays
  env->SetDoubleArrayRegion(jdArr1,0,MIN(num,repl.data[0].numsteps),repl.data[0].preds);
  env->SetDoubleArrayRegion(jdArr2,0,MIN(num,repl.data[0].numsteps),repl.data[0].errs);

  return;
}

JNIEXPORT void JNICALL Java_LineGraph_callMeasureBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr, jint num, jstring host) {
    
  
  DPS fprintf(stderr,"Hi, in callMeasureBufferClient\n"); DPE
  // Acquire endpoint string
  const char* endpointstr = env->GetStringUTFChars(host, 0);

  DPS fprintf(stderr,"EndPoint: %s\n",endpointstr); DPE

  EndPoint ep;  

  if (ep.Parse(endpointstr) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"Invalid Endpoint\n");
    return;
  }

  env->ReleaseStringUTFChars(host, endpointstr);

  Reference<BufferDataRequest,BufferDataReply<Measurement> > ref;

  BufferDataRequest req;
  BufferDataReply<Measurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"measurebufferclient: Can't Connect!!\n");
    return;
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"measurebufferclient: Call failed\n");
    return;
  } 

  ref.Disconnect();

  DPS
  int i;
  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stderr);
  }
  DPE

  // Trasfer data from the jdouble array to the jdoubleArray

    for (int i=0, total=0;i<MIN(num,repl.num)&&total<MIN(num,repl.num);total+=repl.data[i].serlen,i++) { 
      env->SetDoubleArrayRegion(jdArr,total,repl.data[i].serlen,repl.data[i].series);
    }

  return;
}
