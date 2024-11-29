#ifndef ExprNodes_
#define ExprNodes_

#include <iostream>
#include <string>
#include "AVLTree.h"

class VarValue
{
	const unsigned char name_;
	bool data_;

public:
	VarValue (unsigned char x, bool val) : name_ (x), data_ (val)
	{}

	void set (bool x)
	{
		data_ = x;
	}
	bool get () const
	{
		return data_;
	}

	unsigned char name ()
	{
		return name_;
	}

	bool operator<(const VarValue& other) const
	{
		return name_ < other.name_;
	}
	bool operator==(const VarValue& other) const
	{
		return name_ == other.name_;
	}
};

//--------------------------------

class ExprNode
{
public:
	virtual bool calc(AVLTree<VarValue>& workspace) const = 0;
	virtual ~ExprNode() {};
};

//--------------------------------

class NumNode : public ExprNode
{
	const bool num_;
public:
	NumNode(bool x) : num_(x) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return num_;
	}
};

//--------------------------------

class VarNode : public ExprNode
{
	const unsigned char name_;

public:
	VarNode (unsigned char x, AVLTree<VarValue>& workspace) : name_(x)
	{
		VarValue tmp(name_, false);

		AVLTree<VarValue>::iterator pos = workspace.find(tmp);
		if (pos == workspace.end ())
		{
			workspace.insert(tmp);
		}
	}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		AVLTree<VarValue>::iterator pos = workspace.find (VarValue(name_, false));
		return (*pos).get();
	}
};

//--------------------------------

class BinNode : public ExprNode
{
protected:
	ExprNode* left_; // левый операнд.
	ExprNode* right_; // правый операнд.

public:
	BinNode(ExprNode* L, ExprNode* R) : left_(L), right_(R) {}
	~BinNode ()
	{
		delete left_;
		delete right_;
	}
};

//--------------------------------

class UnarNode : public ExprNode
{
protected:
	ExprNode* next_; // операнд.

public:
	UnarNode(ExprNode* node) : next_(node) {}
	~UnarNode ()
	{
		delete next_;
	}
};

//--------------------------------

class NegNode : public UnarNode // отрицание
{
public:
	NegNode(ExprNode* node) : UnarNode(node) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return !next_->calc(workspace);
	}
};

//--------------------------------

class ConjNode : public BinNode // конъюнкция
{
public:
	ConjNode(ExprNode* L, ExprNode* R) : BinNode(L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc(workspace) && right_->calc(workspace);
	}
};

//--------------------------------

class DisNode : public BinNode // дизъюнкция
{
public:
	DisNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc (workspace) || right_->calc (workspace);
	}
};

//--------------------------------

class AddNode : public BinNode // сложение по модулю
{
public:
	AddNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc (workspace) != right_->calc (workspace);
	}
};

//--------------------------------

class ImplNode : public BinNode // импликация
{
public:
	ImplNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc (workspace) <= right_->calc (workspace);
	}
};

//--------------------------------

class InvNode : public BinNode // обратная импликация
{
public:
	InvNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc (workspace) >= right_->calc (workspace);
	}
};

//--------------------------------

class EquNode : public BinNode // эквиваленция
{
public:
	EquNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return left_->calc (workspace) == right_->calc (workspace);
	}
};

//--------------------------------

class SheffNode : public BinNode // штрих Шеффера
{
public:
	SheffNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return !(left_->calc (workspace) && right_->calc (workspace));
	}
};

//--------------------------------

class PierNode : public BinNode // стрелка Пирса
{
public:
	PierNode (ExprNode* L, ExprNode* R) : BinNode (L, R) {}
	bool calc (AVLTree<VarValue>& workspace) const
	{
		return !(left_->calc (workspace) || right_->calc (workspace));
	}
};

#endif // ExprNodes_