# run_tests.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/run_tests.sh).
This script starts the server and waits for it to be ready for responding by pinging it for a while. If server didn't launch, returns `1`.
Tests are written on c++ using `Catch2` library. If you need more accurate testing using cli parameters, provide them to this script. 
Example:
```bash
./run_tests.sh [long_poll]
```
In this example we are running built tests those use the [long_poll](http_api.md#long-poll) system.

> Read more about Catch2 and it's flags [here](tests.md).
# run_app.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/run_app.sh).
This script runs application with default flags and any other flags that you provide. Note that application should already be built.
Example:
```bash
./run_app.sh --test
```

# find_app_PID.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/find_app_PID.sh).
If you launched application asyncly and want to kill it, run `find_app_PID.sh` script to find Process ID and kill it:
```bash
./find_app_PID.sh
sudo kill <PID>
```
# debug_app.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/debug_app.sh).
Same as [`run_app.sh`](#run_app.sh), but runs application inside gdb debugger. Example:
```bash
./debug_app.sh
```

# build_with_tests.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/build_with_tests.sh).
Builds application with all included tests. This script is **necessary** to run before executing, or else you have nothing to execute. 
After that script, [application](#run_app.sh) and [test](#run_tests.sh) scripts can be called. Example:
```bash
./build_with_tests.sh
```
# build_no_tests.sh
[Linkto file](https://github.com/LeeDoor/game_webserver/blob/main/scripts/build_no_tests.sh).
Same as [`build_with_tests.sh`](#build_with_tests.sh), but doesn't waste your time on building tests if you don't need them. After that script, [application](#run_app.sh) script can be called. Example:
```bash
./build_no_tests.sh
```