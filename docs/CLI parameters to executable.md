- [cli parameters](#cli-parameters)
   * [required](#required)
      + [cli static_path](#cli-static_path)
         - [**description**](#description)
         - [**example**](#example)
      + [cli postgres_credentials](#cli-postgres_credentials)
         - [**description**](#description-1)
         - [**example**](#example-1)
   * [not required](#not-required)
      + [cli test](#cli-test)
         - [**description**](#description-2)
         - [**example**](#example-2)
      + [cli redis_credentials](#cli-redis_credentials)
         - [**description**](#description-3)
         - [**example**](#example-3)

<!-- toc end -->



# Cli parameters
## Required
These parameters must be specified to run the program.

### Cli static_path
#### **description**
This flag specifies directory with static files. Static files are files containing data about web pages and their associated scripts and styles.

#### **example**
***application*** is an executable for main program. Also add other necessary flags.
```bash
./application --static_path static/
```

---
### Cli postgres_credentials
#### **description**
Declares credentials for postgresql database. Login and password must be provided as on the example: `myunusiallogin228:p@zzw0rd`
#### **example**
***application*** is an executable for main program. Also add other necessary flags.
```bash
./application --postgres_credentials "Superuser:vjfqqtgo4584"
```

## Not required
These parameters are unnecessary to specify.

### Cli test
#### **description**
Add this parameter for test execution. If you do, application will clear database after each call (tests create too much users per execution). 
#### **example**
***application*** is an executable for main program. Also add other necessary flags.
```bash
./application --test
```

---
### Cli redis_credentials
#### **description**
Password for redis database. If leave undefined if password is unnecessary (undefined in `redis.Conf`)
#### **example**
***application*** is an executable for main program. Also add other necessary flags.
```bash
./application --redis_credentials "V9uqkv5cvjbo9mnfxjotn3u8umbfe4ioj4hde1dn0jjsmih38f"
```
