#include <gpiod.h>
#include <vector>

const char* chipname = "gpiochip0";
struct gpiod_chip* chip;
struct gpiod_line* lineRed;    // Red LED
struct gpiod_line* lineGreen;  // Green LED
struct gpiod_line* lineYellow; // Yellow LED
struct gpiod_line* lineButton; // Pushbutton

class init_gpiod {
private:
    std::vector<struct gpiod_line*> lines;
    std::vector<struct gpiod_chip*> chips;

    typedef u_int64_t int_t;

public : 
    init_gpiod() = delete;

    /**
     * @brief Initialize init_gpiod class
     * @param chip_name 2d char array for all the chip_names being used
     * @param lines Lines to be retrieved from the chip they are in
     * @param size sizes for arrays used
     * @return returns an instance to the class init_gpiod
     *
     * Initialize lines and chips correctly for GPIO Line manipulation
     *
     * size details:
     *  1st element: amount of chip names
     *  Nth element: amount of lines per chip correlating to the size of each array inside lines
     *
     * example:
     *  chip_name = {"gpiochip0", "gpiochip1" };
     *  lines = { { 0, 1, 2 }, {0, 3, 5, 6} };
     *  size = { 2, 3, 4 };
     */
    init_gpiod(const char **chip_name, int_t **lines_in, int_t *size)
    {
        for (int_t i = 0; i < size[0]; ++i) {
            chips.push_back(gpiod_chip_open_by_name(chip_name[i]));

            if (chips.at(i) == NULL) continue;

            for (int_t j = 0; j < size[j + 1]; ++j) {
                lines.push_back(gpiod_chip_get_line(chips.at(i), lines_in[i][j]));
            }
        }
    }
};