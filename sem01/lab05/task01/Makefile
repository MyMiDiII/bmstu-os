INCDIR := ./inc/
SRCDIR := ./src/
OUTDIR := ./out/
UNITDIR := ./unit_tests/

vpath %.c $(SRCDIR) $(UNITDIR)

CC := gcc
PATHFLAG := -I$(INCDIR)
CFLAGS := -std=c99 $(PATHFLAG) -Wall -Werror
OBGSFLAGS := -lcheck -lpthread -lrt

SRCS := $(wildcard $(SRCDIR)*.c)
OBGS := $(patsubst $(SRCDIR)%.c,$(OUTDIR)%.o,$(SRCS))

TEST_SRCS := $(wildcard $(UNITDIR)*.c)
ALL_OBGS := $(patsubst $(UNITDIR)%.c,$(OUTDIR)%.o,$(TEST_SRCS)) $(OBGS)
DEPS := $(patsubst %.o,%.d,$(ALL_OBGS))
TEST_OBGS := $(filter-out $(OUTDIR)main.o,$(ALL_OBGS))

.PHONY : clean debug release

debug : CFLAGS += -DDEBUG -g3
debug : app

release : clean app

app : $(OBGS) 
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
