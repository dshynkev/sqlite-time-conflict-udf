import os
import random
import sqlite3

SUBJECTS = ["CS", "ECE", "MATH"]
DAYS = ["M", "T", "W", "Th", "F", "S", "Su"]
HOURS_PER_WEEK = 6

def init(cur, path):
    with open(path) as f:
        cur.executescript(f.read())

def make_row(cur):
    days_in_week = random.randint(1, 3)
    hours_per_day = HOURS_PER_WEEK / days_in_week

    days = "".join(random.sample(DAYS, days_in_week))

    start_hour = random.randint(6, 21)
    start_minutes = random.choice([0, 30])
    end_hour = start_hour + hours_per_day
    end_minutes = 50 if start_minutes == 0 else 20

    start_time = "%02d:%02d" % (start_hour, start_minutes)
    end_time = "%02d:%02d" % (end_hour, end_minutes)

    json_input = {
        "days" : days,
        "start_time": start_time,
        "end_time": end_time
    }

    cur.execute("INSERT INTO section(mask, days, start_time, end_time)"
                "VALUES (SECT_MASK(?), ?, ?, ?)",
                (str(json_input), days, start_time, end_time))

if __name__ == "__main__":
    rootdir = os.path.dirname(__file__)
    conn = sqlite3.connect(os.path.join(rootdir, "db.sqlite"))
    conn.enable_load_extension(True)
    cur = conn.cursor()

    init(cur, os.path.join(rootdir, "schema.sql"))
    for _ in range(1000000):
        make_row(cur)
    conn.commit()
