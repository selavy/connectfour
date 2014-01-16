CPP = g++

OBJS = main.o

connectfour: $(OBJS)
	$(CPP) -o connectfour $(OBJS)
main.o: main.cpp
	$(CPP) -c main.cpp
clean:
	rm -rf *.o connectfour *.log
