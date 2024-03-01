# compile options
SERVEREXEC=server
CLIENTEXEC=client
TARGETS=$(SERVEREXEC) $(CLIENTEXEC)
SERVERFILES=tcpepoll.cpp
CLIENTFILES=client.cpp
DEBUG=-g

# run options
SERVERIP=127.0.0.1
SERVERPORT=5005


# compile commands
all:$(TARGETS)

server:$(SERVERFILES)
	g++ -o server $(SERVERFILES) $(DEBUG)

clean:
	rm $(TARGETS)

client:$(CLIENTFILES)
	g++ -o client $(CLIENTFILES) $(DEBUG)

# run commands
rs:$(SERVEREXEC)
	./$(SERVEREXEC) $(SERVERIP) $(SERVERPORT)

rc:$(CLIENTEXEC)
	./$(CLIENTEXEC) $(SERVERIP) $(SERVERPORT)