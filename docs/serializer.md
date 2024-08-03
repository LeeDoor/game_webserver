# [serializer](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/serializer)
## what is it for
to communicate with http requests it is necessary to serialize and deserialize data from text according to one single format. i am using JSON. to separate logic of serialization with user-class, i use interface for each module in the project. i separated serializers for faster building and linking.
## graph
graph firstly might look overwhelmly
```mermaid
---
title: serializer
---
classDiagram
    class ISerializer {
        +SerializeEmpty() String
        +SerializeError(error_name, desc) String
        +SerializeMap(std::map(tag_name, value)) String
    }
    
    class JSONSerializer {
    }

    ISerializer <|-- JSONSerializer

    class IHttpSerializer {
        +(De)Serialize(RegistrationData) 
        +(De)Serialize(PublicUser) 
    }
    
    class JSONHttpSerializer {
    }

    IHttpSerializer <|-- JSONHttpSerializer

    class IUserSerializer {
        +(De)Serialize(User) 
        +(De)Serialize(Users) 
    }
    
    class JSONUserSerializer {
    }

    IUserSerializer <|-- JSONUserSerializer

    class ITokenSerializer {
        +(De)Serialize(Token) 
    }
    
    class JSONTokenSerializer {
    }

    ITokenSerializer <|-- JSONTokenSerializer

    class IGameSerializer {
        +(De)Serialize(State) 
        +(De)Serialize(WalkData) 
        +DefineSessionState(json)
    }
    
    class JSONGameSerializer {
    }

    IGameSerializer <|-- JSONGameSerializer

    class ISessionSerializer {
        +(De)Serialize(SessionData) 
    }
    
    class JSONSessionSerializer {
    }

    ISessionSerializer <|-- JSONSessionSerializer
```