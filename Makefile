all: chatClient chatServer
chatClient: chatClient.c
	gcc -Wall -std=c99 -o chatClient chatClient.c -D _XOPEN_SOURCE=601

chatServer: chatServer.c
	gcc -Wall -std=c99 -o chatServer chatServer.c -D _XOPEN_SOURCE=601

clean:
	rm -f chatClient chatServer
