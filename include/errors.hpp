#ifndef ERRORS_HPP_
#define ERRORS_HPP_

#include <iostream>

class ErrorLogger {
  public:
    ErrorLogger(std::ostream& os) : os(os) {
        counter = 0;
    }

    template<typename T>
    std::ostream& operator<< (const T& val) {
        ++counter;
        os << Type() << " Error: ";
        os << val;
        return os;
    }

    inline int GetCounter() { return counter; }

  protected:
    virtual std::string Type() = 0;
    std::ostream& os;

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
static class SemanticErrorLogger : public ErrorLogger {
  public:
    SemanticErrorLogger(std::ostream& os) : ErrorLogger(os) {}
  
  protected:
    std::string Type() {
        return "Semantic";
    }
} semantic_error(std::cerr);

#endif // ERRORS_HPP_