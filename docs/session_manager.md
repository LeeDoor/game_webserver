# [session_manager](https://github.com/LeeDoor/game_webserver/tree/main/src/session_manager)
## What is it for
Session manager provides api to get access to and control data about finished sessions. When any match completed, data about it is stored in database, so players can get information about it.
##  classes
* **ISessionManager** - interface for database manager.
* **SessionManagerPostgres** - `ISessionManager` realization for PostgreSQL database using `pqxx` library.