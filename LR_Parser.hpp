#ifndef LR_PARSER_HPP
#define LR_PARSER_HPP

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
	  School of Computer Science and Engineering,
	  Southeast University, Jiulonghu Campus,
	  Nanjing, China
************************************************************/

#include<stack>
#include "LR_DFA.hpp"

#define ERROR -1
#define SHIFT -2
#define ACCEPT -3

class LR_Parser
{
public:

	LR_Parser(){}
	void upLang(vector<string> s, vector< vector<string> > kws)
	{
		dfa.upLang(s, kws);
	}
	void updatePrior(vector<string> p)
	{
		for (int i = 0; i < p.size(); i++)
			priorTable.push_back(p[i]);
	}
	LR_Parser(vector<string> s, vector< vector<string> > kws, vector<string> p)
	{
		dfa.upLang(s, kws);
		dfa.construct();
		updatePrior(p);
	}
	void dfa_construct()
	{
		dfa.construct();
	}
	bool parseLine(string source, vector<string> &ans_seq)
	{
		stack<string> label_stack, terminal_stack;
		stack<int> state_stack;
		int lastp = -1, point = -1, action, temp_int, next, counter = 0;
		string edge, a, A, top_terminal;
		bool flag = false;

		matchKey(source, point, lastp, a); //a = find the first label in the input sequence

		LR_DFA::state** cur_state = new LR_DFA::state*();

		*cur_state = dfa.start; //place the current state on the starting state in LR_DFA
		state_stack.push(0); //push the starting state into the state_stack

		//Modified Version of Dragon Book Algorithm 4.44 (P160)
		//For dealing with ambiguous grammars
		while (true)
		{
			action = ERROR;
			flag = false;
			if (!terminal_stack.empty())
				top_terminal = terminal_stack.top();

			counter = 0;

			for (int i = 0; i < (*cur_state)->edge_count; i++) //find the matching action
			{
				edge = (*cur_state)->links[i].edge;
				if (equals(edge, a))
				{
					flag = true;

					counter++;
					if (counter == 1)
					{
						action = (*cur_state)->links[i].action;
						continue;
					}

					//dealing with conflict 
					if (!terminal_stack.empty())
					{
						if ((*cur_state)->links[i].action == SHIFT) //action = REDUCE, replace it with SHIFT 
						{
							if (equals(a, top_terminal) || (prior(a, top_terminal)))
								action = SHIFT; //replace it
						}
						else if ((*cur_state)->links[i].action > 0) //action = SHIFT, replace it with REDUCE 
						{
							if ((action == SHIFT) && ((equals(a, top_terminal) || (prior(a, top_terminal))))) break;
							action = (*cur_state)->links[i].action; //replace it
						}
						break;
					}
				}
			}
			if (flag)
			{
				if (action == SHIFT)
				{
					if (dfa.lang.matchLeft(a) == -1) //update terminal_stack
						terminal_stack.push(a);
					label_stack.push(a); //push the current label into the label stack
					dfa.shift(cur_state, a); //go to the next state
					state_stack.push((*cur_state)->id); //push the next state into the state stack 
					matchKey(source, point, lastp, a); //a = next label
				}
				else if (action > 0) //REDUCE
				{
					temp_int = dfa.lang.productions[action].right.size(); //number of labels to be popped out
					ans_seq.push_back(dfa.lang.productions[action].form); //add the production to the output sequence
					for (int i = 0; i < temp_int; i++) 
					{
						//if terminal_stack is non-empty and the label to be popped put is a terminal
						if ((!terminal_stack.empty()) && (dfa.lang.matchLeft(label_stack.top()) == -1))
							terminal_stack.pop(); //popped it out

						label_stack.pop(); //popped out a label from the label_stack 
						state_stack.pop(); //popped out a state from the state_stack
					}
					*cur_state = dfa.states[state_stack.top()].s; //cur = the top state at the state stack
					A = dfa.lang.productions[action].kwords[dfa.lang.productions[action].left]; //A = the header of the production used in reducing 
					label_stack.push(A); //push the current label into the label stack
					dfa.shift(cur_state, A); //GOTO(cur, A)
					state_stack.push((*cur_state)->id); //push the next state into the state stack
				}
				else if (action == ACCEPT) //Syntax Analysis Done
					return true;
				else  //An error is detected
				{
					cout << "AN ERROR HAS OCCURED!" << endl;
					ans_seq.clear();
					return false;
				}
			}
			
		}
		return false;
	}
	void displayTable()
	{
		cout << endl;
		dfa.displayTable();
		cout << endl;
	}
private:
	LR_DFA dfa;
	vector<string> priorTable;
	void matchKey(const string source, int &point, int &lastp, string &a)
	{
		do
		{
			point++;
			a = source.substr(lastp + 1, point - lastp);
		} while (dfa.lang.matchKeyword(a) == -1);
		lastp = point;
	}
	bool prior(const string a, const string b)
	{
		if (equals(a, b)) return false;
		for (int i = 0; i < priorTable.size(); i++)
		{
			if (equals(a, priorTable[i])) return true;
			else if (equals(b, priorTable[i])) return false;
		}
		return false;
	}
	bool equals(const string a, const string b)
	{
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != b[i]) return false;
		return true;
	}
};
#endif