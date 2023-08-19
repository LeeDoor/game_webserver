#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace http_server {

namespace net = boost::asio;
namespace sys = boost::system;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
typedef http::request<http::string_body> HttpRequest;


void ReportError(beast::error_code ec, std::string what);
class SessionBase {
public:
    SessionBase(const SessionBase&) = delete;
    SessionBase& operator= (const SessionBase&) = delete;

    void Run();
protected:
    ~SessionBase() = default;
    explicit SessionBase (tcp::socket&& socket) : stream_(std::move(socket)) {}
    template <typename Body, typename Fields>
    void Write(http::response<Body, Fields>&& response) {
        auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));
        auto self = GetSharedThis();

        http::async_write(stream_, *safe_response, 
            [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
            self->OnWrite(safe_response->need_eof(), ec, bytes_written);
        });
    }
    beast::tcp_stream stream_;
private:
    void Read();
    void OnRead (beast::error_code ec, std::size_t byets_read);
    void OnWrite(bool close, beast::error_code ec, std::size_t bytes_written);
    void Close();
    virtual void HandleRequest(HttpRequest&& request) = 0;
    virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;

    beast::flat_buffer buffer_;
    HttpRequest request_;
    
};

template <typename RequestHandler>
class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>> {
public:
    template<typename Handler>
    Session(tcp::socket&& socket, Handler&& request_handler) 
        : SessionBase(std::move(socket)), request_handler_(std::forward<Handler>(request_handler)) {}
private:
    void HandleRequest(HttpRequest&& request) override {
        std::string ip = stream_.socket().remote_endpoint().address().to_string(); 
        //logger::LogRequest(request, ip);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        request_handler_(std::move(request), [self = this->shared_from_this(), begin](auto&& response) {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            //logger::LogResponse(response, std::chrono::duration_cast<std::chrono::microseconds>(end - begin));
            self->Write(std::move(response));
        });
    }

    std::shared_ptr<SessionBase> GetSharedThis() override {
        return this->shared_from_this();
    }
    RequestHandler request_handler_;
    
};

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:
    template<typename Handler>
    Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler) :
        ioc_(ioc), acceptor_(net::make_strand(ioc)), request_handler_(std::forward<Handler>(request_handler)) {
        // Открываем acceptor, используя протокол (IPv4 или IPv6), указанный в endpoint
        acceptor_.open(endpoint.protocol());
        // После закрытия TCP-соединения сокет некоторое время может считаться занятым,
        // чтобы компьютеры могли обменяться завершающими пакетами данных.
        // Однако это может помешать повторно открыть сокет в полузакрытом состоянии.
        // Флаг reuse_address разрешает открыть сокет, когда он "наполовину закрыт"
        acceptor_.set_option(net::socket_base::reuse_address(true));
        // Привязываем acceptor к адресу и порту endpoint
        acceptor_.bind(endpoint);
        // Переводим acceptor в состояние, в котором он способен принимать новые соединения
        // Благодаря этому новые подключения будут помещаться в очередь ожидающих соединений
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void Run() {
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this())
        );
    }

    void OnAccept(sys::error_code ec, tcp::socket socket) {
        if(ec) {
            return ReportError(ec, "accept");
        }

        AsyncRunSession(std::move(socket));
        DoAccept();
    }

    void AsyncRunSession(tcp::socket&& socket) {
        std::make_shared<Session<RequestHandler>> (std::move(socket), request_handler_)->Run();
    }

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

template <typename RequestHandler>
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler) {
    // При помощи decay_t исключим ссылки из типа RequestHandler,
    // чтобы Listener хранил RequestHandler по значению
    using MyListener = Listener<std::decay_t<RequestHandler>>;
    std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
}

}  // namespace http_server