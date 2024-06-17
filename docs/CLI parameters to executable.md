# CLI parameters
## required
these parameters must be specified to run the program.

### CLI static_path
#### **description**
this flag specifies global directory with static files. static files are files containing data about web pages and their associated scripts and styles.

#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --static_path /home/leedor/Documents/hex_chess_backend/static/
```
---

## not required
these parameters are unnecessary to specify.

### CLI test
#### **description**
add this parameter for test execution. if you do, application will clear database after each call (tests create too much users per execution). #TODO REMOVE SOON: also test db is being use with test flag.
#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --test
```
