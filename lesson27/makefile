.PHONY:all
all:loop_process mykill

mykill:mykill.cc
	g++ -w -o $@ $^
loop_process:loop.cc
	g++ -w -o $@ $^
.PHONY: clean
clean:
	rm -f mykill loop_process
