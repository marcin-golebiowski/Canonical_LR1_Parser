#ifndef LR_DFA_HPP
#define LR_DFA_HPP

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
      School of Computer Science and Engineering, 
      Southeast University, Jiulonghu Campus,
      Nanjing, China
************************************************************/

#include "Language.hpp"

#define ERROR -1
#define SHIFT -2
#define ACCEPT -3

class LR_DFA
{
public:
	Language lang;
	struct Item
	{
		int productionID; //use id to represent a production
		string lookAhead; //lookAhead label
		int dot;		  //the position of dot
		Item(int p, string l, int d)
		{
			productionID = p;
			lookAhead = l;
			dot = d;
		}
		bool operator!= (const Item a) const
		{
			if (productionID != a.productionID) return true;
			if (dot != a.dot) return true;
			if (lookAhead.size() != a.lookAhead.size()) return true;
			for (int i = 0; i < lookAhead.size(); i++)
				if (lookAhead[i] != a.lookAhead[i]) return true;
			return false;
		}
		bool operator== (const Item a) const 
		{
			if ((productionID != a.productionID) || (dot != a.dot))
				return false;
			if (lookAhead.size() != a.lookAhead.size()) return false;
			for (int i = 0; i < lookAhead.size(); i++)
				if (lookAhead[i] != a.lookAhead[i]) return false;
			return true;
		}
		bool operator< (const Item a) const
		{
			if (productionID < a.productionID)
				return true;
			else if (productionID > a.productionID)
				return false;
			if (dot < a.dot)
				return true;
			else if (dot > a.dot)
				return false;
			if (lookAhead < a.lookAhead)
				return true;
			return false;
		}
		bool operator!= (const Item a)
		{
			if (productionID != a.productionID) return true;
			if (dot != a.dot) return true;
			if (lookAhead.size() != a.lookAhead.size()) return true;
			for (int i = 0; i < lookAhead.size(); i++)
				if (lookAhead[i] != a.lookAhead[i]) return true;
			return false;
		}
		bool operator== (const Item a)
		{
			if ((productionID != a.productionID) || (dot != a.dot))
				return false;
			if (lookAhead.size() != a.lookAhead.size()) return false;
			for (int i = 0; i < lookAhead.size(); i++)
				if (lookAhead[i] != a.lookAhead[i]) return false;
			return true;
		}
		bool operator< (const Item a)
		{
			if (productionID < a.productionID)
				return true;
			else if (productionID > a.productionID)
				return false;
			if (dot < a.dot)
				return true;
			else if (dot > a.dot)
				return false;
			if (lookAhead < a.lookAhead)
				return true;
			return false;
		}
	};
	class state
	{
	public:
		set<Item> core;
		int id;
		int edge_count = 0; //number of edges
		struct link
		{ 
			class state* To; //pointer pointing to the next state 
			string edge;     //the label on an edge
			int action;      //SHIFT, ACCEPT, REDUCE

		} *links; 
	};
	struct searchState	//For indexing an existed state in DFA
	{
		set<Item> core;
		state *s;
		searchState(set<Item> c, state* const st)
		{
			for (set<Item>::iterator i = c.begin(); i != c.end(); i++)
				core.insert(*i);
			s = st;
		}
	};
	state* start;
	vector<searchState> states;
	LR_DFA(){}
	LR_DFA(vector<string> s, vector< vector<string> > kws)
	{
		lang.build(s, kws);
		construct();
	}
	void upLang(vector<string> s, vector< vector<string> > kws)
	{
		lang.build(s, kws);
	}
	void construct()
	{
		start = new state();
		start->id = 0;

		//Assume that the starting label S is at the first production.
		start->core.insert(Item(0, "$", 0));

		closure(start->core); 

		states.push_back(searchState(start->core, start));
		construct(start);
	}
	bool shift(state** s, string a) //walking on the edges of DFA
	{
		int action;
		for (int i = 0; i < (*s)->edge_count; i++)
		{
			action = (*s)->links[i].action;
			if ((action == SHIFT) && (equals((*s)->links[i].edge, a)))
			{
				*s = (*s)->links[i].To; //shift to the next state
				return true;
			}
		}
		return false;
	}
	int indexCore(const set<Item> core) //use core as an index to index existed DFA states
	{
		for (int i = 0; i < states.size(); i++)
			if (coreEqual(states[i].core, core))
				return i;
		return -1;
	}
	void displayTable() //display Parsing Table
	{
		int next, action;
		string edge;
		for (int i = 0; i < states.size(); i++)
		{
			cout << "State ";
			if (i < 10) cout << " ";
			cout << i << ":";

			for (int j = 0; j < states[i].s->edge_count; j++)
			{
				action = states[i].s->links[j].action;
				edge = states[i].s->links[j].edge;
				if (action == SHIFT)
				{
					next = indexCore(states[i].s->links[j].To->core);
					if (lang.matchLeft(edge) != -1)
						cout << "  " << edge << ": GOTO" << next;
					else
						cout << "  " << edge << ": S" << next;
				}
				else if (action > 0)
				{
					cout << "  " << edge << ": R" << action;
				}
				else if (action == ACCEPT)
				{
					cout << "  " << edge << ": ACC";
				}
			}
			cout << endl;
		}
	}
private:
	void closure(set<Item> &core_I) //extend inner states, Dragon Book P167 Algorithm 4.53
	{
		int pid, pid2, pid3, temp_int, dot;
		string beta, lookAhead;
		set<string> temp_seq;
		bool change;
		pair<set<Item>::iterator, bool> pair;

		do
		{
			change = false;
			for (set<Item>::iterator i = core_I.begin(); i != core_I.end(); i++)
			{
				pid = (*i).productionID;
				dot = (*i).dot;
				lookAhead = (*i).lookAhead;
				temp_int = lang.productions[pid].right.size() - 1;

				if (dot == temp_int)     //A->aB.C 
					beta = "";
				else if (dot > temp_int) //A->aBC. ignoring
					continue;
				else                     //A->a.BC
					beta = lang.productions[pid].kwords[lang.productions[pid].right[dot + 1]];

				if ((equals(beta, "epsilon"))) beta = "";

				lang.first(beta + lookAhead, temp_seq); // first(beta + lookAhead)

				temp_int = lang.productions[pid].right[dot];
				pid2 = lang.find(lang.CombinedLeft, lang.productions[pid].kwords[temp_int]);

				if (pid2 != -1) //if the label after dot (like B in A->a.B) is a non-terminal label
				{
					for (int k = 0; k < lang.CombinedLeft[pid2].productionsID.size(); k++)
						for (set<string>::iterator b = temp_seq.begin(); b != temp_seq.end(); b++)
						{
							pid3 = lang.CombinedLeft[pid2].productionsID[k];
							pair = core_I.insert(Item(pid3, *b, 0)); //add [B->.C, b] where b belongs to first(beta + lookAhead)
							change = change || pair.second;
						}
				}
				temp_seq.clear();
			}
		} while (change); //no more state are added
	}
	void construct(state* s) //construct a new DFA state, assume that its core is previously constructed 
	{
		set<string> after_dot;
		vector<string> reduce;
		vector<int> reduceID;
		int dot, pid, right_size, edge_count = 0, count = 0, int_flag;
		string temp_string, lookAhead;
		set<Item> new_core;
		pair<set<string>::iterator, bool> pair;

		//find the labels after dot
		for (set<Item>::iterator i = s->core.begin(); i != s->core.end(); i++)
		{
			pid = (*i).productionID;
			dot = (*i).dot;
			lookAhead = (*i).lookAhead;

			right_size = lang.productions[pid].right.size();
			
			//For REDUCE
			if (dot == right_size)
			{
				reduce.push_back(lookAhead); //label on the edge
				reduceID.push_back(pid); //id of the production used in reducing
				edge_count++;
				continue;
			}

			//For SHIFT
			temp_string = lang.productions[pid].kwords[lang.productions[pid].right[dot]];
			pair = after_dot.insert(temp_string);
			if (pair.second) edge_count++;
		}

		if (edge_count == 0) return; //no outing edge, return

		s->edge_count = edge_count;
		s->links = new state::link[edge_count];

		//Dealing with SHIFT
		for (set<string>::iterator i = after_dot.begin(); i != after_dot.end(); i++, count++)
		{
			//like function GOTO(I, X) in Dragon Book P167 Algorithm 4.53
			for (set<Item>::iterator j = s->core.begin(); j != s->core.end(); j++) 
			{
				pid = (*j).productionID;
				dot = (*j).dot;
				lookAhead = (*j).lookAhead;

				right_size = lang.productions[pid].right.size();
				if (dot < right_size)
				{
					temp_string = lang.productions[pid].kwords[lang.productions[pid].right[dot]];

					if (equals(*i, temp_string)) //find[A->a.BC, c]
						new_core.insert(Item(pid, lookAhead, dot + 1)); //add [A->aB.C, c] to new_core
				}
			}

			closure(new_core);

			int_flag = indexCore(new_core); //use core as an index to index existed DFA states

			if (int_flag < 0) //create new state
			{
				s->links[count].edge = *i;
				s->links[count].action = SHIFT;
				s->links[count].To = new state();
				coreUnion(s->links[count].To->core, new_core);
				states.push_back(searchState(s->links[count].To->core, s->links[count].To));
				s->links[count].To->id = states.size() - 1;

				construct(s->links[count].To); //construct the next state recursively
			}
			else  //state exited
			{
				s->links[count].edge = *i;
				s->links[count].action = SHIFT;
				s->links[count].To = states[int_flag].s;
			}

			new_core.clear(); //for latter use
		}

		//Dealing with REDUCE/ACCEPT
		for (int i = 0; i < reduce.size(); i++, count++)
		{
			s->links[count].edge = reduce[i];
			if (reduceID[i] == 0)
				s->links[count].action = ACCEPT;
			else
				s->links[count].action = reduceID[i];
		}

	}
	bool equals(const string a, const string b)
	{
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != b[i]) return false;
		return true;
	}
	void coreUnion(set<Item> &a, const set<Item> b)
	{
		for (set<Item>::iterator i = b.begin(); i != b.end(); i++)
			a.insert(*i);
	}
	bool coreEqual(const set<Item> a, const set<Item> b)
	{
		set<Item>::iterator i = a.begin(), j = b.begin();
		for (; (i != a.end()) && (j != b.end()); i++, j++)
			if ((*i) != (*j)) return false;
		if ((i == a.end()) && (j == b.end())) return true;
		return false;
	}
};
#endif
