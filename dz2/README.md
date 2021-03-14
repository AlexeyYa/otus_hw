[![Build Status](https://travis-ci.com/AlexeyYa/otus_dz02.svg?branch=master)](https://travis-ci.com/AlexeyYa/otus_dz02)
[![Build status](https://ci.appveyor.com/api/projects/status/xyag5xc3m6i7m0if?svg=true)](https://ci.appveyor.com/project/AlexeyYa/otus-dz02)

Takes ip addresses list input, reverse sorts it and outputs:
1. Full list after sort.
2. List of addresses starting with first byte equal to '1'.
3. List of addresses starting with first byte equal to '46' and second byte equal to '70'.
4. List of addresses with any byte equal to '46'.

Input format for address: "text1 \t text2 \t text3 \n", where text1 is ipv4 address, text2 and text3 values are ignored.
Automatic deployement using travis-ci and appveyor to bintray after successfully passing tests.