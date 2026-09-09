#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "files.h"

// opening the log folders and log files
FILE* open_simulation_log(const char *subfolder, const char *prefix) {
    mkdir("logs", 0777);

    char full_folder[512];
    snprintf(full_folder, sizeof(full_folder), "logs/%s", subfolder);

    char temp_path[512];
    snprintf(temp_path, sizeof(temp_path), "%s", full_folder);
    for (char *p = temp_path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(temp_path, 0777);
            *p = '/';
        }
    }
    mkdir(full_folder, 0777);

    char filename[1024];
    FILE *fp = NULL;
    int serial = 1;

    while (1) {
        snprintf(filename, sizeof(filename), "%s/%s_%04d.log", full_folder, prefix, serial);
        fp = fopen(filename, "r");
        if (fp == NULL) break;
        fclose(fp);
        serial++;
    }

    fp = fopen(filename, "w");
    if (fp != NULL) {
        printf("\n📄 [LOG CREATED] %s\n", filename);
        fprintf(fp, "=====================================================\n");
        fprintf(fp, "       NAVAL BATTLE SIMULATION LOG ENTRY - PART 2A   \n");
        fprintf(fp, "=====================================================\n\n");
    }
    return fp;
}

// closing the log files
void close_simulation_log(FILE *fp) {
    if (fp != NULL) {
        fprintf(fp, "\n==================== END OF LOG ====================\n");
        fclose(fp);
    }
}

// function for record and write attacking order
void log_attack_order(FILE *log, int step, TargetInfo order[], int count) {
    if (step > 0) {
        printf("\n🎯 --- Step %d: Target Attack Order (In Range: %d) ---\n", step, count);
        if (log) fprintf(log, "\n🎯 --- Step %d: Target Attack Order (In Range: %d) ---\n", step, count);
    } else {
        printf("\n🎯 --- Target Attack Order (In Range: %d) ---\n", count);
        if (log) fprintf(log, "\n🎯 --- Target Attack Order (In Range: %d) ---\n", count);
    }

    if (count == 0) {
        printf("   [No targets within range!]\n");
        if (log) fprintf(log, "   [No targets within range!]\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        printf("   Priority %2d -> Escort #%d | Impact Power: %.2f | Distance: %.2f m\n",
               i + 1, order[i].id, order[i].impact_power, order[i].distance);
        if (log) {
            fprintf(log, "   Priority %2d -> Escort #%d | Impact Power: %.2f | Distance: %.2f m\n",
                    i + 1, order[i].id, order[i].impact_power, order[i].distance);
        }
    }
}

// function for write and record for shot events
void log_shot_event(FILE *log, double time, const char *shooter, const char *target, 
                    double distance, double angle, double velocity, int is_hit, double damage) {
    printf("[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg) -> %s (Dmg: %.2f)\n",
           time, shooter, target, distance, angle, is_hit ? "HIT 💥" : "MISSED 💨", damage);

    if (log) {
        fprintf(log, "[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg, Vel: %.1fm/s) -> %s (Dmg: %.2f)\n",
                time, shooter, target, distance, angle, velocity, is_hit ? "HIT" : "MISSED", damage);
    }
}

void log_simulation_summary(FILE *log, int b_survived, double cumulative_damage, 
                            int destroyed_escorts, int total_escorts, double total_time) {
    // calculating the remaning health
    double remaining_health = 0.0;
    if (cumulative_damage < 1.0) {
        remaining_health = (1.0 - cumulative_damage) * 100.0;
    }

    printf("\n==================== SIMULATION SUMMARY ====================\n");
    printf(" ⏱️ Total Battle Time   : %.2f seconds\n", total_time);
    printf(" 🛡️ Battleship Status    : %s\n", b_survived ? "SURVIVED 🏆" : "DESTROYED / SUNK ☠️");
    printf(" 💥 Cumulative Damage    : %.2f%% (%.4f)\n", cumulative_damage * 100.0, cumulative_damage);
    printf(" ❤️ Remaining Health     : %.2f%%\n", remaining_health);
    printf(" 🎯 Escorts Destroyed    : %d / %d\n", destroyed_escorts, total_escorts);
    printf("============================================================\n");

    if (log) {
        fprintf(log, "\n==================== SIMULATION SUMMARY ====================\n");
        fprintf(log, " Total Battle Time   : %.2f seconds\n", total_time);
        fprintf(log, " Battleship Status   : %s\n", b_survived ? "SURVIVED" : "DESTROYED / SUNK");
        fprintf(log, " Cumulative Damage   : %.2f%% (%.4f)\n", cumulative_damage * 100.0, cumulative_damage);
        fprintf(log, " Remaining Health    : %.2f%%\n", remaining_health);
        fprintf(log, " Escorts Destroyed   : %d / %d\n", destroyed_escorts, total_escorts);
        fprintf(log, "============================================================\n");
    }
}
