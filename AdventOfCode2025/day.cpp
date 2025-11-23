#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "advent_of_code.h"

using namespace AdventOfCode2025;

namespace net
{
    struct AddrInfoDeleter
    {
        void operator()(struct addrinfo *ai) const noexcept
        {
            if (ai)
            {
                freeaddrinfo(ai);
            }
        }
    };

    using unique_addrinfo = std::unique_ptr<struct addrinfo, AddrInfoDeleter>;

    struct Fd
    {
        int v{-1};
        explicit Fd(int fd) : v(fd) {}
    };

    struct FdCloser
    {
        void operator()(Fd *fd) const noexcept
        {
            if (fd)
            {
                if (fd->v != -1)
                {
                    close(fd->v);
                }

                delete fd;
            }
        }
    };

    using unique_fd = std::unique_ptr<Fd, FdCloser>;

    inline unique_addrinfo resolve(const char *hostname, const char *port)
    {
        struct addrinfo hints = {
            .ai_flags = AI_NUMERICSERV,
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM};
        struct addrinfo *result = nullptr;
        int rc = getaddrinfo(hostname, port, &hints, &result);
        if (rc != 0)
        {
            throw std::runtime_error("getaddrinfo: " + std::string(gai_strerror(rc)));
        }
        return unique_addrinfo(result);
    }

    inline unique_fd connect(unique_addrinfo &ai)
    {
        for (auto rp = ai.get(); rp != nullptr; rp = rp->ai_next)
        {
            int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (fd == -1)
            {
                continue;
            }

            unique_fd sock{new Fd(fd)};

            if (connect(sock->v, rp->ai_addr, rp->ai_addrlen) == 0)
            {
                return sock;
            }
        }

        throw std::runtime_error("connect: all address candidates failed");
    }
}

namespace ssl
{
    inline std::string last_error_string()
    {
        std::array<char, 256> buf{};
        unsigned long code = ERR_get_error();
        if (code == 0)
            return "unknown OpenSSL error";
        ERR_error_string_n(code, buf.data(), buf.size());
        return std::string(buf.data());
    }

    inline void init_once()
    {
        static bool inited = []
        {
            OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);
            return true;
        }();
        (void)inited;
    }

    struct SslCtxDeleter
    {
        void operator()(SSL_CTX *ctx) const noexcept
        {
            if (ctx)
            {
                SSL_CTX_free(ctx);
            }
        }
    };

    using unique_ssl_ctx = std::unique_ptr<SSL_CTX, SslCtxDeleter>;

    inline unique_ssl_ctx make_ssl_ctx()
    {
        init_once();

        SSL_CTX *raw = SSL_CTX_new(TLS_client_method());
        if (!raw)
        {
            throw std::runtime_error("SSL_CTX_new: " + std::string(last_error_string()));
        }

        unique_ssl_ctx ctx(raw);

        if (SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION) != 1)
        {
            throw std::runtime_error("SSL_CTX_set_min_proto_version: " + std::string(last_error_string()));
        }

        SSL_CTX_set_options(ctx.get(), SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);

        if (SSL_CTX_set_default_verify_paths(ctx.get()) != 1)
        {
            throw std::runtime_error("SSL_CTX_set_default_verify_paths: " + std::string(last_error_string()));
        }

        SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);
        return ctx;
    }

    struct SslDeleter
    {
        void operator()(SSL *ssl) const noexcept
        {
            if (ssl)
            {
                SSL_free(ssl);
            }
        }
    };

    using unique_ssl = std::unique_ptr<SSL, SslDeleter>;

    inline unique_ssl connect_fd(SSL_CTX *ctx, int fd, const char *hostname)
    {
        unique_ssl ssl(SSL_new(ctx));

        if (!ssl)
        {
            throw std::runtime_error("SSL_new: " + std::string(last_error_string()));
        }

        if (SSL_set_fd(ssl.get(), fd) != 1)
        {
            throw std::runtime_error("SSL_set_fd: " + std::string(last_error_string()));
        }

        if (hostname != nullptr && SSL_set_tlsext_host_name(ssl.get(), hostname) != 1)
        {
            throw std::runtime_error("SSL_set_tlsext_host_name: " + std::string(last_error_string()));
        }

        if (SSL_connect(ssl.get()) != 1)
        {
            throw std::runtime_error("SSL_connect: " + std::string(last_error_string()));
        }

        return ssl;
    }
}

namespace AdventOfCode2025
{
    std::string Day::Part1()
    {
        std::string input = this->GetInput();
        return this->DoPart1(input);
    }

    std::string Day::Part2()
    {
        std::string input = this->GetInput();
        return this->DoPart2(input);
    }

    std::string Day::GetInput()
    {
        std::ifstream icache;
        const std::string path = "day" + std::to_string(this->dayNumber) + ".txt";

        icache.open(path);

        if (icache.is_open())
        {
            const auto size = icache.seekg(0, std::ios::end).tellg();
            icache.seekg(0, std::ios::beg);
            std::string content(size, '\0');
            icache.read(content.data(), size);
            return content;
        }

        const char *hostname = "adventofcode.com";
        auto ai = net::resolve(hostname, "443");
        auto sock = net::connect(ai);

        SSL_library_init();
        SSL_load_error_strings();

        auto ctx = ssl::make_ssl_ctx();
        auto ssl = ssl::connect_fd(ctx.get(), sock->v, hostname);

        std::ifstream session_file;
        session_file.open("session.txt");

        if (!session_file.is_open())
        {
            throw std::runtime_error("You must put your session cookie in session.txt.");
        }

        std::string session;
        std::getline(session_file, session);
        session_file.close();
        session.erase(session.find_last_not_of("\n") + 1);

        std::string request = "GET /2025/day/" + std::to_string(this->dayNumber) + "/input HTTP/1.1\r\n" +
                              "Host: " +
                              hostname +
                              "\r\n" +
                              "Connection: close\r\n" +
                              "User-Agent: https://github.com/FractalBoy/advent-of-code-2025 by reisner.marc@gmail.com\r\n" +
                              "Cookie: session=" + session + "\r\n" +
                              "\r\n";

        if (SSL_write(ssl.get(), request.c_str(), static_cast<int>(request.size())) != static_cast<int>(request.size()))
        {
            throw std::runtime_error("SSL_write: " + ssl::last_error_string());
        }

        std::array<char, 16 * 1024> buffer{};
        int bytes_read = 0;

        std::string response;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        int status = -1;

        while ((bytes_read = SSL_read(ssl.get(), buffer.data(), static_cast<int>(buffer.size()))) > 0)
        {
            response.append(buffer.data(), bytes_read);

            size_t pos = 0;
            bool headers_done = false;

            while ((pos = response.find("\r\n")) != std::string::npos)
            {
                std::string line = response.substr(0, pos);
                response.erase(0, pos + 2);

                if (line.empty())
                {
                    headers_done = true;
                    body = response;
                    response.clear();
                    break;
                }

                if (status == -1)
                {
                    size_t first_space = line.find(' ');
                    size_t second_space = line.find(' ', first_space + 1);

                    if (first_space == std::string::npos || second_space == std::string::npos)
                    {
                        throw std::runtime_error("Malformed HTTP response status line: " + line);
                    }

                    status = std::stoi(line.substr(first_space + 1, second_space - first_space - 1));
                }
                else
                {
                    size_t colon = line.find(':');

                    if (colon == std::string::npos)
                    {
                        throw std::runtime_error("Malformed HTTP header line: " + line);
                    }

                    std::string key = line.substr(0, colon);
                    std::string value = line.substr(colon + 1);
                    size_t first_non_space = value.find_first_not_of(" \t");

                    if (first_non_space != std::string::npos)
                    {
                        value = value.substr(first_non_space);
                    }

                    headers[key] = value;
                }
            }

            if (headers_done)
            {
                break;
            }
        }

        if (bytes_read < 0)
        {
            throw std::runtime_error("SSL_read: " + ssl::last_error_string());
        }

        int length = atoi(headers["Content-Length"].c_str());
        int remaining = length - static_cast<int>(body.size());

        std::vector<char> body_buffer(remaining);

        if (body.size() < length)
        {
            bytes_read = SSL_read(ssl.get(), body_buffer.data(), remaining);

            if (bytes_read < 0)
            {
                throw std::runtime_error("SSL_read: " + ssl::last_error_string());
            }

            body.append(body_buffer.data(), bytes_read);
        }

        if (status != 200)
        {
            throw std::runtime_error("HTTP request failed with status " + std::to_string(status) + ". Body: " + body);
        }

        std::ofstream ocache;
        ocache.open(path);

        ocache << body;

        return body;
    }
}