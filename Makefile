CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

INCLUDEDIR = include
SRCDIR = src
LIBDIR = lib
TESTDIR = test
BUILDDIR = build

MAINSOURCE := $(SRCDIR)/balancer.cpp
MAINOBJECT := $(MAINSOURCE:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
MAINTARGET := $(MAINSOURCE:$(SRCDIR)/%.cpp=$(BUILDDIR)/%)

TESTSOURCES := $(shell find test -name '*.cpp')
TESTOBJECTS := $(patsubst $(TESTDIR)/%.cpp, $(BUILDDIR)/%.o, $(TESTSOURCES))
TESTTARGETS := $(patsubst $(TESTDIR)/%.cpp, $(BUILDDIR)/%, $(TESTSOURCES))

SOURCES := $(filter-out $(MAINSOURCE), $(shell find src -name '*.cpp'))
OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

LIBSOURCES := $(shell find lib -name '*.cpp')
LIBOBJECTS := $(patsubst $(LIBDIR)/%.cpp, $(BUILDDIR)/%.o, $(LIBSOURCES))


all: $(MAINTARGET)

tests: $(TESTTARGETS)

$(MAINTARGET): $(MAINOBJECT) $(OBJECTS) $(LIBOBJECTS)
	$(CXX) -o $@ $^

$(TESTTARGETS): $(BUILDDIR)/%: $(BUILDDIR)/%.o $(OBJECTS) $(LIBOBJECTS)
	$(CXX) -o $@ $^

$(MAINOBJECT): $(MAINSOURCE)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(TESTOBJECTS): $(BUILDDIR)/%.o: $(TESTDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(LIBOBJECTS): $(BUILDDIR)/%.o: $(LIBDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
