#include "LineGraphApp.h"
#include <stdlib.h>
#include <stdio.h>
//#include "Buffer.h" //
//#include "EndPoint.h" //
//#include "Reference.h" //
//#include "LoadMeasurement.h" //
//#include "glarp.h" //

JNIEXPORT jint JNICALL Java_LineGraphApp_getNativeNumber (JNIEnv *env, jobject this) {
/*
  EndPoint ep;
  int i;

  int num=2; //I set this to return 2, normallt is an argument

  if (ep.Parse("client:tcp:skysaw:5001") || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"loadbufferclient: Failed to parse \"%s\"\n",argv[2]);
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
*/  
  return 0;
}
