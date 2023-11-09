
cd ../cmake-build-debug/bin
./executable & 
FOO_PID=$!
./tests $1
kill $FOO_PID 