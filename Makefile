CC = clang
CXX = clang++
CFLAGS = # -Wall -Wextra -Werror -MMD -MP
LDFLAGS = -lraylib

COMPONENT_DIR = Components

CSOURCES = $(wildcard *.c)
MSOURCES = $(wildcard *.m)
CPPSOURCES = $(wildcard $(COMPONENT_DIR)/*.c)
CUTILSOURCES = $(wildcard utils/*.c)
CPPUTILSOURCES = $(wildcard utils/*.cpp)
LIBCPPSOURCES = $(wildcard lib/*.cpp)
COBJS = $(patsubst %.c, %.o, $(CSOURCES))
MOBJS = $(patsubst %.m, %.o, $(MSOURCES))
COMPONENTOBJS = $(patsubst $(COMPONENT_DIR)/%.c, $(COMPONENT_DIR)/%.o, $(CPPSOURCES))
CUTILOBJS = $(patsubst utils/%.c, utils/%.o, $(CUTILSOURCES))
CPPUTILOBJS = $(patsubst utils/%.cpp, utils/%.o, $(CPPUTILSOURCES))
LIBCPPOBJS = $(patsubst lib/%.cpp, lib/%.o, $(LIBCPPSOURCES))

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDFLAGS += -framework Cocoa
endif

all: monkey

monkey: $(COBJS) $(MOBJS) $(COMPONENTOBJS) $(CUTILOBJS) $(CPPUTILOBJS) $(LIBCPPOBJS)
	$(CXX) -o monkey $(COBJS) $(MOBJS) $(COMPONENTOBJS) $(CUTILOBJS) $(CPPUTILOBJS) $(LIBCPPOBJS) $(LDFLAGS)
	rm -f $(COBJS) $(MOBJS) $(COMPONENTOBJS) $(CUTILOBJS) $(CPPUTILOBJS) $(LIBCPPOBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.m
	$(CC) $(CFLAGS) -c $< -o $@

$(COMPONENT_DIR)/%.o: $(COMPONENT_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

utils/%.o: utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

utils/%.o: utils/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

lib/%.o: lib/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(COBJS) $(MOBJS) $(COMPONENTOBJS) $(CUTILOBJS) $(CPPUTILOBJS) $(LIBCPPOBJS)

run:
	./monkey