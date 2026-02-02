//
// Created by Ian on 31-1-2026.
//

#include "httpParser.hpp"

#include <algorithm>
#include <format>

constexpr std::pair<std::string_view, HttpMethod> methodTable[] = {
    {"GET", HttpMethod::GET},
    {"POST", HttpMethod::POST},
    {"PUT", HttpMethod::PUT},
    {"DELETE", HttpMethod::DELETE_},
    {"HEAD", HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"TRACE", HttpMethod::TRACE},
    {"CONNECT", HttpMethod::CONNECT_},
};

inline std::optional<HttpMethod> parseToHttpMethod(const std::string_view method_view) {
    for (auto [name, method]: methodTable)
        if (name == method_view) return method;

    return std::nullopt;
}

std::vector<std::string_view> getSubstringsByDelimiter(const std::string_view inputStringView, const char delimiter) {
    std::vector<std::string_view> subStrings;

    size_t offset = 0;

    for (auto delimiterPosition = inputStringView.find(delimiter, offset); delimiterPosition != std::string_view::npos; delimiterPosition = inputStringView.find(delimiter, offset)) {
        subStrings.emplace_back(inputStringView.substr(offset, delimiterPosition - offset));
        offset = delimiterPosition + 1;
    }

    subStrings.emplace_back(inputStringView.substr(offset));

    return subStrings;
}

std::optional<std::vector<std::string_view> > getSubstringsByFirstDelimiter(
    std::string_view inputStringView, const char delimiter) {
    const auto delimiterPosition = inputStringView.find(delimiter);

    if (delimiterPosition == std::string_view::npos) {
        return std::nullopt;
    }

    std::vector<std::string_view> subStrings;

    subStrings.emplace_back(inputStringView.substr(0, delimiterPosition));
    subStrings.emplace_back(inputStringView.substr(delimiterPosition + 1));

    return subStrings;
}

bool HttpParser::parseRequestLine(std::span<const uint8_t>& data) {
    const auto lineOption = readHttpLine(data);

    if (!lineOption.has_value()) {
        return false;
    }

    auto requestLineTokens = getSubstringsByDelimiter(lineOption.value(), ' ');

    const auto method = parseToHttpMethod(requestLineTokens[0]);

    if (!method.has_value()) {
        m_errorMessage = std::format("Error parsing method: {}", requestLineTokens[0]);
        m_InternalParseContext.state = HttpParserState::ENCOUNTERED_ERROR;
        return false;
    }

    if (requestLineTokens[2] != "HTTP/1.1") {
        m_errorMessage = std::format("Error parsing HTTP version: {}", requestLineTokens[2]);
        m_InternalParseContext.state = HttpParserState::ENCOUNTERED_ERROR;
        return false;
    }

    m_HttpMessage.method = method.value();
    m_HttpMessage.headers["uri"] = requestLineTokens[1];
    m_HttpMessage.version = HttpVersion::HTTP_1_1;

    return true;
}

bool HttpParser::parseHeaders(std::span<const uint8_t>& data) {
    for (std::optional<std::string_view> lineOption = readHttpLine(data); lineOption.has_value(); lineOption = readHttpLine(data)) {
        if (lineOption->empty()) {
            return true;
        }

        const auto headerTokens = getSubstringsByFirstDelimiter(*lineOption, ':');

        if (!headerTokens.has_value()) {
            m_errorMessage = std::format("Error parsing header {}", lineOption.value());
            m_InternalParseContext.state = HttpParserState::ENCOUNTERED_ERROR;
            return false;
        }

        auto headerNameView = (*headerTokens)[0];
        auto headerValueView = (*headerTokens)[1];

        constexpr auto trim = [](std::string_view &v) {
            while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.remove_prefix(1);
            while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.remove_suffix(1);
        };

        trim(headerNameView);
        trim(headerValueView);

        auto headerName = std::string(headerNameView);
        auto headerValue = std::string(headerValueView);

        std::ranges::transform(headerName, headerName.begin(),
                               [](const unsigned char c) { return std::tolower(c); });

        m_HttpMessage.headers.emplace(std::move(headerName), std::move(headerValue));
    }

    return false;
}

bool HttpParser::parseBody(std::span<const uint8_t>& data) {
    if (m_HttpMessage.contentLength == 0) {
        return true;
    }

    if (!m_HttpMessage.body) {
        m_HttpMessage.body = std::make_unique<uint8_t[]>(m_HttpMessage.contentLength);
    }

    size_t bytesNeeded = m_HttpMessage.contentLength - m_InternalParseContext.totalBodyBytesRead;
    size_t bytesToCopy = std::min(bytesNeeded, data.size());

    std::ranges::copy(data, m_HttpMessage.body.get());

    m_InternalParseContext.totalBodyBytesRead += bytesToCopy;

    data = data.subspan(bytesToCopy);

    if (m_InternalParseContext.totalBodyBytesRead == m_HttpMessage.contentLength) {
        return true;
    }

    return false;
}

std::optional<const std::string_view> HttpParser::readHttpLine(std::span<const uint8_t> &data) {
    const auto* cr = static_cast<const uint8_t*>(std::memchr(data.data(), '\r', data.size()));

    if (!cr || cr + 1 >= data.data() + data.size() || cr[1] != '\n') {
        return std::nullopt;
    }

    const size_t lineLength = cr - data.data();
    std::string_view line(reinterpret_cast<const char*>(data.data()), lineLength);

    data = data.subspan(lineLength + 2);

    return line;
}

ParseResult HttpParser::tryParse(std::span<const uint8_t> data) {
    if (m_InternalParseContext.state == HttpParserState::COMPLETE) {
        return ParseResult::MESSAGE_COMPLETE;
    }

    if (data.empty()) {
        return ParseResult::NEED_MORE_DATA;
    }

    if (m_InternalParseContext.state == HttpParserState::REQUEST_LINE) {
        if (parseRequestLine(data)) {
            m_InternalParseContext.state = HttpParserState::HEADERS;
        }
    }
    if (m_InternalParseContext.state == HttpParserState::HEADERS) {
        if (parseHeaders(data)) {
            m_InternalParseContext.state = HttpParserState::BODY;
        };
    }
    if (m_InternalParseContext.state == HttpParserState::BODY) {
        if (parseBody(data)) {
            m_InternalParseContext.state = HttpParserState::COMPLETE;
            return ParseResult::MESSAGE_COMPLETE;
        };
    }

    if (m_InternalParseContext.state == HttpParserState::ENCOUNTERED_ERROR) {
        return ParseResult::PARSE_ERROR;
    }

    return ParseResult::NEED_MORE_DATA;
}

const HttpMessage &HttpParser::GetParserMessage() const {
    return m_HttpMessage;
}

void HttpParser::reset() {
}
