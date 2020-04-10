#ifndef io_hpp_INCLUDED
#define io_hpp_INCLUDED

#include <vector>
#include <string>

namespace AST {

// class t_print_item {
//   public:

//     // virtual std::string type() = 0;
//     virtual std::string reference() = 0;
    
//     virtual std::string str_val() = 0;
// };

// using t_print_list = std::vector<t_print_item*>;
// class t_print_list {
//   private:
//     std::vector<t_print_item*> items;

//   public:
//     t_print_list(t_print_item *first) : items() {
//         items.push_back(first);
//     }

//     void push_back(t_print_item *item) {
//         items.push_back(item);
//     }
// };

using t_read_item = std::string;
// class t_read_item {
    
// };

// using t_read_list = std::vector<t_read_item*>;
// class t_read_list {

// };

}

#endif // io_hpp_INCLUDED

