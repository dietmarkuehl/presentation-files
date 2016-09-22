COMPILER = gcc
STD      = 14
CPPFLAGS += -std=c++$(STD)
MAP      = 2
SIZE     = 100
# CPPFLAGS += -DMAP=make_map$(MAP)  -DSIZE=$(SIZE)

ifeq ($(COMPILER),gcc)
    DIR = /opt/gcc-current
    CXX = $(DIR)/bin/g++
    CPPFLAGS += -W -Wall
endif
ifeq ($(COMPILER),clang)
    CXX = /opt/llvm-current/bin/clang++
    CPPFLAGS += -W -Wall -ftemplate-depth=8192
endif
ifeq ($(COMPILER),intel)
    CXX = /opt/intel/bin/icc
    CPPFLAGS += -W -Wall
endif

BINDIR = bin/$(COMPILER)-$(STD)

NAME = constexpr-rounding
NAME = constexpr-string
NAME = constexpr-map
NAME = constexpr-switch
NAME = constexpr-bitmap
NAME = constexpr-array
NAME = constexpr-initlist
NAME = make_array
NAME = constexpr-mergesort
NAME = constexpr-sort
NAME = constexpr-perf

CXXFILES = $(NAME).cpp

default: $(BINDIR)/$(NAME).tsk
	$(BINDIR)/$(NAME).tsk

assembler: $(BINDIR)/$(NAME).s

perf:
	for f in 0 1 2 3; \
	do \
		for s in 10 20 40 50 100 150 200 250 300 350; \
		do \
			echo map=$$f size=$$s; \
			time $(CXX) -c constexpr-perf.cpp $(CPPFLAGS) $(CXXFLAGS) -o $(BINDIR)/constexpr-perf.o -DMAP=make_map$$f -DSIZE=$$s; \
		done; \
	done

$(BINDIR)/$(NAME).tsk: $(BINDIR)/$(NAME).o
	$(CXX) -o $@ $(@:%.tsk=%.o)

$(BINDIR)/%.o: %.cpp
	@mkdir -p $(BINDIR)
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $(@:$(BINDIR)/%.o=%.cpp)

$(BINDIR)/%.s: %.cpp
	@mkdir -p $(BINDIR)
	$(CXX) -S -o $@ $(CPPFLAGS) $(CXXFLAGS) $(@:$(BINDIR)/%.s=%.cpp)

.PHONY: depend
.PHONY: make.depend
$(BINDIR)/make.depend depend:
	@mkdir -p $(BINDIR)
	$(CXX) -M $(CPPFLAGS) $(CXXFILES) | sed -e 's@^\(.*\.o:\)@$(BINDIR)/\1@' > $(BINDIR)/make.depend

clean:
	$(RM) -rf $(BINDIR)
	$(RM) mkerr olderr *~

include $(BINDIR)/make.depend
