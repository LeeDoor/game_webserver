# http request API
## static files
to request static file from server, set relative path to root folder in request target like this: 
```
127.0.0.1/folder1/index.html
```
this url will return index.html file in folder1.
to get main index.html file it is not required to write file name. these two lines of request will get same response:
```
127.0.0.1/index.html
127.0.0.1/
``` 

# API

all non-ok responses have same body type. for example:
 
```js
{
    "error_name": "object_is_not_found",
    "description": "object that you are trying to access is not found"
}
```
## Function Tags
### Requires Authorization 
means that request must be authorized with `Authorization` header. example:
```HTTP
Authorization: Bearer FFAADDDDEE12161753563
```
**includes these errors: **
1. status: ***`UNAUTHORIZED`***
given request does not have Authorization header.
```json
{
	"error_name":"unathorized",
	"description":"request must be authorized"
}
```
2. status: ***`UNAUTHORIZED`***
given request has Authorization header but with wrong token.
```json
{
	"error_name":"invalid_token",
	"description":"request authorization is invalid"
}
```
2. status: ***`UNAUTHORIZED`***
given request has Authorization header with valid-formed token but not addressing to anybody.
```json
{
	"error_name":"person_removed",
	"description":"person with this token is unavailable (prob. removed)"
}
```
### Long-Poll
means that request is Long-Poll. response will not arrive immediately, you should wait for response for a while.
**includes these errors: **
1. status: ***`CONFLICT`***
this poll is closed and replaced with the other one.
```json
{
	"error_name":"poll_closed",
	"description":"SessionStateNotifier poll replaced by other"
}
```
## Debug API
debug api is required to get data from app structures. to execute them you need to send admin login and password.

example:
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
debug function for getting users' authentication tokens and uuids.
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
* `200 OK`\
    tokens sent successfully
    *response body example:*
    ```js
    {
        "token1AV24":   "UUID173578",
        "token22724":   "UUID116478",
        "token3AVA324": "UUID1735788",
    }
    ```
---
### API user_data
#### **description**
debug function for getting users' profile info like login and password.
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/user_data?uuid=USER_UUID*
**OR**
*/api/debug/user_data?login=USER_LOGIN&password=USER_PASS*

#### **request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

#### **responses**
* `200 OK`\
    user_data sent successfully
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
debug function for getting queue of users' uuids
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/matchmaking_queue*

#### **request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

#### **responses**
* `200 OK`\
    matchmaking queue sent successfully
    *response body example:*
    ```js
    {
        "UUID123",
        "UUID124",
        "UUID125"
    }
    ```

---
### API sessions_list #TODO
#### **description**
debug function for getting list of sessions with playing users.
#### **allowed methods**
***`GET/HEAD`***
#### **request target**
*/api/debug/sessions_list*

#### **request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

#### **responses**
* `200 OK`\
    sessions list sent successfully

    *response body example:*
    ```js
    {
        "sessionId1":{
            "player1":"UUID123",
            "player2":"UUID228",
        }
    }
    ```
## User API
### API register
#### **action diagram**
```mermaid
---

title: api registger diagram

---

  

sequenceDiagram
actor User

ApiHandler->>Serializer: deserialize body data

Serializer->>ApiHandler: returns login and password from body

  

ApiHandler->>ApiHandler: checks validness of data

ApiHandler->>UserDataManager: adds user to database

UserDataManager->>ApiHandler: returns error code

  


ApiHandler->>User: send registration state
```

#### **allowed methods**
***`POST`***
#### **request target**
*/api/register*

#### **function description**
by given login and password in body, creates account. data stored in db, you cant register with same login more than once. after registration you need to login to play the game and use other features. 

#### **request body example**
```js
    {
        "login": "very_login",
        "password": "abcde12345"
    }
```
#### **login and password criteria**
- login size more or equal to **3**
- password size more or equal to **6**. must contain at least 1 digit

#### **responses**
* `200 OK`\
    registration is ok, user added

    *response body:*
    ```js
    {}
    ```
    
* `400 bad_request`\
    body data is wrong or login and password are invalid

    **error_name meanings**
    - **wrong_login_or_password**: login or password are invalid (watch description with criteria)
    - **body_data_error**: body data is incorrect (watch example above)

* `409 conflict`\
    there is already a user with given login

    **error_name meanings**
    - **login_taken**: login already taken

---
### API login
#### **action diagram**
```mermaid
---

title: api login diagram

---

  

sequenceDiagram
actor User

ApiHandler->>Serializer: deserialize body data

Serializer->>ApiHandler: returns login and password from body

  

ApiHandler->>ApiHandler: checks validness of data

ApiHandler->>UserDataManager: searches for a user with such login and password

UserDataManager->>ApiHandler: returns user or error code

ApiHandler->>TokenManager: creates session with current user

TokenManager->>ApiHandler: returns token of user's session

  


ApiHandler->>User: send login state and token
```

#### **allowed methods**
***`POST`***

#### **request target**
*/api/login*

#### **function description**
by given login and password in body, logins to get authorization token, which is required to play. only registered user can login.

#### **body example**
```js
    {
        "login": "very_login",
        "password": "abcde12345"
    }
```

#### **responses**
* `200 OK`\
    login is successful, token returned
    
    *response body"*
    ```js
    {
        "token": "aabbbccc123"
    }
    ```
    **login and password criteria**
    - login size more or equal to **3**
    - password size more or equal to **6**. must contain at least 1 digit

* `400 bad_request`\
    body data is wrong or login and password doesnt match to any registered user

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
title: api profile diagram
---
sequenceDiagram
actor User
ApiHandler->>ApiHandler: gets Authorization token
ApiHandler->>TokenManager: gets user's uuid by token
TokenManager->>ApiHandler: user's uuid
ApiHandler->>UserDataManager: gets user by uuid
UserDataManager->>ApiHandler: returns user or error code
ApiHandler->>User: send user data
```

#### **allowed methods**
***`GET/HEAD`***

#### **request target**
*/api/profile*

#### **function description**
requires authorization token. by this token gets profile information from db.

#### **responses**
* `200 OK`\
    authorization is correct, send profile data in response
    *response body"*
    ```js
    {
        "login": "loginlogin",
        "password": "pass123123"
    }
    ```
---
## Game API
### API enqueue
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)

#### **action diagram**
```mermaid
---

title: api profile diagram

---

  

sequenceDiagram

actor User

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
to start a game, you need to add yourself to the queue. you will be added to queue to search for an opponent by this api function execution. call wait_for_opponent immediately after that.

#### **request body example**
body must be empty

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
title: api registger diagram
---
sequenceDiagram
actor User1
participant Server
actor User2

User1->>Server: enqueue
User1->>Server: wait_for_opponent
Server-->Server: enqueuing
User2->>Server: enqueue
User2->>Server: wait_for_opponent
Server-->Server: enqueuing
Server->>User1: opponent_found
Server->>User2: opponent_found

```

#### **allowed methods**
***`GET/HEAD`***

#### **request target**  
_api/game/wait_for_opponent_

#### **function description**
long-poll function to read data about future session. use it after enqueuing to the game and wait for response until enemy is found. when found, returns session ID to join the game.

#### **request body example**

```js

```

#### **responses**

* `200 success`
opponent found if body contains SessionId.
*response body:*
```js
{
	"sessionid": "ADD124558846"
}
```
---
### API session_state
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization) 

#### **allowed methods**
***`GET/HEAD`***

#### **request target**  
_/api/game/session_state?sessionId=SESSION_ID_

#### **function description**
request to get session state. session id should be passed as URL parameter.

#### **responses**
* `200 OK`  
*response body:*
***TO SEE RESPONSE EXAMPLE: [[session_state]]***
* `400 url_parameters_error`
```json
{
	"error_name":"url_parameters_error"
	"description":"this api function requires url parameters"
}
```
* `400 wrong_sessionId`
```json
{
	"error_name":"wrong_sessionId"
	"description":"no session with such sessionId"
}
```
---
### API session_state_change
#### [<span style="color:#f58a42"><b>Long-Poll</b></span>](http_api.md#Long-Poll)
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### **action diagram**
```mermaid
sequenceDiagram
	actor Player1
	participant Server
	actor Player2
	Player1 ->> Server: session_state_change
	Player2 ->> Server: session_state_change
	Player1 ->> Server: makes a move
	Server --> Player1: edited_game_state
	Server --> Player2: edited_game_state
	Player1 ->> Server: session_state_change
	Player2 ->> Server: session_state_change
	Player2 ->> Server: makes a move
	Server --> Player1: edited_game_state
	Server --> Player2: edited_game_state
	
```
#### **allowed methods**
***`GET/HEAD`***
#### **request target**  
_/api/game/session_state_change?sessionId=SESSION_ID_

#### **function description**
Long-Poll function hangs until some action happens in the session. once it is, poller gets response with new game state. to get next notification send this function again. if game state happens before you resend poller, you still get LAST game state, immediately.

#### **responses**
* `200 OK`  
*response body:*
	SEE [[session_state]]
* `400 url_parameters_error`
```json
{
	"error_name":"url_parameters_error"
	"description":"this api function requires url parameters"
}
```
* `400 wrong_sessionId`
session you are trying to get access to does not exist.
```json
{
	"error_name":"wrong_sessionId"
	"description":"no session with such sessionId"
}
```
---
### API move
#### [<span style="color:#87ff8b"><b>requires authorization</b></span>](http_api.md#Requires%20Authorization)
#### **allowed methods**
***`POST`***
#### **request target**  
_/api/game/move?sessionId=SESSION_ID_

#### **function description**
function tells the game about player's move. sessionId must be passed as URL parameter, body should contain move information.

#### **request body example**
```json
{
	"move_type":STRING,//"walk", ...
	"posX":UNSIGNED,
	"posY":UNISIGNED
}
```

#### **responses**

* `200 OK`  
everything is ok, now opponent is moving
* `400 wrong_move`
according to the game rules, player cant make such move.
```json
{
	"error_name": "wrong_move",
	"description": "GameManager"
}
```
* `400 not_your_move`
now there is an enemy's move, you cant do anything.
```json
{
	"error_name": "not_your_move",
	"description": "the opponent's move is now"
}
```
* `400 access_denied`
you don't have access to make a move in this match. probably you are not the player.
```json
{
	"error_name": "access_denied",
	"description": "you have no access to do this action"
}
```
* `400 no_such_session`
session you are trying to get access to does not exist.
```json
{
	"error_name": "no_such_session",
	"description": "session you are trying to get access to does not exist"
}
```
* `400 wrong_body_data`
body data is messed up. check the example above.
```json
{
	"error_name": "body_data_error",
	"description": "wrong body data"
}
```