#pragma once
#include <iostream>
#include <string>

// FG  BG  Name  
// 30  40  Black   
// 31  41  Red 
// 32  42  Green   
// 33  43  Yellow  
// 34  44  Blue   
// 35  45  Magenta 
// 36  46  Cyan  
// 37  47  White 
// 90  100 Bright Black (Gray)
// 91  101 Bright Red 
// 92  102 Bright Green 
// 93  103 Bright Yellow 
// 94  104 Bright Blue 
// 95  105 Bright Magenta
// 96  106 Bright Cyan
// 97  107 Bright White

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define GRAY    "\033[90m"      /* Gray */

#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class Progressor {
public:
  inline Progressor(int n) : n_cycles(n) {};
  inline void update();

private:
  int progress{0};
  int n_cycles;
  int last_perc{0};
  bool do_show_bar{true};
  bool update_is_called{false};

  std::string done_char{"\033[90m█\033[0m"};
  std::string todo_char{" "};
  std::string opening_bracket_char{"["};
  std::string closing_bracket_char{"]"};
};

inline void Progressor::update() {
    if (!update_is_called) {
        if (do_show_bar == true) {
            std::cout << opening_bracket_char;
            for (int _ = 0; _ < 50; _++) std::cout << todo_char;
            std::cout << closing_bracket_char << " 0%";
        }
        else std::cout << "0%";
    }
    update_is_called = true;

    int perc = 0;

    // compute percentage, if did not change, do nothing and return
    perc = progress*100./(n_cycles-1);
    if (perc < last_perc) return;

    // update percentage each unit
    if (perc == last_perc + 1) {
        // erase the correct  number of characters
        if      (perc <= 10)                std::cout << "\b\b"   << perc << '%';
        else if (perc  > 10 and perc < 100) std::cout << "\b\b\b" << perc << '%';
        else if (perc == 100)               std::cout << "\b\b\b" << perc << '%';
    }
    if (do_show_bar == true) {
        // update bar every ten units
        if (perc % 2 == 0) {
            // erase closing bracket
            std::cout << std::string(closing_bracket_char.size(), '\b');
            // erase trailing percentage characters
            if      (perc  < 10)               std::cout << "\b\b\b";
            else if (perc >= 10 && perc < 100) std::cout << "\b\b\b\b";
            else if (perc == 100)              std::cout << "\b\b\b\b\b";

            // erase 'todo_char'
            for (int j = 0; j < 50-(perc-1)/2; ++j) {
                std::cout << std::string(todo_char.size(), '\b');
            }

            // add one additional 'done_char'
            if (perc == 0) std::cout << todo_char;
            else           std::cout << done_char;

            // refill with 'todo_char'
            for (int j = 0; j < 50-(perc-1)/2-1; ++j) std::cout << todo_char;

            // readd trailing percentage characters
            std::cout << closing_bracket_char << ' ' << perc << '%';
        }
    }
    last_perc = perc;
    ++progress;
    std::cout << std::flush;
}