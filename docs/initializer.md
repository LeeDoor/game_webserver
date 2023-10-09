# [initializer]("https://github.com/LeeDoor/hex_chess_backend/tree/main/src/initializer")
## what is it for?
this module has one src and one header file. its only task is to launch server in different threads. this module is being used once in main.cpp file.

## Init function
init function is just a copy of main function, i just separated it to clean up main.cpp file.

## RunWorkers function
starts some **Fn** function *n* times, where *n* is number of possible threads.

## PORT
you can change PORT define if you want to change starting port. i ll remove that define soon and most likely I will forget to remove this part of the documentation :)

## graph
i will not add mermaid graph here. fr it ll contain only two blocks why do you need it