
cd ../cmake-build-debug/bin
./executable & 
FOO_PID=$!
./tests $@
kill $FOO_PID 