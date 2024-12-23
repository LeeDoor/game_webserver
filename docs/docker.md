# setup
so you are here to launch the server inside docker container. but first, you need some dependencies installed on your server.
## Machine
if you are running application on your home machine you probably need to edit firewall settings on your machine and in the router.
### Linux ufw
to edit linux firewall settings, you can use ufw software. we need to add exception for port we are running on. there is an example for port 8080:
```bash
sudo ufw allow from any to any port 8080
```
check changes using:
```bash
sudo ufw status
Status: active

To        Action  From
--        ------  ----
8080      ALLOW   Anywhere                 
8080 (v6) ALLOW   Anywhere (v6)
```
## Databases
For each database we are going to use docker and their standard images. For the network HOST will be applied for easier execution.
### PostgreSQL
To run postgreSQL database using docker, run as root: 
```bash
docker run --name postgresql -e POSTGRES_PASSWORD=<PASSWORD> --network=host -d 
postgres
```
Password is 1234 by default.

### Redis
To run Redis database using docker, run as root:
```bash
sudo docker run --name redis-server -d -p 6379:6379 --network=host redis --bind 127.0.0.1
```

# run
> every docker command should be executed as root.
Firstly you should build an application using:
```bash
docker build -t leedoor/game_webserver .
```
To run, use: 
```bash
docker run -it --network=host leedoor/game_webserver
```
