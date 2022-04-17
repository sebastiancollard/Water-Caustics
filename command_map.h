#pragma once

#include <functional>
#include <iostream>
#include <unordered_map>

namespace giv {
namespace io {

template <typename Index, typename... Args> class Commands {
public:
  using index_t = Index;
  using function_t = std::function<void(Args &&...)>;
  using map = std::unordered_map<index_t, function_t>;
  using command_type = typename map::value_type;

  void execute(index_t index, Args &&... args) const {
    auto iter = m_commands.find(index);
    if (iter != m_commands.end())
      iter->second(std::forward<Args>(args)...);
  }

  void bind(index_t index, function_t &&func) {
    auto iter = m_commands.find(index);
    if (iter != m_commands.end())
      std::cerr << "Warning: overwriting command mapped to " << index << '\n';
    m_commands.insert(iter, std::forward<function_t>(func));
  }
  void bind(command_type &&command) {
    auto iter = m_commands.find(command.first);
    if (iter != m_commands.end())
      std::cerr << "Warning: overwriting command mapped to " << command.first
                << '\n';
    m_commands.insert(iter, std::forward<command_type>(command));
  }

  Commands &operator|(command_type &&command) {
    bind(std::forward<command_type>(command));
    return *this;
  }

  static command_type makeCommand(index_t index, function_t &&function) {
    return {index, std::move(function)};
  }

private:
  map m_commands;
};

} // namespace io
} // namespace giv
