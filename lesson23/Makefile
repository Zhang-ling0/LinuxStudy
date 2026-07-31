.PHONY:all
all:Client Server
Server:Server.cc
	g++ -o $@ $^ -std=c++11
Client:Client.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f Server Client