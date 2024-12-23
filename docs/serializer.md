# [serializer](https://github.com/LeeDoor/game_webserver/tree/main/src/serializer)
## What is it for
To communicate with http requests it is necessary to serialize and deserialize data from text according to one single format. I am using JSON. To separate logic of serialization with user-class, i use interface for each module in the project. I separated serializers for faster building and linking.