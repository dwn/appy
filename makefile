ODIR=bin
LDIR=
IDIR=appy
IDIR2=appy-app
CC=emcc
CFLAGS=-I$(IDIR) -I$(IDIR2) -O3 -Wno-shift-negative-value
VERSION=$(shell date -u +%Y.%m.%d)

SRC_FILES := $(wildcard $(IDIR)/*.cpp)
OBJ_FILES := $(patsubst $(IDIR)/%.cpp,$(ODIR)/%.o,$(SRC_FILES))

SRC_FILES2 := $(wildcard $(IDIR2)/*.cpp)
HTML_FILES := $(patsubst $(IDIR2)/%.cpp,$(ODIR)/%.html,$(SRC_FILES2))

.PHONY: clean directories version all

all: clean directories version $(OBJ_FILES) $(HTML_FILES)

###########################
# Make clean
###########################

clean:
	rm -f $(ODIR)/*

###########################
# Make directories
###########################

MKDIR_P = mkdir -p

directories: $(ODIR)

$(ODIR):
	$(MKDIR_P) $(ODIR)

###########################
# Make version file
###########################

version:
	echo "#define APP_VERSION \"$(VERSION)\"" > $(IDIR)/Version.h
	echo " * Version $(VERSION)"

###########################
# Make library
###########################

$(ODIR)/%.o: $(IDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

##########################
# Make apps
##########################

$(ODIR)/%.html: $(IDIR2)/%.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(OBJ_FILES) -s FETCH=1