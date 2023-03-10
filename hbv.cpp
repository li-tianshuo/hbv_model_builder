// Copyright 2022 Tianshuo Li
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "hbv_model.hpp"
/**
 * @brief This function will run HBV model based on given file path.
 * It will read csv file from line2(since there some header exist), 
 * and can detect missing value or non-double value in the data file and skiped to next records.
 * @param data_file The path of data file(csv file or same format)
 * @param parameters_file The path of parameter file (txt or similar format)
 * @param output_path output path
 */
void runHBV(std::string data_file, std::string parameters_file, std::string output_path) {
    std::ifstream input1(data_file);  // declare ifstream to process data file
    std::ifstream input2(parameters_file);  // declare ifstream to process parameters file
    std::ofstream output(output_path);   // declare ofstream to process output file
    if (!input1.is_open()) {
        std::cerr << "Can't Open " << data_file <<"\n";
        exit(-1);
    }
    if (!input2.is_open()) {
        std::cerr << "Can't Open " << parameters_file <<"\n";
        exit(-1);
    }
    if (!output.is_open()) {
        std::cerr << "Can't Open " << output_path <<"\n";
        exit(-1);
    }
    int64_t t = 0;  // t is a indicator to record the lines of file.
    int64_t z = 0;  // z is a indicator to record the columns of data file
    int64_t p_Q = -1;  // p_Q is getting from parameter file and store the column numbers of Q
    int64_t p_T = -1;  // p_T is getting from parameter file and store the column numbers of T
    int64_t p_P = -1;  // p_P is getting from parameter file and store the column numbers of P
    std::string temp;  // temp is used to get string from file as one sentence
    std::string temp1;  // temp1 are used to get string from file as one cell
    std::vector<double> parameters;  // parameters are used to store the parameters information
    std::vector<double> Q;  // Q are used to store Q information from data file.
    std::vector<double> T;  // T are used to store T information from data file.
    std::vector<double> P;  // P are used to store P information from data file.
    double temp3;  // temp3 are used to store the value convert from string to double
    while (std::getline(input2, temp)) {
        if (t <= 15) {
            try {
                parameters.push_back(std::stod(temp));
                } catch(const std::exception& e) {
                    std::cerr << "Parameters contained non-double value" << '\n';
                    std::cerr << e.what() << '\n';
                }
        } else {
            try {
                if (t == 16) {
                    p_T = static_cast<int64_t>(std::stoi(temp));
                } else if (t == 17) {
                    p_P = static_cast<int64_t>(std::stoi(temp));
                } else if (t == 18) {
                    p_Q = static_cast<int64_t>(std::stoi(temp));
                }
            } catch(const std::exception& e) {
                std::cerr << "Parameters contained non-integer value" << '\n';
                std::cerr << e.what() << '\n';
            }
        }
        t++;
    }
    input2.close();
    if (t < 18) {
        std::cout << "Format is incorrect for " << parameters_file <<"\n";
        exit(-1);
    } else if (p_T == p_Q || p_T == p_P || p_P == p_Q) {
        std::cout << "Column position should not be same!" <<"\n";
        exit(-1);
    } else if (p_T == -1 || p_P == -1 || p_Q == -1) {
        std::cout << "Column position is not initialized!" <<"\n";
        exit(-1);
    }
    t = 0;
    while (std::getline(input1, temp)) {
        std::istringstream temp2(temp);
        // temp2 are used to store stringstream based on each lien string value from data file
        z = 0;
        try {
            while (std::getline(temp2, temp1, ',')) {
                z++;
                if (t != 0) {
                    if ((z-1) == (p_P-1)) {
                        temp3 = std::stod(temp1);
                        P.push_back(temp3);
                    } else if ((z-1) == (p_T-1)) {
                        temp3 = std::stod(temp1);
                        T.push_back(temp3);
                    } else if ((z-1) == (p_Q-1)) {
                        temp3 = std::stod(temp1);
                        Q.push_back(temp3);
                    }
                }
            }
            } catch(const std::invalid_argument& e) {
                std::cerr << "Data value contained non-double value" << '\n';
                std::cerr << "This record will skip but complete will continue" << '\n';
                std::cerr << e.what() << '\n';
                uint64_t pos_min = std::min(std::min(Q.size(), P.size()), T.size());
                if (T.size() != pos_min) {
                    T.pop_back();
                }
                if (P.size() != pos_min) {
                    P.pop_back();
                }
                if (Q.size() != pos_min) {
                    Q.pop_back();
                }
            } catch (const std::exception& e) {
                std::cerr << "Something wrong with the program:" << '\n';
            }
        try {
            if (Q.size() != P.size() || Q.size() != T.size() ||T.size() != P.size()) {
                throw std::domain_error("Detect missing value");
            }
        } catch (const std::domain_error& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "This record will skip but complete will continue" << '\n';
            uint64_t pos_min = std::min(std::min(Q.size(), P.size()), T.size());
            // pos_min use to locate the line with error and pop_up other variable to skip this line.
            if (T.size() != pos_min) {
                T.pop_back();
            }
            if (P.size() != pos_min) {
                P.pop_back();
            }
            if (Q.size() != pos_min) {
                Q.pop_back();
            }
        }
        if (t == 0) {
            if ((p_P-1) > z ||(p_T-1) > z || (p_Q-1) > z) {
                std::cerr << "Position of column exceed total length of file" << '\n';
                exit(-1);
            }
        }
        t++;
    }
    if (Q.size() < 2) {
        std::cerr << "The data file should contain at leasts 2-day records" << '\n';
        exit(-1);
    }
    input1.close();
    hbv_model hbv_model(Q, P, T, parameters);
    std::cout << "HBV model build successful!" <<"\n";
    std::vector<double> o_RF = hbv_model.getRF();
    // o_RF are used to get the value of RF from the hbv model and use them to generate the result.csv file.
    std::vector<double> o_AET = hbv_model.getAET();
    // o_AET are used to get the value of AET from the hbv model and use them to generate the result.csv file.
    std::vector<double> o_ET = hbv_model.getET();
    // o_ET are used to get the value of ET from the hbv model and use them to generate the result.csv file.
    std::vector<double> o_storage;
    // o_storage are used to store sum of SUZ and SLZ, and use them to generate the result.csv file.
    std::vector<double> o_Qt = hbv_model.getQt();
    // o_Qt are used to get the value of Qt from the hbv model and use them to generate the result.csv file.
    std::vector<double> o_Qa = hbv_model.getQa();
    // o_Qa are used to get the value of Q_a from the hbv model and use them to generate the result.csv file.
    for (uint64_t i = 0; i < o_RF.size(); i++) {
        o_storage.push_back(hbv_model.getSLZ()[i]+hbv_model.getSUZ()[i]);
    }
    o_Qt.insert(o_Qt.begin(), std::nan(""));
    o_Qa.insert(o_Qa.begin(), std::nan(""));
    o_AET.insert(o_AET.begin(), std::nan(""));
    output << "Days,"
    << "Effective Precipitation (mm/day),"
    << "Evapotranspiration (mm/day),"
    << "Actual Evapotranspiration (mm/day),"
    << "Storage (mm),"
    << "Runoff (mm/day),"
    << "Runoff in Area (m^3/day)\n";
    output << std::setprecision(3) << std::fixed;
    for (uint64_t i = 0; i < o_RF.size(); i++) {
        output << std::to_string(i+1) << ","
            << o_RF[i] << ","
            << o_ET[i] << ","
            << o_AET[i] << ","
            << o_storage[i] << ","
            << o_Qt[i]<< ","
            << o_Qa[i]<< "\n";
    }
    output.close();
    std::cout <<"Data file generated as "<< output_path <<"\n\n";
    hbv_model.getNSE_AD();
}
/**
 * @brief This function will print out help message to the console.
 */
void print_help_message() {
    std::cout<< "   Documentation:" <<"\n";
    std::cout << "Usage: \nhbv data_file_path parameter_file_path output_path" << "\n";
    std::cout << "Options:" << "\n";
    std::cout << "  data_file_path:" << "\n";
    std::cout << "A csv file or file in csv format contained precipitation, temperature, discharge"<< "\n\n";
    std::cout << "  output_path:" << "\n";
    std::cout << "The program will generate data file contained output to that path"<< "\n\n";
    std::cout << "  parameters_file_path:" << "\n";
    std::cout << "A text file that contained all parameters and spilt by line break"<< "\n\n";

    std::cout << "The table below shows each line and parameters"<< "\n";
    std::cout << "|Line | Parameters             |"<< "\n";
    std::cout << "|   1 | T_tr                   |"<< "\n";
    std::cout << "|   2 | DF                     |"<< "\n";
    std::cout << "|   3 | FC                     |"<< "\n";
    std::cout << "|   4 | beta                   |"<< "\n";
    std::cout << "|   5 | alpha                  |"<< "\n";
    std::cout << "|   6 | LP                     |"<< "\n";
    std::cout << "|   7 | k0                     |"<< "\n";
    std::cout << "|   8 | k1                     |"<< "\n";
    std::cout << "|   9 | k2                     |"<< "\n";
    std::cout << "|  10 | Lsuz                   |"<< "\n";
    std::cout << "|  11 | Cperc                  |"<< "\n";
    std::cout << "|  12 | SD_i                   |"<< "\n";
    std::cout << "|  13 | SUZ_i                  |"<< "\n";
    std::cout << "|  14 | SLZ_i                  |"<< "\n";
    std::cout << "|  15 | SM_i                   |"<< "\n";
    std::cout << "|  16 | A                      |"<< "\n";
    std::cout << "|  17 | column position of T   |"<< "\n";
    std::cout << "|  18 | column position of P   |"<< "\n";
    std::cout << "|  19 | column position of Q   |"<< "\n\n";
    std::cout << "Usage: \nhbv [-h][--help][-e][--example]" << "\n";
    std::cout << "Options:" << "\n";
    std::cout << "  -h, --help:" << "\n";
    std::cout << "Print all documentation about this program" << "\n";
    std::cout << "  -e, --example:" << "\n";
    std::cout << "Use example data to generate data file named \"result.csv\" and calculate NSE,"
        << "please make sure \"example_data.csv\" and \"parameters.txt\" is under the same path."
        << "\n";
}
/**
 * @brief This function will print overview of program when user only type command without argument.
 */
void print_overview() {
    std::cout << "  Introduction" <<"\n";
    std::cout <<"This program will build a HBV model and "
        <<"generate data file and calculate NSE based on given dataset."
        <<"\n\n";
    std::cout << "  About HBV model" <<"\n";
    std::cout <<"HBV model is traditional hydrologic model to predict "
        <<"area runoff/discharge based on most basic elements "
        <<"such as precipitation and temperature"
        <<"\n\n";
    std::cout << "  Quick Start / Try a Example" << "\n";
    std::cout <<"This program included a example data file called \"example_data.csv\" "
        << "and parameters \"parameters.txt\", use command below to try a quick example of this program:"
        << "\n\n";
    std::cout << "Usage: hbv -e" << "\n";
    std::cout << "Usage hbv --example" << "\n\n";
    std::cout << "  Documentation" <<"\n";
    std::cout <<"For more information about this program, the help command "
        << "is below:"
        << "\n\n";
    std::cout << "Usage: hbv -h" << "\n";
    std::cout << "Usage hbv --help" << "\n\n";
}
/**
 * @brief The main function will get input from user and provide some argument such as "help" and "example" 
 * or build HBV model with given file path.
 */
int main(int argc, char *argv[]) {
    std::string help1 = "--help";
    std::string help2 = "-h";
    std::string example1 = "--example";
    std::string example2 = "-e";
    std::string data_path, parameter_path;
    if (argc < 2) {
       // Print Overview of Program
       print_overview();
    } else if (argc == 2) {
        // Print Simple command of Program
        if (argv[1] == help1 || argv[1] == help2) {
            print_help_message();
        } else if (argv[1] == example1 || argv[1] == example2) {
            runHBV("example_data.csv", "parameters.txt", "result.csv");
        } else {
            std::cout <<"No command found! Please use --help for more information" <<"\n";
            std::cout << "Usage: hbv --help" << "\n";
        }
    } else {
        // Handle given data set
        if (argc == 4) {
            runHBV(argv[1], argv[2], argv[3]);
        } else {
            std::cout <<"Argument number is incorrect! Please use --help for more information" <<"\n";
            std::cout << "Usage: hbv --help" << "\n";
        }
    }
}
