CC = $(shell which gcc || which clang || which cc)
AR = ar
ARFLAGS = rcs
LIBNAME_STATIC = libgt.a
LIBNAME_SHARED = libgt.0.so

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

all: directories $(DESTDIR)/$(LIBNAME_STATIC) $(DESTDIR)/$(LIBNAME_SHARED)
	rm -rf $(OBJ_DIR)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DESTDIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -fPIC -c $< -o $@

$(DESTDIR)/$(LIBNAME_STATIC): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

$(DESTDIR)/$(LIBNAME_SHARED): $(OBJECTS)
	$(CC) -shared -o $@ $^

clean:
	@rm -rf $(OBJ_DIR) $(DESTDIR)/$(LIBNAME_STATIC) $(DESTDIR)/$(LIBNAME_SHARED)

install: $(DESTDIR)/$(LIBNAME_STATIC) $(DESTDIR)/$(LIBNAME_SHARED)
	install -d $(LIBDIR)
	install -m 644 $(DESTDIR)/$(LIBNAME_STATIC) $(LIBDIR)
	install -m 644 $(DESTDIR)/$(LIBNAME_SHARED) $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -m 644 include/*.h $(INCLUDEDIR)

.PHONY: all clean install directories
