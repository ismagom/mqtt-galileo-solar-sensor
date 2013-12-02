/*
 * Copyright (c) 2013, Ismael Gomez - gomezi@tcd.ie
 */

#include <stdint.h>

#define TO_MILIVOLTS(a) (a*5000/4096)

int ad_init(int idx);
uint64_t ad_read(int idx);
