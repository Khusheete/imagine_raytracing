/* ------------------------------------------------------------------------------------------------------------------ *
*                                                                                                                     *
*                                                                                                                     *
*                                                /\                    ^__                                            *
*                                               /#*\  /\              /##@>                                           *
*                                              <#* *> \/         _^_  \\    _^_                                       *
*                                               \##/            /###\ \è\  /###\                                      *
*                                                \/ /\         /#####n/xx\n#####\                                     *
*                   Ferdinand                       \/         \###^##xXXx##^###/                                     *
*                        Souchet                                \#/ V¨\xx/¨V \#/                                      *
*                     (aka. @Khusheete)                          V     \c\    V                                       *
*                                                                       //                                            *
*                                                                     \o/                                             *
*             ferdinand.souchet@etu.umontpellier.fr                    v                                              *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
* Copyright 2025 Ferdinand Souchet (aka. @Khusheete)                                                                  *
*                                                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated        *
* documentation files (the “Software”), to deal in the Software without restriction, including without limitation the *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     *
* permit persons to whom the Software is furnished to do so, subject to the following conditions:                     *
*                                                                                                                     *
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of    *
* the Software.                                                                                                       *
*                                                                                                                     *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO    *
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      *
* SOFTWARE.                                                                                                           *
*                                                                                                                     *
* ------------------------------------------------------------------------------------------------------------------ */


#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>


#define CSI_BOLD "\e[1m"
#define CSI_YELLOW "\e[38;2;180;180;20m"
#define CSI_RED "\e[38;2;220;50;50m"
#define CSI_CLEAR "\e[0m"


#define LOG_WARNING(warning) std::cerr << CSI_YELLOW << CSI_BOLD << "[WARNING] " << CSI_CLEAR << CSI_YELLOW << warning << CSI_CLEAR << std::endl
#define LOG_ERROR(error) std::cerr << CSI_RED << CSI_BOLD << "[ERROR] " << CSI_CLEAR << CSI_RED << error << CSI_CLEAR << std::endl


#define ASSERT_FATAL_ERROR(predicate, error_str)                        \
if (!(predicate)) {                                                     \
    std::stringstream error;                                            \
    error << "Assertion `" << #predicate << "` failed: " << error_str;  \
    LOG_ERROR(error.str());                                             \
    throw std::runtime_error(error.str());                              \
}


#define ASSERT_WARNING(predicate, warning_str)                              \
if (!(predicate)) {                                                         \
    std::stringstream error;                                                \
    error << "Assertion `" << #predicate << "` failed: " << warning_str;    \
    LOG_WARNING(error.str());                                               \
}
