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
  HRPSI_DEP = Mirror TimeSeries
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

ifeq ($(HAVE_SPIN),YES)
  HSP_DEP = Spin
else
  HSP_DEP =
endif

PROJS = $(HGLA_DEP) $(HGFB_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) $(HREMOS_DEP) $(HPC_DEP) $(HSP_DEP)

all:  $(PROJS)

rebuild_all: clean depend all

GetLoadAvg: force
	cd $(GETLOADAVG_DIR); $(MAKE) $(MAKEFLAGS) RPS_DIR=$(RPS_DIR) all
	cp $(GETLOADAVG_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(GETLOADAVG_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(GETLOADAVG_DIR)/bin/$(ARCH)/$(OS)/test $(RPS_DIR)/bin/$(ARCH)/$(OS)/test_getloadavg

GetFlowBW: force
	cd $(GETFLOWBW_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(GETFLOWBW_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(GETFLOWBW_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(GETFLOWBW_DIR)/bin/$(ARCH)/$(OS)/test $(RPS_DIR)/bin/$(ARCH)/$(OS)/test_getflowbw

TimeSeries: $(HFD_DEP) force
	cd $(TS_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR)  all
	cp $(TS_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(TS_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(TS_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

FracDiff: force
	echo $(FRACDIFF_DIR)
	cd $(FRACDIFF_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(FRACDIFF_DIR)/include/*.h $(RPS_DIR)/include
	cp $(FRACDIFF_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)

Mirror:  force
	cd $(MIRROR_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(MIRROR_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(MIRROR_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(MIRROR_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

RPSInterface:  force
	cd $(RPSINT_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(RPSINT_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(RPSINT_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(RPSINT_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

RemosInterface:  force
	cd $(REMOSINT_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(REMOSINT_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(REMOSINT_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(REMOSINT_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

PredComp:  $(HGLA_DEP) $(HGFB_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) 
	cd $(PREDCOMP_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(PREDCOMP_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(PREDCOMP_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

Spin: $(HMC_DEP) force
	cd $(SPIN_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(SPIN_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(SPIN_DIR)/include/*.h $(RPS_DIR)/include
	-cp $(SPIN_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

clean:
	$(foreach m,$(PROJS),(cd $(m);$(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) clean); )
	(cd TS/FracDiff; $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) clean)
	rm -f $(RPS_DIR)/lib/$(ARCH)/$(OS)/*.a
	rm -f $(RPS_DIR)/bin/$(ARCH)/$(OS)/*

depend:
	$(foreach m,$(PROJS),(cd $(m);$(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) depend) ; )
	(cd TS/FracDiff; $(MAKE) $(MAKEFLAGS) depend)

force: ;