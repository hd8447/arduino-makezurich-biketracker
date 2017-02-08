#ifndef MyMap_h
#define MyMap_h

#include <Arduino.h>

class MyMap {
  public:
    MyMap();
    uint8_t value(double x, double y);
    void load();

  private:
    double xFrom, xTo, yFrom, yTo, xField, yField;
    double xSize, ySize;
    String data;

};


#endif
