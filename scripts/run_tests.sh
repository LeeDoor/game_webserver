cd ..

./copy_executables.sh
./application --test --static_path /home/leedor/Documents/hex_chess_backend/static/ & 
FOO_PID=$!
sleep 1
./test_application $@
kill $FOO_PID
./remove_executables.sh