all: fileClient fileServer


fileClient: fileClient.cpp
	g++ -Wall -g -o fileClient fileClient.cpp ../_cplusframe.cpp

fileServer: fileServer.cpp
	g++ -Wall -g -o fileServer fileServer.cpp ../_cplusframe.cpp

clean:
	rm -f fileClient fileServer
