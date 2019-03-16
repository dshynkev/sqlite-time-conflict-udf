Search for time conflicts with SQLite UDFs
==========================================

The goal of this is primarily to explore the performance of SQLite UDFs on large datasets.

The particular case studied here is course sections, which the University of Waterloo represents in the form
```json
{
  "start_time": "16:00",
  "end_time": "17:20",
  "weekdays": "TTh"
}
```
where start times are aligned to `00` and `30` minutes and end times are aligned to `50` and `20` minutes.

`mask.c` defines an efficiently foldable representation of course sections.
It exports the following SQLite UDFs:

* `BIT_OR`: aggregate with the obvious semantics (SQLite is missing bitwise aggregations):
  ```sql
  > SELECT BIT_OR(column1) FROM (VALUES (0x01), (0x04), (0x18));
  < 29 -- 0b00011101
  ```
* `MASK(str)`: derives a section mask (64-bit integer) from a string describing the section:
  ```sql
  > SELECT MASK('MWF 08:00 09:20');
  < 5910974511382528 -- 0b10101000000000000000000000000000001110000000000000000
  ```
* `OVERLAP(lhs, rhs)`: determines if two sections overlap in time from their masks:
  ```sql
  > SELECT OVERLAP(MASK('M 08:00 09:20'), MASK('M 09:00 09:50'));
  < 1
  ```

To see the results, run `make benchmark`. This will:

* generate a sample dataset of 1 million sections
* search for courses that do not conflict with 3 randomly chosen ones
* display runtime metrics of said search

The execution time came out to ~60ms on my machine, which seems very reasonable.
