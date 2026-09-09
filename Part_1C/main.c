#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/physics.h"
#include "simulation.h"
#include "files.h"

int main() {
    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("        NAVAL SIMULATION - PART 1-C     \n");
    printf("========================================\n");
    printf("1. Static Battlefield (Part 1-A + Cumulative Damage)\n");
    printf("2. Moving Battleship (Part 1-B Sim 1 + Cumulative Damage)\n");
    printf("3. Jammed Gun Mode   (Part 1-B Sim 2 + Cumulative Damage)\n");
    printf("Enter simulation choice (1-3): ");
    
    int sim_choice;
    if (scanf("%d", &sim_choice) != 1 || sim_choice < 1 || sim_choice > 3) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // 🗺️ පියවර 1: Canvas මානයන් ලබාගැනීම
    double canvas_w, canvas_h;
    printf("\n--- Canvas Configuration ---\n");
    printf("Enter Canvas Width and Height (e.g. 1000 1000): ");
    scanf("%lf %lf", &canvas_w, &canvas_h);

    // 🚢 පියවර 2: Battleship තෝරාගැනීම
    printf("\n========== SELECT BATTLESHIP ==========\n");
    printf("1. Notation: 'U' | Name: USS Iowa    | Gun: Mark 7\n");
    printf("2. Notation: 'M' | Name: HMS Monarch | Gun: BL 15\n");
    printf("3. Notation: 'R' | Name: Richelieu   | Gun: Model 35\n");
    printf("4. Notation: 'S' | Name: Sovetsky    | Gun: B-37\n");
    printf("Select your Battleship (1-4): ");

    int b_choice;
    scanf("%d", &b_choice);

    Battleship b;
    b.is_destroyed = 1;

    switch (b_choice) {
        case 1: b.notation = 'U'; sprintf(b.name, "USS Iowa"); sprintf(b.gun_name, "Mark 7"); break;
        case 2: b.notation = 'M'; sprintf(b.name, "HMS Monarch"); sprintf(b.gun_name, "BL 15"); break;
        case 3: b.notation = 'R'; sprintf(b.name, "Richelieu"); sprintf(b.gun_name, "Model 35"); break;
        case 4: b.notation = 'S'; sprintf(b.name, "Sovetsky"); sprintf(b.gun_name, "B-37"); break;
        default:
            b.notation = 'M'; sprintf(b.name, "HMS Monarch"); sprintf(b.gun_name, "BL 15"); break;
    }

    // Battleship මුල් පිහිටීම
    printf("\nEnter Initial Battleship Position (X Y): ");
    scanf("%lf %lf", &b.x, &b.y);

    // ⚡ Battleship Shell Velocity (VB_max) තෝරාගැනීම
    printf("\n--- Battleship Velocity Configuration ---\n");
    printf("1. Enter exact velocity manually\n");
    printf("2. Generate randomly within a custom range\n");
    printf("Select option (1-2): ");

    int vel_option;
    scanf("%d", &vel_option);

    if (vel_option == 1) {
        printf("Enter Battleship Shell Max Velocity (m/s): ");
        scanf("%lf", &b.max_velocity);
    } else {
        double min_v, max_v;
        printf("Enter Minimum Velocity (m/s): ");
        scanf("%lf", &min_v);
        printf("Enter Maximum Velocity (m/s): ");
        scanf("%lf", &max_v);

        if (max_v < min_v) {
            double temp = min_v;
            min_v = max_v;
            max_v = temp;
        }

        b.max_velocity = min_v + ((double)rand() / RAND_MAX) * (max_v - min_v);
    }

    printf("\nSelected: %s (Notation: '%c')\n", b.name, b.notation);
    printf("⚡ Shell Max Velocity (VB_max): %.2f m/s\n", b.max_velocity);

    // 🚤 පියවර 3: Escort නැව් සැකසීම
    int n_escorts;
    printf("\nEnter number of escort ships (max %d): ", MAX_ESCORT_SHIPS);
    scanf("%d", &n_escorts);
    if (n_escorts > MAX_ESCORT_SHIPS) n_escorts = MAX_ESCORT_SHIPS;

    EscortShip escorts[MAX_ESCORT_SHIPS];
    for (int i = 0; i < n_escorts; i++) {
        escorts[i].id = i + 1;
        escorts[i].x = ((double)rand() / RAND_MAX) * canvas_w;
        escorts[i].y = ((double)rand() / RAND_MAX) * canvas_h;
        escorts[i].is_destroyed = 1;
        escorts[i].min_angle = 0.0;
        escorts[i].max_angle = 90.0;

        if (i == 0) {
            sprintf(escorts[i].type, "EA");
            sprintf(escorts[i].type_name, "Escort A");
            escorts[i].max_velocity = 1.2 * b.max_velocity;
            escorts[i].impact_power = 0.08;
        } else {
            int t = rand() % 4;
            if (t == 0) { 
                sprintf(escorts[i].type, "EA"); sprintf(escorts[i].type_name, "Escort A"); 
                escorts[i].impact_power = 0.08; 
            } else if (t == 1) { 
                sprintf(escorts[i].type, "EB"); sprintf(escorts[i].type_name, "Escort B"); 
                escorts[i].impact_power = 0.05; 
            } else if (t == 2) { 
                sprintf(escorts[i].type, "EC"); sprintf(escorts[i].type_name, "Escort C"); 
                escorts[i].impact_power = 0.07; 
            } else { 
                sprintf(escorts[i].type, "ED"); sprintf(escorts[i].type_name, "Escort D"); 
                escorts[i].impact_power = 0.04; 
            }
            escorts[i].max_velocity = ((double)rand() / RAND_MAX) * b.max_velocity;
        }
        sprintf(escorts[i].gun_name, "Standard");
    }

    // 🎯 පියවර 4: Simulation ක්‍රියාත්මක කිරීම සහ අනුක්‍රමික Logs සුරැකීම
    if (sim_choice == 1) {
        FILE *log = open_simulation_log("logs/Part_1A", "A");
        run_part1a_c(&b, escorts, n_escorts, log);
        close_simulation_log(log);
    } else {
        int steps;
        printf("\nEnter number of path steps (e.g. 5 - 20): ");
        scanf("%d", &steps);
        if (steps <= 0) steps = 1;
        if (steps > MAX_PATH_POINTS) steps = MAX_PATH_POINTS;

        Point path[MAX_PATH_POINTS];
        for (int s = 0; s < steps; s++) {
            path[s].x = b.x + s * 10.0;
            path[s].y = b.y + s * 5.0;
        }

        if (sim_choice == 2) {
            FILE *log = open_simulation_log("logs/Part_1B_Sim1", "B1");
            run_part1b_sim1_c(&b, escorts, n_escorts, path, steps, log);
            close_simulation_log(log);
        } else if (sim_choice == 3) {
            int jam_step = (rand() % steps) + 1;
            printf("🎲 Random Jamming scheduled at Step: %d (out of %d)\n", jam_step, steps);

            FILE *log = open_simulation_log("logs/Part_1B_Sim2", "B2");
            run_part1b_sim2_c(&b, escorts, n_escorts, path, steps, jam_step, log);
            close_simulation_log(log);
        }
    }

    return 0;
}
