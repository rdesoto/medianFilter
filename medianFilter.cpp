//
// Author: Rodney DeSot
// Created:May 2015
// Description:  This is a C++ class that calculates the median value in near
//             realtime.  As a new data point is inserted, is replaces the oldest
//             one within the sample window.  This first incarnation uses the
//             simple data type of double.  Future versions will be impleamented
//             as a container.
//

#include "medianFilter.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>

using namespace std;

/*
  Bitmask representatino of the nine conditions that determine the movement of the
  median value within the list.  Each bit repersents a condition with 0 being false
  and 1 being true.  The bit representaion from least significant bit to most significant
  bit are as follows:

    --Eldest node is median node
    --Eldest data value is equal to median data value
    --Eldest data value is greater than median data value
    --New data value is equal to eldest data value
    --New data value is greater than eldest data value
    --New data value is equal to median data value
    --New data value is greater than median data value
    --Eldest data value is stored at the node where new data value is to be inserted into the order list
    --New data value is greater than median->next data value
*/

#define INSERT_NODE_EQ_ELDEST        1    // 2^0 bit 1
#define NEWDATA_GT_MEDIAN_DATA       2    // 2^1 bit 2
#define NEWDATA_EQ_MEDIAN_DATA       4    // 2^2 bit 3
#define NEWDATA_GT_ELDEST_DATA       8    // 2^3 bit 4
#define NEWDATA_EQ_ELDEST_DATA       16   // 2^4 bit 5
#define ELDESTDATA_GT_MEDIAN_DATA    32   // 2^5 bit 6
#define ELDESTDATA_EQ_MEDIAN_DATA    64   // 2^6 bit 7
#define ELDEST_IS_MEDIAN             128  // 2^7 bit 8
#define NEWDATA_GE_MEDIAN_NEXT_DATA  256  // 2^8 bit 9

MedianFilter::MedianFilter(int SampleSize)
{
  if(SampleSize < 1) {
    sampleSize = 0;
    eldest = -1;
    queue = NULL;
    pFirst = pLast = pMedian = NULL;
    return;
  }

  sampleSize = SampleSize;
  sampleCount = 0;
  eldest = 0;
  queue = new Node *[sampleSize];

  for(int i = 0; i < sampleSize; i++) {
    queue[i] = new Node();
    queue[i]->pPrevious = NULL;
    queue[i]->pNext = NULL;

    if(i > 0) {
      queue[i]->pPrevious = queue[i - 1];
      queue[i - 1]->pNext = queue[i];
    }

    queue[i]->data = 0.0;
  }

  pFirst = queue[0];
  pLast = queue[sampleSize - 1];

  if(sampleSize % 2 == 0) {
    pMedian = queue[(int)(sampleSize / 2) - 1];

  } else {
    pMedian = queue[(int)(sampleSize / 2)];
  }
}

MedianFilter::~MedianFilter()
{
  for(int i = 0; i < sampleSize; i++) {
    if(queue[i]) {
      delete queue[i];
      queue[i] = NULL;
    }
  }

  if(queue) {
    delete [] queue;
    queue = NULL;
  }
}

unsigned int MedianFilter::setMedianFlags(const double dataValue, const Node *nodePtr)
{
  unsigned int flags = 0;

  // 2^8 bit 9
  if(pMedian->pNext != NULL && dataValue >= pMedian->pNext->data) {
    flags |= NEWDATA_GE_MEDIAN_NEXT_DATA;
  }

  // 2^7 bit 8
  if(queue[eldest] == pMedian) {
    flags |= ELDEST_IS_MEDIAN;
  }

  // 2^6 bit 7
  if(queue[eldest]->data == pMedian->data) {
    flags |= ELDESTDATA_EQ_MEDIAN_DATA;
  }

  // 2^5 bit 6
  if(queue[eldest]->data > pMedian->data) {
    flags |= ELDESTDATA_GT_MEDIAN_DATA;
  }

  // 2^4 bit 5
  if(dataValue == queue[eldest]->data) {
    flags |= NEWDATA_EQ_ELDEST_DATA;
  }

  // 2^3 bit 4
  if(dataValue > queue[eldest]->data) {
    flags |= NEWDATA_GT_ELDEST_DATA;
  }

  // 2^2 bit 3
  if(dataValue == pMedian->data) {
    flags |= NEWDATA_EQ_MEDIAN_DATA;
  }

  // 2^1 bit 2
  if(dataValue > pMedian->data) {
    flags |= NEWDATA_GT_MEDIAN_DATA;
  }

  // 2^0 bit 1
  if(nodePtr == queue[eldest]) {
    flags |= INSERT_NODE_EQ_ELDEST;
  }

  return flags;
}

int MedianFilter::insert(double dataValue)
{
  //Check for median error
  if(!pMedian) {
    return 1;
  }

  if(eldest == sampleSize) {
    eldest = 0;
  }

  //Node pointer used to point to increment through list
  Node *nodePtr = pFirst;
  // Only 9 flags/bits used, so a int is fine
  unsigned int flags = 0;
  sampleCount++;

  //Get starting point
  if(dataValue >= pMedian->data) {
    nodePtr = pMedian;
  }

  //Find insertion point in list
  while(nodePtr && dataValue >= nodePtr->data) {
    nodePtr = nodePtr->pNext;
  }

  //Set flags for determining median movement
  flags = setMedianFlags(dataValue, nodePtr);
  //Storing values for use later (if needed)
  Node *preInsNext = pMedian->pNext;
  Node *preInsPrev = pMedian->pPrevious;

  //Insert data into the list

  //Generic form of the if ( queue[eldest] == pFirst && dataValue <= queue[eldest]->data) condition
  if(queue[eldest] == nodePtr) {
    queue[eldest++]->data = dataValue;
    return 0;
  }

  if(queue[eldest]->pNext == NULL) {
    pLast = queue[eldest]->pPrevious;

  } else {
    queue[eldest]->pNext->pPrevious = queue[eldest]->pPrevious;

    if(queue[eldest]->pPrevious == NULL) {
      pFirst = queue[eldest]->pNext;
    }
  }

  if(queue[eldest]->pPrevious != NULL) {
    queue[eldest]->pPrevious->pNext = queue[eldest]->pNext;
  }

  if(nodePtr == NULL) {
    queue[eldest]->pPrevious = pLast;
    pLast = queue[eldest];

  } else {
    queue[eldest]->pPrevious = nodePtr->pPrevious;
    nodePtr->pPrevious = queue[eldest];
  }

  queue[eldest]->pNext = nodePtr;

  if(queue[eldest]->pPrevious != NULL) {
    queue[eldest]->pPrevious->pNext = queue[eldest];

  } else {
    pFirst = queue[eldest];
  }

  queue[eldest++]->data = dataValue;
  #ifdef DEBUG
  cerr << " flags = " << std::dec << (unsigned int)flags;
  #endif

  //Update median pointer
  // testing for a specific bitmasks
  if(flags == (NEWDATA_GT_ELDEST_DATA | NEWDATA_GT_MEDIAN_DATA | NEWDATA_GE_MEDIAN_NEXT_DATA)) {   //flags == 266
    pMedian = pMedian->pNext;

  } else if(flags == (ELDESTDATA_GT_MEDIAN_DATA)) {   //flags == 32
    pMedian = pMedian->pPrevious;

  } else if(flags == (ELDEST_IS_MEDIAN | ELDESTDATA_EQ_MEDIAN_DATA)) {   //flags == 192
    pMedian = preInsPrev;

  } else if(flags == (ELDEST_IS_MEDIAN | ELDESTDATA_EQ_MEDIAN_DATA | NEWDATA_GT_ELDEST_DATA | NEWDATA_GT_MEDIAN_DATA | NEWDATA_GE_MEDIAN_NEXT_DATA)) {  // flags == 458
    pMedian = preInsNext;

  } else if(flags == (NEWDATA_GT_ELDEST_DATA | NEWDATA_GT_MEDIAN_DATA)) {   //flags == 10
    pMedian = pMedian->pNext;

  } else if(flags == (NEWDATA_GT_ELDEST_DATA | NEWDATA_EQ_MEDIAN_DATA)) {  //flags == 12
    pMedian = pMedian->pNext;

  } else if(flags == (NEWDATA_GE_MEDIAN_NEXT_DATA | NEWDATA_GT_ELDEST_DATA | NEWDATA_GT_MEDIAN_DATA | ELDESTDATA_EQ_MEDIAN_DATA)) {  //flags == 330
    pMedian = pMedian->pNext;

  } else if(flags == (NEWDATA_GT_ELDEST_DATA | NEWDATA_GT_MEDIAN_DATA | ELDESTDATA_EQ_MEDIAN_DATA)) {  //flags == 74
    pMedian = pMedian->pNext;

  } else if(flags == (ELDESTDATA_EQ_MEDIAN_DATA | NEWDATA_EQ_ELDEST_DATA | NEWDATA_EQ_MEDIAN_DATA)) {   //flags == 84
    pMedian = pMedian->pNext;

  } else if(flags == (NEWDATA_GE_MEDIAN_NEXT_DATA | NEWDATA_GT_ELDEST_DATA | NEWDATA_EQ_MEDIAN_DATA)) {  //flags == 268
    pMedian = pMedian->pNext;

  } else if(flags == (NEWDATA_GE_MEDIAN_NEXT_DATA | ELDEST_IS_MEDIAN | ELDESTDATA_EQ_MEDIAN_DATA | NEWDATA_EQ_ELDEST_DATA | NEWDATA_EQ_MEDIAN_DATA)) {  //flags=468
    pMedian = preInsNext;

  } else if(flags == (NEWDATA_GE_MEDIAN_NEXT_DATA | ELDESTDATA_EQ_MEDIAN_DATA | NEWDATA_EQ_ELDEST_DATA | NEWDATA_EQ_MEDIAN_DATA)) {   //flags == 340
    pMedian = pMedian->pNext;
  }

  return 0;
}

double MedianFilter::getMedian(void)
{
  if(pMedian) {
    //Since % is used to calculate inital median, odd sample size will be one off
    //the median value location in the sort list.  Ex 5 % 2 = 2 when the median would
    //be stored in location 3.  But for even sample size the median is correct.
    if(sampleSize % 2 == 0) {
      return ((pMedian->data + pMedian->pNext->data) / 2);

    } else {
      return (pMedian->data);
    }
  }

  return 0.0;
}

void MedianFilter::print(void)
{
  cerr << endl;

  for(int i = 0; i < sampleSize; i++) {
    if(queue[i] == pMedian) {
      cerr << "*Node=";

    } else {
      cerr << " Node=";
    }

    cerr << std::hex << queue[i] << " pP=";

    if(queue[i]->pPrevious == NULL) {
      cerr << std::setw(9) << "(NULL)";

    } else {
      cerr << std::hex << queue[i]->pPrevious;
    }

    cerr << " pN=";

    if(queue[i]->pNext == NULL) {
      cerr << std::setw(9) << "(NULL)";

    } else {
      cerr << std::hex << queue[i]->pNext;
    }

    cerr << " data=" << std::setprecision(8) << queue[i]->data << endl;
    //fprintf(stderr, "Node=%p pP=%p pN=%p data=%8.5f\n", queue[i], queue[i]->pPrevious, queue[i]->pNext, queue[i]->data);
  }

  Node *ptr = pFirst;

  while(ptr) {
    if(ptr == pMedian) {
      cerr << "  *";

    } else {
      cerr << "   ";
    }

    cerr << std::hex << ptr << " data=" << std::setw(10) << std::setprecision(8) << ptr->data;
    ptr = ptr->pNext;
  }

  cerr << endl;
  ptr = pLast;

  while(ptr) {
    if(ptr == pMedian) {
      cerr << "  *";

    } else {
      cerr << "   ";
    }

    cerr << std::hex << ptr << " data=" << std::setw(10) << std::setprecision(8) << ptr->data;
    ptr = ptr->pPrevious;
  }

  cerr << endl;
}

void MedianFilter::print2(void)
{
  int np=0, nq=0, badMedian=0;
  Node *p, *q;
  double pData, qData;

  cerr << endl;

  p = pFirst;

  while (p) {
    if (np > 0) {
      if (pData > p->data) {
        cerr << "Data in forward chain out of order!" << endl;
        exit(3);
      }
    }
    pData = p->data;

    if (p == queue[eldest]) {
      cerr << "E";
    } else {
      cerr << " ";
    }

    if (p == pMedian) {
      cerr << "*";
    } else {
      cerr << " ";
    }
    cerr << "Node=" << std::hex << p << " pP=";

    if(p->pPrevious == NULL) {
      cerr << std::setw(9) << "(NULL)";
    } else {
      cerr << std::hex << p->pPrevious;
    }

    cerr << " pN=";

    if(p->pNext == NULL) {
      cerr << std::setw(9) << "(NULL)";
    } else {
      cerr << std::hex << p->pNext;
    }

    cerr << " data=" << std::setprecision(8) << p->data << endl;

    if (p == pMedian) {
      if ((sampleSize % 2) == 0) {
        if (np != ((sampleSize / 2) - 1)) ++badMedian;
      } else {
        if (np != (sampleSize / 2)) ++badMedian;
      }
    }

    p = p->pNext;
    ++np;
  }

  if (badMedian) {
    cerr << "Median not pointing to correct location!" << endl;
    exit(4);
  }

  q = pLast;

  while (q) {
    if (nq > 0) {
      if (qData < q->data) {
        cerr << "Data in backward chain out of order!" << endl;
        exit(2);
      }
    }
    qData = q->data;

    q = q->pPrevious;
    ++nq;
  }

  if (np != nq) {
    cerr << "Forward and backward walks have different lengths!" << endl;
    cerr << "Forward length  = " << std::dec << np << endl;
    cerr << "Backward length = " << std::dec << nq << endl;
    exit(1);
  }
}
