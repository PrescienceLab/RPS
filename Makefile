include Makefile.conf

ifeq ($(HAVE_GETLOADAVG),YES)
  HGLA_DEP = GetLoadAvg
  HGLA_CLEAN = cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean 
  HGLA_DEPEND = cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend 
else
  HGLA_DEP =
endif

ifeq ($(HAVE_GETFLOWBW),YES)
  HGFB_DEP = GetFlowBW
  HGFB_CLEAN = 	cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HGFB_DEPEND = cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HGFB_DEP =
endif

ifeq ($(HAVE_TIMESERIES),YES)
  HTS_DEP = TimeSeries
  HTS_CLEAN = cd $(TS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HTS_DEPEND = cd $(TS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HTS_DEP =
endif

ifeq ($(HAVE_WAVELETS),YES)
  HWA_DEP = Wavelets
  HWA_CLEAN = cd $(WAVELETS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HWA_DEPEND = cd $(WAVELETS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HWA_DEP =
endif

ifeq ($(HAVE_FRACDIFF),YES)
  HFD_DEP = FracDiff
  HFD_CLEAN = cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HFD_DEPEND = 	cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HFD_DEP =
endif

ifeq ($(HAVE_MIRROR),YES)
  HMT_DEP = Mirror
  HMT_CLEAN = cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HMT_DEPEND = cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HMT_DEP =
endif

ifeq ($(HAVE_RPSINT),YES)
  HRPSI_DEP = RPSInterface
  HRPSI_CLEAN=cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HRPSI_DEPEND=cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HRPSI_DEP =
endif

ifeq ($(HAVE_REMOSINT),YES)
  HREMOS_DEP = RemosInterface
  HREMOS_CLEAN=cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HREMOS_DEPEND=cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HREMOS_DEP =
endif

ifeq ($(HAVE_PREDCOMP),YES)
  HPC_DEP = PredComp
  HPC_CLEAN=cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HPC_DEPEND=cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HPC_DEP =
endif

ifeq ($(HAVE_JAVAGUI),YES)
  HJG_DEP = JavaGUI
  HJG_CLEAN=cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HJG_DEPEND=cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HJG_DEP =
endif

ifeq ($(HAVE_SPIN),YES)
  HSP_DEP = Spin
  HSP_CLEAN=cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HSP_DEPEND=cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HSP_DEP =
endif

ifeq ($(HAVE_TRACE),YES)
  HT_DEP = Trace
  HT_CLEAN=cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HT_DEPEND=cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HT_DEP =
endif

ifeq ($(HAVE_RTA),YES)
  HRTA_DEP = RTA
  HRTA_CLEAN=cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HRTA_DEPEND=cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HRTA_DEP =
endif

ifeq ($(HAVE_RTSA),YES)
  HRTSA_DEP = RTSA
  HRTSA_CLEAN=cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HRTSA_DEPEND=cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HRTSA_DEP =
endif

ifeq ($(HAVE_FINDER),YES)
  HFIN_DEP = Finder
  HFIN_CLEAN=cd $(FINDER_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HFIN_DEPEND=cd $(FINDER_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HFIN_DEP =
endif

ifeq ($(HAVE_RESEARCHTOOLS),YES)
  HRT_DEP = ResearchTools
  HRT_CLEAN=cd $(RESEARCHTOOLS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) clean
  HRT_DEPEND=cd $(RESEARCHTOOLS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) depend
else
  HRT_DEP =
endif

HSE_DEP = Sensors

PROJS = $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HWA_DEP) $(HMT_DEP) $(HRPSI_DEP) $(HREMOS_DEP) $(HPC_DEP) $(HJG_DEP) $(HSP_DEP) $(HT_DEP) $(HRTA_DEP) $(HRTSA_DEP) $(HFIN_DEP) $(HRT_DEP)

all:  $(PROJS) shared final_fixup

rebuild_all: clean depend all

final_fixup: $(PROJS) shared
	-rm -f $(RPS_DIR)/lib/$(ARCH)/$(OS)/libRPS.a
	$(AR) ruv $(RPS_DIR)/lib/$(ARCH)/$(OS)/libRPS.a $(RPS_DIR)/obj/$(ARCH)/$(OS)/*.o
	$(RANLIB) $(RPS_DIR)/lib/$(ARCH)/$(OS)/libRPS.a
	$(STRIP) `find $(RPS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS`

shared: $(PROJS)
	cp `find $(SHARED_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include

GetLoadAvg: force
	cd $(GETLOADAVG_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(GETLOADAVG_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(GETLOADAVG_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(GETLOADAVG_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(GETLOADAVG_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

GetFlowBW: force
	cd $(GETFLOWBW_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(GETFLOWBW_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(GETFLOWBW_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(GETFLOWBW_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(GETFLOWBW_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

Sensors : $(HGLA_DEP) $(HGFB_DEP) force
	cp `find $(SENSORS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include

TimeSeries: $(HFD_DEP) $(HT_DEP) force
	cd $(TS_DIR); $(MAKE)   RPS_DIR=$(RPS_DIR)  all
	cp `find $(TS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(TS_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(TS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(TS_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

Wavelets: $(HTS_DEP) force
	cd $(WAVELETS_DIR); $(MAKE)   RPS_DIR=$(RPS_DIR)  all
	cp `find $(WAVELETS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(WAVELETS_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(WAVELETS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(WAVELETS_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

FracDiff: force
	cd $(FRACDIFF_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(FRACDIFF_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(FRACDIFF_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(FRACDIFF_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(FRACDIFF_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

Mirror:  force
	cd $(MIRROR_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(MIRROR_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(MIRROR_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for the Mirror
#	cp `find $(MIRROR_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(MIRROR_DIR)/obj/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

RPSInterface:  $(HSE_DEP) $(HTS_DEP) $(HMT_DEP) force
	cd $(RPSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RPSINT_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RPSINT_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for RPSInterface
#	cp `find $(RPSINT_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(RPSINT_DIR)/obj/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

RemosInterface:  $(HRPSI_DEP) force
	cd $(REMOSINT_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(REMOSINT_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(REMOSINT_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
# No binaries for RemosInterface
#	cp `find $(REMOSINT_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(REMOSINT_DIR)/obj/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)


PredComp: $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HWA_DEP) $(HMT_DEP) $(HRPSI_DEP) force

	cd $(PREDCOMP_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(PREDCOMP_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
# No predcomp libraries
#	cp `find $(PREDCOMP_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(PREDCOMP_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
#	cp `find $(PREDCOMP_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

JavaGUI:  $(HSE_DEP) $(HFD_DEP) $(HTS_DEP) $(HMT_DEP) $(HRPSI_DEP) 
	cd $(JAVAGUI_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp $(JAVAGUI_DIR)/*.so $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp $(JAVAGUI_DIR)/*.class $(RPS_DIR)/bin/$(ARCH)/$(OS)

Spin: $(HMT_DEP) $(HTS_DEP) $(HWA_DEP) $(HRPSI_DEP) force
	cd $(SPIN_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(SPIN_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(SPIN_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(SPIN_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(SPIN_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

Trace:  force
	cd $(TRACE_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(TRACE_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(TRACE_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(TRACE_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(TRACE_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

RTA: $(HSP_DEP) $(HT_DEP) $(HPC_DEP) $(HFIN_DEP) force
	cd $(RTA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RTA_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RTA_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(RTA_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(RTA_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

RTSA: $(HRTA_DEP) force
	cd $(RTSA_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(RTSA_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(RTSA_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(RTSA_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(RTSA_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

Finder: $(HRPSI_DEP) $(HMT_DEP) force
	cd $(FINDER_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
	cp `find $(FINDER_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
	cp `find $(FINDER_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
#	cp `find $(FINDER_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
	cp `find $(FINDER_DIR)/obj/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

ResearchTools: $(HRTSA_DEP) force
	cd $(RESEARCHTOOLS_DIR); $(MAKE) RPS_DIR=$(RPS_DIR) all
#	cp `find $(RESEARCHTOOLS_DIR)/include -type f | grep -v CVS` $(RPS_DIR)/include
#	cp `find $(RESEARCHTOOLS_DIR)/lib/$(ARCH)/$(OS)/ -type f | grep -v CVS` $(RPS_DIR)/lib/$(ARCH)/$(OS)
	cp `find $(RESEARCHTOOLS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/bin/$(ARCH)/$(OS)
#	cp `find $(RESEARCHTOOLS_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS` $(RPS_DIR)/obj/$(ARCH)/$(OS)

clean:
	$(HGLA_CLEAN)
	$(HGFB_CLEAN)
	$(HTS_CLEAN)
	$(HWA_CLEAN)
	$(HFD_CLEAN)
	$(HMT_CLEAN)
	$(HRPSI_CLEAN)
	$(HREMOS_CLEAN)
	$(HPC_CLEAN)
	$(HJG_CLEAN)
	$(HSP_CLEAN)
	$(HT_CLEAN)
	$(HRTA_CLEAN)
	$(HRTSA_CLEAN)
	$(HFIN_CLEAN)
	$(HRT_CLEAN)
	rm -f `find $(RPS_DIR)/lib/$(ARCH)/$(OS) -type f | grep -v CVS`
	rm -f `find $(RPS_DIR)/bin/$(ARCH)/$(OS) -type f | grep -v CVS`
	rm -f `find $(RPS_DIR)/obj/$(ARCH)/$(OS) -type f | grep -v CVS`

depend:
	$(HGLA_DEPEND)
	$(HGFB_DEPEND)
	$(HTS_DEPEND)
	$(HWA_DEPEND)
	$(HFD_DEPEND)
	$(HMT_DEPEND)
	$(HRPSI_DEPEND)
	$(HREMOS_DEPEND)
	$(HPC_DEPEND)
	$(HJG_DEPEND)
	$(HSP_DEPEND)
	$(HT_DEPEND)
	$(HRTA_DEPEND)
	$(HRTSA_DEPEND)
	$(HFIN_DEPEND)
	$(HRT_DEPEND)

force: ;
