CC=gcc
CPP=g++
CCFLAGS= -g -Wall 

target= medianFilterTest
objects= medianFilter.o

all: $(target)

$(target): $(objects) $(target).cpp 
	$(CPP) $(objects) -o $@ $@.cpp
	
%.o: %.cpp
	$(CPP) -c $(CCFLAGS) -c $< -o $@
	
%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(objects)
	rm -f $(target)
