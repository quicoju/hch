-- Remove existing reservations
-- ============================
DELETE FROM Reservations;

-- Rooms
-- =====
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
  (3, '103', 3),
  (4, '201', 1),
  (5, '202', 1),
  (6, '203', 1),
  (7, '301', 1),
  (8, '302', 3);

INSERT OR IGNORE INTO rooms_amenities (room_id, amenity_name)
VALUES
  (1, 'Wifi'),
  (2, 'Wifi'),
  (3, 'Wifi'),    (3,'Balcony'),
  (4, 'Balcony'), (4,'AirConditioning'),
  (5, 'Wifi'),
  (6, 'MiniBar');

-- Guests
-- ======
INSERT OR IGNORE INTO guests (id, email)
VALUES
  (1, 'juan.camaney@aol.com'),
  (2, 'sultana.bella@corazon.mx');

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
