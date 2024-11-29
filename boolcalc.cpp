#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "boolexpr.h"

void help ()
{
	std::cout << "Use \nboolcalc [-options] source_file_name destination_file_name\n";
	std::cout << "-table \t building a truth table\n";
	std::cout << "-zh \t construction of the Zhegalkin polynomial\n";
	std::cout << "-isfull \t check the system of functions for completeness\n";
	std::cout << "-h or ? \t getting help using the program\n";
}

int main(int argc, char* argv[]){

	if (argc < 2)
	{
		help();
		return 1;
	}

	if (strcmp (argv[1], "-table") == 0)
	{
		std::ifstream inFile(argv[2]);
		if (!inFile)
		{
			return 2;
		}

		std::ofstream outFile(argv[3]);
		if (!outFile)
		{
			inFile.close();
			return 3;
		}

		try 
		{
			char str[1024];
			BooleanExpression c;
			while (inFile.peek () != EOF)
			{
				inFile.getline (str, 1024);
				c = BooleanExpression (str);
				outFile << c.table () << std::endl;
			}
		}
		catch (const char* error)
		{
			outFile.close ();
			outFile.open (argv[3]);
			outFile << error;
		}

		inFile.close();
		outFile.close();
	}
	else if (strcmp (argv[1], "-zh") == 0)
	{
		std::ifstream inFile (argv[2]);
		if (!inFile)
		{
			return 2;
		}

		std::ofstream outFile (argv[3]);
		if (!outFile)
		{
			inFile.close ();
			return 3;
		}

		try 
		{
			char str[1024];
			BooleanExpression c;
			while (inFile.peek () != EOF)
			{
				inFile.getline (str, 1024);
				c = BooleanExpression (str);
				outFile << std::string (c.zhegalkin ()) << std::endl;
			}
		}
		catch (const char* error)
		{
			outFile.close();
			outFile.open(argv[3]);
			outFile << error;
		}

		inFile.close ();
		outFile.close ();
	}
	else if (strcmp (argv[1], "-isfull") == 0)
	{
		std::ifstream inFile (argv[2]);
		if (!inFile)
		{
			return 2;
		}

		std::ofstream outFile (argv[3]);
		if (!outFile)
		{
			inFile.close ();
			return 3;
		}

		try 
		{
			char str[1024];

			std::vector<BooleanExpression> system;
			BooleanExpression c;
			while (inFile.peek () != EOF)
			{
				inFile.getline (str, 1024);
				c = BooleanExpression(str);
				system.push_back(c);

			}

			if (isFullSystem (system))
			{
				outFile << "yes";
			}
			else
			{
				outFile << "no";
			}
		}
		catch (const char* error)
		{
			outFile << error;
		}

		inFile.close ();
		outFile.close ();
	}
	else if (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "?") == 0)
	{
		help();
	}
  
  return 0;
}