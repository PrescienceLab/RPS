#include <stdio.h>
#include "socks.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "EndPoint.h"

#include "junk.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

EndPoint::EndPoint() 
{ 
  atype=EP_UNKNOWN;
  ctype=COMM_UNKNOWN;
  adx=0; port=0; file=0; pathname=0;
}

EndPoint::EndPoint(const EndPoint &right)
{
  memcpy(this,&right,sizeof(EndPoint)); // XXX
}
  
EndPoint::~EndPoint() 
{
  // if (file && file!=stdin && file!=stdout && file !=) { fclose(file);}}
  CHK_DEL_MAT(pathname);
}
  
EndPoint & EndPoint::operator = (const EndPoint &right) 
{
  memcpy(this,&right,sizeof(EndPoint)); // XX
  return *this;
}


int EndPoint::Parse(const char *s) 
{
  CHK_DEL_MAT(pathname);
  if (file) { 
    fclose(file);
  }
  if (s==0 || strlen(s)==0) {
    return -1;
  }
  char *temp = new char [ strlen(s)+1];
  char *tok;
  strcpy(temp,s);
  
  tok = strtok(temp,":");
  if (!tok) {
    goto FAIL;
  }
  switch (toupper(tok[0])) {
  case 'T':
    atype=EP_TARGET;
    break;
  case 'S':
    if (toupper(tok[1])=='O') {
      atype=EP_SOURCE;
    } else {
      atype=EP_SERVER;
    }
    break;
  case 'C':
    if (toupper(tok[1])=='L') {
      atype=EP_SOURCE;
    } else {
      atype=EP_CONNECT;
    }
    break;
  default:
    goto FAIL;
  }
  
  tok=strtok(0,":");
  if (!tok) {
    goto FAIL;
  }
  switch (toupper(tok[0])) {
  case 'U':
    if (toupper(tok[1])=='N') {
       ctype=COMM_UNIXDOM;
    } else {
       ctype=COMM_UDP;
    }
    break;
  case 'T':
    ctype=COMM_TCP;
    break;
  case 'F':
    ctype=COMM_FILE;
    if (atype==EP_CONNECT) {
      goto FAIL;
    }
    break;
  case 'S':
    ctype=COMM_STDIO;
    if (atype==EP_CONNECT) {
      goto FAIL;
    }
    break;
  default:
    goto FAIL;
  }
  
  
  if (ctype!=COMM_UNIXDOM && (atype==EP_CONNECT || atype==EP_SERVER)) {
    tok=strtok(0,":");
    if (!tok) {
      goto FAIL;
    }
    port=atoi(tok);
  } else {
    if (ctype!=COMM_STDIO && ctype!=COMM_FILE && ctype!=COMM_UNIXDOM) {
      
      tok=strtok(0,":");
      if (!tok) {
	goto FAIL;
      }
      adx=ToIPAddress(tok);
      
      tok=strtok(0,":");
      if (!tok) {
	goto FAIL;
      }
      port=atoi(tok);
    } else {
      tok=strtok(0,":");
      if (!tok) {
	goto FAIL;
      }
      pathname = new char [strlen(tok)+1];
      strcpy(pathname,tok);
      if (ctype==COMM_STDIO) {
	if (!strcasecmp(tok,"stdin")) {
	  if (atype!=EP_SOURCE) {
	    goto FAIL;
	  } else {
	    file=stdin;
	  }
	} else if (!strcasecmp(tok,"stdout")) {
	  if (atype!=EP_TARGET) {
	    goto FAIL;
	  } else {
	    file=stdout;
	  }
	} else if (!strcasecmp(tok,"stderr")) {
	  if (atype!=EP_TARGET) {
	    goto FAIL;
	  } else {
	    file=stderr;
	  }
	} else {
	  if (atype==EP_TARGET) {
	    file = fopen(tok,"wb");
	    if (!file) {
	      goto FAIL;
	    }
	  } else if (atype==EP_SOURCE) { 
	    file = fopen(tok,"rb");
	    if (!file) {
	      goto FAIL;
	    }
	  } else {
	    goto FAIL;
	  }
	}
      }
    }
  }
  delete [] temp;
  
  return 0;
  
 FAIL:
  delete [] temp;
  *this = EndPoint();
  
  return -1;
}
