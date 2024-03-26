cleanup() { 
   ./remove_executables.sh # remove executables after program stopped by Ctrl+C
   echo "\n"
}
trap cleanup SIGINT 

cd ..
./copy_executables.sh
./application $@ 
./remove_executables.sh # remove executables if program terminated by itself