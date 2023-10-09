# hex_chess_backend
backend part of pet-project Hexagonal Chess written on C++.

# build and run
to run server on your machine, you need to have conan(1.60) and cmake installed (also compilers etc).

```
pip install conan==1.*
sudo apt install cmake
```
to build this application i created special shell script build.sh
```
git clone https://github.com/LeeDoor/hex_chess_backend
cd hex_chess_backend
./build.sh
./build/bin/hex_chess
```
port is 8080 by default

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

enjoy 
