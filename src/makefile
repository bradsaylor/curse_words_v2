#taken from tutorial @ http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

IDIR =../include
CC=gcc
CFLAGS=-g -O0 -I$(IDIR) -lncurses

ODIR=../obj
LDIR=../lib

LIBS= 


_DEPS = menu_interface.h init.h display_interface.h editor_interface.h editor_int_input_handler.h editor_int_helper_funcs.h file_interface.h main_loop.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o menu_interface.o init.o display_interface.o editor_interface.o editor_int_input_handler.o editor_int_helper_funcs.o file_interface.o main_loop.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

cursewords: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	rm -f TAGS
	find .. -name "*.[ch]" -print | xargs etags -a
	cp -r ../../cursewords ~/programming/bkps/cursewords_bkp

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
