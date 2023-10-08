# UML diagrams online
i use [this](https://www.mermaidchart.com) website to visualize UML diagrams 

### http server class diagram
```mermaid
---
title: namespace http_server
---
classDiagram
    class ServeHttp{
        +ServeHttp(io_context, endpoint, handler)
    }
    class Listener{
        +Run()
        -OnAccept(error_code, socket)
        -AsyncRunSession(socket&&)
        -io_context&
        -tcp_acceptor
        -request_handler: template lambda
    }
    class Session{
        +Run()
        -Read()
        -OnRead(error_code, bytes_read)
        -HandleRequest(request&&)
        -Write(response&&)
        -OnWrite(error_code, bytes_read)
        -Close()
        -request
    }

    ServeHttp-->Listener : calls Run()
    Listener-->Session : calls Run()
```

### http server call sequence

```mermaid
---
title: http server call sequence
---
flowchart TB
    ServeHttp --> Listener::Run
    subgraph Listener
        Listener::Run --1--> DoAccept
        DoAccept --2--> OnAccept
        OnAccept --3--> AsyncRunSession
        OnAccept --4--> DoAccept
    end
    subgraph Session
        AsyncRunSession --> Session::Run
        Session::Run --> Read
        Read --> OnRead
        OnRead --> HandleRequest
        HandleRequest --> Write
        Write --> OnWrite
        OnWrite --connection is closed--> Close
        OnWrite --socket is up and running--> Read
    end
```

### http handler class diagram

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

    HttpHandler ..> ApiHandler : contains
    HttpHandler ..> StaticHandler : contains
    ApiHandler ..> ApiFunctionExecutor : contains map of
    ApiHandler --> ApiFunctionBuilder : uses
    ApiFunctionExecutor ..> ApiFunction : contains & calls
```
