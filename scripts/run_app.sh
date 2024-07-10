cleanup() { 
   ./remove_executables.sh # remove executables after program stopped by Ctrl+C
   echo "\n"
}
trap cleanup SIGINT 

cd ..
./copy_executables.sh
./application $@ --static_path static --api_path API_functions.txt --postgres_credentials "postgres:1234" --redis_credentials "V9uqkv5cVJBO9mNfxJOtn3u8UmbfE4IoJ4hde1dn0JJsmih38F"
./remove_executables.sh # remove executables if program terminated by itself