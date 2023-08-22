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
-abstract HandleRequest(request&&)
-Write(response&&)
-OnWrite(error_code, bytes_read)
-Close()
-request
}
class Session{
-override HandleRequest(request&&)
-handler: template lambda
}

ServeHttp-->Listener : calls Run()
Listener-->Session : calls Run()
Session--|>Session
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
