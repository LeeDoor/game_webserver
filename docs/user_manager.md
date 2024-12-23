# [user_manager](https://github.com/LeeDoor/game_webserver/tree/main/src/user_manager)
## What is it for?
This module gives api for communicating with database. It contains multithread-safe connection pool and high-level classes for http_hanlder  

## Classes
* **IUserManager** - interface for http_handler to contain database manager as pointer. It is required to have interface for easier moving between databases if i would want to.
* **UserManagerPostgres** - class inherits from **IUserManager**, contains realization of all interface classes for PostgreSQL.
* **User** - struct of user's data. There is a difference *user's* data and *player's* data. User's one contains only information about account, not related to current game. 
* *coming soon* **PlayerData** - struct of player's data. Contains game-oriented game like elo, matches, history etc.
* **RegistrationData** - contains login and password for reading user's request. This struct is in *http_handler* namespace, but it is in this directory for easier access.

## Graph
```mermaid
---
title: namespace user_manager
---
classDiagram

class User {
    uuid
    string login
    string password
}

class IUserManager {
    abstract GenerateUuid() Uuid
    abstract AddLine(User&&) bool
    abstract GetByUuid(uuid&&) User?
    abstract GetByLoginPassword(login&&, password) User?
}

class UserManagerPostgres {
    +GenerateUuid() Uuid
    +AddLine(User&&) bool
    +GetByUuid(uuid&&) User?
    +GetByLoginPassword(login&&, password) User?
}

UserManagerPostgres --|> IUserManager
IUserManager --> User
```