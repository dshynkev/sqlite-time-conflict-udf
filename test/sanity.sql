.load ./mask

CREATE TABLE test(
  name TEXT NOT NULL,
  passed BOOLEAN NOT NULL
);

INSERT INTO test(name, passed)
SELECT 'same_day',
        1 = OVERLAP(SECT_MASK('M 08:00 09:20'), SECT_MASK('M 09:00 09:50'));

INSERT INTO test(name, passed)
SELECT 'overlapping_days',
        1 = OVERLAP(SECT_MASK('MW 08:00 09:20'), SECT_MASK('WF 09:00 09:50'));

INSERT INTO test(name, passed)
SELECT 'different_days',
        0 = OVERLAP(SECT_MASK('M 08:00 09:20'), SECT_MASK('W 09:00 09:50'));

INSERT INTO test(name, passed)
SELECT 'different_times',
        0 = OVERLAP(SECT_MASK('M 08:00 09:20'), SECT_MASK('M 09:30 09:50'));

INSERT INTO test(name, passed)
SELECT 'different_hours',
        0 = OVERLAP(SECT_MASK('M 08:00 09:20'), SECT_MASK('M 18:30 19:50'));

SELECT name,
       CASE passed WHEN 1 THEN 'PASSED' ELSE 'FAILED' END AS result
FROM test;
