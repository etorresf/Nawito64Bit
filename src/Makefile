# files,
EXE = Nawito-22.05_x64

OBJS = bitboard.o board.o book.o eval.o eval_param.o hash.o move.o move_gen.o \
       nawito.o search.o see.o square.o test.o time.o uci.o


# rules
all: $(EXE) .depend

clean:
	$(RM) *.o .depend gmon.out


# general
CC      = clang
LDFLAGS = -lm


# optimisation
CFLAGS += -O3


# warning
CFLAGS += -Wall
CFLAGS += -Wextra


# dependencies
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

.depend:
	$(CC) -MM $(OBJS:.o=.c) > $@

include .depend

