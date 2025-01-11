TOPDIR  = .
SRCDIR  = $(TOPDIR)/libs/Src
OBJDIR  = $(TOPDIR)/Obj
INCDIR1 = $(TOPDIR)/libs/Include
INCDIR2 = $(TOPDIR)/bin/include

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Common source files
COMMON_SRCFILES = $(wildcard $(SRCDIR)/*.c)

# Client source files
CLIENT_SRCFILES = $(TOPDIR)/client.c

# Server source files
SERVER_SRCFILES = $(COMMON_SRCFILES) $(TOPDIR)/server.c

# Object files
CLIENT_OBJFILES = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(CLIENT_SRCFILES)))
SERVER_OBJFILES = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SERVER_SRCFILES)))

CC ?= riscv32-poky-linux-gcc
CSTDFLAG = -std=gnu99

CFLAGS = -Wall -Wextra -DENABLE_RX_CH_REMAP -DENABLE_TX_CH_REMAP -DENABLE_FB_CH_REMAP 
IFLAGS = -I$(INCDIR1) -I$(INCDIR2) 

# Linker flags
LDFLAGS = -lnetconf2 -lm -lpthread -lsysrepo -l:libyang.so

# RISC-V specific flags
ifeq ($(CC), riscv32-poky-linux-gcc)
LDFLAGS_SERVER = $(LDFLAGS) -L$(TOPDIR)/riscv32/lib -lmxml
else
LDFLAGS_SERVER = $(LDFLAGS) -L$(TOPDIR)/x86/lib -lmxml
endif

CLIENT_TARGET = oru-cli
SERVER_TARGET = oru-cli-server

VPATH = $(SRCDIR) $(TOPDIR)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Server target
$(SERVER_TARGET): $(SERVER_OBJFILES)
	$(CC) $(CSTDFLAG) -o $@ $^ $(LDFLAGS_SERVER) 

# Client target
$(CLIENT_TARGET): $(CLIENT_OBJFILES)
	$(CC) $(CSTDFLAG) -o $@ $^ -lm 

$(OBJDIR)/%.o: %.c
	$(CC) $(CSTDFLAG) $(CFLAGS) $(IFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)
