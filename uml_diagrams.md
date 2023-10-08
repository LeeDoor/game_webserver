
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

