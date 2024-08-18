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
## **PostgreSQL**
install postgresql and make sure it is working. you need to change some configs. cd to `/etc/postgresql/<VERSION>/main`
 * `postgresql.conf`:  find line *listen_addresses*, edit and uncomment it like that: `listen_addresses = '*'		# what IP address(es) to listen on;`
* `pg_hba.conf`: just add this line(its ok that file is empty by default): `host all all 0.0.0.0/0 scram-sha-256`

after saving changes ***restart postgresql***: `systemctl restart postgresql`

now you need to create required tables. create all tables in order they are listed [here](database%20sql%20tables). 

## **Redis**
install redis and launch: `redis-server`. config files leave default.

## **docker**
install docker on your machine and cd to root directory.

# run
> every docker command should be executed as root.

to build the container, run this command:
```bash
docker build -t leedoor/hex_chess .
```
when building is finished, run:
```bash
docker run -p 8080:8080 --network="host" leedoor/hex_chess
```
`-p` specifies incoming's port and in-docker port. 
`--network="host"` allows container to connect to postgreSQL and Redis with localhost IP. you can change this property if you configure networks and put databases inside containers too.