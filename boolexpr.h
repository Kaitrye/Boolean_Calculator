#ifndef BoolExpr_
#define BoolExpr_

#include <string>
#include <vector>
#include "exprnodes.h"

class BooleanExpression
{
	AVLTree<VarValue> workspace_;
	ExprNode* root_;
	const char* str_;
	char* str_infix;

	int InfixFilter (const char*, char*);
	void Infix2Postfix (const char*, char*);
	ExprNode* Postfix2Tree (const char*);
	bool calc ();

public:
	BooleanExpression (const char* str = "0");
	BooleanExpression (const BooleanExpression&);
	BooleanExpression (BooleanExpression&&) noexcept;

	std::string table ();
	BooleanExpression zhegalkin ();
	
	BooleanExpression& operator=(const BooleanExpression&);
	BooleanExpression& operator=(BooleanExpression&&) noexcept;
	bool is_equal (const BooleanExpression&) const;
	operator std::string () const;

	~BooleanExpression ();
};

bool operator==(const BooleanExpression&, const BooleanExpression&);
bool check_mono (const std::string&, int, int);
bool isFullSystem (std::vector<BooleanExpression>&);

#endif // BoolExpr_