include $(RPS_DIR)/Makefile.conf


ROOT = .

LIBDIR = $(ROOT)
OBJDIR = $(ROOT)
BINDIR = $(ROOT)
SRCDIR = $(ROOT)
INCDIR = $(ROOT)


LIBNAME = LineGraph
LIBFILE = $(LIBDIR)/lib$(LIBNAME).so


CFLAGS = -I$(INCDIR) \
         $(GETLOADAVG_CFLAGS) \
         $(GETFLOWBW_CFLAGS) \
         $(TIMESERIES_CFLAGS) \
	 $(WAVELETS_CFLAGS) \
         $(FRACDIFF_CFLAGS) \
         $(RECIPES_CFLAGS) \
         $(RPSINT_CFLAGS) \
         $(MIRROR_CFLAGS) \
         $(REMOS_MODELER_CFLAGS) \
         $(LISTPACK_CFLAGS) \
         $(EXPAT_CFLAGS)\
         $(JDK_CFLAGS)


ifeq ($(OPTIMIZE),YES)
  CFLAGS +=  -O -fPIC
  LD1FLAGS = -O -shared -static -fPIC
else
  CFLAGS += -g -gstabs+ -ggdb -fPIC
  LD1FLAGS = -g  -gstabs+ -ggdb  -shared -static -fPIC
endif

LD1FLAGS+=  $(GETLOADAVG_LDFLAGS) \
            $(GETFLOWBW_LDFLAGS)\
            $(REMOS_MODELER_LDFLAGS) \
            $(LISTPACK_LDFLAGS)
LD2FLAGS =  $(RPSINT_LDFLAGS) \
            $(TIMESERIES_LDFLAGS) \
            $(WAVELETS_LDFLAGS) \
            $(FRACDIFF_LDFLAGS)  \
            $(RECIPES_LDFLAGS) \
            $(MIRROR_LDFLAGS) \
            $(EXPAT_LDFLAGS) \
            -lm

ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lsocket -lkvm -lelf
else
  LD3FLAGS = 
endif


LIBOBJS = LineGraph.o

SRCS = $(LIBOBJS:.o=.cpp)

all: $(LIBFILE) LineGraph.class #LineGraphApp.class 

$(LIBFILE) : $(LIBOBJS)
	$(CC) $(LIBOBJS) $(LD1FLAGS) $(LD2FLAGS) $(LD3FLAGS) -o $(LIBFILE)


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(@F)

LineGraph.h : LineGraph.class
	javah -jni LineGraph

#LineGraphApp.class : LineGraphApp.java LineGraph.class
#	javac LineGraphApp.java

LineGraph.class : LineGraph.java
	javac LineGraph.java


.dependencies.$(ARCH).$(OS) : $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

depend: Makefile $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

clean: 
	-rm -f $(EXECS) $(OBJS) $(LIBFILE) LineGraphApp.class libLineGraphApp.so LineGraphApp.o LineGraph.class libLineGraph.so LineGraph.o


include .dependencies.$(ARCH).$(OS)
