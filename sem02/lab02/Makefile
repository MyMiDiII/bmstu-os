INCDIR := ./inc/
SRCDIR := ./src/
OUTDIR := ./out/
UNITDIR := ./unit_tests/

vpath %.c $(SRCDIR) $(UNITDIR)

CC := gcc
PATHFLAG := -I$(INCDIR)
CFLAGS := $(PATHFLAG) -Wall
OBGSFLAGS := -lcheck -lpthread -lrt

SRCS := main.c $(wildcard $(SRCDIR)*.c)
OBGS := $(patsubst $(SRCDIR)%.c,$(OUTDIR)%.o,$(SRCS))

.PHONY : clean debug release

debug : CFLAGS += -DDEBUG -g3
debug : MYFTW
	./MYFTW .

release : clean app.exe

MYFTW : $(OBGS) 
	$(CC) $(CFLAGS) $^ -o $@

$(OUTDIR)%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)%.d : %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(PATHFLAG) -M $< | sed "s^$*.o^$(OUTDIR)$*.o^" > $@

unit_tests.exe : $(TEST_OBGS) 
	$(CC) -o $@ $^ $(OBGSFLAGS) 

include $(DEPS)

func : release
	func_tests/testing.sh

clean :
	rm -f *.exe
	rm -fr $(OUTDIR)
	rm MYFTW
