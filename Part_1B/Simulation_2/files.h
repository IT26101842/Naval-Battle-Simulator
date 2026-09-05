#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// opening and closing the log file
FILE* open_simulation_log(const char *filename);
void close_simulation_log(FILE *log_file);

// battle details and steps recording
void log_step_info(FILE *log_file, int step, int total_steps, double bx, double by, int is_jammed, double min_angle, int jam_step);
void log_attack_event(FILE *log_file, const char *attacker, int attacker_id, const char *target, int target_id, double distance, double angle, double flight_time);
void log_final_summary(FILE *log_file, Battleship *battleship, EscortShip escorts[], int num_escorts);

#endif
