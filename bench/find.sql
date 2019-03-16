SELECT load_extension('./mask');

CREATE TEMP TABLE selection AS
SELECT * FROM section LIMIT 3;

SELECT COUNT(*) FROM section
WHERE NOT OVERLAP(mask, (SELECT BIT_OR(mask) FROM selection));
