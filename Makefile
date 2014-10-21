all:
	$(MAKE) -C gtest-1.7.0/make gtest_main.a 
	$(MAKE) -C src

tests:
	$(MAKE) -C gtest-1.7.0/make gtest_main.a
	$(MAKE) -C src tests

test:
	src/tests
