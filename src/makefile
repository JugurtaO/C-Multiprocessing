#compiler name
CC=gcc

#compiling flags
CFLAGS= -g -Wall -c 

#link edition flags
LDFLAGS= -lm 


#list of executable files
exec:main







#specifiyin the list of dependencies
table.o:table.c ../headers/table.h
main.o:main.c ../headers/table.h

#build object files from c source target
main:main.o table.o
%.o:%.c
	$(CC) $(CFLAGS) $<
	


#build executable from object files
%:%.o 
	$(CC) -o $@ $(LDFLAGS) $^

	
#clean command
clean:
	rm -rf *.o $(exec) 