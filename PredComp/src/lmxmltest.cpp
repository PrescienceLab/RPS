#include "PredComp.h"

main()
{
  LoadMeasurement lm;

  lm.ipaddress=ntohl(GetMyIPAddress());
  lm.timestamp=TimeStamp(0);
  lm.period_usec=1000000;
  lm.smoothingtype=SMOOTH_MACH;
  lm.unsmoothed=0.3;
  lm.avgs[0]=.2;
  lm.avgs[1]=.1;
  lm.avgs[2]=.05;

  lm.Print();

  Buffer b;
  

  b.Resize(lm.GetMaxXMLPackedSize());
  lm.PackToXML(b);
  
  fprintf(stdout, "packed buffer is:\n%s\n",b.Data());

  LoadMeasurement lm2;

  fprintf(stderr,"Begin parse...");
  lm2.UnpackFromXML(b);
  fprintf(stderr,"Done\n");

  lm2.Print();

#define NUM 1000

  TimeStamp start, end;
  Buffer buf;
  int i;

  start = TimeStamp(0);
  for (i=0;i<NUM;i++) {
    buf.Resize(lm.GetPackedSize());
    buf.ResetWrite();
    lm.Pack(buf);
  }      
  end = TimeStamp(0);
  fprintf(stdout, "%lf secs/Pack\n",((double)end-(double)start)/NUM);

  start = TimeStamp(0);
  for (i=0;i<NUM;i++) {
    buf.ResetRead();
    lm.Unpack(buf);
  }      
  end = TimeStamp(0);
  fprintf(stdout, "%lf secs/Unpack\n",((double)end-(double)start)/NUM);


  start = TimeStamp(0);
  for (i=0;i<NUM;i++) {
    buf.Resize(lm.GetMaxXMLPackedSize());
    buf.ResetWrite();
    lm.PackToXML(buf);
  }      
  end = TimeStamp(0);
  fprintf(stdout, "%lf secs/PackToXML\n",((double)end-(double)start)/NUM);

  start = TimeStamp(0);
  for (i=0;i<NUM;i++) {
    buf.ResetRead();
    lm.UnpackFromXML(buf);
  }      
  end = TimeStamp(0);
  fprintf(stdout, "%lf secs/UnpackFromXML\n",((double)end-(double)start)/NUM);


}
