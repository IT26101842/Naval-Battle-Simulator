#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"
#include "simulation.h"
#include "files.h"

// Function for get random numbers between 0 and Max
double get_random_coord(double max) {
    return ((double)rand() / RAND_MAX) * max;
}

int main() {
    srand(time(NULL));

    double canvas_size;
    int choice, k_points, num_escorts;

    printf("========================================\n");
    printf("   NAVAL BATTLEFIELD - PART 1-B (SIM 1) \n");
    printf("========================================\n");

    // Getting canvas number
    printf("Enter canvas size (max 2000): ");
    if (scanf("%lf", &canvas_size) != 1 || canvas_size <= 0) {
        canvas_size = 1500.0;
    } else if (canvas_size > 2000.0) {
        canvas_size = 2000.0;
    }

    // Selecting a battleship
    printf("\nSelect Battleship Type:\n");
    printf("1. HMS Dreadnought\n2. USS Iowa\n3. Bismarck\n4. Yamato\nEnter choice (1-4): ");
    scanf("%d", &choice);

    Battleship player_battleship;
    player_battleship.is_destroyed = 1;

    switch (choice) {
        case 1:
            strcpy(player_battleship.name, "HMS Dreadnought");
            player_battleship.notation = 'U';
            strcpy(player_battleship.gun_name, "BL 12-inch Mk X");
            player_battleship.max_velocity = 110.0;
            break;
        case 2:
            strcpy(player_battleship.name, "USS Iowa");
            player_battleship.notation = 'M';
            strcpy(player_battleship.gun_name, "16-inch/50 caliber");
            player_battleship.max_velocity = 115.0;
            break;
        case 3:
            strcpy(player_battleship.name, "Bismarck");
            player_battleship.notation = 'R';
            strcpy(player_battleship.gun_name, "38 cm SK C/34");
            player_battleship.max_velocity = 120.0;
            break;
        case 4:
            strcpy(player_battleship.name, "Yamato");
            player_battleship.notation = 'S';
            strcpy(player_battleship.gun_name, "46 cm/45 Type 94");
            player_battleship.max_velocity = 118.0;
            break;
        default:
            strcpy(player_battleship.name, "HMS Dreadnought");
            player_battleship.notation = 'U';
            strcpy(player_battleship.gun_name, "BL 12-inch Mk X");
            player_battleship.max_velocity = 110.0;
            break;
    }

    // Getting the path of the battleship
    printf("\nEnter number of path points (k, max %d): ", MAX_PATH_POINTS);
    scanf("%d", &k_points);
    if (k_points <= 0 || k_points > MAX_PATH_POINTS) k_points = 5;

    Point path[MAX_PATH_POINTS];
    printf("\n--- Generated Battleship Path ---\n");
    for (int i = 0; i < k_points; i++) {
        path[i].x = get_random_coord(canvas_size);
        path[i].y = get_random_coord(canvas_size);
        printf("Point %d: (%.2f, %.2f)\n", i + 1, path[i].x, path[i].y);
    }

    // getting number of escortships
    printf("\nEnter number of escort ships (max %d): ", MAX_ESCORT_SHIPS);
    scanf("%d", &num_escorts);
    if (num_escorts <= 0 || num_escorts > MAX_ESCORT_SHIPS) num_escorts = 5;

    EscortShip escorts[MAX_ESCORT_SHIPS];
    printf("\n--- Generated Escort Ships ---\n");

    for (int j = 0; j < num_escorts; j++) {
        escorts[j].id = j + 1;
        escorts[j].x = get_random_coord(canvas_size);
        escorts[j].y = get_random_coord(canvas_size);
        escorts[j].is_destroyed = 1;

        int type_rand = (rand() % 5) + 1;
        double angle_range = 0.0;

        switch (type_rand) {
            case 1:
                strcpy(escorts[j].type, "EA");
                strcpy(escorts[j].type_name, "1936A-class Destroyer");
                strcpy(escorts[j].gun_name, "SK C/34 naval gun");
                escorts[j].impact_power = 0.08;
                angle_range = 20.0;
                escorts[j].max_velocity = 1.2 * player_battleship.max_velocity;
                escorts[j].min_velocity = ((double)rand() / RAND_MAX) * (escorts[j].max_velocity * 0.5);
                break;
            case 2:
                strcpy(escorts[j].type, "EB");
                strcpy(escorts[j].type_name, "Gabbiano-class Corvette");
                strcpy(escorts[j].gun_name, "L/47 dual-purpose gun");
                escorts[j].impact_power = 0.06;
                angle_range = 30.0;
                escorts[j].min_velocity = 20.0 + ((double)rand() / RAND_MAX) * 20.0;
                escorts[j].max_velocity = escorts[j].min_velocity + 30.0 + ((double)rand() / RAND_MAX) * 30.0;
                break;
            case 3:
                strcpy(escorts[j].type, "EC");
                strcpy(escorts[j].type_name, "Matsu-class Destroyer");
                strcpy(escorts[j].gun_name, "Type 89 dual-purpose gun");
                escorts[j].impact_power = 0.07;
                angle_range = 25.0;
                escorts[j].min_velocity = 20.0 + ((double)rand() / RAND_MAX) * 20.0;
                escorts[j].max_velocity = escorts[j].min_velocity + 30.0 + ((double)rand() / RAND_MAX) * 30.0;
                break;
            case 4:
                strcpy(escorts[j].type, "ED");
                strcpy(escorts[j].type_name, "F-class Escort Ships");
                strcpy(escorts[j].gun_name, "SK C/32 naval gun");
                escorts[j].impact_power = 0.05;
                angle_range = 50.0;
                escorts[j].min_velocity = 15.0 + ((double)rand() / RAND_MAX) * 20.0;
                escorts[j].max_velocity = escorts[j].min_velocity + 25.0 + ((double)rand() / RAND_MAX) * 30.0;
                break;
            case 5:
                strcpy(escorts[j].type, "EE");
                strcpy(escorts[j].type_name, "Japanese Kaibokan");
                strcpy(escorts[j].gun_name, "(4.7 inch) naval guns");
                escorts[j].impact_power = 0.04;
                angle_range = 70.0;
                escorts[j].min_velocity = 15.0 + ((double)rand() / RAND_MAX) * 15.0;
                escorts[j].max_velocity = escorts[j].min_velocity + 20.0 + ((double)rand() / RAND_MAX) * 25.0;
                break;
        }

        // calculating the angles
        double max_possible_min_angle = 90.0 - angle_range;
        escorts[j].min_angle = ((double)rand() / RAND_MAX) * max_possible_min_angle;
        escorts[j].max_angle = escorts[j].min_angle + angle_range;

        printf("Escort #%d [%s] placed at: (%.2f, %.2f) | Angles: [%.1f - %.1f deg]\n", 
               escorts[j].id, escorts[j].type, escorts[j].x, escorts[j].y, 
               escorts[j].min_angle, escorts[j].max_angle);
    }

    // stating the simulation
    run_simulation_1(&player_battleship, escorts, num_escorts, path, k_points, canvas_size);

    return 0;
}
