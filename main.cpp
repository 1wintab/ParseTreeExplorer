#include <iostream>
#include <Windows.h>
#include <stack>
#include <string>

struct Node
{
	std::string data; // Contains an operator (+,-,*,/) or an operand(number/variable)
	Node* left;
	Node* right;

	Node(std::string data) : data(data), left(nullptr), right(nullptr) {}
};

// Helper functions
void Shutdown();
void ClearConsole();
void WaitForEnter();

// Menu functions
void ProgramMenu();
void CreateTreeMenu(std::string& postfix, Node*& root);

// Binary tree operations
static void ConfirmAndDeleteTree(std::string& postfix, Node*& root);
static void RemoveTree(Node*& root);
void BuiltBinaryTree(const std::string& postfix, Node*& root);
void TraverseTreeMenu(Node*& root);
static void preOrder(Node* root);
static void postOrder(Node* root);

// Data processing
bool isAlpha(char c);
bool isDigit(char c);
bool isOperator(char c);
static int GetPriority(char op);

std::string CleanInput(std::string input);
void ConvertInfixToPostfix(std::string input, std::string& postfix);

void Shutdown()
{
	ClearConsole();

	std::cout << "========================================================\n"
			  << "                 Closing the program....                \n"
			  << "========================================================\n";

	exit(EXIT_SUCCESS);
}

void ClearConsole()
{
#ifdef _WIN32
	system("cls");
#else
	system("clear"); // For Linux/Mac (if cross-platform needed)
#endif
}

void WaitForEnter()
{
	std::cout << "\nPress Enter to continue...";

	std::cin.clear();                      // Clear any input errors

	if (std::cin.rdbuf()->in_avail() > 0)  // If input buffer has extra characters
	{
		std::cin.ignore(1000, '\n');
	}
	std::cin.get();                        // Wait for Enter key
}

void ProgramMenu()
{
	std::string postfix;  // Postfix notation of arifmetic expression
	// needed for created/deletion 

	Node* root = nullptr; // Initialize binary tree root

	int choice;
	int minOptions = 1;
	int maxOptions = 4;

	do
	{
		ClearConsole();

		std::cout << "========================================================\n"
				  << "                    >> Binary tree <<                   \n"
				  << "========================================================\n"
				  << " #1. Create binary tree.                                \n"
				  << " #2. Delete binary tree.                                \n"
				  << " #3. Show binary tree traversals.                       \n\n"

				  << " #4. Exit program.                                      \n"
				  << "========================================================\n\n"

				  << "Enter option number: ";

		std::cin >> choice;
		ClearConsole();

		switch (choice)
		{
		case 1:
			CreateTreeMenu(postfix, root);
			break;

		case 2:
			ConfirmAndDeleteTree(postfix, root);
			break;

		case 3:
			TraverseTreeMenu(root);
			break;

		case 4:
			Shutdown();
			break;

		default:
			std::cerr << "========================================================\n"
					  << "          Invalid menu option! Please try again.        \n"
					  << "========================================================\n";
			WaitForEnter();
		}
	} while (true);
}

void CreateTreeMenu(std::string& postfix, Node*& root)
{
	if (!postfix.empty())
	{
		char choice;

		do
		{
			std::cout << "========================================================\n"
					  << "               Binaty tree already exists!              \n"
					  << "           Are you sure you want to delelte it?         \n"
					  << "========================================================\n"

					  << "Yes/No (y/n): ";

			std::cin >> choice;
			ClearConsole();

		} while (!(choice == 'y' || choice == 'n'));

		if (choice == 'y')
		{
			RemoveTree(root);
			postfix.clear();

			std::cout << "========================================================\n"
					  << "            Binary tree successfully deleted!           \n"
					  << "========================================================\n";
			WaitForEnter();
			ClearConsole();
		}
		else
		{
			std::cout << "========================================================\n"
					  << "                Binary tree not deleted!                \n"
					  << "========================================================\n";
			WaitForEnter();
			return;
		}
	}

	std::cout << "========================================================\n"
			  << "                    Create binary tree                  \n"
			  << "========================================================\n"
			  << " > Enter arifmetic expression: [e.g. (a + b) * c]       \n\n"
			  << "   ";

	std::string input;

	std::cin.ignore();
	std::getline(std::cin, input);

	if (input.empty())
	{
		ClearConsole();
		std::cerr << "========================================================\n"
				  << "                   Error! Empty input.                  \n"
				  << "            Please, enter a valid expression.           \n"
				  << "========================================================\n";
		WaitForEnter();
		return;
	}
	else if (input.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/() ") != std::string::npos)
	{
		std::cerr << "========================================================\n"
				  << "      Error! Expression contains invalid characters!    \n"
				  << "========================================================\n";
		WaitForEnter();
		return;
	}

	ClearConsole();

	std::cout << "========================================================\n"
			  << "   ! Binary tree creation requires postfix notation, !  \n"
			  << "   !     which will be automatically generated       !  \n"
			  << "   !                from you input.                  !  \n"
			  << "========================================================\n\n"

			  << ">> User input:     " << input << "               \n"
			  << ">> Filtered data:  " << CleanInput(input) << "   \n\n";

	ConvertInfixToPostfix(CleanInput(input), postfix);
	std::cout << ">> Postfix form:   " << postfix << "             \n";
	WaitForEnter();

	ClearConsole();
	BuiltBinaryTree(postfix, root);
	std::cout << "========================================================\n"
			  << "            Binary tree successfully created!           \n"
			  << "========================================================\n";
	WaitForEnter();
}

bool isAlpha(char c)
{
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
}

bool isDigit(char c)
{
	return (c >= '0' && c <= '9');
}

bool isOperator(char c)
{
	return (c == '+' || c == '-' || c == '*' || c == '/');
}


std::string CleanInput(std::string input) // Remove unnecessary characters
{
	std::string result;
	bool prevWasOperand = false;          // Previous was letter or digit
	bool expectMultipliction = false;     // Whether to add * before next operand

	size_t pos;

	while ((pos = input.find("()")) != std::string::npos) // Remove empty parentheses '()'
	{
		input.erase(pos, 2);
	}

	for (size_t i = 0; i < input.length(); i++)
	{
		char c = input[i];

		if (isAlpha(c) || isDigit(c))
		{
			if (prevWasOperand || expectMultipliction)	// For example: 7a -> 7*a
				result += '*';

			result += c;
			prevWasOperand = true;
			expectMultipliction = false;
		}
		else if (c == '+' || c == '-' || c == '*' || c == '/')
		{
			result += c;
			prevWasOperand = false;
			expectMultipliction = false;
		}
		else if (c == '(') // For example: a( -> a*(
		{
			if (prevWasOperand || expectMultipliction)
				result += '*';

			result += c;
			prevWasOperand = false;
			expectMultipliction = false;
		}
		else if (c == ')') // For example: )a -> )*a
		{
			result += c;
			prevWasOperand = false;
			expectMultipliction = true;
		}
	}
	return result;
}

void ConvertInfixToPostfix(std::string input, std::string& postfix)
{
	std::stack<char> operators;

	for (int i = 0; i < input.length(); i++)
	{
		char c = input[i];

		if (c == '(')
		{
			operators.push(c);
		}
		else if (c == ')')
		{
			while (!operators.empty() && operators.top() != '(')
			{
				postfix += operators.top();
				operators.pop();
			}

			operators.pop(); // remove '('
		}
		else if (isAlpha(c) || isDigit(c))
		{
			postfix += c;
		}
		else if (c == '-' || c == '+' || c == '*' || c == '/')
		{
			while (!operators.empty() && GetPriority(operators.top()) >= GetPriority(c))
			{
				postfix += operators.top();
				operators.pop();
			}

			operators.push(c);
		}
	}

	while (!operators.empty()) // Add all remaining operators
	{
		postfix += operators.top();
		operators.pop();
	}
}

static int GetPriority(char op)
{
	if (op == '*' || op == '/')
		return 2;
	if (op == '+' || op == '-')
		return 1;
	return 0;  // For all other characters
}

void BuiltBinaryTree(const std::string& postfix, Node*& root)
{
	std::stack<Node*> NodeStack;

	for (char c : postfix)
	{
		if (isAlpha(c) || isDigit(c))
		{
			NodeStack.push(new Node(std::string(1, c)));
		}
		else if (isOperator(c))
		{
			if (NodeStack.size() < 2)
			{
				std::cerr << "========================================================\n"
						  << "                         Error!                         \n"
						  << "========================================================\n"
						  << "  Not enough operands for operator '" << c << "'         \n"
						  << "  Each operator requires exactly 2 operands             \n"
						  << "  Example:  'a b *' або '2 3 +'                          \n"
						  << "========================================================\n";

				// Clear stack (memory leak prevention)
				while (!NodeStack.empty())
				{
					delete NodeStack.top();
					NodeStack.pop();
				}

				WaitForEnter();
				ClearConsole();

				return;
			}

			Node* right = NodeStack.top();
			NodeStack.pop();

			Node* left = NodeStack.top();
			NodeStack.pop();


			Node* operatorNode = new Node(std::string(1, c));
			operatorNode->right = right;
			operatorNode->left = left;

			NodeStack.push(operatorNode);
		}
	}

	// Final expression validation
	if (NodeStack.size() != 1)
	{
		std::cerr << "========================================================\n"
				  << "                         Error!                         \n"
				  << "             Postfix expression is malformed.           \n"
				  << "========================================================\n";

		// Clear stack (memory leak prevention)
		while (!NodeStack.empty())
		{
			delete NodeStack.top();
			NodeStack.pop();
		}

		WaitForEnter();
		ClearConsole();
		return;
	}

	root = NodeStack.top();
}

static void RemoveTree(Node*& root)
{
	if (root == nullptr)
		return;

	// First delete child nodes
	RemoveTree(root->left);
	RemoveTree(root->right);

	// Then delete current node
	delete root;
	root = nullptr;
}

static void ConfirmAndDeleteTree(std::string& postfix, Node*& root)
{
	if (!postfix.empty())
	{
		char choice;
		do
		{
			std::cout << "========================================================\n"
					  << "    Are you sure you want to delete the binary tree?   \n"
					  << "========================================================\n"

					  << "Yes/No (y/n): ";

			std::cin >> choice;

			ClearConsole();

		} while (!(choice == 'y' || choice == 'n'));

		if (choice == 'y')
		{
			postfix.clear();
			RemoveTree(root);
			std::cout << "========================================================\n"
					  << "           Binary tree successfully deleted!            \n"
					  << "========================================================\n";
			WaitForEnter();
		}
		else
		{
			std::cout << "========================================================\n"
					  << "              Binary tree not deleted!                  \n"
					  << "========================================================\n";
			WaitForEnter();
		}
	}
	else
	{
		std::cerr << "========================================================\n"
			  	  << "             Binary tree not created yet!               \n"
				  << "========================================================\n";
		WaitForEnter();
	}
}

void TraverseTreeMenu(Node*& root)
{
	if (root != nullptr)
	{
		std::cout << "========================================================\n"
				  << "                 Binary Tree Traversals                 \n"
				  << "========================================================\n\n";

		std::cout << "Pre-order: "; preOrder(root); std::cout << "       \n";
		std::cout << "Post-order: "; postOrder(root); std::cout << "   \n\n";

		std::cout << "========================================================\n";

		WaitForEnter();
	}
	else
	{
		std::cerr << "========================================================\n"
				  << "              Binary tree not created yet!              \n"
				  << "========================================================\n";
		WaitForEnter();
	}
}

static void preOrder(Node* root)
{
	if (root == nullptr) return;

	std::cout << root->data << " ";
	preOrder(root->left);
	preOrder(root->right);
}

static void postOrder(Node* root)
{
	if (root == nullptr) return;

	postOrder(root->left);
	postOrder(root->right);

	std::cout << root->data << " ";
}

int main()
{
	ProgramMenu();
	return 0;
}