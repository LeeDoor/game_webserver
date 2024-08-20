## Testing
i am testing my application using **[Catch2](https://github.com/catchorg/Catch2)** framework. All test source files are being built in one single executable. If you want to launch only one concrete test, you can use cli flags which catch 2 suggests. For example, if you want to run only api-oriented tests, you should go to *scripts* folder, build app with tests and launch them:
```sh
cd scripts
./build_with_tests.sh
./run_tests.sh [api]
```
run_tests. Sh script takes parameters and passes it to catch 2's executable. Also run_tests. Sh launches application before tests and kills it after even if you press Ctrl+C. 

more info about Catch 2 command line flags see [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)