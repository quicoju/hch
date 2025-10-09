#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <tuple>

#include <sqlite3.h>

/**
   @brief Thin wrapper around sqlite3.h

   This class attempts to make the usage of the "sqlite3" library
   a bit easier using the features of C++ like RAII and exceptions,
   while keeping overhead low and simplicity in its usage.

   *Usage examples*

   These examples assume that the "SQLite.hh" header
   is included and the code is within a function.

   1. Open a connection:

      SQLite db{"db/file.db"};

   2. Read a file with SQL or DDL statements:
       db.read_file("path/schema.sql");

   3. Execute multiple SQL statements without parameter binding:

      db.execute(R"(
        DELETE FROM table_1;
        DELETE FROM table_2;
        INSERT OR IGNORE INTO table_1(id) VALUES(101), (102), (103);
     )");

   4. INSERT, DELETE and UPDATE statements. Prepare the statement
       and then call "execute"

      auto stmt = db.prepare(R"(
        DELETE FROM reservations
        WHERE room_id = ?
          and date = ?
      )");

      stmt.execute(id, date);

   5. SELECT statements. Prepare the statement and then call "bind" to
      bind the parameters, finally iterate with "next".

      auto stmt = db->prepare(R"(
        SELECT begin_date, duration_days
          FROM reservations
          WHERE room_id = ?
      )");

      stmt.bind(id);

      while (stmt.next()) {
        auto date = stmt.get<std::string>();
       rooms.emplace_back(date});
     }

   Connections are closed one the "db" object is destroyed.
   Exceptions will be thrown if something goes wrong.
 */

struct SQLite {
  SQLite(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
      std::string error = sqlite3_errmsg(db_);
      sqlite3_close(db_);
      throw std::runtime_error{"Cannot open database: " + error};
    }
  }

  ~SQLite() {
    if (db_) {
      sqlite3_close(db_);
    }
  }

  SQLite(const SQLite&) = delete;
  SQLite& operator=(const SQLite&) = delete;

  SQLite(SQLite&& other) noexcept : db_{other.db_} {
    other.db_ = nullptr;
  }

  SQLite& operator=(SQLite&& other) noexcept {
    if (this != &other) {
      if (db_) sqlite3_close(db_);
      db_ = other.db_;
      other.db_ = nullptr;
    }
    return *this;
  }

  void execute(const std::string& sql) {
    int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, nullptr);
    if (result != SQLITE_OK)
      throw std::runtime_error{sqlite3_errmsg(db_)};
  }

  void read_file(const std::string& path) {
      std::ifstream ifs{ path };
      std::stringstream ss;
      ss << ifs.rdbuf();
      execute(ss.str());
  }

  /* This class will take the task of managing the details related to:
   * - statement parameter binding
   * - statement execution
   * - result retrieval
   * - statement cleanup
   * The instance is only allowed to be moved around but not copied
   */
  struct Statement {
    Statement(sqlite3* db, const std::string& sql) : db_{db} {
      if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
        throw std::runtime_error{sqlite3_errmsg(db_)};
      }
    }

    ~Statement() {
      if (stmt_) sqlite3_finalize(stmt_);
    }

    // only allow moving instances, not copying
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&& other) noexcept
      : db_{other.db_}, stmt_{other.stmt_} {
      other.stmt_ = nullptr;
    }

    template<typename... Args>
    void execute(Args&&... args) {
      if constexpr (sizeof...(args) > 0) {
        bind_params(1, std::forward<Args>(args)...);
      }

      int result = sqlite3_step(stmt_);
      if (result != SQLITE_DONE && result != SQLITE_ROW) {
        throw std::runtime_error{sqlite3_errmsg(db_)};
      }
    }

    template<typename... Args>
    void bind(Args&&... args) {
      bind_params(1, std::forward<Args>(args)...);
    }

    bool next() {
      int result = sqlite3_step(stmt_);
      if (result == SQLITE_ROW) return true;
      if (result == SQLITE_DONE) return false;
      throw std::runtime_error{sqlite3_errmsg(db_)};
    }

    template<typename T>
    T get(int column = 0) const {
      if constexpr (std::is_same_v<T, int> || std::is_same_v<T, size_t>) {
        return sqlite3_column_int(stmt_, column);
      }
      else if constexpr (std::is_same_v<T, double>) {
        return sqlite3_column_double(stmt_, column);
      }
      else if constexpr (std::is_same_v<T, std::string>) {
        const char* text = (const char*)sqlite3_column_text(stmt_, column);
        return std::string{text};
      }
      else
        throw std::runtime_error{"Can't 'get' unsupported type"};
    }

  private:
    sqlite3* db_;
    sqlite3_stmt* stmt_ = nullptr;

    template<typename T>
    void bind_param(int index, T&& value) {
      int result = SQLITE_OK;

      if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
        result = sqlite3_bind_int64(stmt_, index, static_cast<int64_t>(value));
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
        result = sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT);
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, const std::string>) {
        result = sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT);
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, const char*>) {
        result = sqlite3_bind_text(stmt_, index, value, -1, SQLITE_TRANSIENT);
      }
      else {
        throw std::runtime_error{"Unsupported parameter type for binding"};
      }

      if (result != SQLITE_OK) {
        throw std::runtime_error{
          "Parameter binding failed: " + std::string{sqlite3_errmsg(db_)}};
      }
    }

    template<typename T, typename... Rest>
    void bind_params(int index, T&& first, Rest&&... rest) {
      bind_param(index, std::forward<T>(first));
      if constexpr (sizeof...(rest) > 0) {
        bind_params(index + 1, std::forward<Rest>(rest)...);
      }
    }

    void bind_params(int) {}
  };

  Statement prepare(const std::string& sql) {
    return Statement{db_, sql};
  }

private:
  sqlite3* db_;
};
