#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "tutorial.h"

using namespace std;

int main()
{
#ifdef USE_VERSION
  cout << "Hello World! Version " << HelloWorld_VERSION_MAJOR << "."
                                  << HelloWorld_VERSION_MINOR << "."
                                  << HelloWorld_VERSION_PATCH << endl;
#else
  cout << "Hello World!" << endl;
#endif
  return 0;
}
