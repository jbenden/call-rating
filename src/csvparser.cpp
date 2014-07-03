#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <string>

#include "csvparser.h"

int
Parser::feed(char ch, char nch)
{
  if (start == 0 && ch == '\"') {
    // Started with an escaped string
    state = 1;
    start++;
    data.str("");
    return 0;
  }

  start++;

  if (state == 1 && ch == '\"') {
    state++;
  } else if (state >= 1 && ch == '\"' && nch != ',') {
    state++;
    ///std::cerr << "State: " << state << std::endl;
    if (state == 3 && nch != ',') {
      data << "\"";
      state = 1;
    }
  } else if (state >= 1 && ch == '\"' && nch == ',') {
    state = 0;
    start = 0;
    line.push_back(data.str());
  } else if (state != 1 && ch == ',') {
    start = 0;
    state = 0;
    line.push_back(data.str());
    data.str("");
  } else if (ch == '\n') {
    start = 0;
    state = 0;
    data.str("");
    return (1);
  } else {
    data << ch;
  }
  //std::cerr << "State: " << state << " Start: " << start << std::endl;
  return 0;
}

/*
int
main (int argc, char *argv[])
{
  Parser p;
  std::ios_base::sync_with_stdio(false);

  while (!std::cin.eof()) {
    if (p.feed(std::cin.get(), std::cin.peek()) == 1) {
        std::vector<std::string> &ref = p.getLine();
        for (int i=0; i<ref.size(); i++) {
            std::cout << i << ": " << ref[i] << std::endl;
        }
        ref.clear();
        std::cout << std::endl;
    }
  }
  return 0;
}
*/
