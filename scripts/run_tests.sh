cd ..

./copy_executables.sh
./application &
FOO_PID=$!
sleep 1
./test_application $@
kill $FOO_PID
./remove_executables.sh