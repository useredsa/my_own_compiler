#ifndef ERRORS_HPP_
#define ERRORS_HPP_

#include <iostream>

namespace compiler {

class Logger {
  public:
    explicit Logger(std::ostream& os) : os_(os) {
        counter = 0;
    }

    template<typename T>
    std::ostream& operator<< (const T& val) {
        ++counter;
        os_ << prefix() << val;
        return os_;
    }

    inline int GetCounter() {
        return counter;
    }

    inline std::ostream& os() {
        return os_;
    }

  protected:
    virtual std::string prefix() = 0;
    std::ostream& os_;

  private:
    int counter;
};

/**
 * @brief A logger for the semantic analysis
 * 
 * Example:
 *     Code:
 *         semantic_error << "Este es mi pana Miguel, él es el test sentence número "
 *                        << semantic_error.GetCounter() << endl;
 *         semantic_error << "Aunque se le ve bien fresco, este es un error diferente\n";
 *         semantic_error << "Y con este ya llevamos " << semantic_error.GetCounter();
 *     Output:
 *         Semantic Error: Este es mi pana Miguel, él es el test sentence número 1
 *         Semantic Error: Aunque se le ve bien fresco, este es un error diferente
 *         Semantic Error: Y con este ya llevamos 3
 */
extern class BasicLogger : public Logger {
  public:
    explicit BasicLogger(std::ostream& os, std::string prefix)
        	: Logger(os), prefix_(std::move(prefix)) {  }
  
  protected:
    inline std::string prefix() {
        return prefix_;
    }

  private:
    std::string prefix_;
} semantic_log, lexical_log, warning_log, internal_log;

} // namespace compiler

#endif // ERRORS_HPP_
