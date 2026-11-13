.PHONY:all
all:client_udp server_udp

server_udp:EchoServerMain.cc
	g++ -o $@ $^ -std=c++17
client_udp:EchoClient.cc
	g++ -o $@ $^ -std=c++17 -static

.PHONY:clean
clean:
	rm -f client_udp server_udp
