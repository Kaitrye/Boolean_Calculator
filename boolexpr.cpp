#include <cstring>
#include "boolexpr.h"
#include <stack>

BooleanExpression::BooleanExpression (const char* str) : root_ (nullptr), str_(str)
{
	size_t len = strlen (str);
	len = (len < 10) ? 20 : 2 * len;

	str_infix = new char[len];
	char* str_postfix = new char[len];

	try
	{
		InfixFilter (str, str_infix);
		Infix2Postfix (str_infix, str_postfix);
		root_ = Postfix2Tree (str_postfix);

	} catch (...)
	{
		delete[] str_postfix;
		throw;
	}

	delete[] str_postfix;
}
BooleanExpression::BooleanExpression (const BooleanExpression& other) : root_ (nullptr), str_ (other.str_)
{
	size_t len = strlen (str_);
	len = (len < 10) ? 20 : 2 * len;

	str_infix = new char[len];
	char* str_postfix = new char[len];

	try
	{
		InfixFilter (str_, str_infix);
		Infix2Postfix (str_infix, str_postfix);
		root_ = Postfix2Tree (str_postfix);
		
	} catch (...)
	{
		delete[] str_postfix;
		throw;
	}

	delete[] str_postfix;
}
BooleanExpression::BooleanExpression (BooleanExpression&& other) noexcept
{
	workspace_ = other.workspace_;
	other.workspace_.empty ();

	str_ = other.str_;
	str_infix = other.str_infix;
	other.str_infix = new char[1];
	other.str_infix[0] = '\0';

	root_ = other.root_;
	other.root_ = nullptr;
}

bool BooleanExpression::calc ()
{
	if (root_ != nullptr)
	{
		return root_->calc (workspace_);
	}

	return 0;
}
std::string BooleanExpression::table ()
{
	size_t size = workspace_.size();
	if (size == 0)
	{
		return (calc ()) ? "1" : "0";
	}

	int deg = 1 << size;
	std::string result = "";

	for (unsigned long long i = 0; i < deg; ++i)
	{
		unsigned long long mask = 1ULL << (size - 1);
		AVLTree<VarValue>::iterator pos = workspace_.begin ();

		while (mask > 0)
		{
			bool bit = (i & mask) != 0;
			(*pos).set (bit);
			++pos;
			mask >>= 1;
		}

		result += (calc ()) ? "1" : "0";
	}

	return result;
}
BooleanExpression BooleanExpression::zhegalkin ()
{
	std::string triangle = table ();
	char result[1024];
	size_t index = 0;
	int global_count = 0;

	int size = triangle.size ();
	if (triangle[0] == '1')
	{
		result[index++] = '1';
		++global_count;
	}

	int start = 1;
	while (start < size)
	{
		for (int i = size - 1; i >= start; --i)
		{
			triangle[i] = (triangle[i] != triangle[i - 1]) ? '1' : '0';
		}

		if (triangle[start] == '1')
		{
			if (global_count > 0)
			{
				result[index++] = '+';
			}
			++global_count;

			size_t work_size = workspace_.size();
			int mask = 1 << (work_size - 1);
			AVLTree<VarValue>::iterator pos = workspace_.begin ();
			int count = 0;

			while (mask > 0)
			{
				if ((start & mask) != 0)
				{
					if (count > 0)
					{
						result[index++] = '&';
					}
					result[index++] = 'x';
					result[index++] = (*pos).name () - 127 + '0';
					++count;
				}

				++pos;
				mask >>= 1;
			}
		}
		++start;
	}

	if (index == 0)
	{
		result[index++] = '0';
	}
	result[index] = '\0';

	return BooleanExpression (result);
}

BooleanExpression& BooleanExpression::operator=(const BooleanExpression& other)
{
	if (this != &other)
	{
		delete root_;
		delete[] str_infix;
		workspace_.clear();

		root_ = nullptr;
		str_ = other.str_;

		size_t len = strlen (str_);
		len = (len < 10) ? 20 : 2 * len;

		str_infix = new char[len];
		char* str_postfix = new char[len];

		try
		{
			InfixFilter (str_, str_infix);
			Infix2Postfix (str_infix, str_postfix);
			root_ = Postfix2Tree (str_postfix);

		} catch (...)
		{
			delete[] str_postfix;
			throw;
		}

		delete[] str_postfix;
	}

	return *this;
}
BooleanExpression& BooleanExpression::operator=(BooleanExpression&& other) noexcept
{
	if (this != &other)
	{
		delete root_;
		delete[] str_infix;
		workspace_.clear();
		workspace_ = other.workspace_;
		other.workspace_.empty();

		str_ = other.str_;
		str_infix = other.str_infix;
		other.str_infix = new char[1];
		other.str_infix[0] = '\0';

		root_ = other.root_;
		other.root_ = nullptr;
	}

	return *this;
}
bool BooleanExpression::is_equal(const BooleanExpression& other) const
{
	size_t len = strlen(str_infix);
	if (len != strlen (other.str_infix))
	{
		return false;
	}

	for (size_t i = 0; i < len; ++i)
	{
		if (str_infix[i] != other.str_infix[i])
		{
			return false;
		}
	}

	return true;
}
bool operator==(const BooleanExpression& left, const BooleanExpression& right)
{
	return left.is_equal(right);
}

BooleanExpression::operator std::string () const
{
	size_t i = 0;
	std::string result = "";

	unsigned char ch;
	while ((ch = str_infix[i++]) != '\0')
	{
		if (ch > 127)
		{
			result += "x";
			result += ch - 127 + '0';
		} 
		else if (ch == '&' || ch == 'v' || ch == '+' || ch == '>' || ch == '<' || ch == '=' || ch == '|' || ch == '^')
		{
			result += " ";
			result += ch;
			result += " ";
		}
		else
		{
			result += ch;
		}
	}

	return result;
}

BooleanExpression::~BooleanExpression ()
{
	delete root_;
	delete[] str_infix;
}

int BooleanExpression::InfixFilter (const char* instr, char* outstr)
{
	size_t in = 0;
	size_t out = 0;
	int cnt = 0;

	char ch;
	while ((ch = instr[in++]) != '\0')
	{
		if (ch == ' ' || ch == '\t' || ch == '\r')
		{
			continue;
		}

		if (ch == 'x')
		{
			size_t tmp = in; 
			char buf[4];
			size_t in_buf = 0;

			while (isdigit ((ch = instr[in++])) != 0 && in_buf < 3) 
			{
				buf[in_buf++] = ch;
			}
			buf[in_buf] = '\0';
			--in;

			if (in_buf != 1)
			{
				throw "error";
			}

			int num = atoi(buf);
			outstr[out++] = 127 + num;
			++cnt;
		}
		else if (ch == '0' || ch == '1' || ch == '(' || ch == ')' || ch == '~' 
				 || ch == '&' || ch == 'v' || ch == '+' || ch == '>' || ch == '<' || ch == '=' || ch == '|' || ch == '^')
		{
			outstr[out++] = ch;
		}
		else
		{
			throw "error";
		}
	}

	outstr[out] = '\0';
	return cnt;
}

const unsigned char ActionsTable[][13] = {
  // 0 ~ & v + > < = | ^ ( ) P 
	{7,2,2,2,2,2,2,2,2,2,2,6,1}, // empty
	{3,2,3,3,3,3,3,3,3,3,2,3,1}, // ~
	{3,2,3,3,3,3,3,3,3,3,2,3,1}, // &
	{3,2,2,3,3,3,3,3,3,3,2,3,1}, // v
	{3,2,2,3,3,3,3,3,3,3,2,3,1}, // +
	{3,2,2,2,2,3,3,3,3,3,2,3,1}, // >
	{3,2,2,2,2,3,3,3,3,3,2,3,1}, // <
	{3,2,2,2,2,2,2,3,3,3,2,3,1}, // =
	{3,2,2,2,2,2,2,2,3,3,2,3,1}, // |
	{3,2,2,2,2,2,2,2,3,3,2,3,1}, // ^
	{5,2,2,2,2,2,2,2,2,2,2,4,1}, // (
};

/*
	1. Записываем символ в outstr.
	2. Добавляем символ в стек.
	3. Записываем символ из стека в outstr.
	4. Удаляем символ из стека.
	5. Ошибка с закрытой скобочкой.
	6. Ошибка с открытой скобочкой.
	7. Выход.
*/

int actionsRowNumber (char ch)
{
	switch (ch)
	{
	case '~': 
		return 1;
	case '&': 
		return 2;
	case 'v': 
		return 3;
	case '+': 
		return 4;
	case '>': 
		return 5;
	case '<': 
		return 6;
	case '=':
		return 7;
	case '|': 
		return 8;
	case '^': 
		return 9;
	case '(': 
		return 10;
	}

	throw "error";
}
int actionsColNumber (unsigned char ch)
{
	switch (ch)
	{
	case 0:
		return 0;
	case '~': 
		return 1;
	case '&': 
		return 2;
	case 'v': 
		return 3;
	case '+': 
		return 4;
	case '>': 
		return 5;
	case '<': 
		return 6;
	case '=':
		return 7;
	case '|': 
		return 8;
	case '^': 
		return 9;
	case '(': 
		return 10;
	case ')': 
		return 11;
	}

	if (ch >= 128 || ch == '0' || ch == '1')
	{
		return 12;
	}

	throw "error";
}

void BooleanExpression::Infix2Postfix (const char* instr, char* outstr)
{
	size_t in = 0;
	size_t out = 0;

	unsigned char action;
	std::stack<char> S;
	size_t row;
	size_t col;

	do
	{
		col = actionsColNumber(instr[in]);
		row = (S.empty()) ? 0 : actionsRowNumber(S.top());
		action = ActionsTable[row][col];

		switch (action)
		{
		    case 1:
				outstr[out++] = instr[in++];
				break;
			case 2:
				S.push(instr[in++]);
				break;
			case 3:
				outstr[out++] = S.top();
				S.pop();
				break;
			case 4:
				S.pop();
				++in;
				break;
			case 5:
				throw "error";
				break;
			case 6:
				throw "error";
				break;
			case 7:
				outstr[out] = '\0';
				break;
		}

	} while (action != 7);
}
ExprNode* BooleanExpression::Postfix2Tree (const char* str)
{
	size_t in = 0;

	std::stack<ExprNode*> S;
	ExprNode* result = nullptr;
	ExprNode* left = nullptr;
	ExprNode* right = nullptr;

	unsigned char ch;
	try
	{ 
		while ((ch = str[in++]) != '\0')
		{
			switch (ch)
			{
			case '~':
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top();
				S.pop();

				result = new NegNode(right);
				break;
			case '&':
				if (S.empty()) {
					throw 1;
				}
				right = S.top();
				S.pop();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new ConjNode(left, right);
				break;
			case 'v': 
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new DisNode (left, right);
				break;
			case '+': 
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new AddNode (left, right);
				break;
			case '=':
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new EquNode (left, right);
				break;
			case '>': 
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new ImplNode (left, right);
				break;
			case '<':
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new InvNode (left, right);
				break;
			case '|': 
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new SheffNode (left, right);
				break;
			case '^': 
				if (S.empty ())
				{
					throw 1;
				}
				right = S.top ();
				S.pop ();

				if (S.empty ())
				{
					throw 1;
				}
				left = S.top ();
				S.pop ();

				result = new PierNode (left, right);
				break;
			case '0':
				result = new NumNode (false);
				break;
			case '1':
				result = new NumNode (true);
				break;
			default:
				if (ch >= 128)
				{
					result = new VarNode(ch, workspace_);
				}
			}

			S.push(result);
		}

		if (S.size () != 1)
		{
			throw 1;
		}

		return S.top();
	}
	catch (int error)
	{
		while (!S.empty())
		{
			result = S.top();
			S.pop();
			delete result;
		}

		throw "error";
	}
}

bool check_mono (const std::string& str, int start, int size)
{
	int half_size = size >> 1;
	if (size <= 2)
	{
		return str[start] <= str[start + half_size];
	}

	bool res1 = check_mono (str, start, half_size);
	bool res2 = check_mono (str, start + half_size, half_size);

	if (res1 && res2)
	{
		int half = start + half_size;
		int end = start + size;

		for (int i = start; i < half; ++i)
		{
			for (int j = half; j < end; ++j)
			{
				if (str[i] > str[j])
				{
					return false;
				}
			}
		}
		return true;
	}

	return false;
}
bool isFullSystem (std::vector<BooleanExpression>& system)
{
	int no_unit_save = 0;
	int no_zero_save = 0;
	int no_self_dual = 0;
	int no_mono = 0;
	int no_linear = 0;

	for (auto pos = system.begin(); pos != system.end(); ++pos)
	{
		// Проверка каждой функции на 5 свойств.
		std::string tab = (*pos).table ();

		// no_zero_save
		if (tab.front() == '1')
		{
			++no_zero_save;
		}

		// no_unit_save
		if (tab.back() == '0')
		{
			++no_unit_save;
		}

		// no_self_dual
		bool is_self_dual = true;
		for (int i = 0, j = tab.size () - 1; i < j; ++i, --j)
		{
			if (tab[i] == tab[j])
			{
				is_self_dual = false;
				break;
			}
		}

		if (!is_self_dual)
		{
			++no_self_dual;
		}

		// no_mono
		if (!check_mono (tab, 0, tab.size ()))
		{
			++no_mono;
		}
		
		// no_linear
		std::string zh = std::string((*pos).zhegalkin());

		bool is_linear = true;
		for (auto ch : zh)
		{
			if (ch == '&')
			{
				is_linear = false;
				break;
			}
		}

		if (!is_linear)
		{
			++no_linear;
		}
	}

	if (no_unit_save > 0 && no_zero_save > 0 && no_self_dual > 0 && no_mono > 0 && no_linear > 0)
	{
		return true;
	}

	return false;
}

