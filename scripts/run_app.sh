cleanup() { 
   ./remove_executables.sh # remove executables after program stopped by Ctrl+C
   echo "\n"
}
trap cleanup SIGINT 

cd ..
./copy_executables.sh
./application $@ --static_path static --postgres_credentials "postgres:1234" 
./remove_executables.sh # remove executables if program terminated by itself