include $(RPS_DIR)/Makefile.conf

ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

EXECSLIST = test_getloadavg

EXECS = $(foreach m, $(EXECSLIST), $(BINDIR)/$(m))

LIBNAME = getloadavg

LIB = $(LIBDIR)/lib$(LIBNAME).a

LIBOBJSLIST = getloadavg.o

MAINOBJSLIST = $(EXECSLIST:=.o)

OBJSLIST = $(LIBOBJSLIST) $(MAINOBJSLIST)

MAINOBJS = $(foreach m, $(MAINOBJSLIST), $(OBJDIR)/$(m))

LIBOBJS = $(foreach m, $(LIBOBJSLIST), $(OBJDIR)/$(m))

OBJS = $(LIBOBJS) $(MAINOBJS)

SRCS = $(foreach m, $(OBJSLIST:.o=.c), $(SRCDIR)/$(m))


CFLAGS = -I$(INCDIR) 

ifeq ($(OPTIMIZE),YES)
   CFLAGS += -O
else
   CFLAGS += -g -gstabs+ -ggdb
endif
ifeq ($(USE_UPTIME),YES) 
   CFLAGS += -DLDAV_USE_UPTIME
endif
ifeq ($(OPTIMIZE),YES)
   LD1FLAGS = -O
else
   LD1FLAGS = -g -gstabs+ -ggdb
endif
LD1FLAGS += -fPIC -L$(LIBDIR) -l$(LIBNAME)
LD2FLAGS =
ifeq ($(OS),SOLARIS)
  LD3FLAGS = -lkvm -lelf
else
  LD3FLAGS = 
endif

all: $(LIB) $(EXECS)

$(LIB): $(LIBOBJS)
	$(AR) ruv $(LIB)  $(LIBOBJS)


ifeq ($(OS),FREEBSD)
  define MakeExeAction
    $(LD) $(LD1FLAGS) $< $(LD2FLAGS) $(LD3FLAGS) -o $(BINDIR)/$(@F)
  endef
else
  define MakeExeAction
    $(LD) $< $(LD1FLAGS) $(LD2FLAGS) $(LD3FLAGS) -o $(BINDIR)/$(@F)
  endef
endif


$(EXECS):$(BINDIR)/%:$(OBJDIR)/%.o 
	$(MakeExeAction)


$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(@F)

.dependencies.$(ARCH).$(OS) : $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

depend: Makefile $(SRCS)
	$(CC) $(CFLAGS) -MM $(SRCS) | awk '/:/ {printf "%s", "$(OBJDIR)/"} {print}' >  .dependencies.$(ARCH).$(OS)

clean:
	-rm -f $(LIBDIR)/*.a $(OBJDIR)/*.o $(BINDIR)/*

include $(ROOT)/.dependencies.$(ARCH).$(OS)
