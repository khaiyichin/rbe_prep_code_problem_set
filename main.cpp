#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <utility>

void question1()
{
    std::cout << "==== Question 1 ====" << std::endl << std::endl;

    // Get input
    int inputInt;
    std::cout << "Enter a positive integer: ";
    std::cin >> inputInt;

    while (inputInt < 0)
    {
        std::cout << "Input is not positive. Please enter a positive integer: ";
        std::cin >> inputInt;
    }

    while (inputInt > 20)
    {
        std::cout << "Factorial of input exceeds a 64-bit integer. Please enter a positive integer lower than 20.";
        std::cin >> inputInt;
    }

    // Calculate factorial
    std::cout << "Factorial of " << inputInt << " = ";

    u_int64_t output = inputInt;

    if (inputInt == 0 || inputInt == 1) std::cout << 1 << std::endl;
    else
    {
        while (inputInt > 1)
        {
            output *= --inputInt;
        }
        
        std::cout << output << std::endl;
    }
}

void question2()
{
    std::cout << "==== Question 2 ====" << std::endl << std::endl;
  
    // Get input
    std::string inputStr;
    std::cout << "Please input a string value: ";
    std::getline(std::cin >> std::ws, inputStr);

    std::cout << "Does the input string have the character 'x'? ";

    // Find 'x'
    if (inputStr.find('x') == std::string::npos)
    {
        std::cout << "No" << std::endl;
    }
    else std::cout << "Yes" << std::endl;
}

/**
 * Class to solve string expressions containing only the 4 basic operations: + - * /
 * and with parentheses.
 */
class ExpressionGroup
{
    public:
    
        ExpressionGroup(const std::string &exp) : expression_(exp)
        {
            // Extract child ExpressionGroups, which are basically parentheses sets
            extractChildren();

            // Evaluate expressions
            evaluate();
        }

        double getValue() const {return value_;}

    private:

        /**
         * Struct to store operator information
         */
        struct Operator
        {
            char op; // operator character

            int expInd; // expression index

            bool multDiv = false; // indicator on whether current operator is the * or / operator

            Operator(const char &o, const int &i) : op(o), expInd(i)
            {
                if (op == '*' || op == '/')
                {
                    multDiv = true;
                }
            };
        };

        /**
         * Struct to store number information and provide arithmetic operations
         */
        struct Number
        {
            double val;

            Number(const double &n) : val(n) {};

            Number(const Number &left, const Number &right, const Operator &op)
            {
                switch (op.op)
                {
                    case '+':
                    {
                        val = left.val + right.val;
                        break;
                    }

                    case '-':
                    {
                        val = left.val - right.val;
                        break;                        
                    }

                    case '*':
                    {
                        val = left.val * right.val;
                        break;                        
                    }

                    case '/':
                    {
                        val = left.val / right.val;
                        break;                        
                    }
                }
            }
        };

        using NumOpVecPair = std::pair< std::vector<Number>, std::vector<Operator> >; ///< alias for pair of vector of Number structs and vector of Operator structs

        /**
         * Extract parentheses sets in expression as ExpressionGroup children
         */
        void extractChildren()
        {
            while (!extractionComplete_)
            {
                // Find outermost parentheses
                int openParInd = expression_.find('(');

                // Check if there are any parentheses
                if (openParInd != std::string::npos) // yes
                {
                    int closeParInd = 0;
                    int parTracker = 1; // once the tracker goes to 0, we found the pairing set of parenthesis

                    for (int i = openParInd+1; i < expression_.size(); ++i)
                    {
                        if (expression_[i] == ')')
                        {
                            closeParInd = i;
                            
                            if (--parTracker == 0) break;
                        }
                        else if (expression_[i] == '(') // an inner open parenthesis
                        {
                            ++parTracker;
                        }
                    }

                    children_.push_back( ExpressionGroup( expression_.substr(openParInd+1, closeParInd - openParInd - 1) ) );
                    expression_.replace(openParInd, closeParInd - openParInd + 1, "z");
                }
                else extractionComplete_ = true; // no
            }
        }

        /**
         * Evaluate expression to compute value
         */
        void evaluate()
        {
            // Convert expression into Number and Operator structs
            NumOpVecPair numOpPair = convertExpressions();

            // Evaluate multiplications and divisions first
            auto multDivOpItr = findNextMultDivOp( numOpPair.second.begin(), numOpPair.second.end() );

            while (multDivOpItr != numOpPair.second.end())
            {
                size_t ind = multDivOpItr - numOpPair.second.begin();

                // Compute result
                std::cout << "\tComputing " << numOpPair.first[ind].val << " " << multDivOpItr->op << " " << numOpPair.first[ind+1].val << std::endl;
                Number result(numOpPair.first[ind], numOpPair.first[ind+1], *multDivOpItr);

                // Replace values in the number pair vector
                numOpPair.first.erase(numOpPair.first.begin() + ind, numOpPair.first.begin() + ind + 2);
                numOpPair.first.insert(numOpPair.first.begin() + ind, result);

                // Remove operator
                numOpPair.second.erase(numOpPair.second.begin() + ind);
                multDivOpItr = findNextMultDivOp( numOpPair.second.begin(), numOpPair.second.end() );
            }

            // Evaluate additions and subtractions
            // Order of operations is always from left to right, so the first 2 numbers in the vector are always combined
            while (numOpPair.second.size() != 0)
            {
                // Compute result
                std::cout << "\tComputing " << numOpPair.first[0].val << " " << numOpPair.second.begin()->op << " " << numOpPair.first[1].val << std::endl;
                Number result(numOpPair.first[0], numOpPair.first[1], *numOpPair.second.begin());

                // Replace values in the number pair vector
                numOpPair.first.erase(numOpPair.first.begin(), numOpPair.first.begin() + 2);
                numOpPair.first.insert(numOpPair.first.begin(), result);

                // Remove operator
                numOpPair.second.erase(numOpPair.second.begin());
            }

            // Debug info
            if (numOpPair.first.size() != 1 || numOpPair.second.size() != 0) std::cout << "ERROR: The vectors containing the numbers and operators have a wrong sizes." << std::endl;

            // Store computed numerical value
            value_ = numOpPair.first[0].val;
        }

        /**
         * Convert string expressions to vector of Number and Operator structs
         */
        NumOpVecPair convertExpressions()
        {
            std::vector<Operator> operators;
            std::vector<Number> numbers;

            // Find all operators
            for (int i = 0; i < expression_.size(); ++i)
            {
                if (expression_[i] == '+' ||
                    expression_[i] == '-' ||
                    expression_[i] == '*' ||
                    expression_[i] == '/')
                {
                    operators.push_back( Operator(expression_[i], i) );
                }
            }

            // Find all numbers
            int startInd = 0;
            int endInd;

            if (operators.size() == 0) // only one child ExpressionGroup or one number
            {
                endInd = expression_.size();

                std::string candidate = expression_.substr(startInd, endInd - startInd);
                
                // Store number
                numbers.push_back( createNumber(candidate) );
            }
            else // current expression contains operator(s), i.e., multiple child ExpressionGroups or numbers
            {
                int opInd = 0;

                while (startInd < (operators.end() - 1)->expInd) // iterate for number of times as the number of operators
                {
                    // Set current number's last digit index
                    endInd = operators[opInd++].expInd; 

                    std::string candidate = expression_.substr(startInd, endInd - startInd);

                    // Store number
                    numbers.push_back( createNumber(candidate) );

                    startInd = endInd + 1;
                }
                numbers.push_back( createNumber( expression_.substr(startInd, expression_.size() - startInd) ) ); // store the final candidate
            }

            return std::make_pair(numbers, operators);
        }

        /**
         * Obtain the value for the next child ExpressionGroup
         */
        double resolveNextChild() {return children_[childResolutionCounter_++].getValue();}

        /**
         * Create a Number struct based on a string
         */
        Number createNumber(const std::string &numStr)
        {
            if (numStr != "z") return Number( std::stof( numStr ) );
            else return Number( resolveNextChild() );
        }

        /**
         * Find the next multiplication or division operator
         */
        std::vector<Operator>::iterator findNextMultDivOp(const std::vector<Operator>::iterator &startItr, const std::vector<Operator>::iterator &endItr)
        {
            // Find the first multiplication or division operator
            auto itr = std::find_if(startItr, endItr,
                [] (const Operator& o)
                {
                    return o.multDiv;
                });

            return itr;
        }

        std::vector<ExpressionGroup> children_; ///< Vector of child ExpressionGroup objects

        std::string expression_; ///< String expression for the current ExptressionGroup

        int childResolutionCounter_ = 0; ///< Counter to keep track of the resolved child ExptressionGroups

        double value_; ///< Computed value of current ExpressionGroup's expression

        bool extractionComplete_ = false; ///< Status on whether all child ExpressionGroups have been extracted
};

void question3()
{
    std::cout << "==== Question 3 ====" << std::endl << std::endl;

    // Get input
    std::string inputStr;
    std::cout << "Please input an arithmetic expression: ";
    std::getline(std::cin >> std::ws, inputStr);

    // Strip white spaces
    inputStr.erase( std::remove_if(inputStr.begin(), inputStr.end(),
        []( char ch ) -> char
        {
            return std::isspace<char>( ch, std::locale::classic() );
        }), inputStr.end() );

    // Remove quotes
    size_t startQuoteInd = inputStr.find("\"");
    size_t endQuoteInd = inputStr.find("\"", startQuoteInd + 1);
    
    std::string expression = inputStr.substr(startQuoteInd + 1, endQuoteInd - startQuoteInd - 1);

    // Build expression groups
    ExpressionGroup expGroup(expression);

    std::cout << "Result of " << expression << " = " << expGroup.getValue() << std::endl;
}

int main()
{
    std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;
    question1();
    std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;
    question2();
    std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;
    question3();

    return 0;
}