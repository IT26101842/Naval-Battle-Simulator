#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_2A/strategy.h"

// file management functions
FILE* open_simulation_log(const char *subfolder, const char *prefix);
void log_attack_order(FILE *fp, int step, TargetInfo order[], int count);
void log_shot_event(FILE *fp, double time, const char *shooter, const char *target, 
                    double distance, double angle, double velocity, int hit, 
                    double impact_factor, double damage);
void log_simulation_summary(FILE *fp, int b_survived, double cumulative_damage, 
                            int destroyed_count, int total_escorts, double total_time);
void log_damaged_escorts_report(const char *sim_type, EscortShip escorts[], 
                                double escort_damage_received[], int n_escorts);
void close_simulation_log(FILE *fp);

#endif
