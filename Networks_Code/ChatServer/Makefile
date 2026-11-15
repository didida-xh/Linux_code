.PHONY:all
all:client_chat server_chat

server_chat:ChatServerMain.cc
	g++ -o $@ $^ -std=c++17
client_chat:ChatClient.cc
	g++ -o $@ $^ -std=c++17 -static

.PHONY:clean
clean:
	rm -f client_chat server_chat