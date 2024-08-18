- [CLI parameters](#cli-parameters)
   * [required](#required)
      + [CLI static_path](#cli-static_path)
         - [**description**](#description)
         - [**example**](#example)
      + [CLI postgres_credentials](#cli-postgres_credentials)
         - [**description**](#description-1)
         - [**example**](#example-1)
   * [not required](#not-required)
      + [CLI test](#cli-test)
         - [**description**](#description-2)
         - [**example**](#example-2)
      + [CLI redis_credentials](#cli-redis_credentials)
         - [**description**](#description-3)
         - [**example**](#example-3)

<!-- TOC end -->



# CLI parameters
## required
these parameters must be specified to run the program.

### CLI static_path
#### **description**
this flag specifies directory with static files. static files are files containing data about web pages and their associated scripts and styles.

#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --static_path static/
```

---
### CLI postgres_credentials
#### **description**
declares credentials for postgreSQL database. login and password must be provided as on the example: `myunusiallogin228:p@zzw0rd`
#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --postgres_credentials "superuser:vjfQQTGo4584"
```

## not required
these parameters are unnecessary to specify.

### CLI test
#### **description**
add this parameter for test execution. if you do, application will clear database after each call (tests create too much users per execution). 
#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --test
```

---
### CLI redis_credentials
#### **description**
password for redis database. if leave undefined if password is unnecessary(undefined in `redis.conf`)
#### **example**
***application*** is an executable for main program. also add other necessary flags.
```bash
./application --redis_credentials "V9uqkv5cVJBO9mNfxJOtn3u8UmbfE4IoJ4hde1dn0JJsmih38F"
```
