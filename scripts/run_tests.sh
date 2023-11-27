
cleanup() { # we need to kill executable if tests are terminated
    kill $FOO_PID 
}

trap cleanup SIGINT # for cleanup 

cd ../cmake-build-debug/bin
./executable & 
FOO_PID=$!
./tests $@ 
kill $FOO_PID 