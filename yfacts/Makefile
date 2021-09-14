CC := gcc
CFLAGS := -Os -N -D__USE_INLINE__
LIBS := -lauto

OBJS := yfacts.o timesync.o
DEPS := error.h timesync.h

all: yfacts

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

yfacts: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
