#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// Starting the log files
FILE* open_log_file(const char *filename);
void close_log_file(FILE *log_file);

// function protocols for write and record each and every step in the simulation
void log_step_header(FILE *log_file, int step, int total_steps, double x, double y);
void log_escort_attack(FILE *log_file, int escort_id, int success);
void log_battleship_counter(FILE *log_file, int escort_id);
void log_victory(FILE *log_file, int step);
void log_summary(FILE *log_file, Battleship *battleship, EscortShip escorts[], int num_escorts);

#endif
