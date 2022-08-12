#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <gpiod.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>

using namespace std::chrono_literals;

#define VAL_CHECK(x) { if (x == -1) return -1; }

// Time variables
time_t rawtime;
struct tm *timeinfo;

struct time_get {
    public:
        friend std::ostream &operator<<(std::ostream &out, time_get &val)
        {
            time_t rawtime_struct;
            struct tm *timeinfo_struct;

            time(&rawtime_struct);
            timeinfo_struct = localtime(&rawtime_struct);

            switch (timeinfo_struct->tm_mon) {
                case 1:
                    out << "Jan ";
                    break;
                case 2:
                    out << "Feb ";
                    break;
                case 3:
                    out << "Mar ";
                    break;
                case 4:
                    out << "Apr ";
                    break;
                case 5:
                    out << "May ";
                    break;
                case 6:
                    out << "Jun ";
                    break;
                case 7:
                    out << "Jul ";
                    break;
                case 8:
                    out << "Aug ";
                    break;
                case 9:
                    out << "Sep ";
                    break;
                case 10:
                    out << "Oct ";
                    break;
                case 11:
                    out << "Nov ";
                    break;
                case 12:
                    out << "Dec ";
                    break;
            }

            out << timeinfo_struct->tm_mday << " " << timeinfo_struct->tm_hour << ":" << timeinfo_struct->tm_min << ":" << timeinfo_struct->tm_sec << " ";
            return out;
        }
} log_time;

struct mode
{
    bool night_is_enabled = false;
    bool day_is_enabled = false;
} light_mode;

int main(int argc, char **argv)
{
    auto current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::string file_name = "/opt/terrarium_control/logs/runtime_";
    file_name.append(std::to_string(current_time));
    file_name.append(".log");

    std::ofstream log_file;
    log_file.open(file_name);

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

    if (chip != NULL)
        log_file << log_time << "Successfully open chip" << std::endl;

    // Open GPIO lines
    ceramicHeatLamp = gpiod_chip_get_line(chip, 2);
    if (ceramicHeatLamp != NULL)
        log_file << log_time << "Successfully got ceramicHeatLamp line (2)" << std::endl;

    UVLamp = gpiod_chip_get_line(chip, 3);
    if (UVLamp != NULL)
        log_file << log_time << "Successfully got UVLamp line (3)" << std::endl;

    baskingLamp100 = gpiod_chip_get_line(chip, 27);
    if (baskingLamp100 != NULL)
        log_file << log_time << "Successfully got baskingLamp100 line (27)" << std::endl;

    baskingLamp75 = gpiod_chip_get_line(chip, 17);
    if (baskingLamp75 != NULL)
        log_file << log_time << "Successfully got baskingLamp75 line (17)" << std::endl;

    emergencyKillSwitch = gpiod_chip_get_line(chip, 18);
    if (emergencyKillSwitch != NULL)
        log_file << log_time << "Successfully got emergencyKillSwith line (18)" << std::endl;

    // Open LED lines for output
    if (gpiod_line_request_output(ceramicHeatLamp, "led_enable_test", 0) != -1)
    {
        log_file << log_time << "Successfully reserved ceramicHeatLamp line (18)" << std::endl;
    } else return -1;
    if (gpiod_line_request_output(UVLamp, "led_enable_test", 0) != -1)
    {
        log_file << log_time << "Successfully reserved UVLamp line (18)" << std::endl;
    } else return -1;
    if (gpiod_line_request_output(baskingLamp100, "led_enable_test", 0) != -1)
    {
        log_file << log_time << "Successfully reserved baskingLamp100 line (18)" << std::endl;
    } else return -1;
    if (gpiod_line_request_output(baskingLamp75, "led_enable_test", 0) != -1)
    {
        log_file << log_time << "Successfully reserved baskingLamp75 line (18)" << std::endl;
    } else return -1;
    if (gpiod_line_request_input(emergencyKillSwitch, "led_enable_test") != -1)
    {
        log_file << log_time << "Successfully reserved emergencyKillSwith line (18)" << std::endl;
    } else return -1;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo->tm_hour >= 8 &&timeinfo->tm_hour < 20) {
        log_file << log_time << "Enabling day mode at: " << std::endl;

        VAL_CHECK(gpiod_line_set_value(ceramicHeatLamp, 1));
        VAL_CHECK(gpiod_line_set_value(UVLamp, 0));
        VAL_CHECK(gpiod_line_set_value(baskingLamp100, 0));
        VAL_CHECK(gpiod_line_set_value(baskingLamp75, 0));

        light_mode.day_is_enabled = true;
        log_file << log_time << "Enabled day mode at: " << std::endl;
    } else if (timeinfo->tm_hour < 8 || timeinfo->tm_hour >= 20) {
        log_file << log_time << "Enabling night mode at: " << std::endl;

        gpiod_line_set_value(ceramicHeatLamp, 0);
        gpiod_line_set_value(UVLamp, 1);
        gpiod_line_set_value(baskingLamp100, 1);
        gpiod_line_set_value(baskingLamp75, 1);

        light_mode.night_is_enabled = true;
        log_file << log_time << "Enabled night mode at: " << std::endl;
    }

    // Blink LEDs in a binary pattern
    while (!gpiod_line_get_value(emergencyKillSwitch))
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        if (timeinfo->tm_hour == 8 && timeinfo->tm_min == 0 && !light_mode.day_is_enabled)
        {
            log_file << log_time << "Enabling day mode at: " << std::endl;

            light_mode.day_is_enabled = true;
            light_mode.night_is_enabled = false;

            gpiod_line_set_value(ceramicHeatLamp, 1);
            gpiod_line_set_value(UVLamp, 0);
            gpiod_line_set_value(baskingLamp100, 0);
            gpiod_line_set_value(baskingLamp75, 0);

            log_file << log_time << "Enabled day mode at: " << std::endl;
        }

        if (timeinfo->tm_hour == 20 && timeinfo->tm_min == 0 && !light_mode.night_is_enabled)
        {
            log_file << log_time << "Enabling night mode at: " << std::endl;

            light_mode.day_is_enabled = false;
            light_mode.night_is_enabled = true;

            gpiod_line_set_value(ceramicHeatLamp, 0);
            gpiod_line_set_value(UVLamp, 1);
            gpiod_line_set_value(baskingLamp100, 1);
            gpiod_line_set_value(baskingLamp75, 1);

            log_file << log_time << "Enabled day mode at: " << std::endl;
        }

        std::this_thread::sleep_for(1s);
    }

    gpiod_line_set_value(ceramicHeatLamp, 1);
    gpiod_line_set_value(UVLamp, 1);
    gpiod_line_set_value(baskingLamp100, 1);
    gpiod_line_set_value(baskingLamp75, 1);

    log_file << log_time << "Turned off all lights" << std::endl;

    // Release lines and chip
    gpiod_line_release(ceramicHeatLamp);
    gpiod_line_release(UVLamp);
    gpiod_line_release(baskingLamp100);
    gpiod_line_release(baskingLamp75);
    gpiod_chip_close(chip);

    log_file << log_time << "All lines and chips are free" << std::endl;

    return 0;
}