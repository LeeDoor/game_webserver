- [http request API](#http%20request%20api)
   * [static files](#static%20files)
- [API](#api)
   * [Function Tags](#function%20tags)
      + [Requires Authorization ](#requires%20authorization)
      + [Long Poll](#long-poll)
   * [Debug API](#debug%20api)
      + [API player_tokens](#api%20player_tokens)
      + [API user](#api%20user)
      + [API matchmaking_queue](#api%20matchmaking_queue)
   * [User API](#user%20api)
      + [API register](#api%20register)
      + [API login](#api%20login)
      + [API profile](#api%20profile)
   * [Game API](#game%20api)
      + [API enqueue](#api%20enqueue)
      + [API wait_for_opponent](#api%20wait_for_opponent)
      + [API session_state](#api%20session_state)
      + [API session_state_change](#api%20session_state_change)
      + [API move](#api%20move)
      + [API resign](#api%20resign)


# Http request API
## Static files
To request static file from server, set relative path to root folder in request target like this: 
```
127.0.0.1/folder1/index.html
```
This url will return index. Html file in folder 1.
To get main index. Html file it is not required to write file name. These two lines of request will get same response:
```
127.0.0.1/index.html
127.0.0.1/
``` 

# API

All non-ok responses have same body type. For example:
 
```js
{
    "error_name": "object_is_not_found",
    "description": "object that you are trying to access is not found"
}
```
## Function Tags
### Requires Authorization 
means that request must be authorized with `Authorization` header. Example:
```HTTP
Authorization: Bearer FFAADDDDEE12161753563
```
**includes these errors: **
1. Status: ***`UNAUTHORIZED`***
Given request does not have Authorization header.
```json
{
	"error_name":"unathorized",
	"description":"request must be authorized"
}
```
2. Status: ***`UNAUTHORIZED`***
Given request has Authorization header but with wrong token.
```json
{
	"error_name":"invalid_token",
	"description":"request authorization is invalid"
}
```
2. Status: ***`UNAUTHORIZED`***
Given request has Authorization header with valid-formed token but not addressing to anybody.
```json
{
	"error_name":"person_removed",
	"description":"person with this token is unavailable (prob. removed)"
}
```
### Long-Poll
Means that request is Long-Poll. Response will not arrive immediately, you should wait for response for a while.
**includes these errors: **
1. Status: ***`CONFLICT`***
This poll is closed and replaced with the other one.
```json
{
	"error_name":"poll_closed",
	"description":"SessionStateNotifier poll replaced by other"
}
```
## Debug API
Debug api is required to get data from app structures. To execute them you need to send admin login and password.

Example:
```js
{ 
	"login": "very_login", 
	"password": "abcde12345" 
}
```
if admin credentials not provided, you get this errror: ***`UNAUTHORIZED`***
```json
{
	"error_name":"invalid_admin",
	"description":"the administrator password is missing or incorrect"
}
```
### API player_tokens
#### **description**
Debug function for getting users' authentication tokens and uuids.
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/player_tokens*

#### **request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

#### **responses**
* `200 OK` \
    Tokens sent successfully
    *response body example:*
    ```js
    {
        "token1AV24":   "UUID173578",
        "token22724":   "UUID116478",
        "token3AVA324": "UUID1735788",
    }
    ```
---
### API user
#### **description**
Debug function for getting users' profile info like login and password.
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/user? Uuid=USER_UUID*
**OR**
*/api/debug/user? Login=USER_LOGIN&password=USER_PASS*

#### **request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

#### **responses**
* `200 OK` \
    User sent successfully
    *response body example:*
    ```js
    {
		"uuid":"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"login":"thebestuserever100"
		"password":"Oksano4kaCute"
    }
    ```
---
### API matchmaking_queue
#### **description**
Debug function for getting queue of users' uuids
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/matchmaking_queue*

#### **request body example**
```js
    {
        "login": "admin 2009",
        "password": "nadejni_parol 2"
    }
```

#### **responses**
* `200 OK`\
    Matchmaking queue sent successfully
    *response body example:*
    ```js
    {
        "UUID 123",
        "UUID 124",
        "UUID 125"
    }
    ```

## User API
### API ping
#### **allowed methods**
***`GET/HEAD`*** 
#### **request target**  
_/api/ping_

#### **function description**
Returns success.

#### **request body example**
```js
{}
```

#### **responses**
* `200 OK`  
### API register
#### **action diagram**
```mermaid
---

Title: api registger diagram

---

  

SequenceDiagram
Actor User

ApiHandler->>Serializer: deserialize body data

Serializer::>ApiHandler: returns login and password from body

  

ApiHandler->>ApiHandler: checks validness of data

ApiHandler->>UserManager: adds user to database

UserManager->>ApiHandler: returns error code

  


ApiHandler->>User: send registration state
```

#### **allowed methods**
***`POST`***
#### **request target**
*/api/register*

#### **function description**
By given login and password in body, creates account. Data stored in db, you cant register with same login more than once. After registration you need to login to play the game and use other features. 

#### **request body example**
```js
    {
        "login": "very_login",
        "password": "abcde 12345"
    }
```
#### **login and password criteria**
- Login size more or equal to **3**
- Password size more or equal to **6**. Must contain at least 1 digit

#### **responses**
* `200 OK`\
    Registration is ok, user added

    *response body:*
    ```js
    {}
    ```
    
* `400 bad_request`\
    Body data is wrong or login and password are invalid

    **error_name meanings**
    - **wrong_login_or_password**: login or password are invalid (watch description with criteria)
    - **body_data_error**: body data is incorrect (watch example above)

* `409 conflict`\
    There is already a user with given login

    **error_name meanings**
    - **login_taken**: login already taken

---
### API login
#### **action diagram**
```mermaid
---

Title: api login diagram

---

  

SequenceDiagram
Actor User

ApiHandler->>Serializer: deserialize body data

Serializer->>ApiHandler: returns login and password from body

  

ApiHandler->>ApiHandler: checks validness of data

ApiHandler->>UserManager: searches for a user with such login and password

UserManager->>ApiHandler: returns user or error code

ApiHandler->>TokenManager: creates session with current user

TokenManager->>ApiHandler: returns token of user's session

  


ApiHandler->>User: send login state and token
```

#### **allowed methods**
***`POST`***

#### **request target**
*/api/login*

#### **function description**
By given login and password in body, logins to get authorization token, which is required to play. Only registered user can login.

#### **body example**
```js
    {
        "login": "very_login",
        "password": "abcde 12345"
    }
```

#### **responses**
* `200 OK`\
    Login is successful, token returned
    
    *response body"*
    ```js
    {
        "token": "aabbbccc 123"
    }
    ```
    **login and password criteria**
    - Login size more or equal to **3**
    - Password size more or equal to **6**. Must contain at least 1 digit

* `400 bad_request`\
    Body data is wrong or login and password doesnt match to any registered user

    **error_name meanings**
    - **body_data_error**: body data is incorrect (watch example above)
    - **wrong_login_or_password**: login or password are invalid (watch description with criteria)
    - **no_such_user**: no user found with given login and password
---
### API profile
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)

#### **action diagram**
```mermaid
---
Title: api profile diagram
---
SequenceDiagram
Actor User
ApiHandler->>ApiHandler: gets Authorization token
ApiHandler->>TokenManager: gets user's uuid by token
TokenManager->>ApiHandler: user's uuid
ApiHandler->>UserManager: gets user by uuid
UserManager->>ApiHandler: returns user or error code
ApiHandler->>User: send user data
```

#### **allowed methods**
***`GET/HEAD`***

#### **request target**
*/api/profile*

#### **function description**
Requires authorization token. By this token gets profile information from db.

#### **responses**
* `200 OK`\
    Authorization is correct, send profile data in response
    *response body"*
    ```js
    {
        "login": "loginlogin",
        "password": "pass 123123"
    }
    ```
---
## Game API
Some of game API functions are declared to some `session`. It's id should be passed as url parameter (see each function's description). This entails the possibility of the following errors:
* `400 no_such_session`
Session you are trying to get access to does not exist.
```json
{
	"error_name": "wrong_sessionId",
	"description": "no session with such sessionId"
}
```
* `400 session_finished`
Session you are trying to get access is already finished. See the game's stats.
```json
{
	"error_name": "session_finished",
	"description": "session is finished"
}
```
### API enqueue
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)

#### **action diagram**
```mermaid
---

Title: api profile diagram

---

  

SequenceDiagram

Actor User

ApiHandler->>ApiHandler: gets Authorization token

  

ApiHandler->>TokenManager: gets user's uuid by token

TokenManager->>ApiHandler: user's uuid

  

ApiHandler->>MMQueue: tries to enqueue player

MMQueue->>ApiHandler: returns enqueuing status

ApiHandler->>User: send enqueue status
```

#### **allowed methods**
***`POST`***

#### **request target**  
_/api/game/enqueue_

#### **function description**
To start a game, you need to add yourself to the queue. You will be added to queue to search for an opponent by this api function execution. Call wait_for_opponent immediately after that.

#### **request body example**
Body must be empty

#### **responses**
* `200 OK`  
*response body:*
    ```js
    {}
    ```
    
* `200 OK`  
**error_name meanings**
    - **enqueue_error**: error happened while enqueuing player (already in queue or wrong token)

---
### API wait_for_opponent
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### [<span style="color:#f58a42"><b>Long-Poll</b></span>](http_api.md#Long-Poll)

#### **action diagram**
```mermaid
---
Title: api registger diagram
---
SequenceDiagram
Actor User 1
Participant Server
Actor User 2

User 1->>Server: enqueue
User 1->>Server: wait_for_opponent
Server-->Server: enqueuing
User 2->>Server: enqueue
User 2->>Server: wait_for_opponent
Server-->Server: enqueuing
Server->>User 1: opponent_found
Server->>User 2: opponent_found

```

#### **allowed methods**
***`GET/HEAD`***

#### **request target**  
_api/game/wait_for_opponent_

#### **function description**
Long-poll function to read data about future session. Use it after enqueuing to the game and wait for response until enemy is found. When found, returns session ID to join the game.

#### **request body example**

```js

```

#### **responses**

* `200 success`
Opponent found if body contains SessionId.
*response body:*
```js
{
	"sessionid": "ADD 124558846"
}
```
---
### API session_state
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization) 

#### **allowed methods**
***`GET/HEAD`***

#### **request target**  
_/api/game/session_state? SessionId=SESSION_ID_

#### **function description**
Request to get session state. Session id should be passed as URL parameter.

#### **responses**
* `200 OK`  
*response body:*
***TO SEE RESPONSE EXAMPLE: [[session_state]]***
* `422 url_parameters_error`
```json
{
	"error_name": "url_parameters_error"
	"description": "this api function requires url parameters"
}
```
* `400 no_such_session`
Session you are trying to get access to does not exist.
```json
{
	"error_name": "wrong_sessionId",
	"description": "no session with such sessionId"
}
```
---
### API session_state_change
#### [<span style="color:#f58a42"><b>Long-Poll</b></span>](http_api.md#Long-Poll)
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### **action diagram**
```mermaid
SequenceDiagram
	Actor Player 1
	Participant Server
	Actor Player 2
	Player 1 ->> Server: session_state_change
	Player 2 ->> Server: session_state_change
	Player 1 ->> Server: makes a move
	Server --> Player 1: updated_event_list
	Server --> Player 2: updated_event_list
	Player 1 ->> Server: session_state_change
	Player 2 ->> Server: session_state_change
	Player 2 ->> Server: makes a move
	Server --> Player 1: updated_event_list
	Server --> Player 2: updated_event_list
	
```
#### **allowed methods**
***`GET/HEAD`***
#### **request target**  
_/api/game/session_state_change? SessionId=SESSION_ID_

#### **request body example**

```js
{
	"from_move": 3
}
```

#### **function description**
Long-Poll function hangs until some action happens in the session. Once it is, poller gets response with new event_list. Pass from_move parameter in body to get list INCLUDING the move you started on and later ones.

#### **responses**
* `200 OK`  
*response body:*
	SEE [[event_list]]
* `422 url_parameters_error`
```json
{
	"error_name": "url_parameters_error"
	"description": "this api function requires url parameters"
}
```
* `400 no_such_session`
Session you are trying to get access to does not exist.
```json
{
	"error_name": "wrong_sessionId",
	"description": "no session with such sessionId"
}
```
---
### API move
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### **allowed methods**
***`POST`***
#### **request target**  
_/api/game/move? SessionId=SESSION_ID_

#### **function description**
Function tells the game about player's move. SessionId must be passed as URL parameter, body should contain move information.

#### **request body example**
```json
{
	"move_type": STRING,//"walk", "resign"
	"X": UNSIGNED,
	"Y":UNISIGNED
}
```

#### **responses**

* `200 OK`  
Everything is ok, now opponent is moving
* `400 wrong_move`
According to the game rules, player cant make such move.
```json
{
	"error_name": "wrong_move",
	"description": "player cant make such move"
}
```
* `400 not_your_move`
Now there is an enemy's move, you cant do anything.
```json
{
	"error_name": "not_your_move",
	"description": "the opponent's move is now"
}
```
* `400 access_denied`
You don't have access to make a move in this match. Probably you are not the player.
```json
{
	"error_name": "access_denied",
	"description": "you have no access to do this action"
}
```
* `400 wrong_body_data`
Body data is messed up. Check the example above.
```json
{
	"error_name": "body_data_error",
	"description": "wrong body data"
}
```

---
### API resign
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### **allowed methods**
***`POST`***
#### **request target**  
_/api/game/resign? SessionId=SESSION_ID_

#### **function description**
Function calls about your resign and ends the game. Url parameter should contain sessionId, where you are resigning. Body is empty.

#### **responses**
* `200 OK`  
* `400 access_denied`
You cant resign with this login token. You should be one of the players.
```json
{
	"error_name": "access_denied",
	"description": "you have no access to do this action"
}
```