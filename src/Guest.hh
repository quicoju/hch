#pragma once

#include <string>
#include "config.h"

// TODO: This class requires a unique identifier for the guests.
// The different backends might want to keep internal identifiers
// for identifying the records, but we need a client facing ID.
// For now we'll use the email address as the client facing ID
// which is unique by nature.
// NOTE: On the other hand, using this identifier might violate
// the user's privacy, that's why this is a temporary solution
struct Guest {
  Guest(std::string_view id, Backend* src) : id{id}, src{src}
  {};

  static std::string record(std::string_view email, Backend* src);

  static Guest find_by_id(std::string_view id, Backend* src);

  // state
  // =====
  const std::string id;

private:
  Backend* src;
};
