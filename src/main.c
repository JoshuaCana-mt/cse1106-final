
#include <stdio.h>
#include <pigpio.h>

#define PIN 26  // GPIO pin for LED

int main()
{
    int choice, count = 0;
    float temp, total = 0;
    float min = 68, max = 76;  // Default temperature range
    float newMin, newMax;
    FILE *file;

    // Initialize GPIO
    if (gpioInitialise() < 0) {
        printf("GPIO initialization failed.\n");
        return 1;
    }

    gpioSetMode(PIN, PI_OUTPUT);
    gpioWrite(PIN, 0);  // LED starts OFF

    // Main program loop
    while (1) {
        printf("\n--- Classroom Monitor ---\n");
        printf("1. Enter temperature\n");
        printf("2. Show average\n");
        printf("3. Change temperature range\n");
        printf("4. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
            break;

        // Option 1: Enter and check temperature
        if (choice == 1) {
            printf("Enter temperature (F): ");

            if (scanf("%f", &temp) != 1)
                break;

            total += temp;
            count++;

            // Display results in three columns
            printf("\nTemperature\tSystem Status\tWarning LED\n");

            // Open CSV file to save readings
            file = fopen("data/temperature_log.csv", "a");

            if (temp >= min && temp <= max) {
                gpioWrite(PIN, 1);  // Turn LED OFF

                printf("%.1f F\t\tNORMAL\t\tOFF\n", temp);

                if (file != NULL)
                    fprintf(file, "%.1f,NORMAL\n", temp);
            }
            else {
                gpioWrite(PIN, 0);  // Turn LED ON

                printf("%.1f F\t\tWARNING\t\tON\n", temp);

                if (file != NULL)
                    fprintf(file, "%.1f,WARNING\n", temp);
            }

            if (file != NULL)
                fclose(file);
            else
                printf("Error: Cannot save temperature.\n");

            // Save the latest system status
            file = fopen("status.txt", "w");

            if (file != NULL) {
                if (temp >= min && temp <= max)
                    fprintf(file, "NORMAL\n");
                else
                    fprintf(file, "WARNING\n");

                fclose(file);
            }
        }

        // Option 2: Calculate average temperature
        else if (choice == 2) {
            if (count > 0)
                printf("Average: %.2f F\n", total / count);
            else
                printf("No readings yet.\n");
        }

        // Option 3: Change temperature range
        else if (choice == 3) {
            printf("Enter new minimum: ");

            if (scanf("%f", &newMin) != 1)
                break;

            printf("Enter new maximum: ");

            if (scanf("%f", &newMax) != 1)
                break;

            if (newMin <= newMax) {
                min = newMin;
                max = newMax;

                printf("Range updated: %.1f - %.1f F\n",
                       min, max);
            }
            else {
                printf("Invalid temperature range.\n");
            }
        }

        // Option 4: Exit program
        else if (choice == 4) {
            break;
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    // Turn LED OFF and release GPIO
    gpioWrite(PIN, 0);
    gpioTerminate();

    return 0;
}
