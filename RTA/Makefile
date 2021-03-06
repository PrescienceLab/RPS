include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

LIBNAME = RTA

LIBFILE = $(LIBDIR)/lib$(LIBNAME).a


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
	  $(WAVELETS_CFLAGS) \
          $(RPSINT_CFLAGS) \
          $(MIRROR_CFLAGS) \
          $(SPIN_CFLAGS) \
          $(FINDER_CFLAGS) \

LD2FLAGS = -L$(LIBDIR) -l$(LIBNAME) \
           $(FINDER_LDFLAGS) \
           $(RPSINT_LDFLAGS) \
           $(MIRROR_LDFLAGS) \
           $(GETLOADAVG_LDFLAGS) \
           $(SPIN_LDFLAGS) \
           $(TIMESERIES_LDFLAGS) \
	   $(WAVELETS_LDFLAGS) \
           $(FRACDIFF_LDFLAGS) \
           $(RECIPES_LDFLAGS) \
          -lm 

ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lnsl -lsocket -lkvm -lelf
else
  LD3FLAGS = 
endif

EXECLIST = find_tau         \
           rta_play         \
           test_rta

APPOBJSLIST = $(EXECLIST:=.o)

LIBOBJSLIST = EstimateExecTime.o RTA.o


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

  define MakeExeAction
    $(LD) $<  $(LD1FLAGS) $(LD2FLAGS) $(LD3FLAGS) -o $(BINDIR)/$(@F)
  endef


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

