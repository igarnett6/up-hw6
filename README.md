# A simple shell (in C)

## Description
A simple messenger allowing you to send messages between a server and client. After starting the server, it will wait for a client to connect. If the client chooses to terminate the connection, the server will wait for another client to connect. If the server chooses to terminate a connection with a client, the client will exit. To terminate the server, hit `ctrl+c`.



## How to compile
To compile, cd into the directory containing chatClient.c and chatServer and run:

`make`

To remove old executables run:

`make clean`

## How to run
First run the server application by entering `./chatServer`. The server allows
the port to also be specified in the format: `./chatServer [port#]`. But can be run without this specification, with the port # defaulting to `13000`.

Then a client can be created using `./chatClient`. The client allows for both
the IP address, and the port to be specified in the format: `./chatClient [addr] [port#]`. The address may be specified without specifying the port #, which will default to `13000`. But if you wish to specify a port number you must also specify the address.
