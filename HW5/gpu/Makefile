CC=nvcc
CFLAG=--compiler-options "-Wall -Werror"

OS=$(shell uname)
ifeq ($(OS), Darwin)    # Mac
LFLAG=-framework OpenGL -framework GLUT
else                    # Linux
LFLAG=-lglut -lGLU -lGL
endif

HDR=$(wildcard *.h)
CCSRC=$(wildcard *.cc)
CUSRC=$(wildcard *.cu)
CCOBJ=$(CCSRC:.cc=.o)
CUOBJ=$(CUSRC:.cu=_cuda.o)

EXE=mandelbrot

.PHONY: clean

$(EXE): $(CCOBJ) $(CUOBJ)
	$(CC) $(CFLAG) -o $@ $^ $(LFLAG)

%.o: %.cc $(HDR)
	$(CC) $(CFLAG) -o $@ -c $<

%_cuda.o: %.cu $(HDR)
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -f $(CCOBJ) $(CUOBJ) $(EXE)

