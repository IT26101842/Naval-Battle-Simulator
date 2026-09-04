#include <math.h>
#include "physics.h"

// Fuctions for calculate distance between ships
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Function for calculate distance of a shot
double calculate_range(double velocity, double angle_deg) {
    double angle_rad = angle_deg * (PI / 180.0);
    return (pow(velocity, 2) * sin(2.0 * angle_rad)) / GRAVITY;
}

// Fuction for calculate the time of a shot
double calculate_time_of_flight(double velocity, double angle_deg) {
    double angle_rad = angle_deg * (PI / 180.0);
    return (2.0 * velocity * sin(angle_rad)) / GRAVITY;
}

// function for check the escortship shooting range
int is_in_escort_range(EscortShip e, Battleship b) {
    double d = calculate_distance(e.x, e.y, b.x, b.y);
    double r_min = calculate_range(e.min_velocity, e.min_angle);
    double r_max = calculate_range(e.max_velocity, e.max_angle);
    
    if (r_min > r_max) {
        double temp = r_min;
        r_min = r_max;
        r_max = temp;
    }
    return (d >= r_min && d <= r_max);
}

// function for check the battleship shooting range
int is_in_battleship_range(Battleship b, EscortShip e) {
    double d = calculate_distance(b.x, b.y, e.x, e.y);
    double r_max = pow(b.max_velocity, 2) / GRAVITY;
    return (d <= r_max);
}
