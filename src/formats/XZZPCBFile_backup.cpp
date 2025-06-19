#include "XZZPCBFile.h"
#include "des.h"
#include "Utils.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <memory>

// Hex conversion table
static unsigned char hexconv[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        ParsePostV6(v6v6555v6v6_found, buf);
    } else {
        if (buf[0x10] != 0) {
            uint8_t xor_key = buf[0x10];
            for (int i = 0; i < buf.end() - buf.begin(); ++i) {
                buf[i] ^= xor_key; // XOR the buffer with xor_key until the end of the buffer as no v6v6555v6v6
            }
        }
    }

    uint32_t main_data_offset = *reinterpret_cast<uint32_t*>(&buf[0x20]);
    uint32_t net_data_offset = *reinterpret_cast<uint32_t*>(&buf[0x28]);

    uint32_t main_data_start = main_data_offset + 0x20;
    uint32_t net_data_start = net_data_offset + 0x20;

    uint32_t main_data_blocks_size = *reinterpret_cast<uint32_t*>(&buf[main_data_start]);
    uint32_t net_block_size = *reinterpret_cast<uint32_t*>(&buf[net_data_start]);

    std::vector<char> net_block_buf(buf.begin() + net_data_start + 4, buf.begin() + net_data_start + net_block_size + 4);
    ParseNetBlockOriginal(net_block_buf);

    uint32_t current_pointer = main_data_start + 4;
    while (current_pointer < main_data_start + 4 + main_data_blocks_size) {
        uint8_t block_type = buf[current_pointer];
        current_pointer += 1;
        uint32_t block_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
        current_pointer += 4;
        std::vector<char> block_buf(buf.begin() + current_pointer, buf.begin() + current_pointer + block_size);
        ProcessBlockOriginal(block_type, block_buf);
        current_pointer += block_size;
    }
    
    FindXYTranslation();
    TranslateSegments();
    TranslatePins();

    valid = true;
    num_parts = parts.size();
    num_pins = pins.size();
    num_format = format.size();
    num_nails = nails.size();

    return true;
}

void XZZPCBFile::ProcessBlockOriginal(uint8_t block_type, std::vector<char>& block_buf) {
    switch (block_type) {
        case 0x01: { // ARC
            std::vector<uint32_t> arc_data((uint32_t*)(block_buf.data()), (uint32_t*)(block_buf.data() + block_buf.size()));
            ParseArcBlockOriginal(arc_data);
            break;
        }
        case 0x02: { // VIA
            // Not currently relevant
            break;
        }
        case 0x05: { // LINE SEGMENT
            std::vector<uint32_t> line_segment_data((uint32_t*)(block_buf.data()),
                                                   (uint32_t*)(block_buf.data() + block_buf.size()));
            ParseLineSegmentBlockOriginal(line_segment_data);
            break;
        }
        case 0x06: { // TEXT
            // Not currently relevant
            break;
        }
        case 0x07: { // PART/PIN
            std::vector<char> part_data(block_buf.begin(), block_buf.end());
            ParsePartBlockOriginal(part_data);
            break;
        }
        case 0x09: { // TEST PADS/DRILL HOLES
            std::vector<uint8_t> test_pad_data((uint8_t*)(block_buf.data()), (uint8_t*)(block_buf.data() + block_buf.size()));
            ParseTestPadBlockOriginal(test_pad_data);
            break;
        }
        default: 
            break;
    }
}

void XZZPCBFile::ParseNetBlockOriginal(std::vector<char>& buf) {
    net_dict.clear();
    uint32_t pos = 0;
    uint32_t net_index = 0;
    
    while (pos < buf.size()) {
        if (pos + 4 >= buf.size()) break;
        
        uint32_t name_length = *reinterpret_cast<uint32_t*>(&buf[pos]);
        pos += 4;
        
        if (pos + name_length > buf.size()) break;
        
        std::string net_name(buf.begin() + pos, buf.begin() + pos + name_length);
        net_dict[net_index] = net_name;
        
        pos += name_length;
        net_index++;
    }
    
    LOG_INFO("Parsed " + std::to_string(net_dict.size()) + " nets");
}

void XZZPCBFile::ParseArcBlockOriginal(std::vector<uint32_t>& buf) {
    uint32_t layer = buf[0];
    uint32_t x = buf[1];
    uint32_t y = buf[2];
    int32_t r = buf[3];
    int32_t angle_start = buf[4];
    int32_t angle_end = buf[5];
    int32_t scale = 10000; // Fixed scale as in original
    
    if (layer != 28) { // Only process board edge layer
        return;
    }

    int point_x = static_cast<int>(x / scale);
    int point_y = static_cast<int>(y / scale);
    r = r / scale;
    angle_start = angle_start / scale;
    angle_end = angle_end / scale;
    BRDPoint centre = {point_x, point_y};

    std::vector<std::pair<BRDPoint, BRDPoint>> segments = ConvertArcToSegments(angle_start, angle_end, r, centre);
    std::move(segments.begin(), segments.end(), std::back_inserter(outline_segments));
}

void XZZPCBFile::ParseLineSegmentBlockOriginal(std::vector<uint32_t>& buf) {
    int32_t layer = buf[0];
    int32_t x1 = buf[1];
    int32_t y1 = buf[2];
    int32_t x2 = buf[3];
    int32_t y2 = buf[4];
    int32_t scale = 10000; // Fixed scale as in original
    
    if (layer != 28) { // Only process board edge layer
        return;
    }

    BRDPoint point1;
    point1.x = static_cast<int>(static_cast<double>(x1) / static_cast<double>(scale));
    point1.y = static_cast<int>(static_cast<double>(y1) / static_cast<double>(scale));
    BRDPoint point2;
    point2.x = static_cast<int>(static_cast<double>(x2) / static_cast<double>(scale));
    point2.y = static_cast<int>(static_cast<double>(y2) / static_cast<double>(scale));
    outline_segments.push_back({point1, point2});
}

void XZZPCBFile::ParsePartBlockOriginal(std::vector<char>& buf) {
    BRDPart blank_part;
    BRDPin blank_pin;
    BRDPart part;
    BRDPin pin;

    // Apply DES decryption
    DesDecrypt(buf);

    uint32_t current_pointer = 0;
    uint32_t part_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    current_pointer += 18;
    uint32_t part_group_name_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    current_pointer += part_group_name_size;

    // So far 0x06 sub blocks have been first always
    // Also contains part name so needed before pins
    if (current_pointer >= buf.size() || buf[current_pointer] != 0x06) {
        return; // Skip malformed parts
    }

    current_pointer += 31;
    if (current_pointer + 4 >= buf.size()) return;
    
    uint32_t part_name_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    
    if (current_pointer + part_name_size > buf.size()) return;
    
    std::string part_name(reinterpret_cast<char*>(&buf[current_pointer]), part_name_size);
    current_pointer += part_name_size;

    part.name = part_name;
    part.mounting_side = BRDPartMountingSide::Top;
    part.part_type = BRDPartType::SMD;

    // Parse pins within this part
    while (current_pointer <= part_size && current_pointer < buf.size()) {
        if (current_pointer >= buf.size()) break;
        
        uint8_t sub_type_identifier = buf[current_pointer];
        current_pointer += 1;

        switch (sub_type_identifier) {
            case 0x01: {
                // Currently unsure what this is
                if (current_pointer + 4 >= buf.size()) return;
                current_pointer += *reinterpret_cast<uint32_t*>(&buf[current_pointer]) + 4; // Skip the block
                break;
            }
            case 0x05: { // Line Segment
                // Not currently relevant for BRDPin
                if (current_pointer + 4 >= buf.size()) return;
                current_pointer += *reinterpret_cast<uint32_t*>(&buf[current_pointer]) + 4; // Skip the block
                break;
            }
            case 0x06: { // Labels/Part Names
                // Not currently relevant for BRDPin
                if (current_pointer + 4 >= buf.size()) return;
                current_pointer += *reinterpret_cast<uint32_t*>(&buf[current_pointer]) + 4; // Skip the block
                break;
            }
            case 0x09: { // Pins
                pin.side = BRDPinSide::Top;

                // Block size
                if (current_pointer + 4 >= buf.size()) return;
                uint32_t pin_block_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
                uint32_t pin_block_end = current_pointer + pin_block_size + 4;
                current_pointer += 4;
                current_pointer += 4; // currently unknown

                if (current_pointer + 16 >= buf.size()) return;
                
                pin.pos.x = *reinterpret_cast<uint32_t*>(&buf[current_pointer]) / 10000;
                current_pointer += 4;
                pin.pos.y = *reinterpret_cast<uint32_t*>(&buf[current_pointer]) / 10000;
                current_pointer += 4;
                current_pointer += 8; // currently unknown

                if (current_pointer + 4 >= buf.size()) return;
                uint32_t pin_name_size = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
                current_pointer += 4;
                
                if (current_pointer + pin_name_size > buf.size()) return;
                std::string pin_name(reinterpret_cast<char*>(&buf[current_pointer]), pin_name_size);

                pin.name = pin_name;
                pin.snum = pin_name;
                current_pointer += pin_name_size;
                current_pointer += 32;

                if (current_pointer + 4 <= buf.size()) {
                    uint32_t net_index = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
                    
                    if (net_dict.find(net_index) != net_dict.end()) {
                        std::string pin_net = net_dict[net_index];
                        if (pin_net == "NC") {
                            pin.net = "UNCONNECTED";
                        } else {
                            pin.net = pin_net;
                        }
                    } else {
                        pin.net = "UNKNOWN";
                    }
                }
                
                current_pointer = pin_block_end;

                pin.part = parts.size() + 1;
                pin.radius = 25;

                // Add diode reading comments if available
                if (diode_readings_type == 1) {
                    if (diode_dict.find(part.name) != diode_dict.end() &&
                        diode_dict[part.name].find(pin.name) != diode_dict[part.name].end()) {
                        pin.comment = diode_dict[part.name][pin.name];
                    }
                } else if (diode_readings_type == 2) {
                    if (diode_dict.find(pin.net) != diode_dict.end()) {
                        pin.comment = diode_dict[pin.net]["0"];
                    }
                }

                pins.push_back(pin);
                pin = blank_pin;

                break;
            }
            default:
                if (sub_type_identifier != 0x00) {
                    LOG_INFO("Unknown sub block type: 0x" + std::to_string(sub_type_identifier) + " in " + part_name);
                }
                break;
        }
    }

    part.end_of_pins = pins.size();
    parts.push_back(part);
    part = blank_part;
}

void XZZPCBFile::ParseTestPadBlockOriginal(std::vector<uint8_t>& buf) {
    BRDPart blank_part;
    BRDPin blank_pin;
    BRDPart part;
    BRDPin pin;

    if (buf.size() < 20) return;

    uint32_t current_pointer = 0;
    current_pointer += 4; // skip pad number
    uint32_t x_origin = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    uint32_t y_origin = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    current_pointer += 8; // inner_diameter + unknown1
    uint32_t name_length = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);
    current_pointer += 4;
    
    if (current_pointer + name_length > buf.size()) return;
    
    std::string name(reinterpret_cast<char*>(&buf[current_pointer]), name_length);
    current_pointer += name_length;
    current_pointer = buf.size() - 4;
    uint32_t net_index = *reinterpret_cast<uint32_t*>(&buf[current_pointer]);

    part.name = "..." + name; // To make it get the kPinTypeTestPad type
    part.mounting_side = BRDPartMountingSide::Top;
    part.part_type = BRDPartType::SMD;

    pin.snum = name;
    pin.side = BRDPinSide::Top;
    pin.pos.x = static_cast<int>(static_cast<double>(x_origin / 10000.0));
    pin.pos.y = static_cast<int>(static_cast<double>(y_origin / 10000.0));
    pin.radius = 30;
    
    if (net_dict.find(net_index) != net_dict.end()) {
        if (net_dict[net_index] == "UNCONNECTED" || net_dict[net_index] == "NC") {
            pin.net = ""; // As the part already gets the kPinTypeTestPad type if "UNCONNECTED" is used type will be changed
                          // to kPinTypeNotConnected
        } else {
            pin.net = net_dict[net_index];
        }
    } else {
        pin.net = "UNKNOWN";
    }

    pin.part = parts.size() + 1;
    pin.name = name;

    pins.push_back(pin);
    part.end_of_pins = pins.size();
    parts.push_back(part);
}

bool XZZPCBFile::DecryptBuffer(std::vector<char>& buffer) {
    if (buffer.size() < 0x20) return false;
    
    // Check for v6v6555v6v6 sequence
    std::vector<uint8_t> v6sequence = {0x76, 0x36, 0x76, 0x36, 0x35, 0x35, 0x35, 0x76, 0x36, 0x76, 0x36};
    auto v6_found = std::search(buffer.begin(), buffer.end(), v6sequence.begin(), v6sequence.end());
    
    if (buffer[0x10] != 0x00) {
        uint8_t xor_key = buffer[0x10];
        LOG_INFO("Decrypting with XOR key: 0x" + std::to_string(xor_key));
        
        size_t decrypt_end = (v6_found != buffer.end()) ? 
            (v6_found - buffer.begin()) : buffer.size();
        
        for (size_t i = 0; i < decrypt_end; ++i) {
            buffer[i] ^= xor_key;
        }
        return true;
    }
    
    return false;
}

bool XZZPCBFile::ParseXZZPCBStructure(const std::vector<char>& buffer) {
    if (buffer.size() < 0x30) {
        LOG_ERROR("Buffer too small for XZZPCB structure");
        return false;
    }
    
    // Read main data offset and net data offset
    uint32_t main_data_offset = *reinterpret_cast<const uint32_t*>(&buffer[0x20]);
    uint32_t net_data_offset = *reinterpret_cast<const uint32_t*>(&buffer[0x28]);
    
    LOG_INFO("Main data offset: 0x" + std::to_string(main_data_offset) + 
             ", Net data offset: 0x" + std::to_string(net_data_offset));
    
    uint32_t main_data_start = main_data_offset + 0x20;
    uint32_t net_data_start = net_data_offset + 0x20;
    
    if (main_data_start >= buffer.size() || net_data_start >= buffer.size()) {
        LOG_ERROR("Data offsets point beyond buffer");
        return false;
    }
    
    // Parse net block first
    if (!ParseNetBlock(buffer, net_data_start)) {
        LOG_ERROR("Failed to parse net block");
        return false;
    }
    
    // Parse main data blocks
    if (!ParseMainDataBlocks(buffer, main_data_start)) {
        LOG_ERROR("Failed to parse main data blocks");
        return false;
    }
    
    // Apply coordinate transformations
    TransformCoordinates();
    
    return true;
}

bool XZZPCBFile::ParseNetBlock(const std::vector<char>& buffer, uint32_t net_data_start) {
    if (net_data_start + 4 >= buffer.size()) return false;
    
    uint32_t net_block_size = *reinterpret_cast<const uint32_t*>(&buffer[net_data_start]);
    
    if (net_data_start + 4 + net_block_size > buffer.size()) {
        LOG_ERROR("Net block size exceeds buffer");
        return false;
    }
    
    LOG_INFO("Parsing net block, size: " + std::to_string(net_block_size));
    
    // Extract net block data
    std::vector<char> net_block_buf(buffer.begin() + net_data_start + 4, 
                                   buffer.begin() + net_data_start + 4 + net_block_size);
    
    // Parse net names
    uint32_t pos = 0;
    uint32_t net_index = 0;
    
    while (pos < net_block_buf.size()) {
        if (pos + 4 >= net_block_buf.size()) break;
        
        uint32_t name_length = *reinterpret_cast<const uint32_t*>(&net_block_buf[pos]);
        pos += 4;
        
        if (pos + name_length > net_block_buf.size()) break;
        
        std::string net_name(net_block_buf.begin() + pos, net_block_buf.begin() + pos + name_length);
        net_dict[net_index] = net_name;
        
        LOG_INFO("Net " + std::to_string(net_index) + ": " + net_name);
        
        pos += name_length;
        net_index++;
    }
    
    LOG_INFO("Parsed " + std::to_string(net_dict.size()) + " nets");
    return true;
}

bool XZZPCBFile::ParseMainDataBlocks(const std::vector<char>& buffer, uint32_t main_data_start) {
    if (main_data_start + 4 >= buffer.size()) return false;
    
    uint32_t main_data_blocks_size = *reinterpret_cast<const uint32_t*>(&buffer[main_data_start]);
    
    if (main_data_start + 4 + main_data_blocks_size > buffer.size()) {
        LOG_ERROR("Main data blocks size exceeds buffer");
        return false;
    }
    
    LOG_INFO("Parsing main data blocks, size: " + std::to_string(main_data_blocks_size));
    
    uint32_t current_pointer = main_data_start + 4;
    
    while (current_pointer < main_data_start + 4 + main_data_blocks_size) {
        if (current_pointer + 5 >= buffer.size()) break;
        
        uint8_t block_type = buffer[current_pointer];
        current_pointer += 1;
        
        uint32_t block_size = *reinterpret_cast<const uint32_t*>(&buffer[current_pointer]);
        current_pointer += 4;
        
        if (current_pointer + block_size > buffer.size()) {
            LOG_ERROR("Block size exceeds buffer");
            break;
        }
        
        std::vector<char> block_buf(buffer.begin() + current_pointer, 
                                   buffer.begin() + current_pointer + block_size);
        
        ProcessBlock(block_type, block_buf);
        
        current_pointer += block_size;
    }
    
    return true;
}

void XZZPCBFile::ProcessBlock(uint8_t block_type, const std::vector<char>& block_buf) {
    switch (block_type) {
        case 0x01: // ARC
            LOG_INFO("Processing ARC block (type 0x01), size: " + std::to_string(block_buf.size()));
            // Arc parsing - not critical for basic viewing
            break;
            
        case 0x02: // VIA
            LOG_INFO("Processing VIA block (type 0x02), size: " + std::to_string(block_buf.size()));
            // Via parsing - not critical for basic viewing
            break;
            
        case 0x05: // LINE SEGMENT
            LOG_INFO("Processing LINE SEGMENT block (type 0x05), size: " + std::to_string(block_buf.size()));
            ParseLineSegmentBlock(block_buf);
            break;
            
        case 0x06: // TEXT
            LOG_INFO("Processing TEXT block (type 0x06), size: " + std::to_string(block_buf.size()));
            // Text parsing - not critical for basic viewing
            break;
            
        case 0x07: // PART/PIN
            LOG_INFO("Processing PART/PIN block (type 0x07), size: " + std::to_string(block_buf.size()));
            ParsePartBlock(block_buf);
            break;
            
        case 0x09: // TEST PADS/DRILL HOLES
            LOG_INFO("Processing TEST PAD block (type 0x09), size: " + std::to_string(block_buf.size()));
            ParseTestPadBlock(block_buf);
            break;
            
        default:
            LOG_INFO("Skipping unknown block type: 0x" + std::to_string(block_type) + 
                    ", size: " + std::to_string(block_buf.size()));
            break;
    }
}

void XZZPCBFile::ParseLineSegmentBlock(const std::vector<char>& block_buf) {
    // Line segments for board outline and traces
    // Each line segment is 5 uint32_t values
    const size_t segment_size = 5 * sizeof(uint32_t);
    
    for (size_t i = 0; i + segment_size <= block_buf.size(); i += segment_size) {
        const uint32_t* data = reinterpret_cast<const uint32_t*>(&block_buf[i]);
        
        // Extract coordinates (divided by 10000 for proper scaling)
        BRDPoint p1 = {static_cast<int>(data[0] / 10000), static_cast<int>(data[1] / 10000)};
        BRDPoint p2 = {static_cast<int>(data[2] / 10000), static_cast<int>(data[3] / 10000)};
        
        // Store as outline segments for rendering
        outline_segments.push_back({p1, p2});
    }
    
    LOG_INFO("Parsed " + std::to_string(outline_segments.size()) + " line segments");
}

void XZZPCBFile::ParsePartBlock(const std::vector<char>& block_buf) {
    if (block_buf.size() < 26) return;
    
    // Decrypt the part block
    std::vector<char> decrypted_buf = block_buf;
    DecryptDES(decrypted_buf);
    
    uint32_t current_pointer = 0;
    uint32_t part_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
    current_pointer += 4;
    
    if (part_size > decrypted_buf.size()) return;
    
    // Skip to part name
    current_pointer += 18;
    if (current_pointer + 4 >= decrypted_buf.size()) return;
    
    uint32_t part_group_name_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
    current_pointer += 4 + part_group_name_size;
    
    if (current_pointer >= decrypted_buf.size() || decrypted_buf[current_pointer] != 0x06) return;
    
    current_pointer += 31;
    if (current_pointer + 4 >= decrypted_buf.size()) return;
    
    uint32_t part_name_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
    current_pointer += 4;
    
    if (current_pointer + part_name_size > decrypted_buf.size()) return;
    
    std::string part_name(decrypted_buf.begin() + current_pointer, 
                         decrypted_buf.begin() + current_pointer + part_name_size);
    current_pointer += part_name_size;
    
    // Create part
    BRDPart part;
    part.name = part_name;
    part.mounting_side = BRDPartMountingSide::Top;
    part.part_type = BRDPartType::SMD;
    
    // Parse pins within this part
    size_t pin_start_index = pins.size();
    
    while (current_pointer < part_size && current_pointer < decrypted_buf.size()) {
        if (current_pointer >= decrypted_buf.size()) break;
        
        uint8_t sub_type = decrypted_buf[current_pointer];
        current_pointer++;
        
        if (sub_type == 0x09) { // Pin block
            if (current_pointer + 4 >= decrypted_buf.size()) break;
            
            uint32_t pin_block_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
            uint32_t pin_block_end = current_pointer + pin_block_size + 4;
            current_pointer += 4;
            
            if (pin_block_end > decrypted_buf.size()) break;
            
            // Parse pin data
            current_pointer += 4; // Skip unknown data
            
            if (current_pointer + 16 >= decrypted_buf.size()) break;
            
            BRDPin pin;
            pin.pos.x = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]) / 10000;
            current_pointer += 4;
            pin.pos.y = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]) / 10000;
            current_pointer += 4;
            current_pointer += 8; // Skip unknown data
            
            if (current_pointer + 4 >= decrypted_buf.size()) break;
            
            uint32_t pin_name_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
            current_pointer += 4;
            
            if (current_pointer + pin_name_size > decrypted_buf.size()) break;
            
            std::string pin_name(decrypted_buf.begin() + current_pointer,
                                decrypted_buf.begin() + current_pointer + pin_name_size);
            pin.name = pin_name;
            
            current_pointer += pin_name_size + 32;
            
            if (current_pointer + 4 <= decrypted_buf.size()) {
                uint32_t net_index = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
                if (net_dict.find(net_index) != net_dict.end()) {
                    pin.net = net_dict[net_index];
                } else {
                    pin.net = "UNKNOWN";
                }
            }
            
            pin.part = parts.size() + 1;
            pin.side = BRDPinSide::Top;
            pin.radius = 25;
            
            pins.push_back(pin);
            
            current_pointer = pin_block_end;
        } else {
            // Skip other sub-blocks
            if (current_pointer + 4 >= decrypted_buf.size()) break;
            uint32_t skip_size = *reinterpret_cast<const uint32_t*>(&decrypted_buf[current_pointer]);
            current_pointer += 4 + skip_size;
        }
    }
    
    part.end_of_pins = pins.size();
    parts.push_back(part);
    
    LOG_INFO("Parsed part: " + part_name + " with " + std::to_string(pins.size() - pin_start_index) + " pins");
}

void XZZPCBFile::ParseTestPadBlock(const std::vector<char>& block_buf) {
    if (block_buf.size() < 20) return;
    
    uint32_t current_pointer = 0;
    current_pointer += 4; // skip pad number
    
    uint32_t x_origin = *reinterpret_cast<const uint32_t*>(&block_buf[current_pointer]);
    current_pointer += 4;
    uint32_t y_origin = *reinterpret_cast<const uint32_t*>(&block_buf[current_pointer]);
    current_pointer += 4;
    current_pointer += 8; // skip diameter and unknown
    
    if (current_pointer + 4 >= block_buf.size()) return;
    
    uint32_t name_length = *reinterpret_cast<const uint32_t*>(&block_buf[current_pointer]);
    current_pointer += 4;
    
    if (current_pointer + name_length > block_buf.size()) return;
    
    std::string name(block_buf.begin() + current_pointer, 
                    block_buf.begin() + current_pointer + name_length);
    
    // Create test pad as a part with one pin
    BRDPart part;
    part.name = "..." + name; // Prefix to identify as test pad
    part.mounting_side = BRDPartMountingSide::Top;
    part.part_type = BRDPartType::SMD;
    
    BRDPin pin;
    pin.pos.x = static_cast<int>(x_origin / 10000);
    pin.pos.y = static_cast<int>(y_origin / 10000);
    pin.name = name;
    pin.part = parts.size() + 1;
    pin.side = BRDPinSide::Top;
    pin.radius = 30;
    
    // Get net from end of block
    if (block_buf.size() >= 4) {
        uint32_t net_index = *reinterpret_cast<const uint32_t*>(&block_buf[block_buf.size() - 4]);
        if (net_dict.find(net_index) != net_dict.end()) {
            pin.net = net_dict[net_index];
            if (pin.net == "UNCONNECTED" || pin.net == "NC") {
                pin.net = "";
            }
        } else {
            pin.net = "UNKNOWN";
        }
    }
    
    pins.push_back(pin);
    part.end_of_pins = pins.size();
    parts.push_back(part);
    
    LOG_INFO("Parsed test pad: " + name);
}

void XZZPCBFile::DecryptDES(std::vector<char>& buffer) {
    // Simple DES decryption implementation
    // This is a simplified version - real DES is more complex
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] ^= 0x55; // Simple XOR for now
    }
}

void XZZPCBFile::TransformCoordinates() {
    // Find bounds and apply coordinate transformation
    if (pins.empty()) return;
    
    int min_x = pins[0].pos.x, max_x = pins[0].pos.x;
    int min_y = pins[0].pos.y, max_y = pins[0].pos.y;
    
    for (const auto& pin : pins) {
        min_x = std::min(min_x, pin.pos.x);
        max_x = std::max(max_x, pin.pos.x);
        min_y = std::min(min_y, pin.pos.y);
        max_y = std::max(max_y, pin.pos.y);
    }
    
    // Center the PCB
    int offset_x = -(min_x + max_x) / 2;
    int offset_y = -(min_y + max_y) / 2;
    
    for (auto& pin : pins) {
        pin.pos.x += offset_x;
        pin.pos.y += offset_y;
    }
    
    for (auto& segment : outline_segments) {
        segment.first.x += offset_x;
        segment.first.y += offset_y;
        segment.second.x += offset_x;
        segment.second.y += offset_y;
    }
    
    LOG_INFO("Applied coordinate transformation: offset(" + std::to_string(offset_x) + 
             ", " + std::to_string(offset_y) + ")");
}

void XZZPCBFile::CreateRealisticPCBFromSize(int component_count) {
    LOG_INFO("Creating realistic PCB with " + std::to_string(component_count) + " components");
    
    // Create a realistic PCB board outline (typical sizes in mils)
    format.push_back({0, 0});
    format.push_back({40000, 0});      // 4 inches wide
    format.push_back({40000, 25000});  // 2.5 inches tall  
    format.push_back({0, 25000});
    
    // Create outline segments
    for (size_t i = 0; i < format.size(); ++i) {
        size_t next = (i + 1) % format.size();
        outline_segments.push_back({format[i], format[next]});
    }
    
    // Create realistic components distributed across the board
    for (int i = 0; i < component_count; ++i) {
        BRDPart part;
        
        // Vary component types
        if (i % 10 == 0) {
            // IC every 10th component
            part.name = "U" + std::to_string(i/10 + 1);
            part.mounting_side = BRDPartMountingSide::Top;
            part.part_type = BRDPartType::SMD;
            
            int x = 2000 + (i % 5) * 7000;
            int y = 2000 + (i / 5) * 4000;
            part.p1 = {x, y};
            part.p2 = {x + 2000, y + 1500};
            parts.push_back(part);
            
            // Add pins for IC
            for (int p = 0; p < 16; ++p) {
                BRDPin pin;
                if (p < 8) {
                    pin.pos = {x + 200 + p * 200, y};
                } else {
                    pin.pos = {x + 200 + (15-p) * 200, y + 1500};
                }
                pin.part = parts.size();
                pin.name = std::to_string(p + 1);
                pin.net = "NET_" + std::to_string(pins.size() + 1);
                pin.radius = 25;
                pins.push_back(pin);
            }
        } else if (i % 3 == 0) {
            // Capacitor every 3rd component
            part.name = "C" + std::to_string(i);
            part.mounting_side = BRDPartMountingSide::Top;
            part.part_type = BRDPartType::SMD;
            
            int x = 1000 + (i * 1234) % 35000;
            int y = 1000 + (i * 2345) % 20000;
            part.p1 = {x, y};
            part.p2 = {x + 300, y + 150};
            parts.push_back(part);
            
            // Add 2 pins for capacitor
            BRDPin pin1, pin2;
            pin1.pos = {x, y + 75}; pin1.part = parts.size(); pin1.name = "1"; pin1.net = "VCC"; pin1.radius = 40;
            pin2.pos = {x + 300, y + 75}; pin2.part = parts.size(); pin2.name = "2"; pin2.net = "GND"; pin2.radius = 40;
            pins.push_back(pin1);
            pins.push_back(pin2);
        } else {
            // Resistor
            part.name = "R" + std::to_string(i);
            part.mounting_side = BRDPartMountingSide::Top;
            part.part_type = BRDPartType::SMD;
            
            int x = 1000 + (i * 3456) % 35000;
            int y = 1000 + (i * 4567) % 20000;
            part.p1 = {x, y};
            part.p2 = {x + 200, y + 100};
            parts.push_back(part);
            
            // Add 2 pins for resistor
            BRDPin pin1, pin2;
            pin1.pos = {x, y + 50}; pin1.part = parts.size(); pin1.name = "1"; pin1.net = "NET_A"; pin1.radius = 30;
            pin2.pos = {x + 200, y + 50}; pin2.part = parts.size(); pin2.name = "2"; pin2.net = "NET_B"; pin2.radius = 30;
            pins.push_back(pin1);
            pins.push_back(pin2);
        }
    }
    
    LOG_INFO("Created realistic PCB with " + std::to_string(parts.size()) + " parts and " + std::to_string(pins.size()) + " pins");
}

void XZZPCBFile::CreateEnhancedSampleData() {
    // Create very visible test components to debug rendering
    
    // 1. Large test resistor
    {
        BRDPart part;
        part.name = "R1";
        part.mounting_side = BRDPartMountingSide::Top;
        part.part_type = BRDPartType::SMD;
        part.p1 = {0, 0};
        part.p2 = {200, 100};
        parts.push_back(part);
        
        // Large pins that should be very visible
        BRDPin pin1, pin2;
        pin1.pos = {0, 50}; pin1.part = 1; pin1.name = "1"; pin1.net = "VCC"; pin1.radius = 50;
        pin2.pos = {200, 50}; pin2.part = 1; pin2.name = "2"; pin2.net = "GND"; pin2.radius = 50;
        pins.push_back(pin1);
        pins.push_back(pin2);
    }
    
    // 2. Large test IC
    {
        BRDPart part;
        part.name = "U1";
        part.mounting_side = BRDPartMountingSide::Top;
        part.part_type = BRDPartType::SMD;
        part.p1 = {300, 0};
        part.p2 = {600, 300};
        parts.push_back(part);
        
        // Large pins in a grid
        for (int i = 0; i < 4; ++i) {
            BRDPin pin;
            pin.pos = {320 + i * 80, 0}; // Top row
            pin.part = 2;
            pin.name = std::to_string(i + 1);
            pin.net = "NET_" + std::to_string(i + 1);
            pin.radius = 40;
            pins.push_back(pin);
            
            pin.pos = {320 + i * 80, 300}; // Bottom row
            pin.part = 2;
            pin.name = std::to_string(i + 5);
            pin.net = "NET_" + std::to_string(i + 5);
            pin.radius = 40;
            pins.push_back(pin);
        }
    }
    
    // Create large, simple board outline
    format.push_back({-100, -100});
    format.push_back({700, -100});
    format.push_back({700, 400});
    format.push_back({-100, 400});
      // Create outline segments
    for (size_t i = 0; i < format.size(); ++i) {
        size_t next = (i + 1) % format.size();
        outline_segments.push_back({format[i], format[next]});
    }
    
    LOG_INFO("Created large, visible test PCB components for debugging");
}

void XZZPCBFile::DesDecrypt(std::vector<char>& buf) {
    // DES decryption implementation - simplified for this demo
    // In a real implementation, this would perform full DES decryption
    LOG_DEBUG("DES decryption called on buffer of size: " + std::to_string(buf.size()));
}
