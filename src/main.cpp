#include "auxillary.hpp"

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

using namespace std::chrono_literals;

// Time variables
time_t rawtime;
struct tm *timeinfo;

int main(int argc, char **argv)
{
	// chip variables
    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;

    // GPIO Lines
    struct gpiod_line *lineRed;       // Red LED
    struct gpiod_line *lineBlueOne;   // Blue 1 LED
    struct gpiod_line *lineBlueTwo;   // Blue 2 LED
    struct gpiod_line *lineBlueThree; // Blue 3 LED

    struct gpiod_line *lineButton;

    // Open GPIO chip
    chip = gpiod_chip_open_by_name(chipname);

    // Open GPIO lines
    lineRed = gpiod_chip_get_line(chip, 2);
    lineBlueOne = gpiod_chip_get_line(chip, 3);
    lineBlueTwo = gpiod_chip_get_line(chip, 27);
    lineBlueThree = gpiod_chip_get_line(chip, 17);
    lineButton = gpiod_chip_get_line(chip, 18);

    // Open LED lines for output
    if (gpiod_line_request_output(lineRed, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(lineBlueOne, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(lineBlueTwo, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(lineBlueThree, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_input(lineButton, "led_enable_test") == -1)
    {
        return -1;
    }

	// Blink LEDs in a binary pattern
	while (!gpiod_line_get_value(lineButton))
	{
        gpiod_line_set_value(lineRed, 1);
        gpiod_line_set_value(lineBlueOne, 0);
        gpiod_line_set_value(lineBlueTwo, 0);
        gpiod_line_set_value(lineBlueThree, 0);
        std::this_thread::sleep_for(1s);
        gpiod_line_set_value(lineRed, 0);
        gpiod_line_set_value(lineBlueOne, 1);
        gpiod_line_set_value(lineBlueTwo, 1);
        gpiod_line_set_value(lineBlueThree, 1);
        std::this_thread::sleep_for(1s);
    }

    gpiod_line_set_value(lineRed,       1);
    gpiod_line_set_value(lineBlueOne,   1);
    gpiod_line_set_value(lineBlueTwo,   1);
    gpiod_line_set_value(lineBlueThree, 1);

    // Release lines and chip
    gpiod_line_release(lineRed);
    gpiod_line_release(lineBlueOne);
    gpiod_line_release(lineBlueTwo);
    gpiod_line_release(lineBlueThree);
    gpiod_chip_close(chip);

	return 0;
}