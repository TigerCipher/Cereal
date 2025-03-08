// ------------------------------------------------------------------------------
//
// Cereal
// Copyright 2025 Matthew Rogers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// File Name: JsonException.h
// Date File Created: 03/07/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once
#include <exception>
#include <string>


#if _DEBUG

    #if defined(__GNUG__) // GCC / Clang
        #include <execinfo.h>
        #include <dlfcn.h>
        #include <cxxabi.h>
        #include <unwind.h>

        #define DEMANGLE_NAME(name)                                                                                              \
            int         status    = 0;                                                                                           \
            char*       demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);                                        \
            std::string result    = (status == 0) ? demangled : name;                                                            \
            free(demangled);                                                                                                     \
            return result

        #define THROW_JSON_EXCEPTION(message)                                                                                    \
            {                                                                                                                    \
                void* buffer[3];                                                                                                 \
                backtrace(buffer, 3);                                                                                            \
                throw cereal::JsonException(message, __FILE__, __LINE__, buffer[2]);                                              \
            }

namespace detail
{
inline std::string GetFunctionName(const void* addr)
{
    std::string result{};
    Dl_info     info;
    if (dladdr(caller_address, &info) && info.dli_sname)
    {
        int   status;
        char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
        if (status == 0 && demangled)
        {
            std::string result = demangled;
            free(demangled);
            result = "Called from: " + result + " (" + info.dli_fname + ")";
        } else
        {
            result = "Called from: " + std::string(info.dli_sname) + " (" + info.dli_fname + ")";
        }
    }

    void*  buffer[3];
    int    nptrs   = backtrace(buffer, 3);
    char** symbols = backtrace_symbols(buffer, nptrs);
    if (nptrs > 2)
    {
        result += "\n\tCaller’s raw symbol: " + symbols[1];
    }

    return result;
}
} // namespace detail

    #elif defined(_MSC_VER) // MSVC
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #include <dbghelp.h>

namespace detail
{

inline std::string GetFunctionName(const void* addr)
{
    std::string result{};
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);

    DWORD64      displacement                                                     = 0;
    DWORD64      address                                                          = (DWORD64) addr;
    char         symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = { 0 };
    SYMBOL_INFO* symbol                                                           = (SYMBOL_INFO*) symbolBuffer;
    symbol->SizeOfStruct                                                          = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen                                                            = MAX_SYM_NAME;

    if (SymFromAddr(process, address, &displacement, symbol))
    {
        result = "Called from: " + std::string(symbol->Name);
    }

    IMAGEHLP_LINE64 lineInfo = { 0 };
    lineInfo.SizeOfStruct    = sizeof(IMAGEHLP_LINE64);
    DWORD lineDisplacement;
    if (SymGetLineFromAddr64(process, address, &lineDisplacement, &lineInfo))
    {
        result += "\n\t(File: " + std::string(lineInfo.FileName) + ", Line: " + std::to_string(lineInfo.LineNumber) + ")";
    }

    return result;
}

} // namespace detail

        #define THROW_JSON_EXCEPTION(message)                                                                                    \
            {                                                                                                                    \
                void* stack[2];                                                                                                  \
                CaptureStackBackTrace(1, 2, stack, nullptr);                                                                     \
                throw JsonException(message, __FILE__, __LINE__, stack[0]);                                                      \
            }

    #endif

#else
    #define THROW_JSON_EXCEPTION(message) throw cereal::JsonException(message)
#endif

namespace cereal
{

class JsonException : public std::exception
{
public:
    explicit JsonException(const std::string_view message) : mMessage(message) {}

    explicit JsonException(const std::string_view message, std::string_view file, int lineNumber, const void* caller);

    [[nodiscard]] const char* what() const noexcept override { return mMessage.c_str(); }

private:
    std::string mMessage{};
};

} // namespace cereal