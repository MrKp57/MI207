# Define directories

SRCDIR		= src
OBJDIR		= obj/release
BINDIR		= bin/release
OBJDIR_D	= obj/debug
BINDIR_D	= bin/debug

# Library targets

LIB_SRC	= $(SRCDIR)/functions.c

LIB_OBJ_C	= $(OBJDIR)/lib_c.o
LIB_OBJ_S	= $(OBJDIR)/lib_s.o
LIB_OBJ_C_D	= $(OBJDIR_D)/lib_c.o
LIB_OBJ_S_D	= $(OBJDIR_D)/lib_s.o

# Server targets

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

OBJS	= $(CLT_OBJ) $(SRV_OBJ) $(LIB_OBJ)
OUT		= $(CLT_OUT) $(SRV_OUT)
OBJS_D	= $(CLT_OBJ_D) $(SRV_OBJ_D) $(LIB_OBJ_D)
OUT_D	= $(CLT_OUT_D) $(SRV_OUT_D)

# Compiler options & flags

CC 		= gcc
CC_D    = $(CC) -D DEBUG

MKDIR_P = mkdir -p

FLAGS	= -g -c -Wall

# Target rules

all: debug release

force: clean all

debug: d_dir $(CLT_OUT_D) $(SRV_OUT_D)

release: r_dir $(CLT_OUT) $(SRV_OUT)

# Folder tree creation

r_dir: ${OBJDIR} ${BINDIR}
d_dir: ${OBJDIR_D} ${BINDIR_D}

${OBJDIR}:
	${MKDIR_P} ${OBJDIR}

${BINDIR}:
	${MKDIR_P} ${BINDIR}

${OBJDIR_D}:
	${MKDIR_P} ${OBJDIR_D}

${BINDIR_D}:
	${MKDIR_P} ${BINDIR_D}

# DEBUG COMPILING

$(CLT_OUT_D): $(CLT_OBJ_D) $(LIB_OBJ_C_D) $(LFLAGS)
	$(CC) -D DEBUG -g $(LIB_OBJ_C_D) $(CLT_OBJ_D) -o $(CLT_OUT_D)

$(SRV_OUT_D): $(SRV_OBJ_D) $(LIB_OBJ_S_D) $(LFLAGS)
	$(CC) -D DEBUG -g $(LIB_OBJ_S_D) $(SRV_OBJ_D) -o $(SRV_OUT_D)
	
$(CLT_OBJ_D): $(CLT_SRC)
	$(CC) -D DEBUG $(FLAGS) $(CLT_SRC) -o $(CLT_OBJ_D)

$(SRV_OBJ_D): $(SRV_SRC)
	$(CC) -D DEBUG $(FLAGS) $(SRV_SRC) -o $(SRV_OBJ_D)

$(LIB_OBJ_C_D): $(LIB_SRC)
	$(CC) -D DEBUG -D _CLIENT $(FLAGS) $(LIB_SRC) -o $(LIB_OBJ_C_D)

$(LIB_OBJ_S_D): $(LIB_SRC)
	$(CC) -D DEBUG -D _SERVER $(FLAGS) $(LIB_SRC) -o $(LIB_OBJ_S_D)

# RELEASE COMPILING

$(CLT_OUT): $(CLT_OBJ) $(LIB_OBJ_C) $(LFLAGS)
	$(CC) -g  $(LIB_OBJ_C) $(CLT_OBJ) -o $(CLT_OUT)

$(SRV_OUT): $(SRV_OBJ) $(LIB_OBJ_S) $(LFLAGS)
	$(CC) -g $(LIB_OBJ_S) $(SRV_OBJ) -o $(SRV_OUT)
	
$(CLT_OBJ): $(CLT_SRC)
	$(CC) $(FLAGS) $(CLT_SRC) -o $(CLT_OBJ)

$(SRV_OBJ): $(SRV_SRC)
	$(CC) $(FLAGS) $(SRV_SRC) -o $(SRV_OBJ)

$(LIB_OBJ_C): $(LIB_SRC)
	$(CC) -D _CLIENT $(FLAGS) $(LIB_SRC) -o $(LIB_OBJ_C)

$(LIB_OBJ_S): $(LIB_SRC)
	$(CC) -D _SERVER $(FLAGS) $(LIB_SRC) -o $(LIB_OBJ_S)

# Clean rule

clean:
	rm -rf bin obj