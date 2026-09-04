#ifndef PHYSICS_H
#define PHYSICS_H

#include "details.h"

double calculate_distance(double x1, double y1, double x2, double y2);
double calculate_range(double velocity, double angle_deg);
double calculate_time_of_flight(double velocity, double angle_deg);
int is_in_escort_range(EscortShip e, Battleship b);
int is_in_battleship_range(Battleship b, EscortShip e);

#endif
