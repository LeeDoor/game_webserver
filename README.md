# hex_chess_backend
backend part of pet-project Hexagonal Chess written on C++.

# build and run
to run server on your machine, you need to have conan(1.60) and cmake installed (also compilers etc).

```
pip install conan==1.*
sudo apt install cmake
```
to build application there are several useful scripts in special folder. for example you can build_with_tests.sh or build_no_tests.sh:
```
git clone https://github.com/LeeDoor/hex_chess_backend
cd hex_chess_backend/scripts
./build_no_tests.sh
./run_app.sh
```
port is 9999 by default

# uml diagrams
while reading docs you can see beatiful diagrams and schemas. framework called [mermaid](https://www.mermaidchart.com)

# documentation
NOTE: I will try to change and supplement the structure of the code immediately along with the documentation.
as i write my code i also try to documentate it. here is the list of readmes for each module:
* [new module creation](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/new_module_creation.md)
* [initializer](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/initializer.md)
* [server](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/server.md)
* [http_handler](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/http_handler.md)
* [serializer(JSON)](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/serializer.md)

# testing
i am testing my application using **[Catch2](https://github.com/catchorg/Catch2)** framework. all test source files are being built in one single executable. if you want to launch only one concrete test, you can use flags which catch2 gives to you. for example, if you want to test only api-oriented tests, you should go to *scripts* folder, build app with tests and launch them:
```
./build_with_tests.sh
./run_tests.sh [api]
```
run_tests.sh script takes parameters and passes it to executable. also run_tests.sh launches application before tests and kills it after, so if you dont want to kill application by yourself, **dont Ctrl+C** while tests are running. 
more info about Catch2 command line flags see [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)