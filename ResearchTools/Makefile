include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include


ifeq ($(OPTIMIZE),YES) 
  CFLAGS = -O
  LD1FLAGS = -O
else
  CFLAGS = -g -gstabs+ -ggdb
  LD1FLAGS = -g  -gstabs+ -ggdb 
endif

CFLAGS += -I$(INCDIR) \
          $(GETLOADAVG_CFLAGS) \
          $(RPSINT_CFLAGS) \
          $(FINDER_CFLAGS) \
          $(SPIN_CFLAGS) \
          $(RTA_CFLAGS) \
          $(MIRROR_CFLAGS) \
          $(TIMESERIES_CFLAGS) \

LD2FLAGS = $(FINDER_LDFLAGS) \
           $(RTA_LDFLAGS) \
           $(RPSINT_LDFLAGS) \
           $(MIRROR_LDFLAGS) \
           $(SPIN_LDFLAGS) \
           $(TIMESERIES_LDFLAGS) -lm

ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lnsl -lsocket -lkvm -lelf
else
  LD3FLAGS = 
endif

EXECLIST = test_pred     \
           test_pred_vartau \
           test_sched \
           test_sched_aggressive

APPOBJSLIST = $(EXECLIST:=.o)

LIBOBJSLIST =

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

