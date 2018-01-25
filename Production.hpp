#ifndef MY_PRODUCTION_HPP
#define MY_PRODUCTION_HPP

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
	  School of Computer Science and Engineering,
      Southeast University, Jiulonghu Campus,
      Nanjing, China
************************************************************/

#include "SimDFA.hpp"
using namespace std;

class Production
{
public:
	vector<string> kwords;
	int left;
	vector<int> right;
	string form;
	Production(){}
	void construct(const string s, const vector<string> kw)
	{
		form = s;
		updateKeyword(kw);
		build(s);
	}
	Production(const string s, const vector<string> kw)
	{
		construct(s, kw);
	}
	void updateKeyword(const vector<string> kw)
	{
		for (int i = 0; i < kw.size(); i++)
			kwords.push_back(kw[i]);
	}
	bool match(string s)
	{
		return simdfa.walk(s);
	}
private:
	SimDFA simdfa;
	int matchKeyword(const string s)
	{
		if (kwords.size() == 0) return -1;
		for (int i = 0; i < kwords.size(); i++)
			if (equals(s, kwords[i])) return i;
		return -1;
	}
	bool equals(const string a, const string b)
	{
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != b[i]) return false;
		return true;
	}
	void build(const string s)
	{
		vector<string> seq;
		form = s;
		kwords.push_back("epsilon"); //epsilon
		kwords.push_back("$");
		int pos1 = s.find_first_of("-");
		if (pos1 == -1) return;
		string temp = s.substr(0, pos1);
		left = matchKeyword(temp);
		pos1++;
		temp = s.substr(pos1 + 1, s.size() - 1 - pos1);
		simdfa.construct(temp, kwords);
		simdfa.get_DFA_Seq(seq);
		int temp1;
		for (int i = 0; i < seq.size(); i++)
		{
			temp1 = matchKeyword(seq[i]);
			right.push_back(temp1);
		}
	}
};

#endif