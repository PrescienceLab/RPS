include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

LIBNAME = wavelets

LIBFILE = $(LIBDIR)/lib$(LIBNAME).a

CFLAGS = -I$(INCDIR)

ifeq ($(OPTIMIZE),YES)
  CFLAGS +=  -O
  LD1FLAGS = -O 
else
  CFLAGS += -g -gstabs+ -ggdb
  LD1FLAGS = -g  -gstabs+ -ggdb 
endif

LD1FLAGS+=  -L$(LIBDIR) -l$(LIBNAME)

LD2FLAGS =  

EXECLIST = testcoefs  \
	   testfilter \
           teststage  \
           testSFWT   \
           testdelay  \
           teststream \
           testblock  \
           testSFWTapprox \
	   testSFWTblock \
	   testSFWTmixed \
	   sample_static_sfwt \
	   block_static_sfwt \
	   sample_static_srwt \
	   block_static_srwt \
	   sample_static_streaming_test


APPOBJSLIST = $(EXECLIST:=.o)

LIBOBJSLIST = coefficients.o       \
              filter.o             \
              downsample.o         \
              upsample.o           \
              sample.o             \
              sampleblock.o        \
              waveletsample.o      \
              waveletsampleblock.o \
              stage.o              \
              transforms.o         \
              delay.o

EXECS   = $(foreach m, $(EXECLIST),$(BINDIR)/$(m))
LIBOBJS = $(foreach m, $(LIBOBJSLIST),$(OBJDIR)/$(m))
APPOBJS = $(foreach m, $(APPOBJSLIST),$(OBJDIR)/$(m))

SRCSLIST = $(LIBOBJSLIST:.o=.cpp) $(APPOBJSLIST:.o=.cpp)

SRCS = $(foreach m, $(SRCSLIST), $(SRCDIR)/$(m))


OBJS  = $(LIBOBJS) $(APPOBJS)


all: $(LIBFILE) $(EXECS)

lib: $(LIBFILE)

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
	-rm -f $(EXECS) $(OBJS) $(LIBFILE) $(SRCDIR)/*~ $(INCDIR)/*~ *~

include .dependencies.$(ARCH).$(OS)

