#*****************************************************************************
#                               ___
#                        __    |   |    __
#                   _   |  |   |   |   |  |   _
#               _._| |__|  |___|   |___|  |__| |_._
#
#                 Fast Frequency Function Generator
#               _ _   __    ___     ___    __   _ _
#                ' |_|  |  |   |   |   |  |  |_| '
#                       |__|   |   |   |__|
#                              |___|
#
#    Copyright 2011 Marcus Jansson <mjansson256@yahoo.se>
#
#
#    This is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this.  If not, see <http://www.gnu.org/licenses/>.
#*****************************************************************************
# Tab size: 4

export

##############################################################
#The target board
##############################################################
ARCH = avr
BOARD = breadboard

##############################################################
#The target MPU
##############################################################
PART_AVRDUDE = t45
PART=attiny45

##############################################################
#Binaries are located in this dir
##############################################################
BINDIR = bin
ELFDIR = bin


##############################################################
# The name of the program, elf and binary
##############################################################
PROGRAM = f3g
ELF = $(ELFDIR)/$(PROGRAM).elf
BINARY = $(BINDIR)/$(PROGRAM).bin


##############################################################
#Sources are located in this dir
##############################################################
SOURCEDIR = src

##############################################################
#Various source subdirectories
##############################################################
STARTUPDIR = $(SOURCEDIR)/cpu/$(ARCH)
CPUDIR = $(SOURCEDIR)/cpu/$(PART)
INCDIR = $(SOURCEDIR)/include
KERNELDIR = $(SOURCEDIR)/kernel
SYSTEMDIR = $(SOURCEDIR)/system
DRIVERSDIR = $(SOURCEDIR)/drivers
FILESYSTEMDIR = $(DRIVERSDIR)/filesystem
FATDIR = $(FILESYSTEMDIR)/fat
APPDIR = $(SOURCEDIR)/app


##############################################################
#The assembler sources
##############################################################

##############################################################
#The CPU peripheral sources
##############################################################
SOURCE += $(CPUDIR)/timer.c
SOURCE += $(CPUDIR)/gpio.c
SOURCE += $(CPUDIR)/pm.c
SOURCE += $(CPUDIR)/adc.c
#SOURCE += $(CPUDIR)/eeprom.c
#SOURCE += $(CPUDIR)/spi.c
#SOURCE += $(CPUDIR)/twi.c
#SOURCE += $(CPUDIR)/usart.c
#SOURCE += $(CPUDIR)/wdt.c


##############################################################
#The driver sources
##############################################################
#SOURCE += $(DRIVERSDIR)/lcd.c
SOURCE += $(DRIVERSDIR)/led.c


##############################################################
SOURCE += $(SOURCEDIR)/main.c


##############################################################
#Header files are located in these files
##############################################################
INCDIRS = -I$(SOURCEDIR) -I$(INCDIR) -I$(INCDIR)/cpu/$(PART)


##############################################################
#Various build programs
##############################################################
CC = avr-gcc
LD = avr-ld
AS = avr-as
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
MKDIR = mkdir
TEST = test
LESS = less
ECHO = echo


##############################################################
#JTAG tool which will be used
##############################################################
PROGRAMMER = avrdude
PROGRAMMERTOOL = stk500v2
PROGRAMMERTOOL_DRAGON = dragon_isp
#JTAGTOOL = jtagicemkii


##############################################################
#Various compile flags etc
##############################################################
#~ LDSCRIPT = $(STARTUPDIR)/linkscript/$(PART).lds

DEBUG = -ggdb
OPT = s

#AFLAGS = -x assembler-with-cpp


CFLAGS += -O${OPT} -mmcu=$(PART) -D$(BOARD) -DBOARD=$(BOARD)
CFLAGS += $(INCDIRS)
#~ CFLAGS = -c
#~ CFLAGS += $(DEBUG) -DO$(OPT)
#~ CFLAGS += -Wall -Wa,-R
#~ CFLAGS += -msoft-float
#~ CFLAGS += -fomit-frame-pointer
#~ CFLAGS += -mrelax
#~ CFLAGS += -fno-common
#~ CFLAGS += -fsection-anchors
#~ CFLAGS += -fdata-sections
#~ CFLAGS += -ffunction-sections
#~ CFLAGS += -mno-use-rodata-section
#~ CFLAGS += -std=gnu99
#~ CFLAGS += -pedantic


##############################################################
#Various link flags etc
##############################################################
#~ LDFLAGS = -Wl,--gc-sections
#~ LDFLAGS += -Wl,--relax
#~ LDFLAGS += -Wl,--direct-data
#~ LDFLAGS += -nostartfiles
#~ LDFLAGS += -mpart=$(PART)
#~ LDFLAGS += -T$(LDSCRIPT)


##############################################################
#The object files
##############################################################
#TODO: Not ok yet. Please, only use BUILDDIR = .
BUILDDIR = .
#~ OBJ = $(ASMSOURCE:%.S=%.o)
#~ OBJ += $(SOURCE:%.c=%.o)
#~ OBJ += $(SYSTEMSOURCE:%.c=%.o)
#~ OBJ += $(APPSOURCE:%.c=%.o)


##############################################################
#Misc. half-unncessary niceness thingies
##############################################################
TEXTPRECOMPILE = @$(ECHO) -n "---  $< "
TEXTPOSTCOMPILE = @$(ECHO) -e "\r[OK]"


##############################################################
#Makefile rules
##############################################################
.PHONY: all
all: makeSureWeAlwaysLink PrintBoardInfo compile ${ELF} ok size crlf

compile:
	@$(TEST) -d $(BINDIR) || $(MKDIR) -p $(BINDIR)
	$(CC) $(CFLAGS) $(SOURCE) -o${ELF}
	$(OBJCOPY) -O binary $(ELF) $(BINARY)
Print:
	@$(ECHO) $(OBJ)

ok:
	@$(ECHO) -e "\r[ALL OK]"

crlf:
	@$(ECHO)

PrintBoardInfo:
	@echo "================================"
	@echo Compiling $(PROGRAM) with -O$(OPT)
	@echo BOARD = $(BOARD)
	@echo PART = $(PART)
	@echo "================================"

#TODO: This should be removed in the final release
makeSureWeAlwaysLink:
	rm -f $(PROGRAM).elf

%.pajo: %.S
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) $(AFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

%.pajo: %.c
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

$(KILLED_BINARY): ./$(PROGRAM).elf
	@$(TEST) -d $(BINDIR) || $(MKDIR) -p $(BINDIR)
	$(OBJCOPY) -O binary $(ELF) $(BINARY)

$(KILLED_ELF): $(OBJ)
	@$(TEST) -d $(ELFDIR) || $(MKDIR) -p $(ELFDIR)
	@echo
	@echo "Linking, using $(LDSCRIPT)"
	@$(ECHO) -n "...  Linking $(ELF)"
	@$(CC) $(LDFLAGS) $(OBJ:%=$(BUILDDIR)/%) -o $(ELF)
	$(TEXTPOSTCOMPILE)


.PHONY: program
program: $(BINARY)
	$(PROGRAMMER) -p$(PART_AVRDUDE) -c${PROGRAMMERTOOL} -e -V -P/dev/ttyUSB0 -v -B 15 -U flash:w:$(BINARY)

.PHONY: dragon
dragon: $(BINARY)
	$(PROGRAMMER) -p$(PART_AVRDUDE) -c${PROGRAMMERTOOL_DRAGON} -e -V -Pusb -v -U flash:w:$(BINARY)

.PHONY: readeeprom
readeeprom: $(BINARY)
	$(PROGRAMMER)  -p${PART} -c${PROGRAMMERTOOL} -PUSB -v eeprom:r:/tmp/eeprom.hex

.PHONY: reset
reset:
	$(PROGRAMMER) reset

.PHONY: run
run:
	$(PROGRAMMER) run

.PHONY: cpuinfo
cpuinfo:
	$(PROGRAMMER) cpuinfo

.PHONY: size
size: $(BINARY)
	@$(SIZE) --target=binary $(BINARY)

.PHONY: gdb
gdb:
	avr32gdbproxy -k -a localhost:4711 -cUSB -e$(JTAGTOOL)

.PHONY: kill
kill:
	killall avr32gdbproxy

.PHONY: killall
	killall avr32gdbproxy

.PHONY: dump
dump:
	$(OBJDUMP) -S -x $(ELF)|less

.PHONY: remake
remake:	clean all

.PHONY: semiclean
semiclean:
	rm -f $(BINARY) $(ELF)

.PHONY: doxygen
doxygen:
	doxygen

.PHONY: clean
clean:
	rm -f $(OBJ) $(ELF) $(BINARY)
