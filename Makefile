CFLAGS = -O3 -march=native -fomit-frame-pointer -ffast-math -fPIC -shared

.PHONY: test benchmark clean

mask.so:
	${CC} ${CFLAGS} mask.c -o $@

test: mask.so
	sqlite3 <test/sanity.sql

benchmark: mask.so
	python3 bench/generate.py
	time sqlite3 bench/db.sqlite <bench/find.sql

clean:
	rm -f mask.so bench/db.sqlite
