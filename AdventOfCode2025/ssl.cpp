#include "advent_of_code.h"

#include <openssl/err.h>
#include <string>
#include <memory>
#include <array>
#include <stdexcept>

namespace ssl
{
    void Deleter::operator()(SSL_CTX *ptr) const
    {
        SSL_CTX_free(ptr);
    }

    void Deleter::operator()(SSL *ptr) const
    {
        SSL_free(ptr);
    }

    std::string last_error_string()
    {
        std::array<char, 256> buf{};
        unsigned long code = ERR_get_error();
        if (code == 0)
            return "unknown OpenSSL error";
        ERR_error_string_n(code, buf.data(), buf.size());
        return std::string(buf.data());
    }

    void init_once()
    {
        static bool inited = []
        {
            OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);
            return true;
        }();
        (void)inited;
    }

    ssl_ctx make_ssl_ctx()
    {
        init_once();

        SSL_CTX *raw = SSL_CTX_new(TLS_client_method());
        if (!raw)
        {
            throw std::runtime_error("SSL_CTX_new: " + std::string(last_error_string()));
        }

        ssl_ctx ctx(raw);

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
        return std::move(ctx);
    }

    ssl connect_host_port(ssl_ctx &ctx, const char *hostname, const char *port)
    {
        ssl ssl(SSL_new(ctx.get()));

        if (!ssl)
        {
            throw std::runtime_error("SSL_new: " + std::string(last_error_string()));
        }

        BIO *b = BIO_new_connect((hostname + std::string(":") + port).data());

        if (!b)
        {
            throw std::runtime_error("BIO_new_connect: " + std::string(last_error_string()));
        }

        if (BIO_do_connect(b) <= 0)
        {
            throw std::runtime_error("BIO_do_connect: " + std::string(last_error_string()));
        }

        // SSL_set_bio transfers ownership of b to ssl, so no need to free b separately
        SSL_set_bio(ssl.get(), b, b);

        if (SSL_set_tlsext_host_name(ssl.get(), hostname) != 1)
        {
            throw std::runtime_error("SSL_set_tlsext_host_name: " + std::string(last_error_string()));
        }

        if (SSL_connect(ssl.get()) != 1)
        {
            throw std::runtime_error("SSL_connect: " + std::string(last_error_string()));
        }

        return std::move(ssl);
    }
}