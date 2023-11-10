## testing
i am testing my application using **[Catch2](https://github.com/catchorg/Catch2)** framework. all test source files are being built in one single executable. if you want to launch only one concrete test, you can use cli flags which catch2 suggests. for example, if you want to run only api-oriented tests, you should go to *scripts* folder, build app with tests and launch them:
```
cd scripts
./build_with_tests.sh
./run_tests.sh [api]
```
run_tests.sh script takes parameters and passes it to catch2's executable. also run_tests.sh launches application before tests and kills it after, so if you dont want to kill application by yourself, **dont Ctrl+C** while tests are running. 
if it happened, please write next commands:
```
$ ps
    PID TTY          TIME CMD
   3896 pts/0    00:00:00 bash
   XXXX pts/0    00:00:00 executable
   9199 pts/0    00:00:00 ps
$ kill XXXX
```
**ps** will print all running processes with process id (PID). find process with name **executable** and write **kill** command with executable's PID as a parameter.

more info about Catch2 command line flags see [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)