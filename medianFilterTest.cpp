//
// Author: Rodney DeSot
// Created:May 2015
// Description:  This is a C++ program used to test the medianFilter class.  The
//             lone input is for the data sample window size.  Then a really large
//             for loop inserts random data then prints the vitals to ensure proper
//             operation.
//

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

#include "medianFilter.h"
using namespace std;

int main (int argv, char **argc) {

  MedianFilter *mf;
  int size;
  double value;

  //initialize random number generator
  srand (time(NULL));

  cout << "What is your sample size?";
  cin >> size;

  mf = new MedianFilter(size);

  mf->print();

  for (int i = 0; i < 1000000000000; i++) {
    value = rand() % 100;
    cout << endl << endl << i << "::Inserting: " << value;
    mf->insert(value);
    mf->print();
  }

  mf->print();
  delete mf;
}
