[![Build Status](https://travis-ci.org/jbenden/call-rating.svg?style=flat&branch=master)](https://travis-ci.org/jbenden/call-rating)
[![Coverage Status](https://coveralls.io/repos/jbenden/call-rating/badge.png?branch=master)](https://coveralls.io/r/jbenden/call-rating?branch=master)
Call Rating
===========

This project implements a call rating engine for processing CSV
formatted files from Asterisk PBX. The engine supports a number of
features to help with complex billing scenarios.

Sample usage
------------

    $ ./src/matcher rates.tsv rates1.tsv < Master.csv > MasterRated.tsv

Rate tables
-----------

Rate tables are comprised of a specially formatted space-separated file.
The first line is global information about the table, while the next
lines are actual routing table entries.

The format of the first line is:

    [Start Hour] [End Hour] [Day of the week or any]

As an example, to cover the entire day for all days in a week,
specify:

    0 23 any

Following the table's header are the routes and their associated
pricing, which are the following fields all separated by a space.

* Destination Prefix (eg: 1 for all of the USA)
* Retail Amount (amount charged per billing interval)
* Vendor Amount (amount costed per billing interval)
* Billing Interval (interval to round calls to. eg: 6 for six second
  billing or 60 for minute interval billing.)
* Connect Charge (fixed amount to charge at start of call. Used for
  rating calls to 411, as an example.)

Asterisk CSV-CDR format
-----------------------

See [this URL](http://www.voip-info.org/wiki/view/Asterisk+cdr+csv) for information regarding the [Asterisk CSV-CDR](http://www.voip-info.org/wiki/view/Asterisk+cdr+csv) format.

Output Format
-------------

The output format is a tab-delimited file containing all of the
original Asterisk CSV-CDR fields, followed by the retail price of the
call. A future version may alter this output, so vendor price becomes
available.

Additional Notes
----------------

The rating engine implemented herein could be used as the rating
engine for [AgileVoice](http://www.agilebill.com/) billing software.
All that is required is appropriate database exporting and importing.
