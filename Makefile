include Makefile.conf

ifeq ($(HAVE_GETLOADAVG),YES)
  HGLA_DEP = GetLoadAvg
else
  HGLA_DEP =
endif

ifeq ($(HAVE_GETFLOWBW),YES)
  HGFB_DEP = GetFlowBW
else
  HGFB_DEP =
endif

ifeq ($(HAVE_TIMESERIES),YES)
  HTS_DEP = TimeSeries
else
  HTS_DEP =
endif

ifeq ($(HAVE_FRACDIFF),YES)
  HFD_DEP = FracDiff
else
  HFD_DEP =
endif

ifeq ($(HAVE_MIRROR),YES)
  HMT_DEP = Mirror
else
  HMT_DEP =
endif

ifeq ($(HAVE_RPSINT),YES)
  HRPSI_DEP = RPSInterface
else
  HRPSI_DEP =
endif

ifeq ($(HAVE_REMOSINT),YES)
  HREMOS_DEP = RemosInterface
else
  HREMOS_DEP =
endif

ifeq ($(HAVE_PREDCOMP),YES)
  HPC_DEP = PredComp
else
  HPC_DEP =
endif

ifeq ($(HAVE_JAVAGUI),YES)
  HJG_DEP = JavaGUI
else
  HJG_DEP =
endif

ifeq ($(HAVE_SPIN),YES)
  HSP_DEP = Spin
else
  HSP_DEP =
endif

ifeq ($(HAVE_TRACE),YES)
  HT_DEP = Trace
else
  HT_DEP =
endif

ifeq ($(HAVE_RTA),YES)
  HRTA_DEP = RTA
else
  HRTA_DEP =
endif

ifeq ($(HAVE_RTSA),YES)
  HRTSA_DEP = RTSA
else
  HRTSA_DEP =
endif

HSE_DEP = Sensors

PROJS = $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) $(HREMOS_DEP) $(HPC_DEP) $(HJG_DEP) $(HSP_DEP) $(HT_DEP) $(HRTA_DEP) $(HRTSA_DEP)

all:  $(PROJS)

rebuild_all: clean depend all

shared: $(PROJS)
	cp `find $(SHARED_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include

GetLoadAvg: force
	cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(GETLOADAVG_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(GETLOADAVG_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(GETLOADAVG_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

GetFlowBW: force
	cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(GETFLOWBW_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(GETFLOWBW_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(GETFLOWBW_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

Sensors : $(HGLA_DEP) $(HGFB_DEP) force
	cp `find $(SENSORS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include

TimeSeries: $(HFD_DEP)
	cd $(TS_DIR); $(MAKE)   RPS_DIR=$(RPS_DIR)  all
	cp `find $(TS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(TS_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(TS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

FracDiff: force
	cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(FRACDIFF_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(FRACDIFF_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(FRACDIFF_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

Mirror:  force
	cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(MIRROR_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(MIRROR_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for the Mirror
#	cp `find $(MIRROR_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

RPSInterface:  $(HSE_DEP) $(HTS_DEP) $(HMT_DEP) force
	cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RPSINT_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RPSINT_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for RPSInterface
#	cp `find $(RPSINT_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

RemosInterface:  $(HRPSI_DEP) force
	cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(REMOSINT_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(REMOSINT_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for RemosInterface
#	cp `find $(REMOSINT_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)


PredComp: $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) 

	cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(PREDCOMP_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
# No predcomp libraries
#	cp `find $(PREDCOMP_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(PREDCOMP_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

JavaGUI:  $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) 
	cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp $(JAVAGUI_DIR)/*.so $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(JAVAGUI_DIR)/*.class $(RPS_DIR)/bin/$(ARCH)/$(OS)

Spin: $(HMT_DEP) $(HRPSI_DEP) force
	cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(SPIN_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(SPIN_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(SPIN_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

Trace: $(HGLA_DEP) $(HSP_DEP) force
	cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(TRACE_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(TRACE_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(TRACE_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

RTA: $(HSP_DEP) $(HT_DEP) $(HPC_DEP) force
	cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RTA_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RTA_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(RTA_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

RTSA: $(HRTA_DEP) force
	cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RTSA_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RTSA_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(RTSA_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)

clean:
	cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean ;
	cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(TS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	#cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean;
	rm -f `find $(RPS_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS`
	rm -f `find $(RPS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS`

depend:
	cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend ;
	cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(TS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;
	#cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend;

force: ;
