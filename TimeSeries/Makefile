include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

LIBNAME = timeseries
LIBFILE = $(LIBDIR)/lib$(LIBNAME).a
LIB_TS = $(LIBFILE)


CFLAGS = -I$(INCDIR) $(RECIPES_CFLAGS) $(FRACDIFF_CFLAGS) \
         $(TRACE_CFLAGS) $(PVM_CFLAGS)

ifeq ($(OPTIMIZE),YES)
   CFLAGS += -O
else
   CFLAGS += -g -gstabs+ -ggdb
endif


LDFLAGS = -L$(TS_LIB_DIR) \
          -l$(LIBNAME) \
          $(RECIPES_LDFLAGS) \
          $(FRACDIFF_LDFLAGS) \
          $(TRACE_LDFLAGS) \
          $(PVM_LDFLAGS) \
          -lm

ifeq ($(OPTIMIZE),YES)
   LDFLAGS += -O
else
   LDFLAGS += -g -gstabs+ -ggdb
endif

#
# These are the execs this make file will build in addition to the lib
#
# ts_example 	   do fit and prediction from command line
# test_generic     different version of example
# test_await       simple example for await and managed models.
# evaluate         standalone evaluation program
# crossval_generic older, non-parallel crossvalidation engine

# crossval_master  pvm crossvalidation master - can also be used in non-parallel manner
# crossval_slave   pvm crossvalidation slave
# impulseresp      fit, and then output impulse response of filter
# perftest         performance testing of models
#
# gfnn             global false nearest neighbor from abarbanel method
# mi               average mutual information profile from abarbanel method
# polycalc         polynomial calculator

EXECLIST = ts_example 	    \
           test_generic     \
           evaluate         \
           crossval_generic \
           impulseresp      \
           perftest         \
           gfnn             \
           mi               \
	   test_await       \
           polycalc

ifeq ($(HAVE_PVM),YES)
   EXECLIST +=  crossval_master crossval_slave
endif


MAINLIST = $(EXECLIST:=.o)

LIB_TS_OBJECTS_LIST = poly.o ar.o arima.o arfima.o etathetapred.o \
                      tools.o ma.o arma.o bestmean.o it.o evaluate_core.o \
                      crossval_core.o mean.o last.o none.o fit.o \
                      await.o refit.o nr-internal.o bestmedian.o

SRCLIST = $(MAINLIST:.o=.cpp) $(LIB_TS_OBJECTS_LIST:.o=.cpp)

EXECS = $(foreach m, $(EXECLIST), $(BINDIR)/$(m))

LIB_TS_OBJECTS = $(foreach m, $(LIB_TS_OBJECTS_LIST), $(OBJDIR)/$(m))

MAINS = $(foreach m, $(MAINLIST),$(OBJDIR)/$(m))

OBJECTS = $(LIB_TS_OBJECTS) $(MAINS)

SRCS = $(foreach m, $(SRCLIST),$(SRCDIR)/$(m))

all: $(LIB_TS) $(EXECS)


$(LIB_TS) : $(LIB_TS_OBJECTS)
	$(AR) ruv $(LIB_TS) $(LIB_TS_OBJECTS)
	$(RANLIB) $(LIB_TS)



define MakeExeAction
	$(CC) $< $(LDFLAGS) -o $(BINDIR)/$(@F)
endef



$(EXECS):$(BINDIR)/%:$(OBJDIR)/%.o $(LIB_TS)
	$(MakeExeAction)



$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(@F)


clean:
	-rm -f $(OBJECTS) $(EXECS) $(LIB_TS)


.dependencies.$(ARCH).$(OS) : $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

depend: Makefile $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)


include .dependencies.$(ARCH).$(OS)

