# setup
So you are here to launch the server inside docker container. but first, you need some dependencies installed on your server.
## Network
if you are running application on your home machine you probably need to edit firewall settings on your machine **and in the router**.
### Windows firewall
You need to edit windows firewall to allow your machine receiving TCP requests on port 8080.
### Linux firewall
to edit linux firewall settings, you can use `ufw` software. we need to add exception for port we are running on. there is an example for port 8080:
```bash
ufw allow from any to any port 8080
```
check changes using:
```bash
ufw status
Status: active

To        Action  From
--        ------  ----
8080      ALLOW   Anywhere                 
8080 (v6) ALLOW   Anywhere (v6)
```
### Docker network
Also to connect every docker container to each other, we need to create our own docker network:`
```bash
docker network create dbnetwork
```
This will create a special `bridge` network. For more details about [docker networking](https://docs.docker.com/engine/network/) and [bridge](https://docs.docker.com/engine/network/drivers/bridge/).
## Database
For each database we are going to use docker and their standard images. For the network HOST will be applied for easier execution.
### PostgreSQL
To run postgreSQL database using docker, run as root: 
```bash
docker run --name postgresql -e POSTGRES_PASSWORD=1234 --network=dbnetwork -p 5432:5432 -d postgres
```
Password is 1234 by default.
#### Database Setup
Note that it's *necessary* to create database and tables. To connect to database with CLI, use: 
```bash
psql -U postgres
```
and enter the password which you provided with the `docker run` command.
the code below creates a new database `game_webserver` and connects to it:
```SQL
CREATE DATABASE game_webserver;
\c game_webserver
```
and then you need to copy-paste all of the SQL code from [this](postgresql_tables.md) file to the psql.
```SQL
NOT THE ACTUAL CODE TO COPY
CREATE TABLE users ( ... 
CREATE INDEX idx_users_login .. 
CREATE TABLE sessions ( ..
```
Also server has a special parameter flags: `--postgres_credentials` and `--postgres_address` to present *login:password* and *URL address* respectively. By default in `./run_app.sh` script server is using **postgres** role to connect with password *1234*. To set this password, run:
```SQL
ALTER USER postgres WITH PASSWORD '1234';
```
Sure you can change password to any other more safe than that. Also it would be nice to create a special role for the database.
### Redis
To run Redis database using docker, run as root:
```bash
docker run --name redis-server -d -p 6379:6379 --network=dbnetwork redis
```

# run
> every docker command should be executed as root.
You should build an application using:
```bash
docker build -t leedoor/game_webserver .
```
To run, use: 
```bash
docker run -it --network=dbnetwork -p 8080:8080 --name webserver leedoor/game_webserver
```
