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

EXECSLIST = fd_example fd_test

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

$(BINDIR)/fd_example: $(LIB) $(OBJDIR)/fd_example.o
	$(F77) $(FFLAGS) $(OBJDIR)/fd_example.o $(FRACDIFF_LDFLAGS) -o $(BINDIR)/fd_example

$(BINDIR)/fd_test: $(LIB) $(OBJDIR)/fd_test.o
	$(F77) $(FFLAGS) $(OBJDIR)/fd_test.o $(FRACDIFF_LDFLAGS) -o $(BINDIR)/fd_test

$(LIB): $(LIBOBJS)
	ar ruv $(LIB) $(LIBOBJS)
	ranlib $(LIB)


$(OBJDIR)/%.o : $(SRCDIR)/%.f
	$(F77) $(FFLAGS) -c $< -o $(OBJDIR)/$(@F)

$(OBJDIR)/%.o : $(SRCDIR)/%.F
	$(F77) $(FFLAGS) -c $< -o $(OBJDIR)/$(@F)

clean:
	rm -f $(OBJS) $(EXECS) $(LIB)


depend:
	echo "No dependencies for fortran"


