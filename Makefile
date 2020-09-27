# -*- mode: makefile; -*-

OSNAME := $(shell uname)

WARNINGS =                                      \
	-Wno-unused-function                    \
	-Wno-deprecated-declarations            \
	-Wno-implicit-fallthrough

CXXFLAGS = -g -O -std=c++2a -W -Wall $(WARNINGS)

CPPFLAGS = -I.
LDFLAGS =

LIBS = -lboost_unit_test_framework -lboost_iostreams

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

TARGETS = $(patsubst %.o,%,$(OBJS))

all: $(TARGETS)

DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(DEPS)

$(DEPENDDIR)/%.d: %.cc $(DEPENDDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

%: %.cc

%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

%: %.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -rf $(TARGETS) $(OBJS)

realclean:
	@rm -rf $(TARGETS) $(OBJS) $(DEPENDDIR)

check: $(TARGETS)
	@for f in *_test; do echo -e "\n$$f:\n"; ./"$$f"; done
