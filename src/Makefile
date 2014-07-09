matcher: matcher.h main.cpp csvparser.cpp csvparser.h
	$(CXX) $(CXXFLAGS) -o matcher main.cpp csvparser.cpp

tests: tests.cpp
	c++ -I../gtest-1.7.0/include -o tests tests.cpp ../gtest-1.7.0/make/gtest_main.a

http: http.cpp matcher.h
	$(CXX) $(CXXFLAGS) -o http http.cpp -lPocoNet -lPocoUtil -lPocoFoundation

httpclient:
	curl -i -X GET 'localhost:9090/rate?destination=1321&source=123456789&length=30'

