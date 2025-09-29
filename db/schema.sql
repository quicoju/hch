-- Tables
-- ======
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

-- Indexes
-- =======
-- This unique index provides basic protection for same day
-- overlappings, the more complicated cases are handled by
-- the "prervent_overlapping_reservations" trigger
CREATE UNIQUE INDEX IF NOT EXISTS idx_reservtions_room_date
    ON reservations(room_id, begin_date);

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
INSERT OR IGNORE INTO rooms (name, capacity) VALUES
  ('room-101', 1),
  ('room-102', 1),
  ('room-103', 1),
  ('room-201', 1),
  ('room-202', 1),
  ('room-203', 1);

