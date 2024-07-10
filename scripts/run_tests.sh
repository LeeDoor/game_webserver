cd ..
./copy_executables.sh
./application --test --static_path static --api_path API_functions.txt --postgres_credentials "postgres:1234" --redis_credentials "V9uqkv5cVJBO9mNfxJOtn3u8UmbfE4IoJ4hde1dn0JJsmih38F" & 
FOO_PID=$!
sleep 1
./test_application $@
kill $FOO_PID
./remove_executables.sh