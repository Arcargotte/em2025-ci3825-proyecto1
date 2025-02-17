### FLAGS DE COMPILACION ####

CC=	gcc

### FLAGS PARA DEBUGGING ####
CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu2x -O0 -ggdb

### COMPILACION ###

PROG= 	simfs
SRC= 	simfs.c
LIBS= 

$(PROG): 	$(SRC)
		$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $@
clean: 
	rm -f $(PROG) *.dSYM *~
all: 	$(PROG)
