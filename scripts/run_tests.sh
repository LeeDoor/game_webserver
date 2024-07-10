cd ..
./copy_executables.sh
./application --test --static_path static --api_path API_functions.txt --postgres_credentials "postgres:1234" & 
FOO_PID=$!
sleep 1
./test_application $@
kill $FOO_PID
./remove_executables.sh