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
