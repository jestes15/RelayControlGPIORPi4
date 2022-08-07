#include <stdio.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "auxillary.hpp"

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
  int i, val;

  // Open GPIO chip
  chip = gpiod_chip_open_by_name(chipname);

  // Open GPIO lines
  lineRed = gpiod_chip_get_line(chip, 24);
  lineGreen = gpiod_chip_get_line(chip, 25);
  lineYellow = gpiod_chip_get_line(chip, 5);
  lineButton = gpiod_chip_get_line(chip, 6);

  // Open LED lines for output
  gpiod_line_request_output(lineRed, "example1", 0);
  gpiod_line_request_output(lineGreen, "example1", 0);
  gpiod_line_request_output(lineYellow, "example1", 0);

  // Open switch line for input
  gpiod_line_request_input(lineButton, "example1");

  // Blink LEDs in a binary pattern
  i = 0;
  while (true) {
    gpiod_line_set_value(lineRed, (i & 1) != 0);
    gpiod_line_set_value(lineGreen, (i & 2) != 0);
    gpiod_line_set_value(lineYellow, (i & 4) != 0);

    // Read button status and exit if pressed
    val = gpiod_line_get_value(lineButton);
    if (val == 0) {
      break;
    }

    std::this_thread::sleep_for(1s);

    usleep(100000);
    i++;
  }

  // Release lines and chip
  gpiod_line_release(lineRed);
  gpiod_line_release(lineGreen);
  gpiod_line_release(lineYellow);
  gpiod_line_release(lineButton);
  gpiod_chip_close(chip);
  return 0;
}