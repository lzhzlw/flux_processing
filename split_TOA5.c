/*******************************************************************************************
Split the raw Campbell TOA5 data to 30 min interval.
The code can be used in Windows, Linux and macOS.

Written by: Zhenhai Liu
Date: 2025-02-10
*******************************************************************************************/

// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

// Define constants
#define MAX_LINE 1024
#define PATH_SIZE 256

// Function to read and convert the raw data
void read_convert(const char *path_in, const char *path_out, const char *filename) {
    // Open the input file
    char file_path[PATH_SIZE];
    snprintf(file_path, sizeof(file_path), "%s%s.dat", path_in, filename);

    FILE *f = fopen(file_path, "r");
    if (!f) {
        perror("Error opening input file");
        return;
    }

    // Create the output directory
    char output_dir[PATH_SIZE];
    snprintf(output_dir, sizeof(output_dir), "%s", path_out);
    #ifdef _WIN32
    _mkdir(output_dir);  // Windows
    #else
    mkdir(output_dir, 0777);  // Linux/macOS
    #endif

    // Read the header lines
    char header[4][MAX_LINE];
    int i;
    for (i = 0; i < 4; i++) {
        if (!fgets(header[i], MAX_LINE, f)) {
            fclose(f);
            return;
        }
    }

    // Read and convert the data
    char line[MAX_LINE];
    char current_file[PATH_SIZE] = "";
    time_t current_start_time = 0;
    FILE *out_f = NULL;

    // Read the data line by line
    while (fgets(line, MAX_LINE, f)) {
        char timestamp_str[32];
		sscanf(line, "\"%31[^\"]\",", timestamp_str); // Read the timestamp string

        // Parse the timestamp string
        struct tm timestamp = {0};
        #ifdef _WIN32    
        sscanf(timestamp_str, "%d-%d-%d %d:%d:%d",
               &timestamp.tm_year, &timestamp.tm_mon, &timestamp.tm_mday,
               &timestamp.tm_hour, &timestamp.tm_min, &timestamp.tm_sec);
        timestamp.tm_year -= 1900;
        timestamp.tm_mon -= 1;
        // Check if the timestamp without milliseconds
        if (strlen(timestamp_str) < 20){
        	time_t raw_time = mktime(&timestamp);
            raw_time -= 1; // Shift entire timestamp back by 1 second
            timestamp = *localtime(&raw_time);
		}
        #else
        if (strchr(timestamp_str, '.')) {
            sscanf(timestamp_str, "%d-%d-%d %d:%d:%d.%*d",
                   &timestamp.tm_year, &timestamp.tm_mon, &timestamp.tm_mday,
                   &timestamp.tm_hour, &timestamp.tm_min, &timestamp.tm_sec);
        } else {
            sscanf(timestamp_str, "%d-%d-%d %d:%d:%d",
                   &timestamp.tm_year, &timestamp.tm_mon, &timestamp.tm_mday,
                   &timestamp.tm_hour, &timestamp.tm_min, &timestamp.tm_sec);
            time_t raw_time = mktime(&timestamp);
            raw_time -= 1; // Shift entire timestamp back by 1 second
            timestamp = *localtime(&raw_time);
        }
        timestamp.tm_year -= 1900;
        timestamp.tm_mon -= 1;
        #endif

        // Calculate the start of the 30-minute period
        timestamp.tm_min = (timestamp.tm_min / 30) * 30; // - 2
        timestamp.tm_sec = 0;
        time_t period_start = mktime(&timestamp);

        // Check if the current period is different from the previous period
        if (current_start_time == 0 || period_start != current_start_time) {
            current_start_time = period_start;
            snprintf(current_file, sizeof(current_file), "%s%s_%04d_%02d_%02d_%02d%02d.dat",
                     path_out, filename,
                     timestamp.tm_year + 1900, timestamp.tm_mon + 1, timestamp.tm_mday,
                     timestamp.tm_hour, timestamp.tm_min);

            // Close the previous file and open a new file
            if (out_f) fclose(out_f);
            out_f = fopen(current_file, "w");
            if (!out_f) {
                perror("Error opening output file");
                fclose(f);
                return;
            }
            int i;
            for (i = 0; i < 4; i++) {
                fputs(header[i], out_f);
            }
        }
        fputs(line, out_f);
    }

    // Close the files
    if (out_f) fclose(out_f);
    fclose(f);
}

// Main function
int main() {
    // Define the input and output directories
    const char *path_in = "F:/flux/site_data/KBS/2022T7/underprocess/";
    const char *path_out = "F:/flux/site_data/KBS/2022T7/split_data/";

    // Open the input directory
    DIR *dir = opendir(path_in);
    if (!dir) {
        perror("Error opening directory");
        return 1;
    }

    // Process each file in the directory
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".dat")) {
            char filename[PATH_SIZE];
            strncpy(filename, entry->d_name, strlen(entry->d_name) - 4);
            filename[strlen(entry->d_name) - 4] = '\0';
            printf("Processing %s...\n", filename);
            read_convert(path_in, path_out, filename);
        }
    }
    closedir(dir);

    printf("Processing complete!\n");
    return 0;
}
