#include "LineGraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "Buffer.h"
#include "EndPoint.h" 
#include "Reference.h"
#include "Measurement.h"
#include "LoadMeasurement.h"
#include "PredictionRequestResponse.h"
#include "glarp.h" 

JNIEXPORT void JNICALL Java_LineGraph_callLoadBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr, jstring host) {
    
  EndPoint ep;  
  int i;       // Counter
  int num=1;   // I set this to return 1, normally is an argument
  char hostName[128] = ""; // The host name and path
  jboolean* hostBool;      // Necessary for GetStringUTFChars, not sure why
  
  // Acquire host name from input argument and build the path
  const char* str = env->GetStringUTFChars(host, hostBool);
  strcat(hostName, str);
  printf("\n%s",hostName);
  env->ReleaseStringUTFChars(host, str);
  if (ep.Parse(hostName) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"loadbufferclient: Failed to parse my input\n");
    exit(-1);
  }

  Reference<BufferDataRequest,BufferDataReply<LoadMeasurement> > ref;

  BufferDataRequest req;
  BufferDataReply<LoadMeasurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"loadbufferclient: Can't Connect!!\n");
    exit(-1);
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"loadbufferclient: Call failed\n");
    ref.Disconnect();
    exit(-1);
  }

  ref.Disconnect();

  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stderr);
  }

  // Display data on the C++ side
  printf("\nC++ data  ");
  for (i=0;i<3;i++) {
      printf("-%d-: %g  ",i+1,repl.data[0].avgs[i]);
  }
  printf("\n");

  // Construct a jdouble array in which to place the data
  jdouble retvalues[3];

  // Fill the array with the data from the system
  for (i=0;i<3;i++) {
      retvalues[i] = (jdouble)repl.data[0].avgs[i];
  }

  // Trasfer data from the jdouble array to the jdoubleArray
  env->SetDoubleArrayRegion(jdArr,0,3,retvalues);
  
  return;
}

JNIEXPORT void JNICALL Java_LineGraph_callPredBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr1, jdoubleArray jdArr2, jstring host) {
      
  EndPoint ep;  
  int i; // Counter
  int num=1; // I set this to return 1, normally is an argument
  char hostName[128] = ""; // The host name and path
  jboolean* hostBool; // Necessary for GetStringUTFChars, not sure why
  
  // Acquire host name from input and build the path
  const char* str = env->GetStringUTFChars(host, hostBool);
  strcat(hostName, str);
  printf("\n%s",hostName);
  env->ReleaseStringUTFChars(host, str);
  if (ep.Parse(hostName) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"predbufferclient: Failed to parse my input\n");
    exit(-1);
  }
  
  Reference<BufferDataRequest,BufferDataReply<PredictionResponse> > ref;
  
  BufferDataRequest req;
  BufferDataReply<PredictionResponse> repl;
        
  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"predbufferclient: Can't Connect!!\n");
    exit(-1);
  }

  req.num=num;
  
  if (ref.Call(req,repl)) { 
    fprintf(stderr,"predbufferclient: Call failed\n");
    ref.Disconnect();
    exit(-1);
  }

  ref.Disconnect();
  
  /*
  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stdout);
  }
  */
  
  // Display 10 values data on the C++ side
  printf("\nC++ data  ");
  for (i=0;i<3;i++) {
    printf("-%d-: %g  ",i+1,repl.data[0].preds[i]);
  }
  printf("\n");
  
  // Construct jdouble arrays in which to place the data
  jdouble retvalues1[180];
  jdouble retvalues2[180];
  
  // Fill the array with the data from the system
  for (i=0;i<180;i++) {
    retvalues1[i] = (jdouble)repl.data[0].preds[i];
    retvalues2[i] = (jdouble)repl.data[0].errs[i];
  }
  
  // Trasfer data from the jdouble arrays to the jdoubleArrays
  env->SetDoubleArrayRegion(jdArr1,0,180,retvalues1);
  env->SetDoubleArrayRegion(jdArr2,0,180,retvalues2);

  return;
}

JNIEXPORT void JNICALL Java_LineGraph_callMeasureBufferClient(JNIEnv *env, jobject obj, jdoubleArray jdArr, jstring host) {
    
  EndPoint ep;  
  int i;       // Counter
  int num=1;   // I set this to return 1, normally is an argument
  char hostName[128] = ""; // The host name and path
  jboolean* hostBool;      // Necessary for GetStringUTFChars, not sure why
  
  // Acquire host name from input argument and build the path
  const char* str = env->GetStringUTFChars(host, hostBool);
  strcat(hostName, str);
  printf("\n%s",hostName);
  env->ReleaseStringUTFChars(host, str);
  if (ep.Parse(hostName) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"loadbufferclient: Failed to parse my input\n");
    exit(-1);
  }

  Reference<BufferDataRequest,BufferDataReply<Measurement> > ref;

  BufferDataRequest req;
  BufferDataReply<Measurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"measurebufferclient: Can't Connect!!\n");
    exit(-1);
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"measurebufferclient: Call failed\n");
  } else {
    for (i=0;i<repl.num;i++) { 
      repl.data[i].Print(stderr);
    }
  }

  ref.Disconnect();

  // Display data on the C++ side
  int valCount = repl.data[0].serlen;
  printf("Measurement of length: %i", valCount);

  // Construct a jdouble array in which to place the data
  jdouble retvalues[valCount];

  // Fill the array with the data from the system
  for (i=0;i<valCount;i++) {
      retvalues[i] = (jdouble)repl.data[0].series[i];
  }

  // Trasfer data from the jdouble array to the jdoubleArray
  env->SetDoubleArrayRegion(jdArr,0,valCount,retvalues);
  
  return;
}
