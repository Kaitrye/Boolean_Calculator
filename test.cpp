#include <string>
#include <gtest/gtest.h>

#include "boolexpr.h"

TEST (Create, default_constructor)
{
	BooleanExpression x;
	BooleanExpression y = "0";

	EXPECT_EQ (BooleanExpression ("0"), x);
	EXPECT_EQ (y, x);
}

TEST (Create, copy_constructor)
{
	BooleanExpression x = "1";
	BooleanExpression y = x;

	EXPECT_EQ (y, x);
	EXPECT_EQ ("1", y);
}

TEST (Create, copy_constructor_real_copy_to)
{
	BooleanExpression x = "0";
	BooleanExpression y = x;
	x = "1";

	EXPECT_EQ ("0", y);
}

TEST (Create, copy_constructor_real_copy_from)
{
	BooleanExpression x = "0";
	BooleanExpression y = x;
	y = "1";

	EXPECT_EQ ("0", x);
}

TEST (Create, move_constructor)
{
	BooleanExpression x = "1";
	BooleanExpression y = std::move (x);

	EXPECT_FALSE (y == x);
	EXPECT_FALSE ("1" == x);
	EXPECT_EQ ("1", y);
}

TEST (Create, Spaces)
{
	BooleanExpression a ("x1&x2");
	BooleanExpression b ("x1 & x2");
	BooleanExpression c ("        x1  &  x2");
	BooleanExpression d ("x1   &         x2       ");
	BooleanExpression e ("x1 \t\t\t\t\t & \t\t x2");

	EXPECT_EQ (a, b);
	EXPECT_EQ (a, c);
	EXPECT_EQ (a, d);
	EXPECT_EQ (a, e);
}

TEST (Create, errors)
{
	EXPECT_THROW (BooleanExpression (""), const char*);
	EXPECT_THROW (BooleanExpression ("a & b"), const char*);
	EXPECT_THROW (BooleanExpression ("x1 $ x2"), const char*);
	EXPECT_THROW (BooleanExpression ("~x"), const char*);
	EXPECT_THROW (BooleanExpression ("x12345"), const char*);
	EXPECT_THROW (BooleanExpression ("(x1 & x2"), const char*);
	EXPECT_THROW (BooleanExpression ("x1 & x2)"), const char*);
	EXPECT_THROW (BooleanExpression ("x1x2"), const char*);
	EXPECT_THROW (BooleanExpression ("x1 x2 & x3 x4"), const char*);
	EXPECT_THROW (BooleanExpression ("{x1}"), const char*);
}

TEST (Assignments, assignment_operator)
{
	BooleanExpression a = "1";
	BooleanExpression b = "0";
	b = a;

	EXPECT_TRUE (a == b);
}

TEST (Assignments, self_assignment)
{
	BooleanExpression a = "1";
	a = a;

	EXPECT_TRUE (a == "1");
}

TEST (Assignments, assignment_return_value)
{
	BooleanExpression a = "1";
	BooleanExpression b = "0";
	(a = b) = a;

	EXPECT_TRUE (a == "0");
	EXPECT_TRUE (b == "0");
}

TEST (To_string, all_basic_operations)
{
	BooleanExpression unit ("1");
	BooleanExpression zero ("0");

	BooleanExpression pos ("x1");
	BooleanExpression neg ("~x1");
	BooleanExpression conj ("x1 & x2");
	BooleanExpression dis ("x1 v x2");
	BooleanExpression add ("x1 + x2");
	BooleanExpression impl ("x1 > x2");
	BooleanExpression inv ("x1 < x2");
	BooleanExpression equ ("x1 = x2");
	BooleanExpression sheff ("x1 | x2");
	BooleanExpression pier ("x1 ^ x2");

	EXPECT_EQ (std::string (unit), "1");
	EXPECT_EQ (std::string (zero), "0");
	EXPECT_EQ (std::string (pos), "x1");
	EXPECT_EQ (std::string (neg), "~x1");
	EXPECT_EQ (std::string (conj), "x1 & x2");
	EXPECT_EQ (std::string (dis), "x1 v x2");
	EXPECT_EQ (std::string (add), "x1 + x2");
	EXPECT_EQ (std::string (impl), "x1 > x2");
	EXPECT_EQ (std::string (inv), "x1 < x2");
	EXPECT_EQ (std::string (equ), "x1 = x2");
	EXPECT_EQ (std::string (sheff), "x1 | x2");
	EXPECT_EQ (std::string (pier), "x1 ^ x2");
}

TEST (To_string, compound_expressions)
{
	BooleanExpression a ("x1 & x2 & x3 & x4 & x5");
	EXPECT_EQ (std::string (a), "x1 & x2 & x3 & x4 & x5");

	a = ("x1 v x2 v x3 v x4 v x5");
	EXPECT_EQ (std::string (a), "x1 v x2 v x3 v x4 v x5");

	a = ("x1 & x2 v x3 & x4 v x5");
	EXPECT_EQ (std::string (a), "x1 & x2 v x3 & x4 v x5");

	a = ("~x1 & x2 v ~x3 & x4 v x5");
	EXPECT_EQ (std::string (a), "~x1 & x2 v ~x3 & x4 v x5");

	a = ("~x1 | x2 v ~x3 & x4 v x5");
	EXPECT_EQ (std::string (a), "~x1 | x2 v ~x3 & x4 v x5");

	a = ("~x1 & x2 v ~x3 & x4 ^ x5");
	EXPECT_EQ (std::string (a), "~x1 & x2 v ~x3 & x4 ^ x5");

	a = ("~x1 & x2 v ~x3 & x4 v x5 + x6");
	EXPECT_EQ (std::string (a), "~x1 & x2 v ~x3 & x4 v x5 + x6");

	a = ("~x1 & x2 v ~x3 & x4 v x5 > ~x6");
	EXPECT_EQ (std::string (a), "~x1 & x2 v ~x3 & x4 v x5 > ~x6");

	a = ("~x1 & x2 v ~x3 & x4 < x5");
	EXPECT_EQ (std::string (a), "~x1 & x2 v ~x3 & x4 < x5");

	a = ("~x1 & x2 | ~x3 & x4 = x5 = x6");
	EXPECT_EQ (std::string (a), "~x1 & x2 | ~x3 & x4 = x5 = x6");
}

TEST (Table, all_basic_operations)
{
	BooleanExpression unit ("1");
	BooleanExpression zero ("0");

	BooleanExpression pos ("x1");
	BooleanExpression neg ("~x1");
	BooleanExpression conj ("x1 & x2");
	BooleanExpression dis ("x1 v x2");
	BooleanExpression add ("x1 + x2");
	BooleanExpression impl ("x1 > x2");
	BooleanExpression inv ("x1 < x2");
	BooleanExpression equ ("x1 = x2");
	BooleanExpression sheff ("x1 | x2");
	BooleanExpression pier ("x1 ^ x2");

	EXPECT_EQ (unit.table (), "1");
	EXPECT_EQ (zero.table (), "0");
	EXPECT_EQ (pos.table (), "01");
	EXPECT_EQ (neg.table (), "10");
	EXPECT_EQ (conj.table (), "0001");
	EXPECT_EQ (dis.table (), "0111");
	EXPECT_EQ (add.table (), "0110");
	EXPECT_EQ (impl.table (), "1101");
	EXPECT_EQ (inv.table (), "1011");
	EXPECT_EQ (equ.table (), "1001");
	EXPECT_EQ (sheff.table (), "1110");
	EXPECT_EQ (pier.table (), "1000");
}

TEST (Table, compound_expressions)
{
	BooleanExpression a ("x1 & x2 & x3 & x4 & x5");
	EXPECT_EQ (a.table (), "00000000000000000000000000000001");

	a = ("x1 v x2 v x3 v x4 v x5");
	EXPECT_EQ (a.table (), "01111111111111111111111111111111");

	a = ("x1 & x2 v x3 & x4 v x5");
	EXPECT_EQ (a.table (), "01010111010101110101011111111111");

	a = ("~x1 & x2 v ~x3 & x4 v x5");
	EXPECT_EQ (a.table (), "01110101111111110111010101110101");

	a = ("~x1 | x2 v ~x3 & x4 v x5");
	EXPECT_EQ (a.table (), "10001010000000001111111111111111");

	a = ("~x1 & x2 v ~x3 & x4 ^ x5");
	EXPECT_EQ (a.table (), "10001010000000001000101010001010");

	a = ("~x1 & x2 v ~x3 & x4 v x5 + x6");
	EXPECT_EQ (a.table (), "0110101001100110101010101010101001101010011001100110101001100110");

	a = ("~x1 & x2 v ~x3 & x4 v x5 > ~x6");
	EXPECT_EQ (a.table (), "1110101011101110101010101010101011101010111011101110101011101110");

	a = ("~x1 & x2 v ~x3 & x4 < x5");
	EXPECT_EQ (a.table (), "10111010111111111011101010111010");

	a = ("~x1 & x2 | ~x3 & x4 = x5 = x6");
	EXPECT_EQ (a.table (), "1111111111111111100101101001100111111111111111111111111111111111");
}

TEST (Table, dummy_variables)
{
	BooleanExpression a ("x4 & x5");
	EXPECT_EQ (a.table (), "0001");

	a = ("x1 v x3 v x5");
	EXPECT_EQ (a.table (), "01111111");

	a = ("~x3 v x5");
	EXPECT_EQ (a.table (), "1101");

	a = ("~x1 & x2 v ~x3 & x4 = x6");
	EXPECT_EQ (a.table (), "10011010010101011001101010011010");
}

TEST (Table, different_order_variables)
{
	BooleanExpression a ("x5 & x4 & x3 & x2 & x1");
	EXPECT_EQ (a.table (), "00000000000000000000000000000001");

	a = ("~x5 & x2 v ~x3 & x4 ^ x1");
	EXPECT_EQ (a.table (), "11001111010001010000000000000000");

	a = ("~x5 & x4 v ~x3 & x2 v x1 + x6");
	EXPECT_EQ (a.table (), "0101100101011001101010100101100110101010101010101010101010101010");
}

TEST (Table, repeat_variables)
{
	BooleanExpression a ("x1 & x1 & x1 & x1 & x1");
	EXPECT_EQ (a.table (), "01");

	a = ("x1 v x2 v x1 v x2");
	EXPECT_EQ (a.table (), "0111");

	a = ("x1 & x2 v x3 & x1 v x2");
	EXPECT_EQ (a.table (), "00110111");
}

TEST (Table, brackets)
{
	BooleanExpression a ("x1 & (x2 & x3) & (x4 & x5)");
	EXPECT_EQ (a.table (), "00000000000000000000000000000001");

	a = ("((((x1 & x2))))");
	EXPECT_EQ (a.table (), "0001");

	a = ("x1 & ((x2 & x3) & (x4 & x5))");
	EXPECT_EQ (a.table (), "00000000000000000000000000000001");

	a = ("(x1) & (~x2) v (x3) & (~x4) v (x5)");
	EXPECT_EQ (a.table (), "01011101010111011111111101011101");

	a = ("~x1 & x2 v ~(x3 & x4) v x5");
	EXPECT_EQ (a.table (), "11111101111111111111110111111101");

	a = ("~((x1 | x2) v ~x3) & x4 v x5");
	EXPECT_EQ (a.table (), "01010101010101010101010101010111");
}

TEST (Zhegalkin, all_basic_operations)
{
	BooleanExpression unit ("1");
	BooleanExpression zero ("0");

	BooleanExpression pos ("x1");
	BooleanExpression neg ("~x1");
	BooleanExpression conj ("x1 & x2");
	BooleanExpression dis ("x1 v x2");
	BooleanExpression add ("x1 + x2");
	BooleanExpression impl ("x1 > x2");
	BooleanExpression inv ("x1 < x2");
	BooleanExpression equ ("x1 = x2");
	BooleanExpression sheff ("x1 | x2");
	BooleanExpression pier ("x1 ^ x2");

	EXPECT_EQ (std::string (unit.zhegalkin ()), "1");
	EXPECT_EQ (std::string (zero.zhegalkin ()), "0");
	EXPECT_EQ (std::string (pos.zhegalkin ()), "x1");
	EXPECT_EQ (std::string (neg.zhegalkin ()), "1 + x1");
	EXPECT_EQ (std::string (conj.zhegalkin ()), "x1 & x2");
	EXPECT_EQ (std::string (dis.zhegalkin ()), "x2 + x1 + x1 & x2");
	EXPECT_EQ (std::string (add.zhegalkin ()), "x2 + x1");
	EXPECT_EQ (std::string (impl.zhegalkin ()), "1 + x1 + x1 & x2");
	EXPECT_EQ (std::string (inv.zhegalkin ()), "1 + x2 + x1 & x2");
	EXPECT_EQ (std::string (equ.zhegalkin ()), "1 + x2 + x1");
	EXPECT_EQ (std::string (sheff.zhegalkin ()), "1 + x1 & x2");
	EXPECT_EQ (std::string (pier.zhegalkin ()), "1 + x2 + x1 + x1 & x2");
}

TEST (Zhegalkin, compound_expressions)
{
	BooleanExpression a ("x1 & x2 & x3 & x4 & x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	a = ("x1 v x2 v x3 v x4 v x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x5 + x4 + x4 & x5 + x3 + x3 & x5 + x3 & x4 + x3 & x4 & x5 + x2 + x2 & x5 + x2 & x4 + \
x2 & x4 & x5 + x2 & x3 + x2 & x3 & x5 + x2 & x3 & x4 + x2 & x3 & x4 & x5 + x1 + x1 & x5 + x1 & x4 + x1 & x4 & x5 + \
x1 & x3 + x1 & x3 & x5 + x1 & x3 & x4 + x1 & x3 & x4 & x5 + x1 & x2 + x1 & x2 & x5 + x1 & x2 & x4 + x1 & x2 & x4 & x5 + \
x1 & x2 & x3 + x1 & x2 & x3 & x5 + x1 & x2 & x3 & x4 + x1 & x2 & x3 & x4 & x5");

	a = ("x1 & x2 v x3 & x4 v x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x5 + x3 & x4 + x3 & x4 & x5 + x1 & x2 + x1 & x2 & x5 + x1 & x2 & x3 & x4 + x1 & x2 & x3 & x4 & x5");

	a = ("x1 > x2 < ~x3 + x4 + x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "1 + x1 + x1 & x5 + x1 & x4 + x1 & x3 + x1 & x2 + x1 & x2 & x5 + x1 & x2 & x4 + x1 & x2 & x3");

	a = ("x1 & ~x2 v x3 = x4 | x5 ^ x6");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x5 + x5 & x6 + x4 & x5 + x4 & x5 & x6 + x3 & x5 + x3 & x5 & x6 + x1 & x5 + x1 & x5 & x6 + x1 & x3 & x5 + \
x1 & x3 & x5 & x6 + x1 & x2 & x5 + x1 & x2 & x5 & x6 + x1 & x2 & x3 & x5 + x1 & x2 & x3 & x5 & x6");
}

TEST (Zhegalkin, dummy_variables)
{
	BooleanExpression a ("x4 & x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x4 & x5");

	a = ("x1 v x3 v x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x5 + x3 + x3 & x5 + x1 + x1 & x5 + x1 & x3 + x1 & x3 & x5");

	a = ("~x3 v x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "1 + x3 + x3 & x5");

	a = ("~x1 & x2 v ~x3 & x4 = x6");
	EXPECT_EQ (std::string (a.zhegalkin ()), "1 + x6 + x4 + x3 & x4 + x2 + x2 & x4 + x2 & x3 & x4 + x1 & x2 + x1 & x2 & x4 + x1 & x2 & x3 & x4");
}

TEST (Zhegalkin, different_order_variables)
{
	BooleanExpression a ("x5 & x4 & x3 & x2 & x1");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	BooleanExpression b ("x1 & x5 & x2 & x4 & x3");
	EXPECT_EQ (std::string (b.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	BooleanExpression c ("x1 & x3 & x2 & x5 & x4");
	EXPECT_EQ (std::string (c.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	a = ("~x5 & x2 v ~x3 & x4 ^ x1");
	EXPECT_EQ (std::string (a.zhegalkin ()), "1 + x4 + x3 & x4 + x2 + x2 & x5 + x2 & x4 + x2 & x4 & x5 + x2 & x3 & x4 + x2 & x3 & x4 & x5 + \
x1 + x1 & x4 + x1 & x3 & x4 + x1 & x2 + x1 & x2 & x5 + x1 & x2 & x4 + x1 & x2 & x4 & x5 + x1 & x2 & x3 & x4 + x1 & x2 & x3 & x4 & x5");
}

TEST (Zhegalkin, repeat_variables)
{
	BooleanExpression a ("x1 & x1 & x1 & x1 & x1");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1");

	a = ("x1 v x2 v x1 v x2");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x2 + x1 + x1 & x2");

	a = ("x1 & x2 v x3 & x1 v x2");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x2 + x1 & x3 + x1 & x2 & x3");
}

TEST (Zhegalkin, brackets)
{
	BooleanExpression a ("x1 & (x2 & x3) & (x4 & x5)");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	a = ("((((x1 & x2))))");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1 & x2");

	a = ("x1 & ((x2 & x3) & (x4 & x5))");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x1 & x2 & x3 & x4 & x5");

	a = ("(x1) & (~x2) v (x3) & (~x4) v (x5)");
	EXPECT_EQ (std::string (a.zhegalkin ()), "x5 + x3 + x3 & x5 + x3 & x4 + x3 & x4 & x5 + x1 + x1 & x5 + x1 & x3 + x1 & x3 & x5 + \
x1 & x3 & x4 + x1 & x3 & x4 & x5 + x1 & x2 + x1 & x2 & x5 + x1 & x2 & x3 + x1 & x2 & x3 & x5 + x1 & x2 & x3 & x4 + x1 & x2 & x3 & x4 & x5");

	a = ("~x1 & x2 v ~(x3 & x4) v x5");
	EXPECT_EQ (std::string (a.zhegalkin ()), "1 + x3 & x4 + x3 & x4 & x5 + x2 & x3 & x4 + x2 & x3 & x4 & x5 + x1 & x2 & x3 & x4 + x1 & x2 & x3 & x4 & x5");
}

TEST (IsFullSystem, true_min_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("x1 | x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("x1 ^ x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 & x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 v x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 > x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("1"), BooleanExpression ("x1 + x2"), BooleanExpression ("x1 & x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("0"), BooleanExpression ("x1 & x2"), BooleanExpression ("x1 = x2") };
	EXPECT_TRUE (isFullSystem (system));
}

TEST (IsFullSystem, true_basic_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("x1 | x2"), BooleanExpression ("x1 & x2"), BooleanExpression ("0"), BooleanExpression ("1") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("x1 ^ x2"), BooleanExpression ("x1 v x2"), BooleanExpression ("x1 > x2"), BooleanExpression ("1") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 & x2"), BooleanExpression ("x1 v x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 v x2"), BooleanExpression ("x1 < x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("1"), BooleanExpression ("x1 + x2"), BooleanExpression ("x1 & x2"), BooleanExpression ("x1 = x2") };
	EXPECT_TRUE (isFullSystem (system));
}

TEST (IsFullSystem, true_compound_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("1"), BooleanExpression ("~x1"), BooleanExpression ("x1 + x2 + x1 & x2 + x2 & x3 + x3 & x1"), BooleanExpression ("x1 & x2 & (x1 + x2)") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("0"), BooleanExpression ("x1 + x2"), BooleanExpression ("x1 > x2"), BooleanExpression ("x1 & x2 = x3 & x1") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("1"), BooleanExpression ("0"), BooleanExpression ("x1 + x2 + x3"), BooleanExpression ("x1 & x2 + x2 & x3 + x3 & x1"), BooleanExpression ("x1 & x2 + x3"), BooleanExpression ("x1 v x2") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("x1 & x2 + x3"), BooleanExpression ("x1 + x2 + 1"), BooleanExpression ("x1 & ~x2"), BooleanExpression ("~x1") };
	EXPECT_TRUE (isFullSystem (system));

	system = { BooleanExpression ("x1 + x2"), BooleanExpression ("x1 = x2"), BooleanExpression ("0"), BooleanExpression ("x1 + x2 + x3"), BooleanExpression ("x1 & x2"), BooleanExpression ("x1 > x2") };
	EXPECT_TRUE (isFullSystem (system));
}

TEST (IsFullSystem, false_min_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("0") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("~x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 & x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 v x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 > x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 < x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 + x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 = x2") };
	EXPECT_FALSE (isFullSystem (system));
}

TEST (IsFullSystem, false_basic_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("x1 & x2"), BooleanExpression ("0") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 v x2"), BooleanExpression ("x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1 + x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("~x1"), BooleanExpression ("x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("1"), BooleanExpression ("x1 + x2"), BooleanExpression ("x1 = x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("0"), BooleanExpression ("x1 & x2"), BooleanExpression ("x1 v x2") };
	EXPECT_FALSE (isFullSystem (system));
}

TEST (IsFullSystem, false_compound_full_system)
{
	std::vector<BooleanExpression> system = { BooleanExpression ("x1 & x2"), BooleanExpression ("x1 & x2 & x3 & x4 & x5 & x6") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 v x2 v 1"), BooleanExpression ("x1 + x2 + x1 & x2 + x2 & x3 + x3 & x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 + x2 + x1 & x2 + x2 & x3 + x3 & x1"), BooleanExpression ("x1 & x2 & (x1 + x2)") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("1"), BooleanExpression ("x1 & x2 = x3 & x1") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 + x2 + x3"), BooleanExpression ("x1 & x2 + x2 & x3 + x3 & x1"), BooleanExpression ("x1 & x2 + x3") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x1 & x2 + x3"), BooleanExpression ("x1 & ~x2") };
	EXPECT_FALSE (isFullSystem (system));

	system = { BooleanExpression ("x9 + x2"), BooleanExpression ("x1 + x2 + x3"), BooleanExpression ("x1 & x2") };
	EXPECT_FALSE (isFullSystem (system));
}