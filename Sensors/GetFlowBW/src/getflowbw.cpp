#include <stdio.h>
#include <stdlib.h>
#if HAVE_REMOS
extern "C" {
#include "remos.h"
#include "memoryPack.h"
	   }

#if 1
#define CHK_DEL(x) { if ((x)!=0) { free(x); } }
#else
#define CHK_DEL(x) 
#endif

void handler(Remos_Error err, Remos_Ptr ptr) {
  remos_print_errorinfo(err);
  abort();
}

Remos_Status remos_get_flow_for_query(char* srcname, 
				      char* dstname,
				      double bw_req,
				      Remos_Flow* data,
				      Remos_Error* error) {
  remos_remulac_id srcid, dstid;
  Remos_Status local_stat;

  local_stat = remos_getidbyname(srcname, 0, &srcid, error);
  if (local_stat != REMOS_OK) return local_stat;

  local_stat = remos_getidbyname(dstname, 0, &dstid, error);
  if (local_stat != REMOS_OK) return local_stat;

  *data = (Remos_Flow) make_node(sizeof(remos_flow_s));
  (*data)->src_node = srcid;
  (*data)->dst_node = dstid;
  (*data)->latency = 0.0;
  (*data)->bandwidth = bw_req;
  (*data)->latency_stats = NULL;
  (*data)->bandwidth_stats = NULL;

  return REMOS_OK;
}

static int getflowbw_inited=0;

int init_getflowbw()
{
  Remos_Error error;
  if (!getflowbw_inited) { 
    remos_remulac_id    my_id;
    if (REMOS_OK!=remos_start("GetFlowBWpdinda", &my_id, NULL, &handler, &error)) {
      fprintf(stderr, "remos_start() failed\n");
      return -1;
    } else {
      getflowbw_inited=1;
      return 0;
    }
  } else {
    return 0;
  }
  
}


int deinit_getflowbw()
{
  Remos_Error error;
  if (getflowbw_inited) { 
    if (REMOS_OK != remos_finish(&error)) { 
      fprintf(stderr, "remos_finish failed\n");
      return -1;
    } else {
      getflowbw_inited=0;
      return 0;
    }
  } else {
    return 0;
  }
}


#if 0
void remos_print_errorinfo(Remos_Error error);
/*
 *      error:	a remos_error returned from one of the other remos
 *		routines.
 *      prints a brief textual description of the error to stderr.
 */

const char* remos_errmsg(Remos_Error error);
#endif

int getflowbw(char *from_host_or_ip, 
	      char *to_host_or_ip, 
	      double *flow_in_megabytes_per_sec) // note mbits/s
{
  double timeframe;
  Remos_Error error;
  Remos_Graph ms_graph=0;
  Remos_Flow flow;
  Remos_Flow_List flow_query_list=0;
       
  if (!getflowbw_inited) { 
    if (init_getflowbw()) {
      return -1;
    }
  }

#define TEST 1

  /* Latest thing I can get */
#if TEST
  timeframe = TIME_NOT_RELEVANT;
#else
  timeframe =-0.0001;  
#endif

  if (REMOS_OK!=(remos_get_flow_for_query(from_host_or_ip, 
					  to_host_or_ip,
					  1000000000000,
					  &flow,
					  &error))) {
    fprintf(stderr, "remos_get_flow_for_query failed\n");
    goto FAIL;
  }
    
  if (0==(flow_query_list = (Remos_Flow_List) remos_new_list(1))) { 
    fprintf(stderr, "out of memory\n");
    goto FAIL;
  }

  remos_list_append(flow,flow_query_list);

  if (REMOS_OK!=remos_flow_info(0, 
				0, 
				flow_query_list, 
				0, 
				ms_graph, 
				&timeframe, 
				&error)) {
    fprintf(stderr, "remos_flow_info failed\n");
    goto FAIL;
  }


  *flow_in_megabytes_per_sec = flow_query_list->list[0]->bandwidth;
	    

  CHK_DEL(flow_query_list);
  remos_delete_graph(&ms_graph,&error);
  return 0;

FAIL:
  CHK_DEL(flow_query_list);
  remos_delete_graph(&ms_graph,&error);
  return -1;

}
    
#else
int init_getflowbw()
{
  return -1;
}


int deinit_getflowbw() 
{
  return -1;
}

int getflowbw(char *from_host_or_ip, 
	      char *to_host_or_ip, 
	      double *flow_in_megabytes_per_sec) // note mbits/s
{
  *flow_in_megabytes_per_sec=0;
  return -1;
}
#endif
