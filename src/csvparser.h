#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <string>

#ifndef CSVPARSER_H
#define CSVPARSER_H

class Parser
{
public:
  Parser() : state(0), start(0) {}
  //~Parser(void);

  int feed(char ch, char nch);

  std::vector<std::string> &getLine(void) { return line; }

protected:

private:
  int state;
  int start;
  std::stringstream data;
  std::vector<std::string> line;
};

#endif
