# hex_chess_backend
backend part of pet-project Hexagonal Chess written on C++.

# build and run
to run server on your machine, you need to have conan 1.60 (**conan 2.0 and newer wouldn't fit**) and cmake installed (also compilers etc).

```sh
pip install conan==1.*
sudo apt install cmake
```
to build application there are several useful scripts in special folder. for example you can build_with_tests.sh or build_no_tests.sh:
```sh
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
* [new module creation](docs/new_module_creation.md)
* [initializer](docs/initializer.md)
* [server](docs/server.md)
* [http_handler](docs/http_handler.md)
* [serializer(JSON)](docs/serializer.md)
* [user_manager](docs/user_manager.md)
* [token_manager](docs/token_manager.md)
* [queue_manager](queue_manager.md)
* [game_manager](docs/game_manager.md)
* [session_manager](docs/session_manager.md)
* [notification_system](docs/notification_system.md)
* [API](docs/http_api.md)
* [CLI parameters to executable](docs/CLI%20parameters%20to%20executable.md)
* [session state](docs/session_state.md)
* [SQL table init](docs/database%20sql%20tables.md)
how to write tests, naming rules etc. [here](docs/tests.md)