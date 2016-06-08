//
// Author: Rodney DeSot
// Created:May 2015
// Description:  This is a C++ class that calculates the median value in near
//             realtime.  As a new data point is inserted, is replaces the oldest
//             one within the sample window.  This first incarnation uses the
//             simple data type of double.  Future versions will be impleamented
//             as a container.
//

#ifndef MEDIANFILTER_H_
#define MEDIANFILTER_H_

class MedianFilter {
  private:
    typedef struct Node { //Double link list node
      Node *pPrevious;    //Pointer to previous node in the list. NULL if it is the first node in list
      Node *pNext;        //Pointer to next node in the list.  NULL if it is the last node in list
      double data;        //Data value stored in node
    } Node;

    int sampleSize;   //Number of samples used for median noise filter
    Node **queue;     //Circular array containing pointers to all the samles indexed chronologically
    Node *pFirst;     //Pointer to the smallest sample value
    Node *pLast;      //Pointer to the largest sample value
    Node *pMedian;    //Pointer to the median sample value

    int eldest;       //Index to the eldest sample in the queue
    long sampleCount;  //Number of samples passed through filter

    unsigned int setMedianFlags(const double dataValue, const Node *nodePtr);

  public:
    MedianFilter(int SampleSize);
    ~MedianFilter(void);
    int insert(double dataValue);
    double getMedian(void);
    void print(void);
    void print2(void);
};

#endif
