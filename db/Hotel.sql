-- Tables
-- ======
CREATE TABLE IF NOT EXISTS rooms (
    id TEXT PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS reservations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    room_id TEXT NOT NULL,
    begin_date DATE NOT NULL,
    duration_days INTEGER NOT NULL CHECK (duration_days > 0),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(room_id) REFERENCES rooms(id) ON DELETE CASCADE
);

-- Indexes
-- =======
CREATE INDEX IF NOT EXISTS idx_reservtions_room_date
    ON reservations(room_id, begin_date);

-- TODOs
-- =====
 --Unique key restriction in the "rooms" table
-- Pre-insert trigger to avoid overlapping reservations

-- Insert some default rooms
INSERT OR IGNORE INTO rooms (id) VALUES
  ('A-101'),
  ('A-102'),
  ('A-103'),
  ('A-201'),
  ('A-202'),
  ('A-203');

