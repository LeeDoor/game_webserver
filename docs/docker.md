# setup
so you are here to launch the server inside docker container. but first, you need some dependencies installed on your server.
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
if you want to connect to database with CLI, use: 
```bash
psql -U postgres
```
and enter the password which you provided with the `docker run` command.
### Redis
To run Redis database using docker, run as root:
```bash
docker run --name redis-server -d -p 6379:6379 --network=dbnetwork redis
```

# run
> every docker command should be executed as root.

Firstly you should build an application using:
```bash
docker build -t leedoor/game_webserver .
```
To run, use: 
```bash
docker run -it --network=dbnetwork -p 8080:8080 --name webserver leedoor/game_webserver
```
