include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

LIBNAME = spin

LIBFILE = $(LIBDIR)/lib$(LIBNAME).a


CC     = c++
LD     = c++ 

ifeq ($(OPTIMIZE),YES) 
  CFLAGS = -O
  LD1FLAGS = -O
else
  CFLAGS = -g -gstabs+ -ggdb
  LD1FLAGS = -g  -gstabs+ -ggdb 
endif

CFLAGS += -I$(INCDIR) \
          $(GETLOADAVG_CFLAGS) \
          $(TIMESERIES_CFLAGS) \
          $(RPSINT_CFLAGS) \
          $(MIRROR_CFLAGS) \

LD2FLAGS = -L$(LIBDIR) -l$(LIBNAME) \
           $(RPSINT_LDFLAGS) \
           $(MIRROR_LDFLAGS) \
           $(GETLOADAVG_LDFLAGS) \
           $(TIMESERIES_LDFLAGS) \
           $(FRACDIFF_LDFLAGS) \
           $(RECIPES_LDGLAGS) \
          -lm 

ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lsocket -lkvm -lelf
else
  LD3FLAGS = 
endif

EXECLIST = find_tau \
           test_apply \
           genload defilter_trace \
           select_from_binary_trace_file \
           alpha_to_network \
           network_to_alpha \
           network_to_ascii \
           ascii_to_network \
           alpha_to_ascii   \
           calibrate        \
           testspin         \
           loadmodeltest    \
           spinserver       \
           spinclient       \
           spinservertest   \
           test_pred        \
           test_pred_vartau        \
           play             \
           test_sched       \
           test_sched_aggressive

APPOBJSLIST = $(EXECLIST:=.o)

LIBOBJSLIST = timing.o \
              apply.o \
              LoadTrace.o \
              Spin.o \
              SpinServer.o \
              EstimateExecTime.o \
              TRACE.o

EXECS   = $(foreach m, $(EXECLIST),$(BINDIR)/$(m))
LIBOBJS = $(foreach m, $(LIBOBJSLIST),$(OBJDIR)/$(m))
APPOBJS = $(foreach m, $(APPOBJSLIST),$(OBJDIR)/$(m))

SRCSLIST = $(LIBOBJSLIST:.o=.cpp) $(APPOBJSLIST:.o=.cpp)

SRCS = $(foreach m, $(SRCSLIST), $(SRCDIR)/$(m))


OBJS  = $(LIBOBJS) $(APPOBJS)


all: $(LIBFILE) $(EXECS)

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

