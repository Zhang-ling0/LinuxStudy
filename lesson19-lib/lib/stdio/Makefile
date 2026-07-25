libmystdio.so:mystdio.o mystring.o
	gcc -shared -o $@ $^
%.o:%.c
	gcc -fPIC -c $<
.PHONY:clean
clean:
	rm -rf *.o *.so mylib
.PHONY:output
output:
	mkdir -p mylib
	mkdir -p mylib/include
	mkdir -p mylib/lib
	cp *.h mylib/include
	cp *.so mylib/lib
	
