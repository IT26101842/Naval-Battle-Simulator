#include <stdio.h>
#include "files.h"

// Function for record and write a file about initial status about the battle field
void save_initial_conditions(const char *filename, Battlefield *bf) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file %s for writing!\n", filename);
        return;
    }

    fprintf(fp, "=== INITIAL BATTLEFIELD CONDITIONS ===\n");
    fprintf(fp, "Canvas Size: %.2f x %.2f\n", bf->canvas_size, bf->canvas_size);
    fprintf(fp, "Battleship: %s (%c) | Pos: (%.2f, %.2f) | Vmax: %.2f\n\n",
            bf->battleship.name, bf->battleship.notation,
            bf->battleship.x, bf->battleship.y, bf->battleship.max_velocity);

    fprintf(fp, "Escort Ships Count: %d\n", bf->num_escorts);
    for (int i = 0; i < bf->num_escorts; i++) {
        EscortShip e = bf->escorts[i];
        fprintf(fp, "ID: %d | Type: %s | Pos: (%.2f, %.2f) | V: [%.1f - %.1f] | Angles: [%.1f - %.1f] | Impact: %.2f\n",
                e.id, e.type, e.x, e.y, e.min_velocity, e.max_velocity, e.min_angle, e.max_angle, e.impact_power);
    }
    fclose(fp);
}

// Function for record and write the final output of the battle
void save_final_conditions(const char *filename, Battlefield *bf) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp, "=== FINAL BATTLEFIELD CONDITIONS ===\n");
    fprintf(fp, "Battleship: %s | Status: %s\n\n",
            bf->battleship.name, bf->battleship.is_destroyed ? "DESTROYED" : "SURVIVED");

    fprintf(fp, "Escort Ships Status:\n");
    for (int i = 0; i < bf->num_escorts; i++) {
        EscortShip e = bf->escorts[i];
        fprintf(fp, "ID: %d | Type: %s | Status: %s\n",
                e.id, e.type, e.is_destroyed ? "DESTROYED" : "ACTIVE");
    }
    fclose(fp);
}
