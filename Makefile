.PHONY: all debug release clean app

CC				=gcc

SRCS			=$(wildcard src/*.c)
OBJS			=$(SRCS:src/%.c=obj/%.o)

SRCDIR		=./src
OBJDIR		=./obj
BINDIR		=./bin

JOBS			=1

all: clean debug release
	@echo "**********************"
	@echo "* Successfully built *"
	@echo "**********************"

debug:
	@echo "************************"
	@echo "* Starting Debug Build *"
	@echo "************************"
	export CFLAGS="-g -DDEBUG -Wall" && export TARGET=debug && $(MAKE) app -j$(JOBS)
	@echo "*********************"
	@echo "* Debug Build Ended *"
	@echo "*********************"

release:
	@echo "**************************"
	@echo "* Starting Release Build *"
	@echo "**************************"
	export CFLAGS="-O2" && export TARGET=release && $(MAKE) app -j$(JOBS)
	@echo "***********************"
	@echo "* Release Build Ended *"
	@echo "***********************"

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$(notdir $@) $^

app: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BINDIR)/shell-$(TARGET)

clean:
	@echo "******* Starting Cleaning *******"
	rm -rf $(OBJDIR)/* $(BINDIR)/*
	@echo "******** Cleaning Ended *********"

#Dynamic
%:
	export $@ && $(MAKE) app -j$(JOBS)