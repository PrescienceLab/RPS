include $(RPS_DIR)/Makefile.conf

ROOT = .


LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

CFLAGS = -I$(INCDIR) \
         $(GETLOADAVG_CFLAGS) \
         $(GETFLOWBW_CFLAGS) \
         -I$(SENSORS_INC_DIR)\
         $(TIMESERIES_CFLAGS) \
         $(FRACDIFF_CFLAGS) \
         $(RECIPES_CFLAGS) \
         $(RPSINT_CFLAGS) \
         $(MIRROR_CFLAGS) \
         $(REMOS_MODELER_CFLAGS) \
         $(LISTPACK_CFLAGS) \
         $(EXPAT_CFLAGS)

ifeq ($(OPTIMIZE),YES)
  CFLAGS +=  -O
  LD1FLAGS = -O 
else
  CFLAGS += -g -gstabs+ -ggdb
  LD1FLAGS = -g  -gstabs+ -ggdb 
endif

LD1FLAGS+=  $(GETLOADAVG_LDFLAGS) \
            $(GETFLOWBW_LDFLAGS)\
            $(REMOS_MODELER_LDFLAGS) \
            $(LISTPACK_LDFLAGS)
LD2FLAGS =  $(RPSINT_LDFLAGS) \
            $(TIMESERIES_LDFLAGS) \
            $(FRACDIFF_LDFLAGS)  \
            $(RECIPES_LDFLAGS) \
            $(MIRROR_LDFLAGS) \
            $(EXPAT_LDFLAGS) \
            $(PREDCOMP_LDFLAGS) \
            -lm
ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lnsl -lsocket -lkvm -lelf
else
  LD3FLAGS = 
endif

EXECLIST = loadclient 				\
        predserver 				\
        predserver_core				\
        predclient 				\
        pred_reqresp_server 			\
        pred_reqresp_client 			\
        loadbuffer 				\
        loadbufferclient 			\
        load2measure 				\
        predbuffer 				\
        predbufferclient                        \
        measureclient                           \
        measurebuffer                           \
        measurebufferclient                     \
        predreconfig                            \
        evalfit                                 \
        predlatclient                           \
        loadlatclient                           \
        loadreconfig                            \
        fileloadserver                          \
        memperf                                 \
        flowbwclient                            \
        flowbwbuffer                            \
        flowbwbufferclient                      \
        flowbw2measure                          \
        flowbwreconfig                          \
        text2measure


ifeq ($(HAVE_GETLOADAVG),YES)
   EXECLIST += loadserver sensorperf hostloadpred
endif

ifeq ($(HAVE_GETFLOWBW),YES)
   EXECLIST += flowbwserver
endif


ifeq ($(HAVE_EXPAT),YES)
   EXECLIST += lmxmltest
endif



APPOBJSLIST = $(EXECLIST:=.o)


EXECS   = $(foreach m, $(EXECLIST),$(BINDIR)/$(m))
LIBOBJS = $(foreach m, $(LIBOBJSLIST),$(OBJDIR)/$(m))
APPOBJS = $(foreach m, $(APPOBJSLIST),$(OBJDIR)/$(m))

SRCSLIST = $(LIBOBJSLIST:.o=.cpp) $(APPOBJSLIST:.o=.cpp)

SRCS = $(foreach m, $(SRCSLIST), $(SRCDIR)/$(m))


OBJS  = $(LIBOBJS) $(APPOBJS)


all: $(EXECS)

$(LIBFILE) : $(LIBOBJS)
	ar ruv $(LIBFILE) $(LIBOBJS)
	ranlib $(LIBFILE)

ifeq ($(OS),FREEBSD)
  define MakeExeAction
    $(LD) $(LD1FLAGS) $<  $(LD2FLAGS) $(LD3FLAGS) -o $(BINDIR)/$(@F)
  endef
else
  define MakeExeAction
    $(LD) $<  $(LD1FLAGS) $(LD2FLAGS) $(LD3FLAGS) -o $(BINDIR)/$(@F)
  endef
endif


$(EXECS):$(BINDIR)/%:$(OBJDIR)/%.o $(LIBFILE)
	$(MakeExeAction)


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(@F)

.dependencies.$(ARCH).$(OS) : $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

depend: Makefile $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

clean: 
	-rm -f $(EXECS) $(OBJS) $(LIBFILE)

include .dependencies.$(ARCH).$(OS)

