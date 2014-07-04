#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <math.h>
#include <deque>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "csvparser.h"
#include "matcher.h"

using namespace std;

int
main(int argc, char *argv[])
{
  Dialplan *dp1 = NULL;
  long id = 0;

  for (int i = 1; i < argc; ++i) {
    ifstream rates(argv[i]);
    if (!rates) {
      fprintf(stderr, "Error reading rate file: %s\n", strerror(errno));
      return (-1);
    }
    Route *r1 = new Route;
    dp1 = new Dialplan(r1);
    dp1->next = dialplans;
    dialplans = dp1;
    dialplans_count++;

    string dow;
    rates >> dp1->startHour;
    rates >> dp1->endHour;
    rates >> dow;
    cout << "dow = " << dow << endl;
    try {
      dp1->dow = Dialplan::to_dow(dow);
    } catch (...) {
      cerr << "Error parsing dialplan." << endl;
      return (-1);
    }

    while (!rates.eof()) {
      string dest;
      rates >> dest;
      Route *r = dp1->find(dest.c_str());
      if (r) {
        rates >> r->retail_price;
        rates >> r->vendor_price;
        rates >> r->billing_interval;
        rates >> r->connect_charge;
        r->id = id++;
        r->dump();
      }
    }
    rates.close();
  }

  cout << "Loaded " << dialplans_count << " dialplans..." << endl;

  Parser p;
  std::ios_base::sync_with_stdio(false);

  while (!std::cin.eof()) {
    if (p.feed(std::cin.get(), std::cin.peek()) == 1) {
        std::vector<std::string> &ref = p.getLine();
        for (int i=0; i<ref.size(); i++) {
            std::cout << ref[i] << "\t";
        }
        double amount = 0.0;
        struct tm tm;
        if (ref[14] == "ANSWERED") {
          Call *c = new Call;
          c->source = ref[1];
          c->destination = ref[2];
          c->seconds = atol(ref[13].c_str());
          strptime(ref[10].c_str(), "%Y-%m-%d %H:%M:%S", &tm);
          c->startTime = mktime(&tm);
          c->endTime = c->startTime + c->seconds;
          amount = c->rate();

          //dump_call(*c);
          delete c;
        }
        cout << amount;
        ref.clear();
        std::cout << std::endl;
    }
  }



  delete dp1;
  dp1 = NULL;

  return (0);
}
