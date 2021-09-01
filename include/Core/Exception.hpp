#pragma once

#include <exception>
#include <string>

#include <fmt/core.h>

#include <Utils/StringUtils.hpp>

namespace spdlog
{
    class logger;
}

namespace obe::Debug
{
    extern std::shared_ptr<spdlog::logger> Log;
}

namespace obe
{
    class DebugInfo
    {
    public:
        std::string_view file;
        int line;
        std::string_view function;
        DebugInfo(std::string_view file, int line, std::string_view function)
            : file(file)
            , line(line)
            , function(function)
        {
        }
    };
#define EXC_INFO_WRAPPER() DebugInfo(__FILE__, __LINE__, __func__)
#define EXC_INFO EXC_INFO_WRAPPER()

    class Exception : public std::exception
    {
    private:
        std::string m_message;

    public:
        Exception(const Exception& e) noexcept
        {
            m_message = e.m_message;
        }
        Exception(std::string id, DebugInfo info)
        {
            m_message = fmt::format("Exception [{}] occured\n", id);
            m_message += fmt::format("  In file: '{}' (line {})\n", info.file, info.line);
            m_message += fmt::format("  In function: {}\n", info.function);
        }
        template <class... Args> void error(Args&&... args)
        {
            const std::string errorMsg = fmt::format(std::forward<Args>(args)...);
            m_message += fmt::format("  Error: {}\n", errorMsg);
            if (Debug::Log)
            {
                fprintf(stderr, "%s", m_message.c_str());
            }
        }
        template <class... Args> void hint(Args&&... args)
        {
            const std::string hintMsg = fmt::format(std::forward<Args>(args)...);
            m_message += fmt::format("  Hint: {}\n", hintMsg);
        }
        const char* what() const noexcept override;
        Exception& nest(const std::exception& exception)
        {
            m_message += "  Cause:\n";
            m_message
                += "    " + Utils::String::replace(exception.what(), "\n", "\n    ");
            return *this;
        }
    };

    inline const char* Exception::what() const noexcept
    {
        return m_message.c_str();
    }
}
