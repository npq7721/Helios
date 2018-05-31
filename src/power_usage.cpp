/*
 * power_usage.cpp
 *
 *  Created on: May 11, 2018
 *      Author: tri
 */

#include "power_usage.h"

double CpuPower::getCurrentValue() {
	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
		timeSample.tms_utime < lastUserCPU){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		percent = (timeSample.tms_stime - lastSysCPU) +
			(timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
}

double CpuPower::writeCurrentVaueToFile(uint64_t hashRate) {
	double percent = getCurrentValue();
	fprintf(statsFile, ", [%ld] - [%f]", hashRate, percent);
	return percent;
}

void CpuPower::closeStats() {
	fprintf(statsFile, "\n");
	fclose(statsFile);
}
