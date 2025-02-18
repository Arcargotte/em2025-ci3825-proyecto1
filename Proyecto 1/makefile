### FLAGS DE COMPILACION ####

CC=	gcc

### FLAGS PARA DEBUGGING ####
CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu23 -O0 -ggdb

### COMPILACION ###

PROG= 	simfs
SRC= 	proyecto.c node.c
### LIBS= 	

$(PROG): 	$(SRC)
		$(CC) $(CFLAGS) $(SRC) -o $@
%.o: %.c $(LIBS)
	$(CC) $(CFLAGS) -c $< -o $@
clean: 
	rm -f $(PROG) *.dSYM *~
	rm -f *.o mi_programa

all: 	$(PROG)
