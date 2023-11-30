# [token_manager](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/token_manager)
application uses authorization system. when player logins by passing login and password to */api/login* (must be registered before) server responses with token value. to have access to forbidden data as concrete user, request must contain **Authorization** header. example:
```
Authorization: Bearer {token}
```
token_manager module contains map from token to player's uuid. with this module handler can get concrete player's uuid.

## classes
* **token** - not a class or struct, just string called *Token*
* **token_generator** - class generates token with 32 hexadecimal numbers
* **token_to_uuid** - class that contains map from token to player's uuid
