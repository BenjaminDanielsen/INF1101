## Authors: 
## Steffen Viken Valvaag <steffenv@cs.uit.no> 
## Morten Grønnesby <morten.gronnesby@uit.no>

LIST_SRC=linkedlist.c
SET_SRC=set.c   # Insert the file name of your set implementation here
SPAMFILTER_SRC=spamfilter.c common.c $(LIST_SRC) $(SET_SRC)
NUMBERS_SRC=numbers.c common.c $(LIST_SRC) $(SET_SRC)
ASSERT_SRC=assert_set.c common.c $(LIST_SRC) $(SET_SRC)
INCLUDE=include

NUMBERS_SRC:=$(patsubst %.c,src/%.c, $(NUMBERS_SRC))
SPAMFILTER_SRC:=$(patsubst %.c,src/%.c, $(SPAMFILTER_SRC))
ASSERT_SRC:=$(patsubst %.c,src/%.c, $(ASSERT_SRC))

CFLAGS=-Wall -Wextra -g -Wpedantic
LDFLAGS=-lm -DLOG_LEVEL=0 -DERROR_FATAL

all: spamfilter numbers

spamfilter: $(SPAMFILTER_SRC) Makefile
	gcc -o $@ $(CFLAGS) $(SPAMFILTER_SRC) -I$(INCLUDE) $(LDFLAGS)

numbers: $(NUMBERS_SRC) Makefile
	gcc -o $@ $(CFLAGS) $(NUMBERS_SRC) -I$(INCLUDE) $(LDFLAGS)

assert: $(ASSERT_SRC) Makefile
	gcc -o $@ $(CFLAGS) $(ASSERT_SRC) -I$(INCLUDE) $(LDFLAGS)

clean:
	rm -f *~ *.o *.exe spamfilter numbers assert
