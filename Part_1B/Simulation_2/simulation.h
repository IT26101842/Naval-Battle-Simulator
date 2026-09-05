#ifndef SIMULATION_H
#define SIMULATION_H

#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"

// checking the angle when the distance is given
int find_elevation_angle(double distance, double velocity, double min_angle, double max_angle, double *angle_out);

// function prototype for run simulation 2
void run_simulation_2(Battleship *battleship, 
                      EscortShip escorts[], 
                      int num_escorts, 
                      Point path[], 
                      int num_points, 
                      int jam_step, 
                      double canvas_size);

#endif
