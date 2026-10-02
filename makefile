CXX = g++
CXXFLAGS = -Wall -std=c++11

email_heap: email_heap.cpp
	$(CXX) $(CXXFLAGS) -o email_heap email_heap.cpp

clean:
	rm -f email_heap