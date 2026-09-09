#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "files.h"

// function for create directory
static void create_dir_if_not_exists(const char *path) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    mkdir(tmp, 0777);
}

// opening log files
FILE* open_simulation_log(const char *subfolder, const char *prefix) {
    char dir_path[256];
    snprintf(dir_path, sizeof(dir_path), "logs/%s", subfolder);
    create_dir_if_not_exists(dir_path);

    char file_path[512];
    int run_id = 1;
    FILE *fp = NULL;

    while (1) {
        snprintf(file_path, sizeof(file_path), "%s/%s_%04d.log", dir_path, prefix, run_id);
        FILE *check = fopen(file_path, "r");
        if (!check) {
            break;
        }
        fclose(check);
        run_id++;
    }

    fp = fopen(file_path, "w");
    if (fp) {
        printf("\n📄 [LOG CREATED] %s\n", file_path);
    } else {
        printf("\n❌ Error creating log file: %s\n", file_path);
    }
    return fp;
}

// function for record and write attacking logs
void log_attack_order(FILE *fp, int step, TargetInfo order[], int count) {
    if (!fp) return;
    if (step > 0) {
        fprintf(fp, "\n--- Step %d: Target Attack Order (In Range: %d) ---\n", step, count);
        printf("\n--- Step %d: Target Attack Order (In Range: %d) ---\n", step, count);
    } else {
        fprintf(fp, "\n--- Target Attack Order (In Range: %d) ---\n", count);
        printf("\n--- Target Attack Order (In Range: %d) ---\n", count);
    }

    if (count == 0) {
        fprintf(fp, "[No targets within range]\n");
        printf("[No targets within range]\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "Priority %d -> Escort #%d | Impact Power: %.4f | Distance: %.2f m\n",
                i + 1, order[i].id, order[i].impact_power, order[i].distance);
        printf("Priority %d -> Escort #%d | Impact Power: %.4f | Distance: %.2f m\n",
               i + 1, order[i].id, order[i].impact_power, order[i].distance);
    }
}

// function for record and write the shots details
void log_shot_event(FILE *fp, double time, const char *shooter, const char *target, 
                    double distance, double angle, double velocity, int hit, 
                    double impact_factor, double damage) {
    if (hit) {
        printf("[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg) -> HIT 💥 (IP: %.4f | Dmg: %.2f)\n",
               time, shooter, target, distance, angle, impact_factor, damage);
        if (fp) {
            fprintf(fp, "[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg) -> HIT (IP: %.4f | Dmg: %.2f)\n",
                    time, shooter, target, distance, angle, impact_factor, damage);
        }
    } else {
        printf("[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg) -> MISSED 💨\n",
               time, shooter, target, distance, angle);
        if (fp) {
            fprintf(fp, "[t=%.2fs] %s fires at %s (Dist: %.1fm, Angle: %.2f deg) -> MISSED\n",
                    time, shooter, target, distance, angle);
        }
    }
}

//function for write and record the battle summary
void log_simulation_summary(FILE *fp, int b_survived, double cumulative_damage, 
                            int destroyed_count, int total_escorts, double total_time) {
    printf("\n================ SIMULATION SUMMARY ================\n");
    printf("⏱️ Total Battle Time : %.2f seconds\n", total_time);
    printf("🚢 Battleship Status : %s\n", b_survived ? "SURVIVED 🏆" : "SUNK ☠️");
    printf("💥 Cumulative Damage: %.2f%% (%.4f)\n", cumulative_damage * 100.0, cumulative_damage);
    printf("❤️ Remaining Health  : %.2f%%\n", (1.0 - cumulative_damage > 0 ? (1.0 - cumulative_damage) * 100.0 : 0.0));
    printf("🚤 Escorts Destroyed : %d / %d\n", destroyed_count, total_escorts);
    printf("====================================================\n");

    if (fp) {
        fprintf(fp, "\n================ SIMULATION SUMMARY ================\n");
        fprintf(fp, "Total Battle Time : %.2f seconds\n", total_time);
        fprintf(fp, "Battleship Status : %s\n", b_survived ? "SURVIVED" : "SUNK");
        fprintf(fp, "Cumulative Damage: %.2f%% (%.4f)\n", cumulative_damage * 100.0, cumulative_damage);
        fprintf(fp, "Remaining Health  : %.2f%%\n", (1.0 - cumulative_damage > 0 ? (1.0 - cumulative_damage) * 100.0 : 0.0));
        fprintf(fp, "Escorts Destroyed : %d / %d\n", destroyed_count, total_escorts);
        fprintf(fp, "====================================================\n");
    }
}

// function for write and record the damage escort report
void log_damaged_escorts_report(const char *sim_type, EscortShip escorts[], 
                                double escort_damage_received[], int n_escorts) {
    FILE *fp = open_simulation_log("Part_2C/Damaged_Escorts", sim_type);
    if (!fp) return;

    fprintf(fp, "================ DAMAGED ESCORT SHIPS REPORT ================\n");
    fprintf(fp, "Simulation Type: %s\n", sim_type);
    fprintf(fp, "Criteria: Ships that took damage but are NOT destroyed.\n\n");

    int count = 0;
    for (int i = 0; i < n_escorts; i++) {
        if (escorts[i].is_destroyed == 1 && escort_damage_received[i] > 0.0) {
            count++;
            double remaining_health = 1.0 - escort_damage_received[i];
            if (remaining_health < 0.0) remaining_health = 0.0;

            fprintf(fp, "Escort ID #%d (%s) -> Damage Taken: %.4f (%.2f%%) | Remaining Health: %.4f (%.2f%%)\n",
                    escorts[i].id, escorts[i].type,
                    escort_damage_received[i], escort_damage_received[i] * 100.0,
                    remaining_health, remaining_health * 100.0);
        }
    }

    if (count == 0) {
        fprintf(fp, "No surviving escort ships with damage (all either untouched or fully destroyed).\n");
    } else {
        fprintf(fp, "\nTotal Damaged Surviving Escorts: %d\n", count);
    }

    fprintf(fp, "=============================================================\n");
    close_simulation_log(fp);
}

void close_simulation_log(FILE *fp) {
    if (fp) {
        fclose(fp);
    }
}
