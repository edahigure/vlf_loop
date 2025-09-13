# Compiladores
CC      = gcc
FC      = gfortran
CXX     = g++

# Flags de compilación
CFLAGS   = -std=c99 -O2 -Wall
FFLAGS   = -O3 -static  # ¡ESTÁTICO para Fortran!
CXXFLAGS = -std=c++0x -O2 -Wall
LDFLAGS  = -static      # ¡Enlace estático para C/C++!

# Programas a compilar
ALL = bin2sac FKRPROG makeascii sac2bin waveint window

all: $(ALL)

# Regla general para programas C
%: %.c getpar.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Reglas específicas para casos especiales
bin2sac: bin2sac.c getpar.c
	gcc -std=c99 -O2 -Wall -fstack-protector-strong -D_FORTIFY_SOURCE=2 bin2sac.c getpar.c -o bin2sac

makeascii: makeascii.c getpar.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

sac2bin: sac2bin.c getpar.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

window: window.c getpar.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Programas Fortran
FKRPROG: FKRPROG.f
	$(FC) $(FFLAGS) $^ -o $@

waveint: wvint9.f cwrite.o
	$(CC) -c cwrite.c
	$(FC) $(FFLAGS) wvint9.f cwrite.o -o waveint

# Utilidades
clean:
	rm -f $(ALL) *.o

install: all
	mkdir -p ../bin
	cp $(ALL) ../bin/

.PHONY: all clean install



