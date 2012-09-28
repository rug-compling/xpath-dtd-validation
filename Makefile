all: queryvalidator

queryvalidator: SimpleDTD.o QueryScope.o textfile.o main.o
	g++ -lboost_system-mt -lboost_filesystem-mt -lxerces-c -lxml2 -lxqilla -o $@ -Wall \
		QueryScope.o SimpleDTD.o textfile.o main.o

%.o: %.cpp
	g++ -Wall -I/usr/include/libxml2 -c -o $@ $<
