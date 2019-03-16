#include <stddef.h>
#include <stdint.h>

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

/*
** A section mask is of the form [unused] [day_mask] [time_mask] where:
**                               64    56 55      48 47        0
**  - time_mask bits correspond to half-hour intervals of the 24-hour day
**  - day_mask bits correspond to the 7 days of the week
** It is therefore best represented as a 64-bit unsigned integer.
*/
typedef uint64_t mask_t;

void mask_or_step(sqlite3_context *context, int argc, sqlite3_value **argv) {
  mask_t *buffer = (mask_t *)sqlite3_aggregate_context(context, sizeof(mask_t));
  mask_t x = sqlite3_value_int64(argv[0]);
  *buffer |= x;
}

void mask_passthrough(sqlite3_context *context) {
  mask_t *buffer = (mask_t *)sqlite3_aggregate_context(context, sizeof(mask_t));
  sqlite3_result_int64(context, *buffer);
}

static mask_t try_day(const unsigned char *str) {
  switch (str[0]) {
  case 'M':
    return 0b0000001;
    break;
  case 'T':
    /* We are guaranteed str[0] != '\0', so this is within bounds */
    if (str[1] == 'h')
      return 0b0001000;
    else
      return 0b0000010;
    break;
  case 'W':
    return 0b0000100;
    break;
  case 'F':
    return 0b0010000;
    break;
  case 'S':
    /* Likewise */
    if (str[1] == 'u')
      return 0b1000000;
    else
      return 0b0100000;
    break;
  default:
    return 0;
  }
}

inline static int is_digit(unsigned char c) { return '0' <= c && c <= '9'; }

static mask_t try_time(const unsigned char *str) {
  if (!(is_digit(str[0]) && is_digit(str[1]) && str[2] == ':' &&
        is_digit(str[3]) && is_digit(str[4])))
    return 0;
  /* Twice the number of hours */
  int halves = 2 * (10 * (str[0] - '0') + (str[1] - '0'));
  switch (str[3]) {
  case '2':
  case '3':
    halves += 1;
    break;
  case '5':
    halves += 2;
    break;
  }
  return 1ULL << halves;
}

/*
** The sole argument is any encoding of a single section
** where days of the week and times occur in plaintext.
*/
void section_mask(sqlite3_context *context, int argc, sqlite3_value **argv) {
  mask_t day_mask = 0;
  mask_t start_mask = 0, end_mask = 0;
  for (const unsigned char *str = sqlite3_value_text(argv[0]); *str; ++str) {
    day_mask |= try_day(str);
    if (!start_mask)
      start_mask |= try_time(str);
    else
      end_mask |= try_time(str);
  }
  /* Must overlap in days and hours separately */
  day_mask = ((start_mask - 1) ^ (end_mask - 1)) | (day_mask << 48);
  sqlite3_result_int64(context, day_mask);
}

void section_overlap(sqlite3_context *context, int argc, sqlite3_value **argv) {
  mask_t lhs = sqlite3_value_int64(argv[0]);
  mask_t rhs = sqlite3_value_int64(argv[1]);
  mask_t overlap = lhs & rhs;
  /* Must overlap in days and hours separately */
  overlap = (overlap & ((1ULL << 48) - 1)) && (overlap & ~((1ULL << 48) - 1));
  sqlite3_result_int(context, overlap);
}

int sqlite3_mask_init(sqlite3 *db, char **es, const sqlite3_api_routines *api) {
  SQLITE_EXTENSION_INIT2(api);

  sqlite3_create_function(db, "BIT_OR", 1, SQLITE_ANY | SQLITE_DETERMINISTIC,
                          NULL, NULL, mask_or_step, mask_passthrough);
  sqlite3_create_function(db, "MASK", 1, SQLITE_ANY | SQLITE_DETERMINISTIC,
                          NULL, section_mask, NULL, NULL);
  sqlite3_create_function(db, "OVERLAP", 2, SQLITE_ANY | SQLITE_DETERMINISTIC,
                          NULL, section_overlap, NULL, NULL);

  return SQLITE_OK;
}
