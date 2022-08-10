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
    struct gpiod_line *ceramicHeatLamp;
    struct gpiod_line *UVLamp;
    struct gpiod_line *baskingLamp100;
    struct gpiod_line *baskingLamp75;

    struct gpiod_line *emergencyKillSwitch;

    // Open GPIO chip
    chip = gpiod_chip_open_by_name(chipname);

    // Open GPIO lines
    ceramicHeatLamp = gpiod_chip_get_line(chip, 2);
    UVLamp = gpiod_chip_get_line(chip, 3);
    baskingLamp100 = gpiod_chip_get_line(chip, 27);
    baskingLamp75 = gpiod_chip_get_line(chip, 17);
    emergencyKillSwitch = gpiod_chip_get_line(chip, 18);

    // Open LED lines for output
    if (gpiod_line_request_output(ceramicHeatLamp, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(UVLamp, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(baskingLamp100, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_output(baskingLamp75, "led_enable_test", 0) == -1)
    {
        return -1;
    }
    if (gpiod_line_request_input(emergencyKillSwitch, "led_enable_test") == -1)
    {
        return -1;
    }

	// Blink LEDs in a binary pattern
	while (!gpiod_line_get_value(emergencyKillSwitch))
	{
        gpiod_line_set_value(ceramicHeatLamp, 1);
        gpiod_line_set_value(UVLamp, 0);
        gpiod_line_set_value(baskingLamp100, 0);
        gpiod_line_set_value(baskingLamp75, 0);
        std::this_thread::sleep_for(1s);
        gpiod_line_set_value(ceramicHeatLamp, 0);
        gpiod_line_set_value(UVLamp, 1);
        gpiod_line_set_value(baskingLamp100, 1);
        gpiod_line_set_value(baskingLamp75, 1);
        std::this_thread::sleep_for(1s);
    }

    gpiod_line_set_value(ceramicHeatLamp,       1);
    gpiod_line_set_value(UVLamp,   1);
    gpiod_line_set_value(baskingLamp100,   1);
    gpiod_line_set_value(baskingLamp75, 1);

    // Release lines and chip
    gpiod_line_release(ceramicHeatLamp);
    gpiod_line_release(UVLamp);
    gpiod_line_release(baskingLamp100);
    gpiod_line_release(baskingLamp75);
    gpiod_chip_close(chip);

	return 0;
}