CC = $(shell which gcc || which clang || which cc)
AR = ar
ARFLAGS = rcs
LIBNAME = libgt.a

DESTDIR := lib/
CFLAGS := -Wall -Wextra -O3

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj

ifeq ($(OS),Windows_NT)
	SOURCES = $(SRC_DIR)/gt.c $(SRC_DIR)/font_iso10646_9x18.c $(SRC_DIR)/_win.c
else
    SOURCES = $(SRC_DIR)/gt.c $(SRC_DIR)/font_iso10646_9x18.c $(SRC_DIR)/_linux.c
endif

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: directories $(DESTDIR)/$(LIBNAME)
	rm -rf $(OBJ_DIR)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DESTDIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(DESTDIR)/$(LIBNAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

clean:
	@rm -rf $(OBJ_DIR) $(DESTDIR)/$(LIBNAME)

install: $(DESTDIR)/$(LIBNAME)
	install -d $(LIBDIR)
	install -m 644 $(DESTDIR)/$(LIBNAME) $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -m 644 include/*.h $(INCLUDEDIR)

.PHONY: all clean install directories
