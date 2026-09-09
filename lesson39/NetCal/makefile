.PHONY:all
all: netcal_server netcal_client
netcal_client:NetCalClient.cc
	g++ -o $@ $^ -std=c++17;
netcal_server:NetCalServer.cc
	g++ -o $@ $^ -std=c++17;
.PHONY:clean
clean:
	rm -f netcal_server netcal_client