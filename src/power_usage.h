// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Helios Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HELIOS_POWER_USAGE_H_
#define HELIOS_POWER_USAGE_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/vtimes.h>
#include <stdint.h>
//#include "util.h"
using namespace std;


class CpuPower {
public:
	CpuPower() : CpuPower(NULL, "") {}
	CpuPower(const char* fileName, const char* algos) {
		FILE* file;
		struct tms timeSample;
		char line[128];

		lastCPU = times(&timeSample);
		lastSysCPU = timeSample.tms_stime;
		lastUserCPU = timeSample.tms_utime;

		file = fopen("/proc/cpuinfo", "r");
		numProcessors = 0;
		while(fgets(line, 128, file) != NULL){
			if (strncmp(line, "processor", 9) == 0) numProcessors++;
		}

		fclose(file);

		/*fs::path pathStatsFile(gArgs.GetArg("-conf", "stats.csv"));
		if (!pathStatsFile.is_complete()) {
			pathStatsFile = GetDataDir(false) / pathStatsFile;

		}*/
		statsFile = fopen("/home/tri/.helios/stats.csv", "a");
		fprintf(statsFile, "%s", algos);
		if(fileName != NULL) {
		//	statsFile = fopen(fileName, "a");
			//fprintf(statsFile, "test");
		} else {
			//statsFile = NULL;
		}
	}
	~CpuPower() {
		/*
		if(statsFile != NULL) {
			fprintf(statsFile, "\n");
			fclose(statsFile);
		}*/
	}

	double getCurrentValue();
	clock_t getLastCPU() {return lastCPU;}
	clock_t getLastSysCPU() {return lastSysCPU;}
	clock_t getLastUserCPU() {return lastUserCPU;}
	int getNumProcessors() {return numProcessors;}
	double writeCurrentVaueToFile(uint64_t hashRate);
	void closeStats();

protected:
	clock_t lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	FILE* statsFile;
};

#endif /* end of HELIOS_POWER_USAGE_H*/
