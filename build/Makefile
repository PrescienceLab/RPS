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
  HTS_DEP = TS
else
  HTS_DEP =
endif

ifeq ($(HAVE_FRACDIFF),YES)
  HFD_DEP = FracDiff
else
  HFD_DEP =
endif

ifeq ($(HAVE_LOADMON),YES)
  HLM_DEP = LoadMon
else
  HLM_DEP =
endif

ifeq ($(HAVE_SPIN),YES)
  HSP_DEP = Spin
else
  HSP_DEP =
endif

PROJS = $(HGLA_DEP) $(HGFB_DEP) $(HFD_DEP) $(HTS_DEP) $(HLM_DEP) $(HSP_DEP)

all:  $(PROJS)

rebuild_all: clean depend all


GetLoadAvg: force
	cd $(GETLOADAVG_DIR); $(MAKE) $(MAKEFLAGS) RPS_DIR=$(RPS_DIR) all
	cp $(GETLOADAVG_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	-cp $(GETLOADAVG_DIR)/bin/$(ARCH)/$(OS)/test $(RPS_DIR)/bin/$(ARCH)/$(OS)/test_getloadavg

GetFlowBW: force
	cd $(GETFLOWBW_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(GETFLOWBW_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	-cp $(GETFLOWBW_DIR)/bin/$(ARCH)/$(OS)/test $(RPS_DIR)/bin/$(ARCH)/$(OS)/test_getflowbw

TS: $(HFD_DEP) force
	cd $(TS_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR)  all
	cp $(TS_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	-cp $(TS_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

FracDiff: force
	echo $(FRACDIFF_DIR)
	cd $(FRACDIFF_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(FRACDIFF_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)

LoadMon:  $(HTS_DEP) $(HGLA_DEP) $(HGFB_DEP) force
	cd $(LOADMON_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(LOADMON_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
	-cp $(LOADMON_DIR)/bin/$(ARCH)/$(OS)/* $(RPS_DIR)/bin/$(ARCH)/$(OS)

Spin: $(HLM_DEP) force
	cd $(SPIN_DIR); $(MAKE) $(MAKEFLAGS)  RPS_DIR=$(RPS_DIR) all
	cp $(SPIN_DIR)/lib/$(ARCH)/$(OS)/*.a $(RPS_DIR)/lib/$(ARCH)/$(OS)
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