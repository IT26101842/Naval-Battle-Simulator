#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "details.h"
#include "simulation.h"

double rand_double(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

// Function configure battleship
void initialize_battlefield_from_user(Battlefield *bf) {
    printf("=========================================\n");
    printf("        BATTLEFIELD CONFIGURATION        \n");
    printf("=========================================\n");

    // getting Canvas Size (D)
    printf("Enter Canvas Size D (0 to D) [Default: 1000]: ");
    if (scanf("%lf", &bf->canvas_size) != 1 || bf->canvas_size <= 0) {
        bf->canvas_size = 1000.0;
    }

    // selecting the battleship
    printf("\n--- SELECT YOUR BATTLESHIP ---\n");
    printf("  [U] USS Iowa (BB-61)      | Gun: 50-caliber Mark 7 (Vmax: 250 m/s)\n");
    printf("  [M] MS King George V      | Gun: 356 mm Mark VII   (Vmax: 230 m/s)\n");
    printf("  [R] Richelieu             | Gun: 15 inch Mle 1935  (Vmax: 240 m/s)\n");
    printf("  [S] Sovetsky Soyuz-class  | Gun: 16 inch B-37      (Vmax: 260 m/s)\n");
    
    char choice;
    printf("Choose Battleship (U/M/R/S) [Default: U]: ");
    scanf(" %c", &choice);

    switch (choice) {
        case 'M': case 'm':
            snprintf(bf->battleship.name, sizeof(bf->battleship.name), "MS King George V");
            bf->battleship.notation = 'M';
            bf->battleship.max_velocity = 230.0;
            break;
        case 'R': case 'r':
            snprintf(bf->battleship.name, sizeof(bf->battleship.name), "Richelieu");
            bf->battleship.notation = 'R';
            bf->battleship.max_velocity = 240.0;
            break;
        case 'S': case 's':
            snprintf(bf->battleship.name, sizeof(bf->battleship.name), "Sovetsky Soyuz-class");
            bf->battleship.notation = 'S';
            bf->battleship.max_velocity = 260.0;
            break;
        case 'U': case 'u':
        default:
            snprintf(bf->battleship.name, sizeof(bf->battleship.name), "USS Iowa (BB-61)");
            bf->battleship.notation = 'U';
            bf->battleship.max_velocity = 250.0;
            break;
    }

    // getting escortship number
    do {
        printf("\nEnter Number of Escort Ships (1 to 50): ");
        scanf("%d", &bf->num_escorts);
        if (bf->num_escorts < 1 || bf->num_escorts > 50) {
            printf("⚠️ Invalid input! Please enter a value between 1 and 50.\n");
        }
    } while (bf->num_escorts < 1 || bf->num_escorts > 50);

    // getting battleship coordinates
    printf("\nEnter Battleship X coordinate (0 to %.1f): ", bf->canvas_size);
    scanf("%lf", &bf->battleship.x);
    printf("Enter Battleship Y coordinate (0 to %.1f): ", bf->canvas_size);
    scanf("%lf", &bf->battleship.y);

    bf->battleship.is_destroyed = 0;

    // giving randomly selected details to escortships
    const char *types[] = {"EA", "EB", "EC", "ED", "EE"};
    double impact_powers[] = {0.08, 0.06, 0.07, 0.05, 0.04};
    double angle_ranges[] = {20.0, 30.0, 25.0, 50.0, 70.0};

    for (int i = 0; i < bf->num_escorts; i++) {
        bf->escorts[i].id = i + 1;
        int t = rand() % 5;
        snprintf(bf->escorts[i].type, sizeof(bf->escorts[i].type), "%s", types[t]);
        bf->escorts[i].impact_power = impact_powers[t];

        bf->escorts[i].x = rand_double(0, bf->canvas_size);
        bf->escorts[i].y = rand_double(0, bf->canvas_size);

        bf->escorts[i].min_angle = rand_double(5.0, 30.0);
        bf->escorts[i].max_angle = bf->escorts[i].min_angle + angle_ranges[t];

        bf->escorts[i].min_velocity = rand_double(20.0, 50.0);
        if (t == 0) {
            bf->escorts[i].max_velocity = 1.2 * bf->battleship.max_velocity;
        } else {
            bf->escorts[i].max_velocity = rand_double(bf->escorts[i].min_velocity, bf->battleship.max_velocity - 10.0);
        }

        bf->escorts[i].is_destroyed = 0;
    }
}

int main() {
    srand((unsigned int)time(NULL));

    Battlefield bf;
    initialize_battlefield_from_user(&bf);

    run_part1_A(&bf);

    return 0;
}
