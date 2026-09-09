#ifndef FILES_H
#define FILES_H

#include <stdio.h>

// making new logs opening with a folder
FILE* open_simulation_log(const char *folder, const char *prefix);
void close_simulation_log(FILE *log_file);

void log_step_info(FILE *log_file, int step, int total_steps, double bx, double by, int is_jammed, double min_angle, int jam_step);
void log_attack_event(FILE *log_file, const char *attacker, int attacker_id, const char *target, int target_id, double distance, double angle, double flight_time);

// giving the summary with the number of destroyed escortships
void log_summary(FILE *log_file, int b_sunk, double cumulative_damage, int end_step, int destroyed_escorts, int total_escorts);

#endif
