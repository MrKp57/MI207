# Define directories

SRCDIR		= src
OBJDIR		= obj/release
BINDIR		= bin/release
OBJDIR_D	= obj/debug
BINDIR_D	= bin/debug

# Client targets

SRV_SRC	= $(SRCDIR)/server.c

SRV_OBJ		= $(OBJDIR)/server.o
SRV_OUT		= $(BINDIR)/server
SRV_OBJ_D	= $(OBJDIR_D)/server.o
SRV_OUT_D	= $(BINDIR_D)/server

# Client targets

CLT_SRC		= $(SRCDIR)/client.c

CLT_OBJ		= $(OBJDIR)/client.o
CLT_OUT		= $(BINDIR)/client
CLT_OBJ_D	= $(OBJDIR_D)/client.o
CLT_OUT_D	= $(BINDIR_D)/client

# Common targets

OBJS	= $(CLT_OBJ) $(SRV_OBJ)
OUT		= $(CLT_OUT) $(SRV_OUT)
OBJS_D	= $(CLT_OBJ_D) $(SRV_OBJ_D)
OUT_D	= $(CLT_OUT_D) $(SRV_OUT_D)

# Compiler options & flags

CC 		= gcc
CC_D    = $(CC) -D DEBUG

FLAGS	= -g -c -Wall

# Target rules

all: $(SRV_OUT) $(CLT_OUT) $(SRV_OUT_D) $(CLT_OUT_D)

debug: $(SRV_OUT_D) $(CLT_OUT_D)

release: $(SRV_OUT) $(CLT_OUT)

# DEBUG COMPILING

$(CLT_OUT_D): $(CLT_OBJ_D) $(LFLAGS)
	$(CC_D) -g $(CLT_OBJ_D) -o $(CLT_OUT_D)

$(SRV_OUT_D): $(SRV_OBJ_D) $(LFLAGS)
	$(CC_D) -g $(SRV_OBJ_D) -o $(SRV_OUT_D)
	
$(CLT_OBJ_D): $(CLT_SRC)
	$(CC_D) $(FLAGS) $(CLT_SRC) -o $(CLT_OBJ_D)

$(SRV_OBJ_D): $(SRV_SRC)
	$(CC_D) $(FLAGS) $(SRV_SRC) -o $(SRV_OBJ_D)

# RELEASE COMPILING

$(CLT_OUT): $(CLT_OBJ) $(LFLAGS)
	$(CC) -g $(CLT_OBJ) -o $(CLT_OUT)

$(SRV_OUT): $(SRV_OBJ) $(LFLAGS)
	$(CC) -g $(SRV_OBJ) -o $(SRV_OUT)
	
$(CLT_OBJ): $(CLT_SRC)
	$(CC) $(FLAGS) $(CLT_SRC) -o $(CLT_OBJ)

$(SRV_OBJ): $(SRV_SRC)
	$(CC) $(FLAGS) $(SRV_SRC) -o $(SRV_OBJ)

# Clean rules

clean:
	rm -f $(OBJS) $(OUT) $(OBJS_D) $(OUT_D)