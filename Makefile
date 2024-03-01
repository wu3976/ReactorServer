TARGETS=server client
SERVERFILES=tcpepoll.cpp
CLIENTFILES=client.cpp
DEBUG=-g

all:$(TARGETS)

server:$(SERVERFILES)
	g++ -o server $(SERVERFILES) $(DEBUG)

clean:
	rm $(TARGETS)

client:$(CLIENTFILES)
	g++ -o client $(CLIENTFILES) $(DEBUG)