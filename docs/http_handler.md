# [http_handler]("https://github.com/LeeDoor/hex_chess_backend/tree/main/src/http_handler")
## what is it for
this module contains a bunch of files to handle each http request. this module is used by [server]("https://github.com/LeeDoor/hex_chess_backend/tree/main/src/server") module via **HttpHandler** class.
## classes
* **HttpHandler** - connecting point with this module. its *operator()* handles passed HTTP reuqest and responses it, sending data to **one** of passed send functions (one for string response, other for file response). inside contains two separated handlers for API requests and static file requests
* **Static Handler** - handles requests for static files, like html pages and javascript scripts. it also requires sender for string because if request is wrong, it will send string error text in JSON format.
* **Api Handler** - handles api requests. in project there will be a bunch of api entry points, so i created a class system to handle them more comfortable. each API entry point is presented with map, where key is HTTP target, like */api/send/privet/* and value is **ApiFunctionExecutor** object. each **ApiFunctionExecutor** object is created with special builder **ApiFunctionBuilder**.
* **ApiFunctionBuilder** - creates **ApiFunctionExecutor** objects with required:
    1. allowed methods
    2. LIST WILL BE FINISHED AS PROJECT PROGRESS MOVE ON
* **ApiFunctionExecutor** - launches execution of **ApiFunction**. in fact, this class is a wrapper over the **ApiFunction**, since first it runs all the verification functions to correlate the request data and the allowed data (for example, so that the request method is correct) and only then **ApiFunction** is executed.
* **ApiFunction** - contains data built by **ApiFunctionBuilder**. it is just a function of api method.

## graph
whole class system looks like this: 
```mermaid
---
title: namespace http_handler
---
classDiagram
    class HttpHandler {
        +operator() (http_request&&, send_string&&, send_file&&)
        -HandleRequest(http_request&&, send_string&&, send_file&&)
        -IsApiRequest(http_request&) bool

        -StaticHandler
        -ApiHandler
    }
    note for HttpHandler "handles all http requests on server"

    class ApiHandler {
        +HandleApiFunction(http_request, sender_func)
        -request_to_executor: map < string, ApiFunctionExeuctor >
        -BuildTargetsMap()
    }
    note for ApiHandler "handles all http API requests"

    class StaticHandler {
        +HandleFile(path, root, tringSender, fileSender)
        -SendFile(path, fileSender)
        -SendWrongPathError(stringSender)
        -SendNoAccessError(stringSender)
        -IsSubdirectory(path, root): bool
        
    }
    note for StaticHandler "handles all requests for files"

    class ApiFunction {
        -allowed_methods: vector of methods
        -executor: Function param: request, sender_func
    }
    note for ApiFunction "function unit being executed at the end"

    class ApiFunctionExecutor {
        +execute(http_request, sender_func)
        -MatchMethod() bool
        -api_function : ApiFunction
    }
    note for ApiFunctionExecutor "checks all requirements before\ncalling ApiFunction\n(e.g. auth check, method check)"

    class ApiFunctionBuilder {
        +SetMethods(vector< method >)
        +SetExecutionFunction(function)
        +GetProduct() ApiFunctionExeuctor

        -allowed_methods: vector < methods >
        -execution_function: function
    }
    note for ApiFunctionBuilder "creates ApiFunctionExecutor.\nsets its ApiFunction parameters,\nallowed methods, auth requirement etc."

    class JsonHandler 
    HttpHandler ..> ApiHandler : contains
    HttpHandler ..> StaticHandler : contains
    JsonHandler <.. ApiHandler : depends
    JsonHandler <.. StaticHandler :depends
    ApiHandler ..> ApiFunctionExecutor : contains map of
    ApiHandler --> ApiFunctionBuilder : uses
    ApiFunctionExecutor ..> ApiFunction : contains & calls
```
