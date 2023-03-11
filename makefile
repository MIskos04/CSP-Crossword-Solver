CC = gcc
EXEC = crossw
CFLAGS = -Werror -Wall -g -Wunused-variable
SRCS = arguments.o bitmap.o crossw.o dvo_fc_cbj_solver.o entities.o input_funcs.o


$(EXEC): $(SRCS)
	$(CC) $(SRCS) -o $(EXEC) 
	
clean:
	rm -f *.o
	rm -rf crossw