CC = gcc

CFLAGS = -Wall -Wextra -Wwrite-strings $(shell pkg-config gtk+-3.0 --cflags)

CPPFLAGS = -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE

LDFLAGS = $(shell pkg-config gtk+-3.0 --libs)

DEBUG = -ggdb

SRCS = \
dialog.c \
interface.c \
main.c \
menu.c \
monitor.c \
settings.c \
window.c

OBJS = $(SRCS:.c=.o)

TARGETS = monitor

%.o: %.c
	$(CC) $(DEBUG) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $<

all: $(TARGETS)

monitor: $(OBJS)
	$(CC) $(DEBUG) $(LDFLAGS) -o $@ $^

clean:
	-$(RM) $(OBJS) $(TARGETS) >/dev/null 2>&1

CHK_SOURCES = $(SRCS) $(SRCS:.c=.h)
check-syntax:
	$(CC) $(CFLAGS) $(CPPFLAGS) -fsyntax-only $(CHK_SOURCES)

.PHONY: clean check-syntax
