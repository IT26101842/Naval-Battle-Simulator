#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/home/zv/Desktop/Naval-Battle-Simulator/Part_1A/details.h"
#include "simulation.h"

int main() {
    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("   NAVAL BATTLE SIMULATOR - PART 2B     \n");
    printf("========================================\n");
    printf("1. Static Battlefield (Part 1-A + Continuous Fire)\n");
    printf("2. Moving Battleship (Part 1-B Sim 1 + Continuous Fire)\n");
    printf("3. Jammed Gun Mode   (Part 1-B Sim 2 + Continuous Fire)\n");
    printf("Enter simulation choice (1-3): ");
    
    int sim_choice;
    if (scanf("%d", &sim_choice) != 1 || sim_choice < 1 || sim_choice > 3) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // ⏱️ Battleship Reload Time (TB_q)
    double tb_q;
    printf("\nEnter Battleship Reload Time (TB_q in seconds): ");
    if (scanf("%lf", &tb_q) != 1 || tb_q <= 0.0) {
        tb_q = 5.0;
    }

    // ⏱️ Escort Ships Reload Times (TE_p) 
    double te_p[4];
    printf("\n--- Escort Reload Time (TE_p) Configuration ---\n");
    printf("1. Enter manually for all 4 types (EA, EB, EC, ED)\n");
    printf("2. Generate randomly within a custom range\n");
    printf("Select option (1-2): ");
    int te_option;
    if (scanf("%d", &te_option) != 1) te_option = 2;

    if (te_option == 1) {
        printf("Enter TE for EA (seconds): "); scanf("%lf", &te_p[0]);
        printf("Enter TE for EB (seconds): "); scanf("%lf", &te_p[1]);
        printf("Enter TE for EC (seconds): "); scanf("%lf", &te_p[2]);
        printf("Enter TE for ED (seconds): "); scanf("%lf", &te_p[3]);
    } else {
        double min_t, max_t;
        printf("Enter Minimum TE (seconds): "); scanf("%lf", &min_t);
        printf("Enter Maximum TE (seconds): "); scanf("%lf", &max_t);
        if (max_t < min_t) { double tmp = min_t; min_t = max_t; max_t = tmp; }

        for (int i = 0; i < 4; i++) {
            te_p[i] = min_t + ((double)rand() / RAND_MAX) * (max_t - min_t);
        }
    }
    printf("Configured TE_p -> EA: %.2fs | EB: %.2fs | EC: %.2fs | ED: %.2fs\n",
           te_p[0], te_p[1], te_p[2], te_p[3]);

    // Canvas Configuration
    double canvas_w, canvas_h;
    printf("\nEnter Canvas Width and Height (e.g. 1000 1000): ");
    if (scanf("%lf %lf", &canvas_w, &canvas_h) != 2) {
        canvas_w = 1000.0; canvas_h = 1000.0;
    }

    // Battleship Selection
    printf("\n========== SELECT BATTLESHIP ==========\n");
    printf("1. USS Iowa (Gun: Mark 7)\n");
    printf("2. HMS Monarch (Gun: BL 15)\n");
    printf("3. Richelieu (Gun: Model 35)\n");
    printf("4. Sovetsky (Gun: B-37)\n");
    printf("Select your Battleship (1-4): ");
    int b_choice;
    scanf("%d", &b_choice);

    Battleship b;
    b.is_destroyed = 1;
    switch (b_choice) {
        case 1: b.notation = 'U'; snprintf(b.name, sizeof(b.name), "USS Iowa"); break;
        case 2: b.notation = 'M'; snprintf(b.name, sizeof(b.name), "HMS Monarch"); break;
        case 3: b.notation = 'R'; snprintf(b.name, sizeof(b.name), "Richelieu"); break;
        case 4: b.notation = 'S'; snprintf(b.name, sizeof(b.name), "Sovetsky"); break;
        default: b.notation = 'M'; snprintf(b.name, sizeof(b.name), "HMS Monarch"); break;
    }

    printf("\nEnter Initial Battleship Position (X Y): ");
    scanf("%lf %lf", &b.x, &b.y);

    // 💥 Battleship Shell Max Velocity Configuration 
    printf("\n--- Battleship Shell Max Velocity Configuration ---\n");
    printf("1. Enter manually\n");
    printf("2. Generate randomly within a custom range\n");
    printf("Select option (1-2): ");
    int vel_option;
    if (scanf("%d", &vel_option) != 1) vel_option = 1;

    if (vel_option == 1) {
        printf("Enter Battleship Shell Max Velocity (m/s): ");
        scanf("%lf", &b.max_velocity);
    } else {
        double min_v, max_v;
        printf("Enter Minimum Shell Velocity (m/s): ");
        scanf("%lf", &min_v);
        printf("Enter Maximum Shell Velocity (m/s): ");
        scanf("%lf", &max_v);
        if (max_v < min_v) { double tmp = min_v; min_v = max_v; max_v = tmp; }

        b.max_velocity = min_v + ((double)rand() / RAND_MAX) * (max_v - min_v);
        printf("🎲 Generated Battleship Shell Max Velocity: %.2f m/s\n", b.max_velocity);
    }

    // making escortships acoording for the types
    int n_escorts;
    printf("\nEnter number of escort ships (max %d): ", MAX_ESCORT_SHIPS);
    scanf("%d", &n_escorts);
    if (n_escorts > MAX_ESCORT_SHIPS) n_escorts = MAX_ESCORT_SHIPS;
    if (n_escorts < 1) n_escorts = 1;

    const char *types[4] = {"EA", "EB", "EC", "ED"};
    double angle_ranges[4] = {20.0, 30.0, 25.0, 50.0};
    double impact_powers[4] = {0.08, 0.06, 0.07, 0.05};

    EscortShip escorts[MAX_ESCORT_SHIPS];
    for (int i = 0; i < n_escorts; i++) {
        escorts[i].id = i + 1;
        escorts[i].x = ((double)rand() / RAND_MAX) * canvas_w;
        escorts[i].y = ((double)rand() / RAND_MAX) * canvas_h;
        escorts[i].is_destroyed = 1;

        int t = rand() % 4;
        snprintf(escorts[i].type, sizeof(escorts[i].type), "%s", types[t]);
        escorts[i].impact_power = impact_powers[t];

        double range = angle_ranges[t];
        escorts[i].min_angle = ((double)rand() / RAND_MAX) * (90.0 - range);
        escorts[i].max_angle = escorts[i].min_angle + range;

        if (t == 0) {
            escorts[i].max_velocity = 1.2 * b.max_velocity;
        } else {
            double factor = 0.50 + (((double)rand() / RAND_MAX) * 0.45);
            escorts[i].max_velocity = b.max_velocity * factor;
        }
        escorts[i].min_velocity = escorts[i].max_velocity * (0.20 + (((double)rand() / RAND_MAX) * 0.30));
    }

    // starting simulations
    if (sim_choice == 1) {
        run_part2b_static_simulation(b, escorts, n_escorts, tb_q, te_p);
    } else {
        int steps;
        printf("\nEnter number of path steps: ");
        scanf("%d", &steps);
        if (steps <= 0) steps = 1;
        if (steps > MAX_PATH_POINTS) steps = MAX_PATH_POINTS;

        Point path[MAX_PATH_POINTS];
        for (int s = 0; s < steps; s++) {
            path[s].x = b.x + s * 10.0;
            path[s].y = b.y + s * 5.0;
        }

        if (sim_choice == 2) {
            run_part2b_moving_simulation(b, escorts, n_escorts, path, steps, tb_q, te_p);
        } else if (sim_choice == 3) {
            int jam_step = (rand() % steps) + 1;
            double jam_min_angle = 15.0 + ((double)rand() / RAND_MAX) * 20.0;

            printf("\n🎲 Jamming scheduled at Step: %d (out of %d)\n", jam_step, steps);
            printf("⚠️ Gun jammed min angle restricted to: %.2f deg\n", jam_min_angle);

            run_part2b_jammed_simulation(b, escorts, n_escorts, path, steps, jam_step, jam_min_angle, tb_q, te_p);
        }
    }

    return 0;
}
