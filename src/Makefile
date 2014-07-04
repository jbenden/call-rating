matcher: matcher.h main.cpp csvparser.cpp csvparser.h
	$(CXX) $(CXXFLAGS) -o matcher main.cpp csvparser.cpp

tests: tests.cpp
	c++ -I../gtest-1.7.0/include -o tests tests.cpp ../gtest-1.7.0/make/gtest_main.a
