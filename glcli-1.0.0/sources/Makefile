TOPDIR  = .
SRCDIR  = $(TOPDIR)/libs/Src
OBJDIR  = $(TOPDIR)/Obj
INCDIR1 = $(TOPDIR)/libs/Include

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Common source files
COMMON_SRCFILES = $(wildcard $(SRCDIR)/*.c)

# Client source files
#CLIENT_SRCFILES = $(COMMON_SRCFILES) $(TOPDIR)/client.c
CLIENT_SRCFILES = $(TOPDIR)/client.c

# Server source files
SERVER_SRCFILES = $(COMMON_SRCFILES) $(TOPDIR)/server.c

# Object files
CLIENT_OBJFILES = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(CLIENT_SRCFILES)))
SERVER_OBJFILES = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SERVER_SRCFILES)))

CC ?= riscv32-poky-linux-gcc
CSTDFLAG = -std=gnu99

CFLAGS = -Wall -Wextra -DENABLE_RX_CH_REMAP -DENABLE_TX_CH_REMAP -DENABLE_FB_CH_REMAP 
IFLAGS = -I$(INCDIR1)

CLIENT_TARGET = oru-cli
SERVER_TARGET = oru-cli-server

VPATH = $(SRCDIR) $(TOPDIR)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Server target
$(SERVER_TARGET): $(SERVER_OBJFILES)
	$(CC) $(CSTDFLAG) -o $@ $^ -lm -lpthread

# Client target
$(CLIENT_TARGET): $(CLIENT_OBJFILES)
	$(CC) $(CSTDFLAG) -o $@ $^ -lm 

$(OBJDIR)/%.o: %.c
	$(CC) $(CSTDFLAG) $(CFLAGS) $(IFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)
