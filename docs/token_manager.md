# [token_manager](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/token_manager)
application uses authorization system. when player logins by passing login and password to */api/login* (must be registered before) server responses with token value. to have access to forbidden data as concrete user, request must contain **Authorization** header. example:
```
Authorization: Bearer {token}
```
token_manager module communicates with redis database to keep link between token and user's login.

## classes
* **token** - not a class or struct, just string called *Token*
* **TokenGenerator** - class generates token with 32 hexadecimal numbers
* **ITokenManager** - interface for token manager. interface used for more easy and smooth migration between db if needed.
*  **TokenManagerRedis** - child of *i_token_manager*. implements all features for *redis* bd.

## graph
```mermaid
---
title: namespace token_manager
---

classDiagram
	class ITokenManager{
		+GenerateToken()
		+GetUuidByToken(token) uuid?
		+GetValue() Map Token->Uuid
	}
	class TokenManagerRedis{
	}
	class Token{
		+GenerateToken()
	}
	class TokenGenerator{
	}
	ITokenManager --> Token
	TokenManagerRedis --|> ITokenManager
	TokenManagerRedis --> TokenGenerator
```
