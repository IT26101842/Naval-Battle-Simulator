#ifndef DETAILS_H
#define DETAILS_H

#define GRAVITY 9.8
#define MAX_ESCORT_SHIPS 50
#define PI 3.14159265358979323846
#define MAX_PATH_POINTS 100

// data structure for escort ship
typedef struct {
    int id;
    char type[5];          // type of ship "EA,EB,EC.."
    char type_name[30];
    char gun_name[30];
    double x, y;           // coordinates
    double min_velocity;
    double max_velocity;
    double min_angle;      
    double max_angle;      
    double impact_power;
    int is_destroyed;      // 0 = "destroy" 1 = "saved"
} EscortShip;

// data structure for battle ship
typedef struct {
    char name[30];
    char notation;         // 'U', 'M', 'R', 'S'
    char gun_name[30];
    double x, y;
    double max_velocity;   // VB_max
    int is_destroyed;
} Battleship;

// data structure for canvas
typedef struct {
    double canvas_size;    // D
    int num_escorts;       // N max 50
    Battleship battleship;
    EscortShip escorts[MAX_ESCORT_SHIPS];
} Battlefield;

// Data structure for path points
typedef struct {
	double x;
	double y;
} Point;

#endif
