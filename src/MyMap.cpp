#include "MyMap.h"


MyMap::MyMap() {

}

// Map: “001100011110112211112210011110011100”
// left-down: 47.3227 / 8.4541
// upper-right: 47.4400 / 8.6312

void MyMap::load() {
  // TODO: load from SPIFF
  xFrom = 47.3227000; // LATITUDE
  xTo =   47.4400000;
  yFrom = 8.454100;  // LONGITUDE
  yTo =   8.631200;
  xSize = 6;
  ySize = 6;
  xField = (xTo - xFrom) / xSize/2;
  yField = (yTo - yFrom) / ySize/2;
  String data = "001100011110112211112210011110011100";
}


uint8_t MyMap::value(double x, double y) {
  double xxx =  ( x - xFrom + xField) / (xTo - xFrom) * xSize; // halbes Feld wegen den Runden
  int8_t xx=(int8_t) xxx;
  if (xx < 0 || xx >= xSize) return 0;

  double yyy =  ( y - yFrom + yField) / (yTo - yFrom) * ySize;
  int8_t yy=(int8_t) yyy;
  if (yy < 0 || yy >= ySize) return 0;

  int8_t index = yy * xSize + xx;
  if (index < 0 || index >= xSize*ySize) return 0;
  
  uint8_t dd = data[index]-'0';
  return dd;
}

