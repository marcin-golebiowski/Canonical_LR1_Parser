#ifndef MYDFA_HPP_INCLUDED
#define MYDFA_HPP_INCLUDED

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
	  School of Computer Science and Engineering,
	  Southeast University, Jiulonghu Campus,
	  Nanjing, China
************************************************************/

#include "SimLexTree.hpp"

class SimDFA
{
public:
	struct state
	{
		set<int> core;
		bool endable = false;
		int actionCount;
		struct link{ struct state* To; int edge; } *links;
	};
	struct searchState
	{
		set<int> core;
		state *s;
		searchState(set<int> c, state* const st)
		{
			for (set<int>::iterator i = c.begin(); i != c.end(); i++)
				core.insert(*i);
			s = st;
		}
	};
	SimDFA(){}
	SimDFA(string pattern, vector<string> kw)
	{
		construct(pattern, kw);
	}
	SimDFA(string pattern, string* kw, int size_kw)
	{
		construct(pattern, kw, size_kw);
	}
	void construct(string pattern, string* kw, int size_kw)
	{
		Tree.updateKeywords(kw, size_kw);
		Tree.buildTree(pattern);
		construct();
	}
	void construct(string pattern, vector<string> kw)
	{
		Tree.updateKeywords(kw);
		Tree.buildTree(pattern);
		construct();
	}
	bool walk(string* a, int seqLength)
	{
		vector<int> seq;
		Tree.kwStringtoInt(a, seqLength, seq);
		return walk(start, seq, 0);
	}
	bool walk(vector<string> a)
	{
		vector<int> seq;
		Tree.kwStringtoInt(a, seq);
		return walk(start, seq, 0);
	}
	bool walk(string source)
	{
		vector<int> seq;
		int temp, lastPos = -1, counter = 0, right;
		while (counter < source.size())
		{
			right = source.size() - 1;
			temp = Tree.matchKeyword(source.substr(counter, right - counter + 1));
			while ((temp == -1) && (counter < right))
			{
				right--;
				temp = Tree.matchKeyword(source.substr(counter, right - counter + 1));
			}
			if (temp != -1)
			{
				counter = right + 1;
				seq.push_back(temp);
				continue;
			}
			counter++;
		}
		return walk(start, seq, 0);
	}
	void printTree()
	{
		Tree.printTree();
	}
	string getName()
	{
		return name;
	}
	void setName(string n)
	{
		name = n;
	}
	void addKeywords(string s)
	{
		Tree.kwords.push_back(s);
	}
	void updateKeywords(vector<string> kw)
	{
		Tree.updateKeywords(kw);
	}
	void get_DFA_Seq(vector<string> &seq)
	{
		state** s = new state*();
		*s = start;
		while (!(*s)->endable)
		{
			seq.push_back(Tree.kwords[(*s)->links[0].edge]);
			*s = (*s)->links[0].To;
		}
		seq.pop_back();
	}

private:
	int stateSize = 0;
	vector<searchState> states;
	SimLexTree Tree;
	string name;
	state* start;
	void construct()
	{
		start = new state();
		setUnion(start->core, Tree.root->firstpos);
		states.push_back(searchState(start->core, start));
		construct(start);
	}
	void construct(state* s)
	{
		set<int> actions;
		int count = 0;
		vector<int> coreAction;
		SimLexTree::BinNode** b = new SimLexTree::BinNode*();
		state** st = new state*();
		bool endFlag = true;

		for (set<int>::iterator i = s->core.begin(); i != s->core.end(); i++)
		{
			SimLexTree::indexNode(Tree.root, *i, b);
			actions.insert((*b)->content); //get all actions
			coreAction.push_back((*b)->content); //coreAction: position -> action
		}
		s->actionCount = actions.size();
		if (s->actionCount == 0)
		{
			s->endable = true;
			return;
		}
		s->links = new state::link[s->actionCount];
		bool flag;
		set<int> U;
		for (set<int>::iterator i = actions.begin(); i != actions.end(); i++, count++) //for all actions
		{
			int k = 0;
			for (set<int>::iterator j = s->core.begin(); j != s->core.end(); j++, k++)
				if (coreAction[k] == *i)
				{
					SimLexTree::indexNode(Tree.root, *j, b);
					setUnion(U, (*b)->followpos); //U represent the core in the next state
				}
			flag = indexCore(U, st); 
			//st represents a pointer to the existed state, if there is an existed state with core U

			if (!flag) //create a new state
			{
				endFlag = false; 
				s->links[count].edge = *i;
				s->links[count].To = new state();
				setUnion(s->links[count].To->core, U);
				states.push_back(searchState(s->links[count].To->core, s->links[count].To));
				construct(s->links[count].To);
			}
			else //pointing to the existed state
			{
				s->links[count].edge = *i;
				s->links[count].To = (*st);
			}
			U.clear();
			break;
		}
		s->endable = endFlag; //just simplify the process for determining whether it is enable 
	}
	bool walk(state* s, int* indexSeq, int seqLength, int depth) //simple depth first search
	{
		if (depth == seqLength)
		{
			if (s->endable) return true;
			return false;
		}
		for (int i = 0; i < s->actionCount; i++)
			if (s->links[i].edge == indexSeq[depth])
				return walk(s->links[i].To, indexSeq, seqLength, depth + 1);
		return false;
	}
	bool walk(state* s, vector<int> indexSeq, int depth) //simple depth first search
	{
		if (depth == indexSeq.size())
		{
			if (s->endable) return true;
			return false;
		}
		for (int i = 0; i < s->actionCount; i++)
			if (s->links[i].edge == indexSeq[depth])
				return walk(s->links[i].To, indexSeq, depth + 1);
		return false;
	}
	void setUnion(set<int> &a, const set<int> b)
	{
		for (set<int>::iterator i = b.begin(); i != b.end(); i++)
			a.insert(*i);
	}
	bool setEqual(const set<int> a, const set<int> b)
	{
		if (a.size() != b.size()) return false;
		set<int>::iterator i = a.begin(), j = b.begin();
		for (; (i != a.end()) && (j != b.end()); i++, j++)
			if (*i != *j) return false;
		if ((i == a.end()) && (j == b.end())) return true;
		return false;
	}
	bool indexCore(const set<int> core, state** ans)
	{
		for (vector<searchState>::iterator i = states.begin(); i != states.end(); i++)
			if (setEqual((*i).core, core))
			{
				*ans = (*i).s;
				return true;
			}
		return false;
	}
};

#endif