include $(RPS_DIR)/Makefile.conf



ROOT = .

LIBDIR = $(ROOT)/lib/$(ARCH)/$(OS)
OBJDIR = $(ROOT)/obj/$(ARCH)/$(OS)
BINDIR = $(ROOT)/bin/$(ARCH)/$(OS)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

FFLAGS = -I$(INCDIR) 

ifeq ($(OPTIMIZE),YES)
  FFLAGS  += -O
endif

EXECSLIST = example test

EXECS = $(foreach m, $(EXECSLIST), $(BINDIR)/$(m))

LIBNAME = fd

LIB = $(LIBDIR)/lib$(LIBNAME).a

LIBOBJSLIST = fdcore.o fdsim.o fdhess.o fdout.o\
        fdmach.o fdmin.o fdlin.o fdgam.o fdran.o

MAINOBJSLIST = $(EXECSLIST:=.o)

OBJSLIST = $(LIBOBJSLIST) $(MAINOBJSLIST)

MAINOBJS = $(foreach m, $(MAINOBJSLIST), $(OBJDIR)/$(m))

LIBOBJS = $(foreach m, $(LIBOBJSLIST), $(OBJDIR)/$(m))

OBJS = $(LIBOBJS) $(MAINOBJS)

SRCS = $(foreach m, $(OBJSLIST:.o=.f), $(SRCDIR)/$(m))

all: $(LIB) $(EXECS)

$(BINDIR)/example: $(LIB) $(OBJDIR)/example.o
	f77 $(FFLAGS) $(OBJDIR)/example.o $(LIB) -o $(BINDIR)/example

$(BINDIR)/test: $(LIB) $(OBJDIR)/test.o
	f77 $(FFLAGS) $(OBJDIR)/test.o $(LIB) -o $(BINDIR)/test

$(LIB): $(LIBOBJS)
	ar ruv $(LIB) $(LIBOBJS)
	ranlib $(LIB)


$(OBJDIR)/%.o : $(SRCDIR)/%.f
	f77 $(FFLAGS) -c $< -o $(OBJDIR)/$(@F)

$(OBJDIR)/%.o : $(SRCDIR)/%.F
	f77 $(FFLAGS) -c $< -o $(OBJDIR)/$(@F)

clean:
	rm -f $(OBJS) $(EXECS) $(LIB)


depend:
	echo "No dependencies for fortran"


