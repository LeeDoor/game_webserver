# [http_handler](https://github.com/LeeDoor/game_webserver/tree/main/src/http_handler)
## What is it for
This module contains a bunch of files to handle each http request. This module is used by [server](server.md) module via **HttpHandler** class.
## Classes
* **HttpHandler** - connecting point with this module. Its *operator ()* handles passed HTTP reuqest and responses it, sending data to **one** of passed send functions (one for string response, other for file response). Inside contains two separated handlers for API requests and static file requests.
* **Static Handler** - handles requests for static files, like html pages and javascript scripts. It also requires sender for string because if request is wrong, it will send string error text in JSON format.
* **Api Handler** - handles api requests. In project there will be a bunch of api entry points, so i created a class system to handle them more comfortable. Each API entry point is presented with map, where key is HTTP target, like */api/send/privet/* and value is **ApiFunctionExecutor** object. Each **ApiFunctionExecutor** object is created with special builder **ApiFunctionBuilder**.
* **Game Handler** - handles api requests directed to the game e.g. make a move, give up, offer a draw. Game apis are */api/game/some_function*.
* **ApiFunctionBuilder** - creates **ApiFunctionExecutor** objects with required:
    1. Allowed methods
    2. Execution function
    3. Needs authorization or not
    4. Is debug method
* **ApiFunctionExecutor** - launches execution of **ApiFunction**. In fact, this class is a wrapper over the **ApiFunction**, since first it runs all the verification functions to correlate the request data and the allowed data (for example, so that the request method is correct) and only then **ApiFunction** is executed.
* **ApiFunction** - contains data built by  **ApiFunctionBuilder**. It is just a function of api method.
* **ApiFunctionDirector** - handles **ApiFunctionBuilder** to create functions and not overwhelm \*_handler's code (at the cost of overwhelming the schema and the number of files).
* **ResponseMaker** - bunch of functions for preparing http responses. Moved this to separate cpp because it takes a lot of lines to form it up each time i need it.

All handlers have [ISerializer](serializer.md) object. This object can serialize/deserialize data to string and vice versa. Made it with interface because i may want to change format from JSON to XML or smth in future.

## Ownership
Handlers contain objects to handle requests.
* **send_manager** - all api send functions moved here to avoid overwhelming api_handler class.
* **[serializer](serializer.md)** - serializes and deserializes response and request bodies
* **user_manager** - [database manager class](user_manager.md)
* [**token_manager**](token_manager.md) - contains map to get uuid of player by token
* [**queue_manager**](queue_manager.md) - class to communicate with queue.
* [**game_manager**](game_manager.md) - class to balance players in queue, put them into the game and make in-game moves.
## Graph
Whole class system looks like this: 
```mermaid
---
title: namespace http_handler
---

classDiagram
    class HttpHandler {
        +operator() (http_request&&, sender&&)
        -HandleRequest(http_request&&, send_string&&, send_file&&)
        -IsApiRequest(http_request&) bool

        -StaticHandler
        -ApiHandler
        -ISerializer
    }

    class ApiHandler {
        +HandleApiFunction(http_request, sender_func)
        -request_to_executor: map < string, ApiFunctionExeuctor >
        -BuildTargetsMap()

        -ApiFunctionMethod(RequestNSender)

        -GetTokenFromHeader(header) token 

  -request_to_executor_      
        -serializer
        -send_manager(responser_)
        -user_manager(dm_)
        -token_to_uuid (ttu_)
        -matchmaking_queue(mm_queue_)
    }

    class StaticHandler {
        +HandleFile(path, root, stringSender, fileSender)
        -SendFile(path, fileSender)
        -SendWrongPathError(stringSender)
        -SendNoAccessError(stringSender)
        -IsSubdirectory(path, root): bool

        -ISerializer
    }

	class ApiFunctionParams{
		+allowed_methods: vector of methods
        +executor: Function param: request, sender_func
        +is_debug
	}

    class ApiFunction {
        -allowed_methods: vector of methods
        -executor: Function param: request, sender_func
	    +is_debug
	}

    class ApiFunctionExecutor {
        +execute(http_request, sender_func)
        +GetApiFunction(): apiFunction
        -MatchMethod(method) bool
        -MatchAuthorization(request)
        
        -api_function : ApiFunction
        -token_to_uuid: TokenToUuid?
    }

    class ApiFunctionBuilder {
        +Methods(vector< method >)
        +ExecFunc(function)
        +GetHead()
        +Post()
        +NeedAuthor(ttu)
        +ForDebug()
        +GetProduct() ApiFunctionExeuctor

        -allowed_methods: vector < methods >
        -execution_function: function
        -is_debug: bool
        -token_to_uuid: TokenToUuid?
    }

    class ResponseBuilder {
        +BodyText(body&&)
        +File(const path&)
        +Status(status)
        +Header(name, value&&)
        +Allow(const methods&)
        +GetProduct(const request&)

        -response_: http::response < template_body_type >
    }

    class ApiFunctionDirector {
		GetSomeApiFunction(ExecutorFunction&&) ApiFunctionExecutor
    }

    ApiHandler --> ResponseBuilder : uses
    StaticHandler --> ResponseBuilder : uses
    HttpHandler ..> ApiHandler : contains
    HttpHandler ..> StaticHandler : contains
    ApiHandler ..> ApiFunctionExecutor : contains map of
    ApiHandler --> ApiFunctionDirector : uses
    ApiFunctionDirector --> ApiFunctionBuilder : uses
    ApiFunctionBuilder --> ApiFunctionExecutor : creates
    ApiFunctionExecutor ..> ApiFunction : contains & calls
    ApiFunction --> ApiFunctionParams : uses

```
