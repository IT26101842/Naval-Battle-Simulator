#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "simulation.h"

int main() {
    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("   NAVAL BATTLE SIMULATOR - PART 2A     \n");
    printf("========================================\n");
    printf("1. Static Battlefield (Part 1-A + Greedy Attack + Return Fire)\n");
    printf("2. Moving Battleship (Part 1-B Sim 1 + Greedy Attack + Return Fire)\n");
    printf("3. Jammed Gun Mode   (Part 1-B Sim 2 + Greedy Attack + Return Fire)\n");
    printf("Enter simulation choice (1-3): ");
    
    int sim_choice;
    if (scanf("%d", &sim_choice) != 1 || sim_choice < 1 || sim_choice > 3) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // getting the reload time
    double tb_q;
    printf("\nEnter Battleship Reload Time (TB_q in seconds): ");
    if (scanf("%lf", &tb_q) != 1 || tb_q <= 0.0) {
        tb_q = 5.0; // Default reload time
    }

    // getting dimensions of the canvas
    double canvas_w, canvas_h;
    printf("\n--- Canvas Configuration ---\n");
    printf("Enter Canvas Width and Height (e.g. 1000 1000): ");
    if (scanf("%lf %lf", &canvas_w, &canvas_h) != 2) {
        canvas_w = 1000.0;
        canvas_h = 1000.0;
    }

    // selecting the battleship
    printf("\n========== SELECT BATTLESHIP ==========\n");
    printf("1. Notation: 'U' | Name: USS Iowa    | Gun: Mark 7\n");
    printf("2. Notation: 'M' | Name: HMS Monarch | Gun: BL 15\n");
    printf("3. Notation: 'R' | Name: Richelieu   | Gun: Model 35\n");
    printf("4. Notation: 'S' | Name: Sovetsky    | Gun: B-37\n");
    printf("Select your Battleship (1-4): ");

    int b_choice;
    scanf("%d", &b_choice);

    Battleship b;
    b.is_destroyed = 1; // 1 = Active

    switch (b_choice) {
        case 1: b.notation = 'U'; snprintf(b.name, sizeof(b.name), "USS Iowa"); snprintf(b.gun_name, sizeof(b.gun_name), "Mark 7"); break;
        case 2: b.notation = 'M'; snprintf(b.name, sizeof(b.name), "HMS Monarch"); snprintf(b.gun_name, sizeof(b.gun_name), "BL 15"); break;
        case 3: b.notation = 'R'; snprintf(b.name, sizeof(b.name), "Richelieu"); snprintf(b.gun_name, sizeof(b.gun_name), "Model 35"); break;
        case 4: b.notation = 'S'; snprintf(b.name, sizeof(b.name), "Sovetsky"); snprintf(b.gun_name, sizeof(b.gun_name), "B-37"); break;
        default:
            b.notation = 'M'; snprintf(b.name, sizeof(b.name), "HMS Monarch"); snprintf(b.gun_name, sizeof(b.gun_name), "BL 15"); break;
    }

    // battleship coordinates
    printf("\nEnter Initial Battleship Position (X Y): ");
    scanf("%lf %lf", &b.x, &b.y);

    // Battleship Shell Velocity (VB_max) Configuration
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

    // escortship configaration
    int n_escorts;
    printf("\nEnter number of escort ships (max %d): ", MAX_ESCORT_SHIPS);
    scanf("%d", &n_escorts);
    if (n_escorts > MAX_ESCORT_SHIPS) n_escorts = MAX_ESCORT_SHIPS;
    if (n_escorts < 1) n_escorts = 1;

    // defining variables of the escortships
    const char *types[5] = {"EA", "EB", "EC", "ED", "EE"};
    const char *type_names[5] = {"Escort A", "Escort B", "Escort C", "Escort D", "Escort E"};
    double angle_ranges[5] = {20.0, 30.0, 25.0, 50.0, 70.0};
    double impact_powers[5] = {0.08, 0.06, 0.07, 0.05, 0.04};

    EscortShip escorts[MAX_ESCORT_SHIPS];
    for (int i = 0; i < n_escorts; i++) {
        escorts[i].id = i + 1;
        escorts[i].x = ((double)rand() / RAND_MAX) * canvas_w;
        escorts[i].y = ((double)rand() / RAND_MAX) * canvas_h;
        escorts[i].is_destroyed = 1; // 1 = Active

        int t = rand() % 5;
        snprintf(escorts[i].type, sizeof(escorts[i].type), "%s", types[t]);
        snprintf(escorts[i].type_name, sizeof(escorts[i].type_name), "%s", type_names[t]);
        escorts[i].impact_power = impact_powers[t];

        // making the angles
        double range = angle_ranges[t];
        escorts[i].min_angle = ((double)rand() / RAND_MAX) * (90.0 - range);
        escorts[i].max_angle = escorts[i].min_angle + range;

        // preparing the velocities
        if (t == 0) {
            escorts[i].max_velocity = 1.2 * b.max_velocity;
        } else {
            // EB, EC, ED, EE velocities
            double factor = 0.50 + (((double)rand() / RAND_MAX) * 0.45); 
            escorts[i].max_velocity = b.max_velocity * factor;
        }

        // min velocity
        escorts[i].min_velocity = escorts[i].max_velocity * (0.20 + (((double)rand() / RAND_MAX) * 0.30));

        snprintf(escorts[i].gun_name, sizeof(escorts[i].gun_name), "Standard");
    }

    // starting the simulation
    if (sim_choice == 1) {
        run_part2a_static_simulation(b, escorts, n_escorts, tb_q);
    } else {
        int steps;
        printf("\nEnter number of path steps (e.g. 5 - 20): ");
        scanf("%d", &steps);
        if (steps <= 0) steps = 1;
        if (steps > MAX_PATH_POINTS) steps = MAX_PATH_POINTS;

        // getting the paths
        Point path[MAX_PATH_POINTS];
        for (int s = 0; s < steps; s++) {
            path[s].x = b.x + s * 10.0;
            path[s].y = b.y + s * 5.0;
        }

        if (sim_choice == 2) {
            run_part2a_moving_simulation(b, escorts, n_escorts, path, steps, tb_q);
        } else if (sim_choice == 3) {
            int jam_step = (rand() % steps) + 1;
            double jam_min_angle = 15.0 + ((double)rand() / RAND_MAX) * 20.0; // Random Jam angle

            printf("\n🎲 Random Jamming scheduled at Step: %d (out of %d)\n", jam_step, steps);
            printf("⚠️ Gun jammed min angle restricted to: %.2f deg\n", jam_min_angle);

            run_part2a_jammed_simulation(b, escorts, n_escorts, path, steps, jam_step, jam_min_angle, tb_q);
        }
    }

    return 0;
}
