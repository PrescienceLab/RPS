#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "stdio.h"
#include "remos.h"
#include "memoryPack.h"

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


main(){

  /* ******************************************************************* */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* Edit the variables and #defines between this comment bar and the next */
  /* one to fit your network. */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* ******************************************************************* */

  /* the number of network nodes involved in the queries.  Also the count */
  /* of elements in names */
#define NUMNODES 8

  /* names of the nodes.  Fill in your own. */
  char *names[NUMNODES] = {
    "manchester-8",
    "manchester-3", 
    "manchester-2", 
    "manchester-7",
    "manchester-4",
    "manchester-1",
    "manchester-6",
    "manchester-3", 
  };

  /* the number of flows you will ask about. If you change this, you must */
  /* also change the remos_list_append calls just before the remos_flow_info */
  /* call below. */
#define NUMFLOWS 18
  /* scale by which bandwidths are divided before the actual request is */
  /* made.  Allows convenient adjustment to available network resources for*/
  /* testing purposes. */
#define SCALE    10.0

  double bw_reqs[NUMFLOWS] = {
    /* first three are used as fixed flows. */
    1000000, 1000000, 1000000, /* 0..2 */
    /* next batch used as variable flows */
    900000, 900000, 900000,    /* 3..5 */
    750000, 500000, 10065535,     /* 6..8 */
    1048576,                   /* 9 */
    /* remainder are flow queries */
    750000, 750000,   /* 10..11 */
    500000, 500000, 100000,    /* 12..14 */
    100000, 100000, 12345678 };/* 15..17 */

  /* ******************************************************************* */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* end of stuff to edit                                                */
  /* ******************************************************************* */
  /* ******************************************************************* */
  /* ******************************************************************* */

  remos_remulac_id remids[NUMNODES];
  remos_remulac_id my_id;
  Remos_Error error;
  Remos_Status stat;
  Remos_Host_List hlist;
  Remos_Node_List nlist;
  Remos_Node nodes[NUMNODES];
  Remos_Graph ms_graph;

  double time;
  int i, k, rpt;
  Remos_Flow the_flow;
  Remos_Flow_List vfs, ffs, fqs;
  Remos_Flow flows[NUMFLOWS];
  Remos_Flow flowptr;

  /* change the argument from 0 to 1 for a painfully detailed debug dump */
  /* of all communications between the modeller and the collector.       */
  set_testing_remos(0);

  fprintf(stderr, "starting remos test\n");
  stat = remos_start("test", &my_id, NULL, &handler, &error);
  fprintf(stderr, "status of remos_start was: %d\n", stat);
  if (stat != REMOS_OK) {
    fprintf(stderr,
	    "Remos test FAILED.\n\tCheck to verify that a collector is running\n");
    fprintf(stderr, "\ton the host and port that you specified.\n\n");
    exit(1);
  }

  /*  fprintf(stderr, "trying remos_hostlist 128.2\n");
  stat = remos_hostlist(inet_addr("128.2.0.0"), &hlist, &error);
  fprintf(stderr, "status of remos_hostlist was: %d\n", stat); */


  time = TIME_NOT_RELEVANT;

  nlist = (Remos_Node_List) remos_new_list(2);

/*
  for (i=0; i<NUMNODES; i++) {
    stat = remos_getidbyname(names[i], 0, &(remids[i]), &error);
    if (stat == REMOS_OK) 
      stat = remos_get_node(remids[i], &(nodes[i]), &time, &error);
    fprintf(stderr, "status of remos_get_node was: %d\n", stat);
    if (stat == REMOS_OK) {
      remos_list_append(nodes[i], nlist);
      fprintf(stderr, "success for node %s\n", names[i]);
    } else {
      fprintf(stderr, "failure for node %s\n", names[i]);
    }
  }

  */

  /*  stat = remos_get_graph(nlist, &ms_graph, &time, &error);
  fprintf(stderr, "status of remos_get_graph was: %d\n", stat);
  if (stat == REMOS_OK) {
    fprintf(stderr, "*** graph had %d nodes and %d edges\n",
            ms_graph->nodes->elem_count,
	    ms_graph->edges->elem_count);
  }
  */

  for (i=0; i<NUMFLOWS; i++) {
      stat = remos_get_flow_for_query(names[i%NUMNODES], names[(i+1)%NUMNODES],
				      bw_reqs[i]/SCALE, &flows[i], &error);
    fprintf(stderr, "status of remos_get_flow_for_query was: %d\n", stat);
    if (stat == REMOS_OK) {
      fprintf(stderr, "*** flow %d had bandwidth %f\n", 
	      i, flows[i]->bandwidth);
      fprintf(stderr, "    source %s (%x), and\n", 
	      flows[i]->src_node->hostname, flows[i]->src_node->host_ip_addr);
      fprintf(stderr, "    dest.  %s (%x)\n",
	      flows[i]->dst_node->hostname, flows[i]->dst_node->host_ip_addr);
    } else {
      fprintf(stderr, "\tfailure was %s", remos_errmsg(error));
    }
  }
  
  ffs = (Remos_Flow_List) remos_new_list(3);
  vfs = (Remos_Flow_List) remos_new_list(3);
  fqs = (Remos_Flow_List) remos_new_list(2);

  remos_list_append(flows[0], ffs);
  remos_list_append(flows[1], ffs);
  remos_list_append(flows[2], ffs);

  remos_list_append(flows[3], vfs);
  remos_list_append(flows[4], vfs);
  remos_list_append(flows[5], vfs);
  remos_list_append(flows[6], vfs);
  remos_list_append(flows[7], vfs);
  remos_list_append(flows[8], vfs);
  remos_list_append(flows[9], vfs);


  remos_list_append(flows[10], fqs);
  remos_list_append(flows[11], fqs);
  remos_list_append(flows[12], fqs);
  remos_list_append(flows[13], fqs);
  remos_list_append(flows[14], fqs);
  remos_list_append(flows[15], fqs);
  remos_list_append(flows[16], fqs);
  remos_list_append(flows[17], fqs);


  /* put this loop back in to get a much more lengthy test of remos */
  /*  for (rpt=0; rpt<100; rpt++) { */
  stat = remos_flow_info(ffs, vfs, fqs, 0, ms_graph, &time, &error);
  /* stat = remos_flow_info(NULL, vfs, NULL, 0, ms_graph, &time, &error); */
  if (stat == REMOS_OK) {
    k = 0;
    fprintf(stderr, "*** remos_flow_info reports success\n");
    
    if ((ffs != NULL) && (ffs->elem_count > 0)) {
      fprintf(stderr, "==== fixed flows ====\n");
	k = 0;
      for (remos_list_all_elements(ffs, flowptr, i)) {
      fprintf(stderr, "\t flow from %s (%x) to %s (%x):\n",
	      flowptr->src_node->hostname,
	      flowptr->src_node->host_ip_addr,
	      flowptr->dst_node->hostname,
	      flowptr->dst_node->host_ip_addr); 
      fprintf(stderr,
	      "\t\tbandwidth requested: %f\n\t\tbandwidth received: %f\n",
	      bw_reqs[k]/SCALE,
	      flowptr->bandwidth);
      k++;
      }
    }

    if ((vfs != NULL) && (vfs->elem_count > 0)) {
      fprintf(stderr, "==== variable flows ====\n");
      for (remos_list_all_elements(vfs, flowptr, i)) {
      fprintf(stderr, "\t flow from %s (%x) to %s (%x):\n",
	      flowptr->src_node->hostname,
	      flowptr->src_node->host_ip_addr,
	      flowptr->dst_node->hostname,
	      flowptr->dst_node->host_ip_addr); 
      fprintf(stderr,
	      "\t\tbandwidth requested: %f\n\t\tbandwidth received: %f\n",
	      bw_reqs[k]/SCALE,
	      flowptr->bandwidth);
      k++;
      }
    }

    if ((fqs != NULL) && (fqs->elem_count > 0)) {
      fprintf(stderr, "==== flow queries ====\n");
      for (remos_list_all_elements(fqs, flowptr, i)) {
      fprintf(stderr, "\t flow from %s (%x) to %s (%x):\n",
	      flowptr->src_node->hostname,
	      flowptr->src_node->host_ip_addr,
	      flowptr->dst_node->hostname,
	      flowptr->dst_node->host_ip_addr); 
      fprintf(stderr,
	      "\t\tbandwidth requested: %f\n\t\tbandwidth received: %f\n",
	      bw_reqs[k]/SCALE,
	      flowptr->bandwidth);
      k++;
      }
    }

  } else {
    fprintf(stderr,
	    "remos_flow_info on flow queries failed.  Minor status was %d\n",
	    remos_errno(error));
    fprintf(stderr, "\tfailure was %s", remos_errmsg(error));
  }
  /*  } */


  stat = remos_flow_info(ffs, NULL, NULL, 0, ms_graph, &time, &error);
  if (stat == REMOS_OK) {
    fprintf(stderr, "*** remos_flow_info on fixed flows reports success\n");
    for(remos_list_all_elements(ffs, flowptr, i)) {
      fprintf(stderr, "\t flow from %x to %x got bandwidth %f\n",
	      flowptr->src_node->host_ip_addr,
	      flowptr->dst_node->host_ip_addr,
	      flowptr->bandwidth);
    }
  } else {
    fprintf(stderr, "remos_flow_info failed.  Minor status was %d\n",
	    remos_errno(error));
  }
	    

  stat = remos_flow_info(NULL, vfs, NULL, 0, ms_graph, &time, &error);
  if (stat == REMOS_OK) {
    fprintf(stderr, "*** remos_flow_info on variable flows reports success\n");
    for(remos_list_all_elements(vfs, flowptr, i)) {
      fprintf(stderr, "\t flow from %x to %x got bandwidth %f\n",
	      flowptr->src_node->host_ip_addr,
	      flowptr->dst_node->host_ip_addr,
	      flowptr->bandwidth);
    }
  } else {
    fprintf(stderr, "remos_flow_info failed.  Minor status was %d\n",
	    remos_errno(error));
  }
	    

  stat = remos_finish(&error);
  fprintf(stderr, "status of remos_finish was: %d\n", stat);
}
    
