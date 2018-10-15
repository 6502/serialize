ALL: test

clean:
	rm -rf test

test: test.cpp serialize.h
	g++ -Wall -O3 -otest test.cpp
