SRCDIR	= src
OBJDIR	= obj
BINDIR	= bin

SRV_OBJ	= $(OBJDIR)/server.o
SRV_SRC	= $(SRCDIR)/server.c
SRV_OUT	= $(BINDIR)/server

CLT_OBJ	= $(OBJDIR)/client.o
CLT_SRC	= $(SRCDIR)/client.c
CLT_OUT = $(BINDIR)/client

OBJS	= $(CLT_OBJ) $(SRV_OBJ)
OUT		= $(CLT_OUT) $(SRV_OUT)


CC_D    = $(CC) -D DEBUG

FLAGS	= -g -c -Wall

#debug:
#	CC = gcc -D debug
#	make all
	
all: $(SRV_OUT) $(CLT_OUT)

	
$(CLT_OUT): $(CLT_OBJ) $(LFLAGS)
	$(CC) -g $(CLT_OBJ) -o $(CLT_OUT)

$(SRV_OUT): $(SRV_OBJ) $(LFLAGS)
	$(CC) -g $(SRV_OBJ) -o $(SRV_OUT)
	
$(CLT_OBJ): $(CLT_SRC)
	$(CC) $(FLAGS) $(CLT_SRC) -o $(CLT_OBJ)

$(SRV_OBJ): $(SRV_SRC)
	$(CC) $(FLAGS) $(SRV_SRC) -o $(SRV_OBJ)

clean:
	rm -f $(OBJS) $(OUT)