CC := gcc
CFLAGS := -Os -N -D__USE_INLINE__
LIBS := -lauto

OBJS := yfacts.o timesync.o debug.o sntp.o
DEPS := error.h module.h timesync.h debug.h sntp.h

all: yfacts

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

yfacts: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
