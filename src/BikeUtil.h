#ifndef BikeUtil_h
#define BikeUtil_h

static uint8_t int8(uint8_t val, bool valid) {
  return valid ? val : 0;
}

static uint16_t int16(uint16_t val, bool valid) {
  return valid ? val : 0;
}


static int32_t int32(int32_t val, bool valid) {
  if (valid) {
    return  (val < magicGpsValue  && val > -magicGpsValue) ? 0 : val;
  }
  return 0;
}


#endif
