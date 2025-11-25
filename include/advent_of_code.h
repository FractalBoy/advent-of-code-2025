#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <array>
#include <stdexcept>

namespace ssl
{
    struct Deleter
    {
        void operator()(SSL_CTX *ptr) const;
        void operator()(SSL *ptr) const;
    };

    using ssl = std::unique_ptr<SSL, Deleter>;
    using ssl_ctx = std::unique_ptr<SSL_CTX, Deleter>;

    std::string last_error_string();
    ssl_ctx make_ssl_ctx();
    ssl connect_host_port(ssl_ctx &ctx, const char *hostname, const char *port);
}

namespace AdventOfCode2025
{
    class Day
    {
    public:
        virtual ~Day() = default;
        std::string Part1();
        std::string Part2();

    protected:
        int dayNumber;

    private:
        std::string GetInput();
        virtual std::string DoPart1(std::string &) = 0;
        virtual std::string DoPart2(std::string &) = 0;
    };

    class Day1 : public Day
    {
    public:
        Day1();
        ~Day1() = default;

    private:
        std::string DoPart1(std::string &) override;
        std::string DoPart2(std::string &) override;
    };

    struct HttpResponse
    {
        int status;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    class HttpsClient
    {
    public:
        HttpsClient() = default;
        ~HttpsClient() = default;

        std::unique_ptr<HttpResponse> GetInput(int day);

    private:
        void ReadChunkedBody(ssl::ssl &ssl, std::string &response);
        void ReadContentLengthBody(ssl::ssl &ssl,
                                   const std::unordered_map<std::string, std::string> &headers,
                                   std::string &response);
    };
}
