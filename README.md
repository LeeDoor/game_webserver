# hex_chess_backend
backend part of pet-project Hexagonal Chess written on C++.

# build and run
to run server on your machine, you need to have conan 1.60 (**conan 2.0 and newer wouldn't fit**) and cmake installed (also compilers etc).

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
* [database_manager](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/database_manager.md)
* [token_manager](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/token_manager.md)
* [API](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/http_api.md)

how to write tests, naming rules etc. [here](https://github.com/LeeDoor/hex_chess_backend/blob/main/docs/tests.md)