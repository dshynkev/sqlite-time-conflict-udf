SELECT load_extension('./mask');

CREATE TABLE section (
  id INTEGER PRIMARY KEY,
  mask INTEGER NOT NULL,
  days TEXT NOT NULL,
  start_time TEXT NOT NULL,
  end_time TEXT NOT NULL
);
