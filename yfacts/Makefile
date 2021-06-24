CC := gcc
CFLAGS := -Os -N
LIBS := -lauto

OBJS := yfacts.o

all: yfacts

$(OBJS): $(OBJS:.o=.c)
	$(CC) $(CFLAGS) -c -o $@ $^

yfacts: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
