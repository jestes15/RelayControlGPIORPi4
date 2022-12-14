#include <gpiod.h>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <map>
#include <iostream>

#define VERSION 0.2

#ifndef PROGRAM_NAME
#define PROGRAM_NAME
const char *name = "gpio_manipulation_program";
#endif

struct bug_report
{
    const char *err_txt;
    int err_val;

    friend std::ostream& operator<<(std::ostream &out, bug_report &emp) {
        out << "Error Number: " << emp.err_val << std::endl
            << "Error Text: " << emp.err_txt << std::endl;
        return out;
    }
};

namespace init_gpiod_types
{
    typedef u_int64_t int_t;
    ;
}

class init_gpiod
{
private:
    std::map<std::string, struct gpiod_line *> input_lines;
    std::map<std::string, struct gpiod_line *> output_lines;

    std::vector<std::string> input_lines_names;
    std::vector<std::string> output_lines_names;

    std::vector<struct gpiod_chip *> chips;

    std::vector<struct bug_report> errors;

public:
    init_gpiod() = delete;

    /**
     *
     * Initialize lines and chips correctly for GPIO Line manipulation.
     *
     * size details:<
     *  1st element: amount of chip names.
     *  Nth element: amount of lines per chip correlating to the size of each array inside lines.
     *
     * The lines array is to have two arrays per chip, the first being input and the second being output.
     *
     * example:
     *  chip_name = {"gpiochip0" };
     *  lines = { { { ("line0", 0),  ("line1", 1) }, { (  "line2", 2) } } };
     *
     * @brief Initialize init_gpiod class
     * @param chip_name 2d char array for all the chip_names being used
     * @param lines Lines to be retrieved from the chip they are in
     * @return An instance to the class init_gpiod
     *
     */
    init_gpiod(std::vector<const char *> chip_name, std::vector<std::vector<std::vector<std::pair<std::string, init_gpiod_types::int_t>>>> lines_in)
    {
        for (init_gpiod_types::int_t i = 0; i < chip_name.size(); ++i)
        {
            auto open_chip_ret_val = gpiod_chip_open_by_name(chip_name[i]);

            if (open_chip_ret_val == NULL)
            {
                std::string err = "Could not open chip: ";
                err.append(chip_name[i]);
                bug_report err_struct = {
                    .err_txt = err.c_str(),
                    .err_val = NULL};

                errors.push_back(err_struct);
            }
            else
            {
                chips.push_back(open_chip_ret_val);
            }

            if (chips.at(i) == NULL)
                continue;

            for (init_gpiod_types::int_t j = 0; j < lines_in.at(i).at(0).size(); ++j)
            {
                auto val = gpiod_chip_get_line(chips.at(i), lines_in.at(i).at(0).at(j).second);

                if (val == NULL)
                {
                    std::string err = "Could not get line: " + std::to_string(i);
                    err.append("With name: " + lines_in.at(i).at(0).at(j).first);
                    bug_report err_struct = {
                        .err_txt = err.c_str(),
                        .err_val = NULL};

                    errors.push_back(err_struct);
                }
                else
                {
                    input_lines[lines_in.at(i).at(0).at(j).first] = val;
                    input_lines_names.push_back(lines_in.at(i).at(0).at(j).first);
                }
            }

            for (init_gpiod_types::int_t j = 0; j < lines_in.at(i).at(1).size(); ++j)
            {
                auto val = gpiod_chip_get_line(chips.at(i), lines_in.at(i).at(1).at(j).second);

                std::cout << lines_in.at(i).at(1).at(j).first << std::endl;
                std::cout << lines_in.at(i).at(1).at(j).second << std::endl;

                if (val == NULL)
                {
                    std::string err = "Could not get line: " + std::to_string(i);
                    err.append("With name: " + lines_in.at(i).at(1).at(j).first);
                    bug_report err_struct = {
                        .err_txt = err.c_str(),
                        .err_val = NULL};

                    errors.push_back(err_struct);
                }
                else
                {
                    output_lines[lines_in.at(i).at(1).at(j).first] = val;
                    output_lines_names.push_back(lines_in.at(i).at(1).at(j).first);
                }
            }
        }
    }

    ~init_gpiod()
    {
        for (auto [names, line] : input_lines)
        {
            gpiod_line_release(line);
        }

        for (auto [names, line] : output_lines)
        {
            gpiod_line_release(line);
        }

        for (auto i : chips)
        {
            gpiod_chip_close(i);
        }
    }

    /**
     *
     * @brief Initialize input lines
     * @return Amount of errors that occured during function runtime
     *
     */
    int init_input_lines()
    {
        int num_of_errors = 0;

        for (init_gpiod_types::int_t i = 0; i < this->input_lines.size(); ++i)
        {
            if (gpiod_line_request_input(input_lines[input_lines_names.at(i)], name) == -1)
            {
                bug_report err = {
                    .err_txt = "Could not reserve line",
                    .err_val = -1};

                errors.push_back(err);
                ++num_of_errors;
            }
        }

        return num_of_errors;
    }

    /**
     *
     * @brief Initialize output lines
     * @return Amount of errors that occured during function runtime
     *
     */
    int init_output_lines()
    {
        int num_of_errors = 0;

        for (init_gpiod_types::int_t i = 0; i < this->input_lines_names.size(); ++i)
        {
            if (gpiod_line_request_output(output_lines[output_lines_names.at(i)], name, 0) == -1)
            {
                bug_report err = {
                    .err_txt = "Could not reserve line",
                    .err_val = -1};

                errors.push_back(err);
                ++num_of_errors;
            }
        }

        return num_of_errors;
    }

    /**
     *
     * @brief Initialize output lines
     * @param lines a vector of pairs that include the name and value to set that line to
     * @return Amount of errors that occured during function runtime
     *
     */
    int set_lines(std::vector<std::pair<std::string, init_gpiod_types::int_t>> lines)
    {
        int num_of_errors = 0;
        for (auto [name, value] : lines)
        {
            if (gpiod_line_set_value(output_lines[name], value) == -1)
            {
                std::string err_str = "Could not set line " + name;

                bug_report err = {
                    .err_txt = err_str.c_str(),
                    .err_val = -1};

                errors.push_back(err);
                ++num_of_errors;
            }
        }

        return num_of_errors;
    }

    /**
     *
     * @brief Returns copy of the errors vector
     * @return vector of errors, even if empty
     *
     */
    std::vector<struct bug_report> get_errors()
    {
        return errors;
    }

    /**
     *
     * @brief Returns copy of the input_lines_names vector
     * @return vector of input_lines_names, even if empty
     *
     */
    std::vector<std::string> get_input_lines()
    {
        return input_lines_names;
    }

    /**
     *
     * @brief Returns copy of the output_lines_names vector
     * @return vector of output lines_names, even if empty
     *
     */
    std::vector<std::string> get_output_lines()
    {
        return output_lines_names;
    }

    /**
     *
     * @brief Returns copy of the chips vector
     * @return vector of chips, even if empty
     *
     */
    std::vector<struct gpiod_chip *> get_chips()
    {
        return chips;
    }

    /**
     *
     * @brief Returns copy of the output_lines map
     * @return vector of chips, even if empty
     *
     */
    std::map<std::string, struct gpiod_line *> get_output_lines_map()
    {
        return output_lines;
    }

    /**
     *
     * @brief Returns copy of the chips vector
     * @return vector of chips, even if empty
     *
     */
    std::map<std::string, struct gpiod_line *> get_input_lines_map()
    {
        return input_lines;
    }
};