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

CREATE TABLE amenities (
  name TEXT PRIMARY KEY
);

CREATE TABLE rooms_amenities (
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

-- Insert some default rooms
INSERT OR IGNORE INTO amenities(name)
VALUES
  ('AirConditioning'),
  ('Balcony'),
  ('MiniBar'),
  ('Wifi');

INSERT OR IGNORE INTO rooms (id, name, capacity)
VALUES
  (1, '101', 1),
  (2, '102', 1),
  (3, '103', 1),
  (4, '201', 1),
  (5, '202', 1),
  (6, '203', 1);

INSERT OR IGNORE into rooms_amenities (room_id, amenity_name)
VALUES
  (1, 'Wifi'),
  (2, 'Wifi'),    (2,'Balcony'),
  (3, 'Wifi'),    (3,'Balcony'),
  (4, 'Balcony'), (4,'AirConditioning'),
  (5, 'Wifi'),
  (6, 'MiniBar');


-- Rates
-- =====
INSERT OR IGNORE INTO rate_types (id, name)
VALUES
    (1, 'Base'),
    (2, 'Capacity'),
    (3, 'Amenity');

INSERT OR IGNORE INTO rates (type_id, key_name, value)
VALUES
    (1, '',        58.99), -- default base rate
    (1, '101',    100.99), -- premium room rate
    (2, '',         0.20), -- default capacity surcharge
    (3, 'Wifi',     5.00), -- wifi amenity
    (3, 'Balcony', 15.00); -- balcony amenity
