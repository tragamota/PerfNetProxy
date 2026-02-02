//
// Created by Ian on 30-1-2026.
//

#ifndef PERFNETPROXY_HTTPPARSER_H
#define PERFNETPROXY_HTTPPARSER_H

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <cstdint>

enum class HttpVersion : uint8_t {
    HTTP_1_1,
    HTTP_2
};

enum class HttpMethod : uint8_t {
    GET,
    POST,
    PUT,
    DELETE_,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT_
};

enum class ParseResult : uint8_t {
    NEED_MORE_DATA,
    MESSAGE_COMPLETE,
    PARSE_ERROR
};

enum class HttpParserState : uint8_t {
    REQUEST_LINE,
    HEADERS,
    BODY,
    COMPLETE,
    ENCOUNTERED_ERROR
};

struct HttpMessage {
    HttpMethod method;
    HttpVersion version;

    bool keepAlive = false;
    bool chunked = false;

    std::string_view uri{};
    std::string_view host{};
    std::string_view connection{};
    std::string_view accept{};
    std::string_view acceptEncoding;
    size_t contentLength = 0;
    std::string_view contentType{};

    std::unordered_map<std::string, std::string> headers;
    std::unique_ptr<uint8_t[]> body;
};

class HttpParser {
    struct HttpParserContext {
        constexpr static size_t MAX_HEADER_SIZE = 1024;
        constexpr static size_t MAX_BODY_SIZE = 10 * 1024 * 1024;

        HttpParserState state = HttpParserState::REQUEST_LINE;

        size_t totalHeaderBytesRead{0};
        size_t totalBodyBytesRead{0};
    };

    HttpParserContext m_InternalParseContext{};
    HttpMessage m_HttpMessage{};

    std::string m_errorMessage {};

    bool parseRequestLine(std::span<const uint8_t>& data);
    bool parseHeaders(std::span<const uint8_t>& data);
    bool parseBody(std::span<const uint8_t>& data);

    [[nodiscard]] static std::optional<const std::string_view> readHttpLine(std::span<const uint8_t> &data);
public:
    HttpParser() = default;
    ~HttpParser() = default;

    ParseResult tryParse(std::span<const uint8_t> data);

    [[nodiscard]] const HttpMessage& GetParserMessage() const;
    [[nodiscard]] const std::string& GetErrorMessage() const { return m_errorMessage; }

    void reset();
};


#endif //PERFNETPROXY_HTTPPARSER_H
