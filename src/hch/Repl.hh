#pragma once

#include <sstream>
#include <string>

struct Repl {
  using string = std::string;
  using Tokens = std::vector<string>;

  virtual string prompt() const = 0;
  virtual void execute(const string&, const Tokens&) = 0;

  void run() {
    while (true) {
      char *line = readline(prompt().c_str());
      if (!line) break;              /* EOF */
      if (*line) add_history(line);  /* don't keep empty lines */
      execute_command(tokenize(line));
      free(line);
    }
  }

private:
  Tokens tokenize(const string &line) {
    std::istringstream iss(line);
    string token;
    Tokens tokens;

    while (iss >> token)
      tokens.push_back(token);

    return tokens;
  }

  void execute_command(const Tokens& t) {
    if (t.empty()) return;
    string command = t[0];
    execute(command, t);
  }
};
