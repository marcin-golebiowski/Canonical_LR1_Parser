#ifndef LEXTREE_HPP_INCLUDED
#define LEXTREE_HPP_INCLUDED

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
	  School of Computer Science and Engineering,
	  Southeast University, Jiulonghu Campus,
	  Nanjing, China
************************************************************/

#include<iostream>
#include<string>
#include<vector>
#include<iterator>
#include<set>
using namespace std;

#define NUL 0
#define CAT 1
#define LEAF 2

class SimLexTree
{
public:
	struct BinNode
	{
		int type = NUL, id, content;
		bool nullable = false;
		set<int> firstpos, lastpos, followpos;
		struct BinNode *lchild, *rchild;
	};
	SimLexTree(){}
	SimLexTree(vector<string> strvec)
	{
		updateKeywords(strvec);
	}
	SimLexTree(string* a, int size_a)
	{
		updateKeywords(a, size_a);
	}
    void buildTree(string a)
    {
		lex = a;
		root = new BinNode();
        buildTree(a+"@", root);
		updateID(root);
		updateNodes(root);
		updateFollow(root);
    }
    void printTree()
    {
        display(root);
		cout << endl;
    }
	void printFollow()
	{
		BinNode **b = new BinNode*();
		for (int i = 1; i <= nodeSize; i++)
		{
			indexNode(root, i, b);
			for (set<int>::iterator i = (*b)->followpos.begin(); i != (*b)->followpos.end(); i++)
				cout << *i << " ";
			cout << endl;
		}
	}
	void updateKeywords(string* a, int size_a)
	{
		for (int i = 0; i < size_a; i++)
			kwords.push_back(a[i]);
		kwords.push_back("$");
		kwords.push_back("@");
	}
	void updateKeywords(vector<string> s)
	{
		for (int i = 0; i < s.size(); i++)
			kwords.push_back(s[i]);
		kwords.push_back("$");
		kwords.push_back("@");
	}
	void kwStringtoInt(const vector<string> s, vector<int> &ans)
	{
		for (int i = 0; i < s.size(); i++)
			ans.push_back(matchKeyword(s[i]));
	}
	void kwStringtoInt(string* s, int size_s, vector<int> &ans)
	{
		for (int i = 0; i < size_s; i++)
			ans.push_back(matchKeyword(s[i]));
	}
	static bool indexNode(BinNode*a, int index, BinNode** ans)
	{
		if ((index > a->id) || (a->type == NUL)) return false;
		if ((a->type == LEAF) && (a->id == index))
		{
			*ans = a;
			return true;
		}
		if (indexNode(a->lchild, index, ans))
			return true;
		return indexNode(a->rchild, index, ans);
	}
	int matchKeyword(string a)
	{
		if (kwords.size() == 0) return -1;
		for (int i = 0; i < kwords.size(); i++)
		{
			if (equals(a, kwords[i]))
				return i;
		}
		return -1;
	}
    vector<string> kwords;
	vector<int> leaves;
    string lex;
	BinNode* root;
	int nodeSize = 0;
private:
	void buildTree(string a, BinNode* r)
	{
		int length = a.size(), left, right, ans = -1;

		right = length - 1;
		left = length - 2;

		r->rchild = new BinNode();
		r->lchild = new BinNode();

		if (length == 0) return;

		while ((left >= 0) && (a[left] != ')'))
			left--;
		left++;

		string sub_temp = a.substr(left, right - left + 1);
		ans = matchKeyword(sub_temp);
		if (ans == -1)
		{
			int temp_left = left;
			while ((temp_left <= right) && (matchKeyword(a.substr(temp_left, right - temp_left + 1)) == -1))
				temp_left++;

			if (temp_left <= right)
			{
				left = temp_left;
				ans = matchKeyword(a.substr(left, right - left + 1));
			}
		}

		if (left == 0)
		{
			r->type = LEAF;
			r->content = ans;
			return;
		}

		r->rchild->type = LEAF;
		r->rchild->content = ans;

		r->type = CAT;

		buildTree(a.substr(0, left), r->lchild); //Left associativity is assumed.
	}
	void updateID(BinNode *a) 
	{
		if (a->type == NUL) return;
		if (a->type == LEAF)
		{
			a->id = ++nodeSize;
			leaves.push_back(a->content);
			return;
		}
		updateID(a->lchild);
		updateID(a->rchild);
		a->id = nodeSize; //if a is a non-leaf node, then its id is the max id in its children
	}
	void updateNodes(BinNode *a)
	{
		if (a->type == NUL)
		{
			a->nullable = true;
			return;
		}
		else if (a->type == LEAF)
		{
			a->nullable = false;
			a->firstpos.insert(a->id);
			a->lastpos.insert(a->id);
			return;
		}

		updateNodes(a->lchild);
		updateNodes(a->rchild);

		if (a->type == CAT)
		{
			a->nullable = a->lchild->nullable && a->rchild->nullable;
			if (a->lchild->nullable)
			{
				setUnion(a->firstpos, a->lchild->firstpos);
				setUnion(a->firstpos, a->rchild->firstpos);
			}
			else
				setUnion(a->firstpos, a->lchild->firstpos);
			if (a->rchild->nullable)
			{
				setUnion(a->lastpos, a->lchild->lastpos);
				setUnion(a->lastpos, a->rchild->lastpos);
			}
			else
				setUnion(a->lastpos, a->rchild->lastpos);
		}
	}
	void updateFollow(BinNode *a) //based on the algorithm given by the dragon book
	{
		if ((a->type == LEAF) || (a->type == NUL)) return;
		updateFollow(a->lchild);
		updateFollow(a->rchild);
		BinNode** b = new BinNode*();
		if (a->type == CAT)
		{
			for (set<int>::iterator i = a->lchild->lastpos.begin(); i != a->lchild->lastpos.end(); i++)
			{
				indexNode(root, *i, b);
				setUnion((*b)->followpos, a->rchild->firstpos);
			}
		}
	}
	void setUnion(set<int> &a, const set<int> b)
	{
		for (set<int>::iterator i = b.begin(); i != b.end(); i++)
			a.insert(*i);
	}
	void display(BinNode* r)
	{
		if (r->type == LEAF)
		{
			cout << kwords[r->content] << " ";
			return;
		}

		if (r->lchild->type != NUL)
		{
			display(r->lchild);
		}

		else if (r->type == CAT)
			cout << " o ";

		if (r->rchild->type != NUL)
			display(r->rchild);
	}
    bool equals(const string a, const string b)
    {
        if (a.size() != b.size()) return false;
        for (int i = 0; i < a.size(); i++)
            if (a[i] != b[i]) return false;
        return true;
    }
};

#endif // LEXTREE_HPP_INCLUDED
