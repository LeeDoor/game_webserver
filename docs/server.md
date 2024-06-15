# [server](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/server)
## what is it for
server module launches server and starts handling incoming HTTP requests.
## classes
here you can see graph of classes in server module:
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
        -AsyncRunSession(socket&&, ISerializer, Players)
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
## actions
here is the sequence of operations which each class preparing:
* **listener** class waits for request and passses handling to **session** class
* **session** reads data from user, handles it with [http_handler](http_handler.md) and sends back response from server
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
