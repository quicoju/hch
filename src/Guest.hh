#pragma once

// TODO: This class requires a unique identifier for the guests.
// The different backends might want to keep internal identifiers
// for identifying the records, but we need a client facing ID.
// For now we'll use the email address as the client facing ID
// which is unique by nature.
// NOTE: On the other hand, using this identifier might violate
// the user's privacy, that's why this is a temporary solution
struct Guest {
  Guest(const std::string& id)
  {
    // TODO: maybe some logic to validate the ID
    email = id;
  }

  const std::string& id() const
  {
    return email;
  }

private:
  // state
  // =====
  std::string email;
};
