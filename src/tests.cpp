#include <gtest/gtest.h>
#include <exception>

#include "matcher.h"

void
dump_call(Call &call)
{
  cout << "Call from +" << call.source << " to +" << call.destination << endl;
  cout << "Total Length of call: " << call.seconds << endl;
  cout << "Total cost of call: " << call.price << endl;
  cout << "Route Price      Seconds" << endl;
  for (Leg *l = call.head; l!=NULL; l = l->next) {
    cout << setw(5) << setfill('0') << l->route->id << setfill(' ') << " " << setw(10) << l->price << " " << setw(7) << l->seconds << endl;
  }
}

class CallDefaultTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
          Route *r1 = new Route, *r2 = new Route;
          r2->retail_price = 0.0035;
          r2->vendor_price = 0.00175;
          r2->billing_interval = 6;
          r2->connect_charge = 0;
          r1->destination[1] = r2;
          dp1 = new Dialplan(r1);
          dp1->startHour = 0;
          dp1->endHour = 23;
          dp1->dow = DOW_ALL;
          dialplans = dp1;
          dialplans_count = 1;
        }

        virtual void TearDown() {
            delete dp1;
        }

        Dialplan *dp1;
};

TEST_F(CallDefaultTest, AllDayOfWeekHandling) {

  EXPECT_EQ(Dialplan::to_dow("any"), DOW_ALL);

}

TEST_F(CallDefaultTest, MonDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("mon"), DOW_MONDAY);
}

TEST_F(CallDefaultTest, TueDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("tue"), DOW_TUESDAY);
}

TEST_F(CallDefaultTest, WedDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("wed"), DOW_WEDNESDAY);
}

TEST_F(CallDefaultTest, ThuDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("thu"), DOW_THURSDAY);
}

TEST_F(CallDefaultTest, FriDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("fri"), DOW_FRIDAY);
}

TEST_F(CallDefaultTest, SatDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("sat"), DOW_SATURDAY);
}

TEST_F(CallDefaultTest, SunDayOfWeekHandling) {
  EXPECT_EQ(Dialplan::to_dow("sun"), DOW_SUNDAY);
}

TEST_F(CallDefaultTest, BadDayOfWeekHandling) {
  EXPECT_ANY_THROW(Dialplan::to_dow("fsd"));
}

TEST_F(CallDefaultTest, 8DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(8), DOW_ALL);
}

TEST_F(CallDefaultTest, 1DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(1), DOW_MONDAY);
}

TEST_F(CallDefaultTest, 2DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(2), DOW_TUESDAY);
}

TEST_F(CallDefaultTest, 3DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(3), DOW_WEDNESDAY);
}

TEST_F(CallDefaultTest, 4DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(4), DOW_THURSDAY);
}

TEST_F(CallDefaultTest, 5DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(5), DOW_FRIDAY);
}

TEST_F(CallDefaultTest, 6DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(6), DOW_SATURDAY);
}

TEST_F(CallDefaultTest, 0DayOfWeek) {
  EXPECT_EQ(Dialplan::to_dow(0), DOW_SUNDAY);
}

TEST_F(CallDefaultTest, BadIntegerDayOfWeek) {
  EXPECT_ANY_THROW(Dialplan::to_dow(10));
}

TEST_F(CallDefaultTest, Default_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "12165551212";
  c->seconds = 5;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 5);
  EXPECT_EQ(amount, 0.0035);
  EXPECT_EQ(c->head->vendorPrice, 0.00175);
}

TEST_F(CallDefaultTest, No_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "911";
  c->seconds = 5;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  EXPECT_ANY_THROW(c->rate());
}

class CallSixtyTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
          Route *r1 = new Route, *r2 = new Route;
          r2->retail_price = 0.025;
          r2->vendor_price = 0.00175;
          r2->billing_interval = 60;
          r2->connect_charge = 0;
          r1->destination[1] = r2;
          dp1 = new Dialplan(r1);
          dp1->startHour = 0;
          dp1->endHour = 23;
          dp1->dow = DOW_ALL;
          dialplans = dp1;
          dialplans_count = 1;
        }

        virtual void TearDown() {
            delete dp1;
        }

        Dialplan *dp1;
};

TEST_F(CallSixtyTest, Default_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "12165551212";
  c->seconds = 5;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 5);
  EXPECT_EQ(amount, 0.025);
  EXPECT_EQ(c->head->vendorPrice, 0.00175);
}

class CallTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
          Route *r1 = new Route, *r2 = new Route, *r3 = new Route;
          r3->retail_price = 0.002;
          r3->vendor_price = 0;
          r3->billing_interval = 6;
          r3->connect_charge = 0;
          r2->destination[4] = r3;
          r2->retail_price = 0.0035;
          r2->vendor_price = 0.00175;
          r2->billing_interval = 6;
          r2->connect_charge = 0;
          r1->destination[1] = r2;
          dp1 = new Dialplan(r1);
          dp1->startHour = 0;
          dp1->endHour = 23;
          dp1->dow = DOW_ALL;
          dialplans = dp1;
          dialplans_count = 1;
        }

        virtual void TearDown() {
            delete dp1;
        }

        Dialplan *dp1;
};

TEST_F(CallTest, Default_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "12165551212";
  c->seconds = 5;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 5);
  EXPECT_EQ(amount, 0.0035);
  EXPECT_EQ(c->head->vendorPrice, 0.00175);
}

TEST_F(CallTest, Default_Specific_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "14165551212";
  c->seconds = 5;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 5);
  EXPECT_EQ(amount, 0.002);
}

TEST_F(CallTest, Default_Specific2_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "14165551212";
  c->seconds = 65;
  strptime("2014-07-04 08:19:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 65);
  EXPECT_EQ(amount, 0.022);
}

class CallDoubleTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
          Route *r1 = new Route, *r2 = new Route, *r3 = new Route;
          r3->retail_price = 0.002;
          r3->vendor_price = 0;
          r3->billing_interval = 6;
          r3->connect_charge = 0;
          r2->destination[4] = r3;
          r2->retail_price = 0.0035;
          r2->vendor_price = 0.00175;
          r2->billing_interval = 6;
          r2->connect_charge = 0;
          r1->destination[1] = r2;
          dp1 = new Dialplan(r1);
          dp1->startHour = 0;
          dp1->endHour = 11;
          dp1->dow = DOW_ALL;
          dialplans = dp1;
          dialplans_count = 1;

          Route *r4 = new Route, *r5 = new Route, *r6 = new Route;
          r6->retail_price = 0.0025;
          r6->vendor_price = 0;
          r6->billing_interval = 6;
          r6->connect_charge = 0;
          r5->destination[4] = r6;
          r5->retail_price = 0.0035;
          r5->vendor_price = 0.00175;
          r5->billing_interval = 6;
          r5->connect_charge = 0;
          r4->destination[1] = r5;
          dp2 = new Dialplan(r4);
          dp2->startHour = 12;
          dp2->endHour = 23;
          dp2->dow = DOW_ALL;
          dialplans->next = dp2;
          dialplans_count++;
        }

        virtual void TearDown() {
          delete dp1;
          delete dp2;
        }

  Dialplan *dp1, *dp2;
};

TEST_F(CallDoubleTest, Default_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "12165551212";
  c->seconds = 302;
  memset(&tm, 0, sizeof(tm));
  strptime("2014-07-04 11:58:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 238);
  ASSERT_TRUE(c->head->next != NULL);
  EXPECT_EQ(c->head->next->seconds, 64);
  //dump_call(*c);
  EXPECT_EQ(c->head->price, 0.14);
  EXPECT_EQ(c->head->vendorPrice, 0.07);
  EXPECT_EQ(c->head->next->price, 0.0385);
  EXPECT_EQ(c->head->next->vendorPrice, 0.01925);
  EXPECT_DOUBLE_EQ(0.1785, amount);
}

TEST_F(CallDoubleTest, Targeted_Matching_Double_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "14165551212";
  c->seconds = 302;
  memset(&tm, 0, sizeof(tm));
  strptime("2014-07-04 11:58:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 238);
  ASSERT_TRUE(c->head->next != NULL);
  EXPECT_EQ(c->head->next->seconds, 64);
  // dump_call(*c);
  EXPECT_EQ(c->head->price, 0.10);
  EXPECT_EQ(c->head->next->price, 0.022);
  EXPECT_DOUBLE_EQ(0.122, amount);
}

TEST_F(CallDoubleTest, Targeted_Matching_VeryLongCall_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "14165551212";
  memset(&tm, 0, sizeof(tm));
  strptime("2014-07-04 11:58:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  memset(&tm, 0, sizeof(tm));
  strptime("2014-07-05 01:05:00", "%Y-%m-%d %H:%M:%S", &tm);
  c->endTime = mktime(&tm);
  c->seconds = c->endTime - c->startTime;
  double amount = c->rate();
  EXPECT_EQ(c->head->seconds, 3901);
  EXPECT_EQ(c->head->next->seconds, 43200);
  EXPECT_EQ(c->head->next->next->seconds, 64);
  //dump_call(*c);
  EXPECT_EQ(c->head->price, 1.302);
  EXPECT_EQ(c->head->next->price, 18.0);
  EXPECT_EQ(c->head->next->next->price, 0.022);
  EXPECT_DOUBLE_EQ(19.324, amount);
}

class CallDoubleDayTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
          Route *r1 = new Route, *r2 = new Route, *r3 = new Route;
          r3->retail_price = 0.002;
          r3->vendor_price = 0;
          r3->billing_interval = 6;
          r3->connect_charge = 0;
          r2->destination[4] = r3;
          r2->retail_price = 0.0035;
          r2->vendor_price = 0.00175;
          r2->billing_interval = 6;
          r2->connect_charge = 0;
          r1->destination[1] = r2;
          dp1 = new Dialplan(r1);
          dp1->startHour = 0;
          dp1->endHour = 11;
          dp1->dow = DOW_MONDAY;
          dialplans = dp1;
          dialplans_count = 1;

          Route *r4 = new Route, *r5 = new Route, *r6 = new Route;
          r6->retail_price = 0.0025;
          r6->vendor_price = 0;
          r6->billing_interval = 6;
          r6->connect_charge = 0;
          r5->destination[4] = r6;
          r5->retail_price = 0.0035;
          r5->vendor_price = 0.00175;
          r5->billing_interval = 6;
          r5->connect_charge = 0;
          r4->destination[1] = r5;
          dp2 = new Dialplan(r4);
          dp2->startHour = 12;
          dp2->endHour = 23;
          dp2->dow = DOW_TUESDAY;
          dialplans->next = dp2;
          dialplans_count++;
        }

        virtual void TearDown() {
          delete dp1;
          delete dp2;
        }

  Dialplan *dp1, *dp2;
};

TEST_F(CallDoubleDayTest, Default_Matching_Route) {
  struct tm tm;
  Call *c = new Call;
  c->source = "1";
  c->destination = "12165551212";
  c->seconds = 302;
  memset(&tm, 0, sizeof(tm));
  strptime("2014-07-04 11:58:55", "%Y-%m-%d %H:%M:%S", &tm);
  c->startTime = mktime(&tm);
  c->endTime = c->startTime + c->seconds;
  EXPECT_ANY_THROW(c->rate());
}
