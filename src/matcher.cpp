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

using namespace std;

class Dialplan;
static Dialplan *dialplans;
int dialplans_count = 0;

class Route
{
public:
  Route *destination[10];
  double retail_price;
  double vendor_price;
  double billing_interval;
  double connect_charge;
  long id;

  void dump(void) {
    cout << "Retail Price:     " << retail_price << endl;
    cout << "Vendor Price:     " << vendor_price << endl;
    cout << "Billing Interval: " << billing_interval << endl;
    cout << "Connect Charge:   " << connect_charge << endl;
  }

  double price(double minutes) {
    return (ceil(minutes / billing_interval) * billing_interval * retail_price + connect_charge);
  }
};

class Dialplan
{
public:

  Dialplan(Route *h) : head(h) {}
  ~Dialplan() {
    Dialplan::free_routes(head);
    Dialplan *d = next;
    while (d) {
      d = d->next;
      delete d;
      d = NULL;
    }
  }
  static void free_routes(Route *head) {
    Route *r = head;
    while (r) {
      for (int i=0; i<10; i++) {
        //printf("Route(%d)\n", i);
        r = head->destination[i];
        if (r) {
          Dialplan::free_routes(r);
          //printf("Free Route %p\n", r);
          delete r;
          r = NULL;
        }
      }
    }
  }
  Dialplan *next;
  Route *head;

  int startHour;
  int endHour;
#define DOW_MONDAY   1
#define DOW_TUESDAY  2
#define DOW_WEDNESDAY 4
#define DOW_THURSDAY 8
#define DOW_FRIDAY 16
#define DOW_SATURDAY 32
#define DOW_SUNDAY 64
#define DOW_ALL (DOW_MONDAY|DOW_TUESDAY|DOW_WEDNESDAY|DOW_THURSDAY|DOW_FRIDAY|DOW_SATURDAY|DOW_SUNDAY)
  int dow;

  void dump(void) {
    cout << "Dialplan startHour=" << startHour << " endHour=" << endHour;
    cout << " dow=" << dow << endl;
  }

  static int to_dow(const string &input) {
    if (input == "any")
      return DOW_ALL;
    else if (input == "mon")
      return DOW_MONDAY;
    else if (input == "tue")
      return DOW_TUESDAY;
    else if (input == "wed")
      return DOW_WEDNESDAY;
    else if (input == "thu")
      return DOW_THURSDAY;
    else if (input == "fri")
      return DOW_FRIDAY;
    else if (input == "sat")
      return DOW_SATURDAY;
    else if (input == "sun")
      return DOW_SUNDAY;
    else
      throw string("Error parsing day of week");
  }

  static int to_dow(const int input) {
    if (input == 8)
      return DOW_ALL;
    else if (input == 1)
      return DOW_MONDAY;
    else if (input == 2)
      return DOW_TUESDAY;
    else if (input == 3)
      return DOW_WEDNESDAY;
    else if (input == 4)
      return DOW_THURSDAY;
    else if (input == 5)
      return DOW_FRIDAY;
    else if (input == 6)
      return DOW_SATURDAY;
    else if (input == 0)
      return DOW_SUNDAY;
    else
      throw string("Error parsing integer day of week");
  }

  static Dialplan *match(int start, int dow) {
    Dialplan *d = dialplans;
    for (int i=0; i<dialplans_count; i++) {
      //cout << "match(" << start << ", " << dow << "): startHour=" << d->startHour << " endHour=" << d->endHour << " dow=" << d->dow << endl;
      if (start >= d->startHour
          && dow & d->dow)
        return d;
      d = d->next;
    }
    return (NULL);
  }

  Route *match(const char *e163) {
    //cout << "match(" << e163 << ")" << endl;
    Route *matchingRoute = head;
    for (int i=0; i<strlen(e163); i++) {
      char buf[2];
      buf[0] = e163[i];
      buf[1] = 0;
      int digit = atoi(buf);
      // cout << digit << endl;
      assert(matchingRoute);
      if (matchingRoute->destination[digit])
        matchingRoute = matchingRoute->destination[digit];
      else
        break;
    }
    return matchingRoute;
  }

  Route *find(const char *e163) {
    //cout << "find(" << e163 << ")" << endl;
    if (strlen(e163) == 0) return NULL;
    Route *matchingRoute = head;
    for (int i=0; i<strlen(e163); i++) {
      char buf[2];
      buf[0] = e163[i];
      buf[1] = 0;
      int digit = atoi(buf);
      if (matchingRoute->destination[digit])
        matchingRoute = matchingRoute->destination[digit];
      else
        {
          Route *r = new Route;
          matchingRoute->destination[digit] = r;
          r->billing_interval = matchingRoute->billing_interval;
          r->retail_price = matchingRoute->retail_price;
          r->vendor_price = matchingRoute->vendor_price;
          r->connect_charge = matchingRoute->connect_charge;
          matchingRoute = matchingRoute->destination[digit];
        }
    }
    return (matchingRoute);
  }
};

class Leg
{
public:
  Leg *next;
  Dialplan *dialplan;
  Route *route;
  double price;
  double seconds;
};

class Call
{
public:
  Call() : head(NULL), price(0.0), seconds(0.0), source(""), destination("") {}
  ~Call() {
    Leg *l = head;
    while (l) {
      //printf("Free Leg %p\n", l);
      register Leg *j = l;
      l = l->next;
      if (j) delete j;
    }
  }

  double rate(void) {
    Dialplan *dp = NULL;
    Route *m;
    double lenOfCall = this->seconds;
    time_t timeOfCall = this->startTime;
    time_t ttlCall = this->endTime;
    double total = 0.0;

    while (lenOfCall > 0.0) {
      struct tm * timeinfo = localtime(&timeOfCall);
      long startHour = timeinfo->tm_hour;

      dp = Dialplan::match(startHour, Dialplan::to_dow(timeinfo->tm_wday));
      if (!dp) {
        printf("Cannot find a matching dialplan.\n");
        throw string("Cannot rate call. No matching dialplan.");
      }

      struct tm tinfo;
      tinfo.tm_year = timeinfo->tm_year;
      tinfo.tm_mon = timeinfo->tm_mon;
      tinfo.tm_mday = timeinfo->tm_mday;
      tinfo.tm_hour = dp->endHour;
      tinfo.tm_min = 59;
      tinfo.tm_sec = 59;

      time_t endingInterval = mktime(&tinfo);
      if (endingInterval > ttlCall) {
        endingInterval = ttlCall;
      }
      time_t rem = endingInterval + 1 - timeOfCall;
      time_t billingSeconds = rem;
      if (billingSeconds < 0) billingSeconds = 0;
      lenOfCall -= rem;

      m = dp->match(this->destination.c_str());
      //m->dump();
      total += m->price(billingSeconds);
      if (rem < 1) break;

      Leg *l = new Leg();
      l->price = m->price(billingSeconds);
      l->seconds = billingSeconds;
      l->route = m;
      l->dialplan = dp;
      l->next = this->head;
      this->head = l;

      if (billingSeconds > 0) {
        struct tm tinfo;
        tinfo.tm_year = timeinfo->tm_year;
        tinfo.tm_mon = timeinfo->tm_mon;
        tinfo.tm_mday = timeinfo->tm_mday;
        tinfo.tm_hour = dp->endHour + 1;
        tinfo.tm_min = 0;
        tinfo.tm_sec = 0;
        time_t toc = mktime(&tinfo);
        timeOfCall = toc;
      }
    }
    this->price = total;
    return (this->price);
  }
  Leg *head;
  double price;
  double seconds;
  string source, destination;
  time_t startTime, endTime;
};

void
dump_call(Call &call)
{
  cout << "Call from +" << call.source << " to +" << call.destination << endl;
  cout << "Total Length of call: " << call.seconds << endl;
  cout << "Total cost of call: " << call.price << endl;
  for (Leg *l = call.head; l!=NULL; l = l->next) {
    cout << "Leg,Price,Seconds" << endl;
    cout << l->route->id << "," << l->price << "," << l->seconds << endl;
  }
}

void
dump_tm(struct tm *tm)
{
    cout << tm->tm_year + 1900 << "-" << tm->tm_mon << "-" << tm->tm_mday;
    cout << " ";
    cout << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << endl;
}

int
main(int argc, char *argv[])
{
  /*
  Route r1, r2, r3;
  r3.retail_price = 0.002;
  r3.vendor_price = 0;
  r3.billing_interval = 6;
  r3.connect_charge = 0;

  r2.destination[4] = &r3;
  r2.retail_price = 0.0035;
  r2.vendor_price = 0.00175;
  r2.billing_interval = 6;
  r2.connect_charge = 0;
  r1.destination[1] = &r2;
  Dialplan dp1(&r1);
  dp1.startHour = 0;
  dp1.endHour = 23;
  dp1.dow = DOW_ALL;
  dialplans = &dp1;
  dialplans_count = 1;
  */
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
