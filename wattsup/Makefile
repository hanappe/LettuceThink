ifeq ($(mode),)
mode = debug
endif

ifeq ($(mode),release)
OPT= -O2 -g
endif
ifeq ($(mode),debug)
OPT= -O0 -g -D_DEBUG
endif

CC = gcc
CFLAGS += -std=gnu99 -Wall ${OPT}
CXX = g++
CXXFLAGS += -Wall ${OPT} ${CPDN_INC} ${DBUS_INC}
AR = ar

VERSION = 1.0

buildir = .

targets := wattsup 
objects := $(addprefix $(buildir)/,wattsup.o wu_termios.o Options.o Stats.o StatsUpdater.o)
depends := $(filter %.d,$(objects:.o=.d))

all: $(addprefix $(buildir)/,$(targets))

$(buildir)/wattsup: $(objects)
	$(CXX) $^ -o $@ 

depends: $(depends)

$(buildir)/%.d: %.c
	@mkdir -p `dirname $@`
	$(CC) -o $@ $(CFLAGS) $< -MM -MT '$@ $(buildir)/$*.o'

$(buildir)/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) -o $@ $(CFLAGS) $< -c

$(buildir)/%.d: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) -o $@ $(CXXFLAGS) $< -MM -MT '$@ $(buildir)/$*.o'

$(buildir)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) -o $@ $(CXXFLAGS) $< -c

clean:
	rm -f *.o *~
