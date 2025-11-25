#include "advent_of_code.h"
#include <memory>
#include <fstream>
#include <stdexcept>
#include <array>
#include <vector>
#include <bits/stdc++.h>

namespace AdventOfCode2025
{
    std::unique_ptr<HttpResponse> HttpsClient::GetInput(int day)
    {
        auto ctx = ssl::make_ssl_ctx();
        const char *hostname = "adventofcode.com";
        auto ssl_conn = ssl::connect_host_port(ctx, hostname, "443");

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

        std::string request = "GET /2025/day/" + std::to_string(day) + "/input HTTP/1.1\r\n" +
                              "Host: " +
                              hostname +
                              "\r\n" +
                              "Connection: close\r\n" +
                              "User-Agent: https://github.com/FractalBoy/advent-of-code-2025 by reisner.marc@gmail.com\r\n" +
                              "Cookie: session=" + session + "\r\n" +
                              "\r\n";

        if (SSL_write(ssl_conn.get(), request.c_str(), static_cast<int>(request.size())) != static_cast<int>(request.size()))
        {
            throw std::runtime_error("SSL_write: " + ssl::last_error_string());
        }

        std::array<char, 16 * 1024> buffer{};
        int bytes_read = 0;

        std::string response;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        int status = -1;

        while ((bytes_read = SSL_read(ssl_conn.get(), buffer.data(), static_cast<int>(buffer.size()))) > 0)
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
                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
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

        if (headers.find("transfer-encoding") != headers.end() &&
            headers["transfer-encoding"] == "chunked")
        {
            this->ReadChunkedBody(ssl_conn, body);
        }
        else
        {
            this->ReadContentLengthBody(ssl_conn, headers, body);
        }

        auto http_response = std::make_unique<HttpResponse>();
        http_response->status = status;
        http_response->headers = headers;
        http_response->body = body;

        return std::move(http_response);
    }

    void HttpsClient::ReadChunkedBody(ssl::ssl &ssl, std::string &response)
    {
        std::string body;

        while (1)
        {
            int pos = response.find("\r\n");
            std::array<char, 16 * 1024> buffer{};

            if (pos == std::string::npos)
            {
                int read = SSL_read(ssl.get(), buffer.data(), static_cast<int>(buffer.size()));
                if (read < 0)
                {
                    throw std::runtime_error("SSL_read: " + ssl::last_error_string());
                }
                response.append(buffer.data(), read);
                pos = response.find("\r\n");
            }

            if (pos == std::string::npos)
            {
                throw std::runtime_error("Malformed chunked body: missing chunk size line.");
            }

            std::string chunk_size_str = response.substr(0, pos);
            response.erase(0, pos + 2);
            size_t chunk_size = std::stoul(chunk_size_str, nullptr, 16);

            if (chunk_size == 0)
            {
                break;
            }

            chunk_size += 2; // account for trailing \r\n

            if (response.size() >= chunk_size)
            {
                body += response.substr(0, chunk_size);
                response.erase(0, chunk_size);
            }
            else
            {
                int remaining = static_cast<int>(chunk_size - response.size());
                body += response;
                response.clear();

                std::vector<char> buffer(remaining);
                int bytes_read = SSL_read(ssl.get(), buffer.data(), static_cast<int>(buffer.size()));

                if (bytes_read < 0)
                {
                    throw std::runtime_error("SSL_read: " + ssl::last_error_string());
                }

                body.append(buffer.data(), bytes_read);
            }

            body.erase(body.size() - 2); // remove trailing \r\n
        }

        response = body;
    }

    void HttpsClient::ReadContentLengthBody(ssl::ssl &ssl,
                                            const std::unordered_map<std::string, std::string> &headers,
                                            std::string &response)
    {
        if (headers.find("content-length") == headers.end())
        {
            throw std::runtime_error("Content-Length header not found for non-chunked body.");
        }

        size_t content_length = std::stoul(headers.at("content-length"));
        int remaining = content_length - static_cast<int>(response.size());

        if (remaining < 0)
        {
            throw std::runtime_error("Response body is larger than Content-Length.");
        }

        if (remaining == 0)
        {
            return;
        }

        std::vector<char> buffer(remaining);
        int bytes_read = SSL_read(ssl.get(), buffer.data(), static_cast<int>(buffer.size()));

        if (bytes_read < 0)
        {
            throw std::runtime_error("SSL_read: " + ssl::last_error_string());
        }

        response.append(buffer.data(), bytes_read);
    }
}