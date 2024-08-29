cd ..
./copy_executables.sh
gdb --args ./application --test --static_path static --postgres_credentials "postgres:1234"
./remove_executables.sh