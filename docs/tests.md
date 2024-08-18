## testing
i am testing my application using **[Catch2](https://github.com/catchorg/Catch2)** framework. all test source files are being built in one single executable. if you want to launch only one concrete test, you can use cli flags which catch2 suggests. for example, if you want to run only api-oriented tests, you should go to *scripts* folder, build app with tests and launch them:
```sh
cd scripts
./build_with_tests.sh
./run_tests.sh [api]
```
run_tests.sh script takes parameters and passes it to catch2's executable. also run_tests.sh launches application before tests and kills it after even if you press Ctrl+C. 

more info about Catch2 command line flags see [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)