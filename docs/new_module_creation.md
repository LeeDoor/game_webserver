# what is module
project consists of several modules which a tied up with each other. 

each node is clickable to read more info about module.

# how to create module
root contains bash script called [*add_dependency.sh*](https://github.com/LeeDoor/hex_chess_backend/blob/main/add_dependency.sh). launch it with name of new module like:
```
./add_dependency.sh initializer
```
it will create new directory with default cmake file. also you **need** to enter the name of module in main cmake:
```
#list with all libraries
set (DEPENDENCIES serializer initializer server http_handler ENTER MODULE NAME HERE)
```
and in newly created one:
```
set(LIBRARY_NAME )
#creating library
add_library(${LIBRARY_NAME}
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})
```

# created CMake file
newly created cmake file contains variable *LIBRARY_NAME* - name of library. the variable is needed so that other modules can refer to it by name.

add your source files (.cpp and .hpp prefered) to add_library command. 

add all required libraries to `target_link_libraries` command. PRIVATE and PUBLIC options are necessary for faster project building. all libraries communication is shown in *Obsidian* canvas [[dependencies.canvas|dependencies]].

leave `target_include_directories` unmodified. DIRECTORIES is a list with every directory in the project with header files.