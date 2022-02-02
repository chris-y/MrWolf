CC := gcc
CFLAGS := -Os -N -D__USE_INLINE__
LIBS := -lauto

OBJS := mrwolf.o timesync.o debug.o sntp.o tzlib.o
DEPS := error.h module.h timesync.h debug.h sntp.h tzlib.h

all: Mr.Wolf

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

Mr.Wolf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
