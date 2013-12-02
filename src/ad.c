/*
 * Copyright (c) 2013, Ismael Gomez - gomezi@tcd.ie
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GPIO_MAP_MAX 	3
char* gpio_map[GPIO_MAP_MAX] = {"37", "36", "23"};

int sysfs_write(char *str, char *file) {
	FILE *f;
	int ret = -1;
	f = fopen(file, "w");
	if (f != NULL) {
		if (-1 != fwrite(str, strlen(str), 1, f)) {
			ret = 0;
		} else {
			perror("fwrite");
		}
	} else {
		perror("fopen");
	}
	fclose(f);
	return ret;
}

int sysfs_read(char *file) {
	FILE *f;
	int ret = -1;
	f = fopen(file, "r");
	if (f != NULL) {
		char tmp[64];
		if (-1 != fread(tmp, 64, 1, f)) {
			ret = atoi(tmp);
		} else {
			perror("fread");
		}
	} else {
		perror("fopen");
	}
	fclose(f);
	return ret;
}

int ad_init(int idx) {
	if (idx >=0 && idx < GPIO_MAP_MAX) {
		char tmp[256];
		sysfs_write(gpio_map[idx], "/sys/class/gpio/export");
		snprintf(tmp, 256, "/sys/class/gpio/gpio%s/direction",gpio_map[idx]);
		sysfs_write("out", tmp);
		snprintf(tmp, 256, "/sys/class/gpio/gpio%s/value",gpio_map[idx]);
		sysfs_write("0", tmp);
		return 0;
	} else {
		return -1;
	}
}

uint64_t ad_read(int idx) {
	char tmp[256];
	snprintf(tmp, 256, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw",idx);
	return (uint64_t) sysfs_read(tmp);
}
