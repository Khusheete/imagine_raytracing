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


#include "source_preprocessing.hpp"
#include "debug.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <variant>


namespace tputils {

  SourceFile::SourceFile(const std::filesystem::path &p_path) {
    int ifdef_depth = 0;
    int ifndef_ignore = -1;

    std::stack<size_t> ifdef_stack;
    std::set<VariableId> defined;

    std::ostringstream section_text_stream;
    std::string line;
    std::string current_section_name;
    Section *current_section = &header;

    std::stack<std::ifstream> files;
    std::stack<std::filesystem::path> file_paths;
    std::stack<unsigned int> line_numbers;
    const std::filesystem::path parent_dir = p_path.parent_path();
    auto open_file = [&](const std::filesystem::path &p_path, const bool local) {
      std::filesystem::path file;
      if (local) {
        file = parent_dir / p_path;
      } else {
        file = p_path;
      }
      files.emplace(file);
      if (!files.top().good()) {
        error = std::format(
          "Cannot open file {}: {}", file.relative_path().string(), std::strerror(errno)
        );
        return false;
      }
      file_paths.push(std::move(file));
      line_numbers.push(0);
      return true;
    };
    if (!open_file(p_path, false)) return;

    auto section_push_text = [&]() -> void {
      std::string str = section_text_stream.str();
      if (!str.empty()) {
        current_section->push_back(section_text_stream.str());
        section_text_stream.str(std::string());
      }
    };

    auto set_error = [&](const std::string &p_error) -> void {
      error = std::format(
        "Error in `{}:{}`: {}",
        file_paths.top().relative_path().string(),
        line_numbers.top(),
        p_error
      );
    };

    while (true) {
      if (!std::getline(files.top(), line)) {
        files.pop();
        file_paths.pop();
        line_numbers.pop();
        if (files.empty()) {
          break;
        }
        continue;
      }
      
      line_numbers.top() += 1;
      
      if (line.starts_with('#')) { // This is a preprocessor expression
        std::stringstream line_ss(line);
        std::string preprocess;
        line_ss >> preprocess;
        
        bool known_preproc = true;

        // Not the most efficient, but this will do :)
        if (!preprocess.compare("#section")) {
          current_section_name.clear();
          line_ss >> current_section_name;

          if (ifdef_depth != 0) {
            set_error(std::format(
              "Section {} is starting but if clause is not finished",
              current_section_name
            ));
            return;
          }

          if (files.size() > 1) {
            set_error("Cannot have #section inside included file");
            return;
          }

          section_push_text();

          // Setup next section
          if (!has_section(current_section_name)) {
            sections[current_section_name] = Section();
          }
          current_section = &sections[current_section_name];
        } else if (!preprocess.compare("#ifdef")) {
          ifdef_depth += 1;
          if (ifndef_ignore != -1) continue;

          std::string variable_name;
          line_ss >> variable_name;
          VariableId id = _get_or_add_variable_id(variable_name);

          section_push_text();

          ifdef_stack.push(current_section->size());

          current_section->push_back(PreprocIfDef{
            .variable = id,
            .otherwise_index = 0,
            .kind = PreprocIfDef::Kind::IF_DEF,
          });
        } else if (!preprocess.compare("#ifndef")) {
          ifdef_depth += 1;
          if (ifndef_ignore != -1) continue;

          std::string variable_name;
          line_ss >> variable_name;
          VariableId id = _get_or_add_variable_id(variable_name);

          if (defined.contains(id)) { // Optimize out ifndef
            ifndef_ignore = ifdef_depth;
            continue;
          }

          section_push_text();

          ifdef_stack.push(current_section->size());

          current_section->push_back(PreprocIfDef{
            .variable = id,
            .otherwise_index = 0,
            .kind = PreprocIfDef::Kind::IF_NDEF,
          });
        } else if (!preprocess.compare("#elseifdef")) {
          if (ifdef_depth == 0) {
            set_error("elseifdef does not follow a ifdef clause");
            return;
          }

          if (ifndef_ignore != -1) {
            if (ifndef_ignore == ifdef_depth) {
              // Override
              ifndef_ignore = -1;
            } else {
              continue;
            }
          }

          std::string variable_name;
          line_ss >> variable_name;
          VariableId id = _get_or_add_variable_id(variable_name);

          bool first_unoptimized_ifdef = ifdef_stack.size() != (size_t)ifdef_depth;

          section_push_text();

          if (first_unoptimized_ifdef) {
            ifdef_stack.push(current_section->size());
          } else {
            PreprocIfDef &prev = std::get<PreprocIfDef>(current_section->at(ifdef_stack.top()));
            ifdef_stack.top() = prev.otherwise_index = current_section->size();
          }

          current_section->push_back(PreprocIfDef{
            .variable = id,
            .otherwise_index = 0,
            .kind = (first_unoptimized_ifdef)? PreprocIfDef::Kind::IF_DEF : PreprocIfDef::Kind::ELSE_DEF,
          });
        } else if (!preprocess.compare("#elseifndef")) {
          if (ifdef_depth == 0) {
            set_error("elseifndef does not follow a ifdef clause");
            return;
          }

          if (ifndef_ignore != -1) {
            if (ifndef_ignore == ifdef_depth) {
              // Override
              ifndef_ignore = -1;
            } else {
              continue;
            }
          }
          
          std::string variable_name;
          line_ss >> variable_name;
          VariableId id = _get_or_add_variable_id(variable_name);

          if (defined.contains(id)) { // Optimize out ifndef
            ifndef_ignore = ifdef_depth;
            continue;
          }

          bool first_unoptimized_ifdef = ifdef_stack.size() != (size_t)ifdef_depth;

          section_push_text();

          if (first_unoptimized_ifdef) {
            ifdef_stack.push(current_section->size());
          } else {
            PreprocIfDef &prev = std::get<PreprocIfDef>(current_section->at(ifdef_stack.top()));
            ifdef_stack.top() = prev.otherwise_index = current_section->size();
          }

          current_section->push_back(PreprocIfDef{
            .variable = id,
            .otherwise_index = 0,
            .kind = (first_unoptimized_ifdef)? PreprocIfDef::Kind::IF_NDEF : PreprocIfDef::Kind::ELSE_NDEF,
          });
        } else if (!preprocess.compare("#else")) {
          if (ifdef_depth == 0) {
            set_error("else does not follow a ifdef clause");
            return;
          }

          if (ifndef_ignore != -1) {
            if (ifndef_ignore == ifdef_depth) {
              // Override
              ifndef_ignore = -1;
              continue;
            } else {
              continue;
            }
          }

          bool first_unoptimized_ifdef = ifdef_stack.size() != (size_t)ifdef_depth;

          if (!first_unoptimized_ifdef) {
            section_push_text();

            PreprocIfDef &prev = std::get<PreprocIfDef>(current_section->at(ifdef_stack.top()));
            ifdef_stack.top() = prev.otherwise_index = current_section->size();

            current_section->push_back(PreprocIfDef{
              .variable = VARIABLE_NONE,
              .otherwise_index = 0,
              .kind = PreprocIfDef::Kind::ELSE,
            });
          }
        } else if (!preprocess.compare("#endif")) {
          if (ifdef_depth == 0) {
            set_error("endif does not follow a ifdef clause");
            return;
          }

          ifdef_depth -= 1;
          if (ifndef_ignore > ifdef_depth) {
            // No longer ignoring values
            ifndef_ignore = -1;
            continue;
          } else if (ifndef_ignore != -1) {
            continue;
          }

          // Push the text for the current section
          section_push_text();

          // If the stack is too small, it means that the last PreprocIfDef was an optimized #else.
          // When that's the case, there is no otherwise_index to set.
          if (ifdef_stack.size() == (size_t)ifdef_depth + 1) {
            // Set ifdef otherwise jump index
            size_t ifdef_index = ifdef_stack.top();
            ifdef_stack.pop();
          std::get<PreprocIfDef>(current_section->at(ifdef_index)).otherwise_index = current_section->size();
          }
        } else if (!preprocess.compare("#define")) {
          std::string variable_name;
          line_ss >> variable_name;

          VariableId id = _get_or_add_variable_id(variable_name);
          defined.insert(id);

          section_push_text();
          current_section->push_back(PreprocDefine{
           .variable = id,
          });
        } else if (!preprocess.compare("#include")) {
          if (ifndef_ignore != -1) continue;

          char c;
          line_ss >> c;

          bool local_path;

          if (c == '\"') {
            local_path = true;
          } else if (c == '<') {
            local_path = false;
          } else {
            set_error(std::format("Unknown include path delimiter `{}`", c));
            return;
          }

          std::string path;
          line_ss >> path;
          const char last_char = path[path.size() - 1];
          path.erase(path.end() - 1);
          if (c == '\"' && last_char != '\"') {
            set_error("Missing closing quotation mark in include");
            return;
          } else if (c == '<' && last_char != '>') {
            set_error("Missing closing angular bracket in include");
            return;
          }

          if (!open_file(path, local_path)) return;
        } else {
          known_preproc = false;
        }

        if (known_preproc) continue;
      }

      // If nothing happened before, add the line to the section_text_stream
      if (ifndef_ignore == -1) {
        section_text_stream << line << "\n";
      }
    }

    // Push the last section text
    section_push_text();
  }


  std::string SourceFile::get_section_text(const std::string &p_name) const {
    std::set<VariableId> defined;
    return _get_section_text(p_name, defined);
  }


  std::string SourceFile::_get_section_text(const Section *p_section, std::set<VariableId> &p_defined) const {
    const size_t section_size = p_section->size();

    std::ostringstream section_text;
    size_t index = 0;
    bool else_jump = false;

    while (index < section_size) {
      const SectionToken &token = p_section->at(index);

      if (std::holds_alternative<PreprocIfDef>(token)) {
        const PreprocIfDef &ifdef = std::get<PreprocIfDef>(token);

        bool is_else = (uint8_t)ifdef.kind & (uint8_t)PreprocIfDef::Kind::ELSE;
        if (!else_jump && is_else) {
          index = ifdef.otherwise_index;
          continue;
        }

        bool is_ifdef = (uint8_t)ifdef.kind & (uint8_t)PreprocIfDef::Kind::IF_DEF;
        if (ifdef.variable != VARIABLE_NONE && is_ifdef != p_defined.contains(ifdef.variable)) {
          else_jump = !is_else || else_jump;
          index = ifdef.otherwise_index;
          continue;
        }
      } else if (std::holds_alternative<PreprocDefine>(token)) {
        const PreprocDefine &define = std::get<PreprocDefine>(token);
        p_defined.insert(define.variable);
      } else if (std::holds_alternative<std::string>(token)) {
        const std::string &text = std::get<std::string>(token);
        section_text << text;
      }

      else_jump = false;
      index += 1;
    }

    return section_text.str();
  }


  std::string SourceFile::_get_section_text(const std::string &p_name, std::set<VariableId> &p_defined) const {
    bool available_section = has_section(p_name);
    ASSERT_WARNING(available_section, "Section " << p_name << " does not exist in source preprocessor.");
    if (!available_section) return "";

    std::ostringstream section_text;
    section_text << _get_section_text(&header, p_defined);
    section_text << _get_section_text(&sections.at(p_name), p_defined);
    return section_text.str();
  }


  bool SourceFile::has_section(const std::string &p_name) const {
    return sections.contains(p_name);
  }


  SourceFile::VariableId SourceFile::_get_or_add_variable_id(const std::string &p_variable_name) {
    if (variables.contains(p_variable_name)) {
      return variables[p_variable_name];
    }
    const VariableId var_id = next_var_id;
    variables[p_variable_name] = var_id;
    next_var_id += 1;
    return var_id;
  }
}
