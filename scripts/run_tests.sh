cleanup() {
    ./remove_executables.sh # remove executables after program stopped by Ctrl+C
    kill $FOO_PID
    echo "\n"
}
trap cleanup SIGINT

check_webserver() {
    curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/ping
}

cd ..
./copy_executables.sh
./application --log_dir logs/log.txt --test --static_path static --postgres_credentials "postgres:1234" &
FOO_PID=$!

echo "Waiting for the webserver to start..."
counter=0
while [[ $(check_webserver) != "200" ]]; do
    sleep 0.3
    echo -e "\e[33mping...\e[0m"
    ((counter++))
    if [[ $counter -eq 10 ]]; then
        if [[ $(check_webserver) != "200" ]]; then
            echo -e "\e[31mserver cant start. aborting\e[0m"
            exit 1
        fi
        break
    fi
done

echo -e "\e[32m...pong!\e[0m"

./test_application $@
kill $FOO_PID
./remove_executables.sh
