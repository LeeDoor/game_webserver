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

## API

all non-ok responses have same body type. for example:

```js
{
    "error_name": "object_is_not_found",
    "description": "object that you are trying to access is not found"
}
```

<details>
<summary><b>register</b></summary>

**request target**
*/api/register*

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

</details>

<details>
<summary><b>login</b></summary>

**request target**
*/api/login*

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

</details>

<details>
<summary><b>profile</b></summary>

**requires authorization**

**request target**
*/api/profile*

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

</details>