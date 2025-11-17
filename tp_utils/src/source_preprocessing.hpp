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


#include <cstdint>
#include <filesystem>
#include <limits>
#include <variant>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>


namespace tputils {


  class SourceFile {
  public:

    inline bool has_error() const { return !error.empty(); }
    inline const std::string &get_error() const { return error; }

    std::string get_section_text(const std::string &p_name) const;

    template<typename Iter>
    std::string get_section_text(const std::string &p_name, const Iter &p_defined) const {
      std::set<VariableId> defined;
      for (const std::string &var : p_defined) {
        if (variables.contains(var)) {
          defined.insert(variables.at(var));
        }
      }

      return _get_section_text(p_name, defined);
    }
    bool has_section(const std::string &p_name) const;
    inline size_t get_section_count() const { return sections.size(); }

    SourceFile(const std::filesystem::path &p_path);

  private:
    typedef size_t VariableId;
    static constexpr VariableId VARIABLE_NONE = std::numeric_limits<size_t>::max();
    
    struct PreprocIfDef {
      VariableId variable = VARIABLE_NONE; // If this is a variable none
      size_t otherwise_index;
      enum class Kind : uint8_t {
        IF_NDEF   = 0b00,
        IF_DEF    = 0b01,
        ELSE      = 0b10,
        ELSE_NDEF = ELSE | IF_NDEF,
        ELSE_DEF  = ELSE | IF_DEF,
      } kind;
    };

    struct PreprocDefine {
      VariableId variable;
    };

    typedef std::variant<PreprocIfDef, PreprocDefine, std::string> SectionToken;

    typedef std::vector<SectionToken> Section;

  private:
    VariableId _get_or_add_variable_id(const std::string &p_variable_name);
    std::string _get_section_text(const Section *p_section, std::set<VariableId> &p_defined) const;
    std::string _get_section_text(const std::string &p_name, std::set<VariableId> &p_defined) const;

  private:
    Section header;
    std::unordered_map<std::string, Section> sections;
    std::unordered_map<std::string, VariableId> variables;
    VariableId next_var_id = 0;

    std::string error;
  };
}

