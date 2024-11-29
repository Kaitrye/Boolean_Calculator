#ifndef AVLTree_
#define AVLTree_

template <typename T>
class AVLTree
{
	struct Node
	{
		Node* left_;
		Node* right_;
		Node* parent_;
		T data_;
		signed char balance_;

		Node (const T& X) : left_ (nullptr), right_ (nullptr), parent_ (nullptr), data_ (X), balance_ (0) {}

		size_t height () const
		{
			size_t L = (left_ == nullptr) ? 0 : left_->height ();
			size_t R = (right_ == nullptr) ? 0 : right_->height ();

			return (L < R) ? R + 1 : L + 1;
		}

		Node* minimum ()
		{
			Node* now = this;
			while (now->left_ != nullptr)
			{
				now = now->left_;
			}

			return now;
		}

		Node* maximum ()
		{
			Node* now = this;
			while (now->right_ != nullptr)
			{
				now = now->right_;
			}

			return now;
		}

		Node* next ()
		{
			if (right_ != nullptr)
			{
				return right_->minimum ();
			}

			Node* now = this;
			Node* parent = now->parent_;

			while (parent != nullptr && now == parent->right_)
			{
				now = parent;
				parent = now->parent_;
			}

			return parent;
		}

		Node* prev ()
		{
			if (left_ != nullptr)
			{
				return left_->maximum ();
			}

			Node* now = this;
			Node* parent = now->parent_;

			while (parent != nullptr && now == parent->left_)
			{
				now = parent;
				parent = now->parent_;
			}

			return parent;
		}

		~Node ()
		{
			delete left_;
			delete right_;
		}
	};

	Node* root_; // корень дерева.
	size_t count_; // количество элементов в дереве.

	// функции поворотов для балансировки.
	Node* rightRotate (Node*);
	Node* leftRotate (Node*);
	Node* bigRightRotate (Node*);
	Node* bigLeftRotate (Node*);

public:
	AVLTree () : root_ (nullptr), count_ (0) {}

	size_t size () const
	{
		return count_;
	}
	size_t height () const
	{
		return root_->height ();
	}

	class iterator
	{
		Node* current_;

	public:
		iterator (Node* x = nullptr) : current_ (x) {}

		T& operator*()
		{
			return current_->data_;
		}

		bool operator==(const iterator& other) const
		{
			return (current_ == other.current_);
		}
		bool operator!=(const iterator& other) const
		{
			return (current_ != other.current_);
		}

		iterator& operator++()
		{
			if (current_ != nullptr)
			{
				current_ = current_->next ();
			} else
			{
				throw "Iterator: out of the bounds";
			}

			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp (current_);

			if (current_ != nullptr)
			{
				current_ = current_->next ();
			} else
			{
				throw "Iterator: out of the bounds";
			}

			return tmp;
		}

		friend class AVLTree;
	};

	iterator begin () const
	{
		if (root_ != nullptr)
		{
			return iterator (root_->minimum ());
		}

		return iterator ();
	}

	iterator end () const
	{
		return iterator ();
	}

	class reverse_iterator
	{
		Node* current_;

	public:
		reverse_iterator (Node* x = nullptr) : current_ (x)
		{}

		T& operator*()
		{
			return current_->data_;
		}

		bool operator==(const reverse_iterator& other) const
		{
			return (current_ == other.current_);
		}
		bool operator!=(const reverse_iterator& other) const
		{
			return (current_ != other.current_);
		}

		reverse_iterator& operator++()
		{
			if (current_ != nullptr)
			{
				current_ = current_->prev ();
			} else
			{
				throw "Iterator: out of the bounds";
			}

			return *this;
		}

		reverse_iterator operator++(int)
		{
			reverse_iterator tmp (current_);

			if (current_ != nullptr)
			{
				current_ = current_->prev ();
			} else
			{
				throw "Iterator: out of the bounds";
			}

			return tmp;
		}
	};

	reverse_iterator rbegin () const
	{
		if (root_ != nullptr)
		{
			return reverse_iterator (root_->maximum ());
		}

		return reverse_iterator ();
	}
	reverse_iterator rend () const
	{
		return reverse_iterator ();
	}

	void insert (const T&);
	iterator find (const T&) const;
	void erase (const iterator&);

	void clear ()
	{
		delete root_;
		root_ = nullptr;
		count_ = 0;
	}

	void empty ()
	{
		root_ = nullptr;
		count_ = 0;
	}

	~AVLTree ()
	{
		clear ();
	}
};

template <typename T>
void AVLTree<T>::insert (const T& x)
{
	++count_;
	if (root_ == nullptr)
	{
		root_ = new Node (x);
		return;
	}

	Node* now = root_;
	Node* parent;

	do
	{
		parent = now;
		if (x < now->data_)
		{
			now = now->left_;
		} else
		{
			now = now->right_;
		}

	} while (now != nullptr);

	now = new Node (x);
	now->parent_ = parent;

	if (x < parent->data_)
	{
		parent->left_ = now;
	} else
	{
		parent->right_ = now;
	}

	// Восстановление баланса
	do
	{
		if (now->parent_->left_ == now)
		{
			--now->parent_->balance_;
		} else
		{
			++now->parent_->balance_;
		}
		now = now->parent_;

		switch (now->balance_)
		{
		case 2:
			if (now->right_->balance_ == -1)
			{
				now = bigLeftRotate (now);
			} else
			{
				now = leftRotate (now);
			}
			break;
		case -2:
			if (now->left_->balance_ == 1)
			{
				now = bigRightRotate (now);
			} else
			{
				now = rightRotate (now);
			}
			break;
		}
	} while (now != root_ && now->balance_ != 0);
}

template<typename T>
typename AVLTree<T>::iterator AVLTree<T>::find (const T& x) const
{
	Node* now = root_;
	while (now != nullptr)
	{
		if (x == now->data_)
		{
			return now;
		}

		if (x < now->data_)
		{
			now = now->left_;
		} else
		{
			now = now->right_;
		}
	}

	return iterator (now);
}

template <typename T>
void AVLTree<T>::erase (typename AVLTree<T>::iterator const& pos)
{
	Node* toDelete = pos.current_;
	if (toDelete == nullptr)
	{
		return;
	}

	Node* alt;
	Node* notBalanced;

	if (toDelete->right_ == nullptr)
	{
		alt = toDelete->left_;
	} else if (toDelete->left_ == nullptr)
	{
		alt = toDelete->right_;
	} else
	{
		alt = toDelete->right_->minimum ();
		if (alt->parent_ != toDelete)
		{
			alt->parent_->left_ = nullptr;
			alt->right_ = toDelete->right_;
			toDelete->right_->parent_ = alt;
		}

		alt->left_ = toDelete->left_;
		toDelete->left_->parent_ = alt;
	}

	if (alt == nullptr)
	{
		notBalanced = toDelete->parent_;
		if (notBalanced != nullptr)
		{
			if (notBalanced->left_ == toDelete)
			{
				++notBalanced->balance_;
			} else
			{
				--notBalanced->balance_;
			}
		}
	} else
	{
		alt->balance_ = toDelete->balance_;
		if (alt->parent_ == toDelete)
		{
			notBalanced = alt;
			if (toDelete->left_ == alt)
			{
				++notBalanced->balance_;
			} else
			{
				--notBalanced->balance_;
			}
		} else
		{
			notBalanced = alt->parent_;
			++notBalanced->balance_;
		}
	}

	if (toDelete->parent_ == nullptr)
	{
		root_ = alt;
	} else
	{
		if (toDelete->parent_->left_ == toDelete)
		{
			toDelete->parent_->left_ = alt;
		} else
		{
			toDelete->parent_->right_ = alt;
		}
	}

	if (alt != nullptr)
	{
		alt->parent_ = toDelete->parent_;
	}

	toDelete->left_ = nullptr;
	toDelete->right_ = nullptr;
	delete toDelete;

	while (notBalanced != nullptr)
	{
		switch (notBalanced->balance_)
		{
		case 2:
			if (notBalanced->right_->balance_ == -1)
			{
				notBalanced = bigLeftRotate (notBalanced);
			} else
			{
				notBalanced = leftRotate (notBalanced);
			}
			break;
		case -2:
			if (notBalanced->left_->balance_ == 1)
			{
				notBalanced = bigRightRotate (notBalanced);
			} else
			{
				notBalanced = rightRotate (notBalanced);
			}
			break;
		}

		if (notBalanced->balance_ == -1 || notBalanced->balance_ == 1)
		{
			break;
		}

		if (notBalanced->parent_ != nullptr)
		{
			if (notBalanced->parent_->left_ == notBalanced)
			{
				++notBalanced->parent_->balance_;
			} else
			{
				--notBalanced->parent_->balance_;
			}
		}

		notBalanced = notBalanced->parent_;
	}
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::leftRotate (typename AVLTree<T>::Node* a)
{
	if (a->balance_ != 2)
	{
		return a;
	}

	Node* b = a->right_;

	if (b->balance_ == -1)
	{
		return a;
	}

	b->parent_ = a->parent_;
	if (a->parent_ == nullptr)
	{
		root_ = b;
	} else
	{
		if (a->parent_->left_ == a)
		{
			a->parent_->left_ = b;
		} else
		{
			a->parent_->right_ = b;
		}
	}

	a->right_ = b->left_;
	if (b->left_ != nullptr)
	{
		b->left_->parent_ = a;
	}

	a->parent_ = b;
	b->left_ = a;

	if (b->balance_ == 1)
	{
		a->balance_ = 0;
		b->balance_ = 0;
	} else
	{
		a->balance_ = 1;
		b->balance_ = -1;
	}

	return b;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::rightRotate (typename AVLTree<T>::Node* b)
{
	if (b->balance_ != -2)
	{
		return b;
	}

	Node* a = b->left_;

	if (a->balance_ == 1)
	{
		return b;
	}

	a->parent_ = b->parent_;
	if (b->parent_ == nullptr)
	{
		root_ = a;
	} else
	{
		if (b->parent_->left_ == b)
		{
			b->parent_->left_ = a;
		} else
		{
			b->parent_->right_ = a;
		}
	}

	b->left_ = a->right_;
	if (a->right_ != nullptr)
	{
		a->right_->parent_ = b;
	}

	a->right_ = b;
	b->parent_ = a;

	if (a->balance_ == -1)
	{
		a->balance_ = 0;
		b->balance_ = 0;
	} else
	{
		a->balance_ = 1;
		b->balance_ = -1;
	}

	return a;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::bigLeftRotate (typename AVLTree<T>::Node* a)
{
	if (a->balance_ != 2)
	{
		return a;
	}

	Node* c = a->right_;

	if (c->balance_ != -1)
	{
		return a;
	}

	Node* b = c->left_;

	c->left_ = b->right_;
	if (b->right_ != nullptr)
	{
		b->right_->parent_ = c;
	}

	b->right_ = c;
	c->parent_ = b;

	b->parent_ = a->parent_;
	if (a->parent_ == nullptr)
	{
		root_ = b;
	} else
	{
		if (a->parent_->left_ == a)
		{
			a->parent_->left_ = b;
		} else
		{
			a->parent_->right_ = b;
		}
	}

	a->right_ = b->left_;
	if (b->left_ != nullptr)
	{
		b->left_->parent_ = a;
	}

	a->parent_ = b;
	b->left_ = a;

	a->balance_ = 0;
	c->balance_ = 0;
	if (b->balance_ == 1)
	{
		a->balance_ = -1;
	} else if (b->balance_ == -1)
	{
		c->balance_ = 1;
	}
	b->balance_ = 0;

	return b;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::bigRightRotate (typename AVLTree<T>::Node* c)
{
	if (c->balance_ != -2)
	{
		return c;
	}

	Node* a = c->left_;

	if (a->balance_ != 1)
	{
		return c;
	}

	Node* b = a->right_;

	a->right_ = b->left_;
	if (b->left_ != nullptr)
	{
		b->left_->parent_ = a;
	}

	a->parent_ = b;
	b->left_ = a;

	b->parent_ = c->parent_;
	if (c->parent_ == nullptr)
	{
		root_ = b;
	} else
	{
		if (c->parent_->left_ == c)
		{
			c->parent_->left_ = b;
		} else
		{
			c->parent_->right_ = b;
		}
	}

	c->left_ = b->right_;
	if (b->right_ != nullptr)
	{
		b->right_->parent_ = c;
	}

	b->right_ = c;
	c->parent_ = b;

	a->balance_ = 0;
	c->balance_ = 0;

	if (b->balance_ == 1)
	{
		a->balance_ = -1;
	} else if (b->balance_ == -1)
	{
		c->balance_ = 1;
	}
	b->balance_ = 0;

	return b;
}

#endif // AVLTree_