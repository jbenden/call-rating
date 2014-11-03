#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "matcher.h"

using namespace Poco::Util;
using namespace Poco::Net;
using namespace std;


class MyRequestHandler : public HTTPRequestHandler
{
public:
  virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
  {
    resp.setStatus(HTTPResponse::HTTP_OK);

    if (req.getMethod() == "GET") {
      if (req.getURI().compare(0, req.getURI().size(), "/rate", 0, 4)) {
        //cout << "Rating request received." << endl;
        string source, destination;
        string timeStart, length;
        time_t timet;
        HTMLForm form(req);
        NameValueCollection::ConstIterator i = form.begin();
        while (i != form.end()) {
          //cout << i->first << ": " << i->second << endl;
          if (i->first == "source")
            source = i->second;
          if (i->first == "destination")
            destination = i->second;
          if (i->first == "startTime")
            timeStart = i->second;
          if (i->first == "length")
            length = i->second;
          i++;
        }
        timet = atol(timeStart.c_str());
        if (timet == 0) timet = time(NULL);

        Call *c = new Call;
        c->source = source.c_str();
        c->destination = destination.c_str();
        c->seconds = atol(length.c_str());
        c->startTime = timet;
        c->endTime = c->startTime + c->seconds;
        double amount = c->rate();

        if (req.hasToken("Accept", "application/json")) {
          resp.setContentType("application/json");
          ostream& out = resp.send();
          out << "{" << endl;
          out << "\t\"source\": \"" << source << "\"," << endl;
          out << "\t\"destination\": \"" << destination << "\"," << endl;
          out << "\t\"amount\": " << amount << "," << endl;
          out << "\t\"routes\": [" << endl;
          for (Leg *l = c->head; l!=NULL; l = l->next) {
            if (l != c->head) out << ",";
            out << "[\"" << l->route->id << "\",\"" << l->price << "\",\"" << l->seconds << "\"]";
            out << endl;
          }
          out << "\t]" << endl;
          out << "}";
          out.flush();
        } else {
          resp.setContentType("text/xml");
          ostream& out = resp.send();
          out << "<call>" << endl;
          out << "\t<source>" << source << "</source>" << endl;
          out << "\t<destination>" << destination << "</destination>" << endl;
          out << "\t<amount>" << amount << "</amount>" << endl;
          out << "\t<routes>" << endl;
          for (Leg *l = c->head; l!=NULL; l = l->next) {
            out << "\t\t<route id=\"" << l->route->id << "\" price=\"" << l->price << "\" seconds=\"" << l->seconds << "\" />" << endl;
          }
          out << "\t</routes>" << endl;
          out << "</call>";
          out.flush();
        }
        delete c;
      }
    }
  }

private:
  static int count;
};

int MyRequestHandler::count = 0;

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
  virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &)
  {
    return new MyRequestHandler;
  }
};

class MyServerApp : public ServerApplication
{
protected:
  int main(const vector<string> &)
  {
    HTTPServer s(new MyRequestHandlerFactory, ServerSocket(9090), new HTTPServerParams);

    s.start();
    cout << endl << "Server started" << endl;

    waitForTerminationRequest();  // wait for CTRL-C or kill

    cout << endl << "Shutting down..." << endl;
    s.stop();

    return Application::EXIT_OK;
  }
};

int main(int argc, char** argv)
{
  MyServerApp app;
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
      }
    }
    rates.close();
  }

  cout << "Loaded " << dialplans_count << " dialplans..." << endl;

  return app.run(argc, argv);
}
