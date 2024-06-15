# [initializer](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/initializer)
## what is it for?
this module has one src and one header file. its only task is to launch server in different threads. this module is being used once in main.cpp file.

## Init function
init function parses parameters from argument line and starts server
## StartServer function
initializes handler_parameters, runs [server](server.md) on multiple threads

## RunWorkers function
starts some **Fn** function *n* times, where *n* is number of possible threads.

## PORT
you can change PORT define if you want to change starting port. also soon you would be able to change it in arguments
