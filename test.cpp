

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>


int main(){

    time_t time1 = 1738607674; // Example timestamp
    time_t time2 = 1738669413; // Example timestamp

    // Get difference in seconds
    double diff_seconds = difftime(time2, time1);

    // If you need other units
    double diff_minutes = diff_seconds / 60.0;
    double diff_hours = diff_seconds / 3600.0;
    int diff_days = diff_seconds / 86400;

    printf("%f seconds\n", diff_seconds);
    printf("%f minutes\n", diff_minutes);
    printf("%f hours\n", diff_hours);
}