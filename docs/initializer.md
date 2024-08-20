# [initializer](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/initializer)
## What is it for?
This module has one source and one header file. Its only task is to launch server in different threads. This module is being used once in main. Cpp file.

## Init function
Init function parses parameters from argument line and starts server.
## StartServer function
initializes handler_parameters, runs [server](server.md) on multiple threads.

## RunWorkers function
Starts some **Fn** function *n* times, where *n* is number of possible threads.

## PORT
You can change PORT define if you want to change starting port. Also soon you would be able to change it in arguments.
