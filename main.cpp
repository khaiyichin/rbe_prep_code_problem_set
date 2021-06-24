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
    std::cin >> inputStr;

    std::cout << "Does the input string have the character 'x'? ";

    // Find 'x'
    if (inputStr.find('x') == std::string::npos)
    {
        std::cout << "No" << std::endl;
    }
    else std::cout << "Yes" << std::endl;
}


class ExpressionGroup
{
    public:
    
        ExpressionGroup(const std::string &exp) : expression_(exp)
        {
            // Extract parentheses set
            extractChildren();

            // Evaluate expressions
            evaluate();
        }

        double getValue() const {return value_;}

    private:

        struct OperatorPair
        {
            char op;
            int ind;
            bool multDiv = false;

            OperatorPair(const char &o, const int &i) : op(o), ind(i)
            {
                if (op == '*' || op == '/')
                {
                    multDiv = true;
                }
            };
        };

        struct NumberPair
        {
            double num;
            int minInd;
            int maxInd;

            // Constructor
            NumberPair(const double &n, const int &minI, const int &maxI) : num(n), minInd(minI), maxInd(maxI) {};

            // Combine Constructor
            NumberPair(const NumberPair &left, const NumberPair &right, const OperatorPair &op)
            {
                minInd = left.minInd;
                maxInd = right.maxInd;

                switch (op.op)
                {
                    case '+':
                    {
                        num = left.num + right.num;
                        break;
                    }

                    case '-':
                    {
                        num = left.num - right.num;
                        break;                        
                    }

                    case '*':
                    {
                        num = left.num * right.num;
                        break;                        
                    }

                    case '/':
                    {
                        num = left.num / right.num;
                        break;                        
                    }
                }
            }
        };

        using NumOpPair = std::pair< std::vector<NumberPair>, std::vector<OperatorPair> >;

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
                    int parTracker = 1;

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
                else // no
                {
                    extractionComplete_ = true;
                }
            }
        }

        std::vector<OperatorPair>::iterator findNextMultDivOp(const std::vector<OperatorPair>::iterator &startItr, const std::vector<OperatorPair>::iterator &endItr)
        {
            // Evaluate multiplications and divisions first
            auto itr = std::find_if(startItr, endItr,
                [] (const OperatorPair& o)
                {
                    return o.multDiv;
                });

            return itr;
        }

        void evaluate()
        {
            NumOpPair numOpPair = convertExpressions();

            // Evaluate multiplications and divisions first
            auto multDivOpItr = findNextMultDivOp( numOpPair.second.begin(), numOpPair.second.end() );

            while (multDivOpItr != numOpPair.second.end())
            {
                size_t ind = multDivOpItr - numOpPair.second.begin();

                // Compute result
                std::cout << "Computing " << numOpPair.first[ind].num << " " << multDivOpItr->op << " " << numOpPair.first[ind+1].num << std::endl;
                NumberPair result(numOpPair.first[ind], numOpPair.first[ind+1], *multDivOpItr);

                // Replace values in the number pair vector
                numOpPair.first.erase(numOpPair.first.begin() + ind, numOpPair.first.begin() + ind + 2);
                numOpPair.first.insert(numOpPair.first.begin() + ind, result);

                // Remove operator
                numOpPair.second.erase(numOpPair.second.begin() + ind);
                multDivOpItr = findNextMultDivOp( numOpPair.second.begin(), numOpPair.second.end() );
            }

            // Evaluate additions and subtractions
            // Order of operations is always from left to right, so the first 2 numbers in the vector are always combined
            std::cout << "DEBUG numOpPair sizes " << numOpPair.first.size() << " " << numOpPair.second.size() << std::endl;

            while (numOpPair.second.size() != 0)
            {
                // Compute result
                NumberPair result(numOpPair.first[0], numOpPair.first[1], *numOpPair.second.begin());

                std::cout << "debug numOpPair.first[0] " << numOpPair.first[0].num << " numOpPair.first[1] " << numOpPair.first[1].num << std::endl;
                std::cout << "DEBUG result " << result.num << std::endl;

                // Replace values in the number pair vector
                numOpPair.first.erase(numOpPair.first.begin(), numOpPair.first.begin() + 2);
                numOpPair.first.insert(numOpPair.first.begin(), result);

                // Remove operator
                numOpPair.second.erase(numOpPair.second.begin());
            }

            std::cout << "got here " << numOpPair.first[0].num << " " << numOpPair.first.size() << std::endl;

            if (numOpPair.first.size() != 1 || numOpPair.second.size() != 0) std::cout << "WRONG!" << std::endl;  // debug

            // Assign fully computed value to current ExpressionGroup
            value_ = numOpPair.first[0].num;

            std::cout << "Result " << value_ << std::endl;

            // Iterate through number and operator groups
        }

        NumOpPair convertExpressions()
        {
            std::vector<OperatorPair> operators;
            std::vector<NumberPair> numbers;

            // Find all operators
            for (int i = 0; i < expression_.size(); ++i)
            {
                if (expression_[i] == '+' ||
                    expression_[i] == '-' ||
                    expression_[i] == '*' ||
                    expression_[i] == '/')
                {
                    operators.push_back( OperatorPair(expression_[i], i) );
                }
            }

            // Find all numbers
            int startInd = 0;

            for (auto const &op : operators)
            {
                int endInd = op.ind;

                std::string candidate = expression_.substr(startInd, endInd - startInd);

                // Check if candidate is actually a number or is a child expression group
                if (candidate != "z") numbers.push_back( NumberPair(std::stof( candidate ), startInd, endInd) ); // regular number string
                else // placeholder for child expression group
                {
                    numbers.push_back( NumberPair(resolveNextChild(), startInd, endInd) ) ;
                }
                startInd = op.ind + 1;
            }
            numbers.push_back( NumberPair(std::stof( expression_.substr(startInd, expression_.size() - startInd) ), startInd, startInd) ); // get final number

            return std::make_pair(numbers, operators);
        }

        double resolveNextChild()
        {
            return children_[childResolutionCounter_++].getValue();
        }

        std::vector<ExpressionGroup> children_;
        std::string expression_;
        int childResolutionCounter_ = 0; 
        double value_;
        bool extractionComplete_ = false;
        bool childrenEvaluated_ = false;
};

void question3()
{
    std::cout << "==== Question 3 ====" << std::endl << std::endl;
    std::cout << "Please input an arithmetic expression: ";

    // Read inputs
    std::string inputStr;
    std::getline(std::cin >> std::ws, inputStr);

    // Strip white spaces
    inputStr.erase( std::remove_if(inputStr.begin(), inputStr.end(),
        []( char ch ) -> char
        {
            return std::isspace<char>( ch, std::locale::classic() );
        }), inputStr.end() );

    // Build expression groups
    ExpressionGroup expGroup(inputStr);

    // Evaluate expression


    // Break them into different operators


}

int main()
{
    // question1();
    std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;
    // question2();
    std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;
    question3();

    return 0;
}