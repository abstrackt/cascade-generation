IDIR =../include
CC=g++
CFLAGS=-I $(IDIR) -std=c++14

ODIR=obj

_DEPS = optimizer.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cascades.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cascades: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

$(shell mkdir -p $(ODIR))