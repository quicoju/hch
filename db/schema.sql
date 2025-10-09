-- Room
-- ====
CREATE TABLE IF NOT EXISTS rooms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    capacity INTEGER NOT NULL CHECK (capacity > 0)
);

CREATE TABLE IF NOT EXISTS reservations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    room_id INTEGER NOT NULL,
    begin_date DATE NOT NULL,
    duration_days INTEGER NOT NULL CHECK (duration_days > 0),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(room_id) REFERENCES rooms(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS amenities (
  name TEXT PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS rooms_amenities (
  room_id TEXT,
  amenity_name TEXT,
  FOREIGN KEY (room_id) REFERENCES rooms(id),
  FOREIGN KEY (amenity_name) REFERENCES amenities(name)
);

-- Rates
-- =====
CREATE TABLE IF NOT EXISTS rate_types (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE -- 'Base', 'Capacity', 'Amenity'
);

CREATE TABLE IF NOT EXISTS rates (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_id INTEGER NOT NULL,
    key_name TEXT NOT NULL DEFAULT '', -- room_id, amenity name, or empty for global
    value REAL NOT NULL,
    FOREIGN KEY(type_id) REFERENCES rate_types(id)
);

-- Indexes
-- =======
-- This unique index provides basic protection for same day
-- overlappings, the more complicated cases are handled by
-- the "prervent_overlapping_reservations" trigger
CREATE UNIQUE INDEX IF NOT EXISTS idx_reservtions_room_date
    ON reservations(room_id, begin_date);

-- Index for efficient lookups
CREATE INDEX IF NOT EXISTS idx_rates_type_key
    ON rates(type_id, key_name);

-- Triggers
-- ========
CREATE TRIGGER IF NOT EXISTS prevent_overlapping_reservations
BEFORE INSERT ON reservations
FOR EACH ROW
BEGIN
    SELECT CASE
      WHEN EXISTS (
        SELECT 1 FROM reservations
         WHERE room_id = NEW.room_id
           AND date(NEW.begin_date, '+' || NEW.duration_days || ' days') > begin_date
           AND NEW.begin_date < date(begin_date, '+' || duration_days || ' days')
        ) THEN RAISE(ABORT, 'Room is already reserved for overlapping dates')
    END;
END;
