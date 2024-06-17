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
## Debug API

debug api is required to get data from app structures. to execute them you need to send admin login and password.

### player_tokens

**request target**
*/api/debug/player_tokens*

**request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

**responses**
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
    
* `401 unauthorized`\
    admin login or password are invalid.

### user_data

**request target**
*/api/debug/user_data*

**request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

**responses**
* `200 OK`\
    user_data sent successfully

    *response body example:*
    ```js
    {
        {"player_serega", "2012_serega@enjoyer"},
        {"player_vasyan", "2009_vasili@pupkin"},
        {"na@ibat0R", "Ilinskaya12"}
    }
    ```
    
* `401 unauthorized`\
    admin login or password are invalid.

### matchmaking_queue

**request target**
*/api/debug/matchmaking_queue*

**request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

**responses**
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
    
* `401 unauthorized`\
    admin login or password are invalid.


### sessions_list

**request target**
*/api/debug/sessions_list*

**request body example**
```js
    {
        "login": "admin2009",
        "password": "nadejni_parol2"
    }
```

**responses**
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
    
* `401 unauthorized`\
    admin login or password are invalid.



## User API
### register
**action diagram**
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
**request target**
*/api/register*

**function description**
by given login and password in body, creates account. data stored in db, you cant register with same login more than once. after registration you need to login to play the game and use other features. 

**request body example**
```js
    {
        "login": "very_login",
        "password": "abcde12345"
    }
```
**login and password criteria**
- login size more or equal to **3**
- password size more or equal to **6**. must contain at least 1 digit

**responses**
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

### login
**action diagram**
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

**request target**
*/api/login*

**function description**
by given login and password in body, logins to get authorization token, which is required to play. only registered user can login.

**body example**
```js
    {
        "login": "very_login",
        "password": "abcde12345"
    }
```

**responses**
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

### profile
<span style="color:#87ff8b"><b>requires authorization</b></span>

**action diagram**
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


**request target**
*/api/profile*

**function description**
requires authorization token. by this token gets profile information from db.

**responses**
* `200 OK`\
    authorization is correct, send profile data in response
    
    *response body"*
    ```js
    {
        "login": "loginlogin",
        "password": "pass123123"
    }
    ```

* `401 unauthorized`\
    no authorization header, invalid token or person with this token is removed

    **error_name meanings**
    - **unathorized**: request must be authorized with Authorization header
    - **invalid_token**: request authorization is invalid (token in Authorization header has wrong format)
    - **person_removed**: person with this token is unavailable (probably removed)

