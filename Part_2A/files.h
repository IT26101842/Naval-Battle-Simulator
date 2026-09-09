#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "strategy.h"

// opening and closing log files
FILE* open_simulation_log(const char *subfolder, const char *prefix);
void close_simulation_log(FILE *fp);

// order of attacking
void log_attack_order(FILE *log, int step, TargetInfo order[], int count);
void log_shot_event(FILE *log, double time, const char *shooter, const char *target, 
                    double distance, double angle, double velocity, int is_hit, double damage);

// summary log
void log_simulation_summary(FILE *log, int b_survived, double cumulative_damage, 
                            int destroyed_escorts, int total_escorts, double total_time);

#endif
