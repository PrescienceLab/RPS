#include <stdlib.h>
#include <iostream>
#include <vector>

#include "sample.h"
#include "waveletsample.h"

void usage()
{
  cerr << "testsamp\n";
}

int main(int argc, char *argv[])
{
  if (argc!=1) {
    usage();
    exit(-1);
  }

  Sample s1(0);
  s1.SetSampleValue(20);
  
  InputSample is1(0);
  is1.SetSampleValue(30);

  OutputSample os1(0);
  os1.SetSampleValue(40);

  WaveletInputSample wis1(0);
  wis1.SetSampleValue(50);

  WaveletOutputSample wos1(0);
  wos1.SetSampleValue(60);

  return 0;
}
