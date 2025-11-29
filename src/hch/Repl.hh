#pragma once

#include <sstream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>


struct Repl {
  using string = std::string;
  using Tokens = std::vector<string>;

  virtual string prompt() const = 0;
  virtual void execute(const string&, const Tokens&) = 0;
  virtual std::vector<string> supported_commands() const { return {}; }

  void run() {
    setup_completion();

    while (true) {
      char *line = readline(prompt().c_str());
      if (!line) break;              /* EOF */
      if (*line) add_history(line);  /* don't keep empty lines */
      execute_command(tokenize(line));
      free(line);
    }
  }

private:
  static Repl* current_repl;

  Tokens tokenize(const string &line) {
    Tokens tokens;
    string token;
    bool in_quotes = false;
    char quote_char = '\0';

    for (auto c: line) {
      if (!in_quotes && (c == '"' || c == '\'')) {
        in_quotes = true;
        quote_char = c;
      }
      else if (in_quotes && c == quote_char) {
        in_quotes = false;
      }
      else if (!in_quotes && std::isspace(c)) {
        if (token.size()) {
          tokens.push_back(token);
          token.clear();
        }
      }
      else {
        token += c;
      }
    }
    // TODO: address the case when the quote isn't properly closed

    // in case it's the last token
    if (token.size())
      tokens.push_back(token);

    return tokens;
  }

  void execute_command(const Tokens& t) {
    if (t.empty()) return;
    string command = t[0];
    execute(command, t);
  }

  void setup_completion() {
    current_repl = this;
    rl_attempted_completion_function = command_completion;
  }

  static char** command_completion(const char* text, int start, int end) {
    if (current_repl && start == 0)
      return rl_completion_matches(text, command_generator);
    return nullptr;
  }

  static char* command_generator(const char* text, int state) {
    static std::vector<string> commands;
    static size_t index;

    if (state == 0) {
      commands = current_repl->supported_commands();
      index = 0;
    }

    while(index < commands.size()) {
      const std::string& cmd = commands[index++];
      if (cmd.find(text) == 0)
        return strdup(cmd.c_str());
    }
    return nullptr;
  }
};

// for suporting command completion
Repl* Repl::current_repl = nullptr;
