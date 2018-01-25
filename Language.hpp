#ifndef MYLANGUAGE_HPP
#define MYLANGUAGE_HPP

/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
      School of Computer Science and Engineering,
      Southeast University, Jiulonghu Campus,
      Nanjing, China
************************************************************/

#include "Production.hpp"

class Language
{
public:
	struct LeftAndProduction
	{
		int left;
		bool nullable;
		vector<int> productionsID;
	};
	vector<string> re;
	vector<Production> productions;
	vector<string> new_kwords;
	vector<vector<int>> new_right;
	vector<LeftAndProduction> CombinedLeft;
	Language(){};
	void updateReserve(const vector<string> k)
	{
		for (int i = 0; i < k.size(); i++)
			re.push_back(k[i]);
	}
	void build(const vector<string> p, const vector< vector<string> > kws)
	{
		int temp;
		string leftp, str, newp;
		for (int i = 0; i < p.size(); i++)
		{
			str = p[i];
			temp = p[i].find(">");
			leftp = p[i].substr(0, temp + 1);
			str.erase(0, temp + 1);
			do
			{
				temp = str.find("|");
				if (temp != -1)
				{
					newp = str.substr(0, temp);
					str.erase(0, temp + 1);
					productions.push_back(Production());
					productions[productions.size() - 1].updateKeyword(re);
					productions[productions.size() - 1].construct(leftp + newp, kws[i]);
				}
			} while ((temp != -1) && (str.size() > 0));
			if (str.size() > 0)
			{
				productions.push_back(Production());
				productions[productions.size() - 1].updateKeyword(re);
				productions[productions.size() - 1].construct(leftp + str, kws[i]);
			}
		}

		for (int i = 0; i < productions.size(); i++)
		{
			for (int j = 0; j < productions[i].kwords.size(); j++)
			{
				temp = matchKeyword(productions[i].kwords[j]);
				if (temp == -1)
					new_kwords.push_back(productions[i].kwords[j]);
			}
		}
		int temp1;
		for (int i = 0; i < productions.size(); i++)
		{
			temp = matchKeyword(productions[i].kwords[productions[i].left]);
			if (temp != -1)
			{
				temp1 = matchLeft(productions[i].kwords[productions[i].left]);
				if (temp1 == -1)
				{
					CombinedLeft.push_back(LeftAndProduction());
					CombinedLeft[CombinedLeft.size() - 1].left = temp;
					CombinedLeft[CombinedLeft.size() - 1].productionsID.push_back(i);
				}
				else
					CombinedLeft[temp1].productionsID.push_back(i);
			}
			new_right.push_back(vector<int>());
			for (int j = 0; j < productions[i].right.size(); j++)
			{
				temp1 = productions[i].right[j];
				temp = matchKeyword(productions[i].kwords[temp1]);
				new_right[i].push_back(temp);
			}
		}
		bool flag;
		for (int k = 0; k < CombinedLeft.size(); k++)
		{
			flag = false;
			for (int i = 0; i < CombinedLeft[k].productionsID.size(); i++)
			{
				for (int j = 0; j < new_right[CombinedLeft[k].productionsID[i]].size(); j++)
				{
					if (!flag)
						flag = equals("epsilon", new_kwords[new_right[CombinedLeft[k].productionsID[i]][j]]);
					if (flag) break;
				}
				if (flag) break;
			}
			CombinedLeft[k].nullable = flag;
		}
	}
	void first(const string label, set<string> &ans)
	{
		int temp, id, pos = 0;
		bool all_null = true;
		string now, temp_str;
		do
		{
			pos++;
			if (pos > label.size()) break;
			temp_str = label.substr(0, pos);
			id = matchKeyword(temp_str);
		}
		while (matchKeyword(temp_str) == -1);

		if (id == -1) //label not found
		{
			ans.insert(label.substr(0, 1));
			return;
		}

		id = matchLeft(temp_str);

		if (id == -1) //terminal, first(aB) = first(a) = a
		{
			ans.insert(temp_str); 
			return;
		}

		for (int i = 0; i < CombinedLeft[id].productionsID.size(); i++)
		{
			all_null = true;
			for (int j = 0; j < new_right[CombinedLeft[id].productionsID[i]].size(); j++)
			{
				now = new_kwords[new_right[CombinedLeft[id].productionsID[i]][j]];
				temp = matchLeft(now);
				if (temp == -1) //terminal 
				{
					ans.insert(now);
					all_null = false;
					break;
				}
				else //non-terminal
				{
					if (temp != id) //avoid infinite loop
						first(now, ans);

					if (!CombinedLeft[temp].nullable)
					{
						all_null = false;
						break;
					}
					else
						continue;
				}
			}
			if (all_null) ans.insert("epsilon");
		}
	}
/*
	void follow(string label, set<string> &ans)
	{
		int temp, temp1 = -2, counter, temp2, pos, id;
		string last, now, temp_str;

		do
		{
			pos++;
			if (pos > label.size()) break;
			temp_str = label.substr(0, pos);
			id = matchKeyword(temp_str);
		} while (matchKeyword(temp_str) == -1);

		if (id == -1)
		{
			ans.insert(label.substr(0, 1));
			return;
		}

		id = matchLeft(temp_str);

		if (id == -1)
		{
			ans.insert(temp_str);
			return;
		}

		if (id == 0) ans.insert("$"); //if it is the beginning state, the end_of_line label should be included

		for (int i = 0; i < CombinedLeft.size(); i++)
			for (int j = 0; j < CombinedLeft[i].productionsID.size(); j++)
			{
				temp1 = -2;
				counter = new_right[CombinedLeft[i].productionsID[j]].size() - 1;
				do
				{
					now = new_kwords[new_right[CombinedLeft[i].productionsID[j]][counter]];
					temp = matchLeft(now);
					if (temp != -1)
					{
						if (temp == id) //target label B is found in this production
						{
							if (temp1 == -1) //like B in (A -> cBa), next label is a
								ans.insert(last);
							else if (temp1 == -2) //like B in (A -> cB)
							{
								if (!equals(now, new_kwords[CombinedLeft[i].left])) //to avoid infinite loop
									follow(new_kwords[CombinedLeft[i].left], ans);
							}
							else
							{
								do
								{
									now = new_kwords[new_right[CombinedLeft[i].productionsID[j]][counter + 1]];
									temp2 = matchLeft(now);
									first(now, ans);
									counter++;
								} while ((counter < new_right[CombinedLeft[i].productionsID[j]].size() - 1) && (CombinedLeft[temp2].nullable));

								if (CombinedLeft[temp2].nullable) //like B in (A -> cBD), when epsilon belongs to first(D)
								{
									if (!equals(new_kwords[CombinedLeft[i].left], now)) //to avoid infinite loop 
										follow(new_kwords[CombinedLeft[i].left], ans);
								}
							}
							break;
						}
					}
					counter--;
					temp1 = temp;
					last = now;
				} while (counter >= 0);
			}
		for (set<string>::iterator i = ans.begin(); i != ans.end(); i++)
			if (equals("epsilon", *i))
			{
				ans.erase(i);
				break;
			}
	}
*/
	int find(const vector<LeftAndProduction> a, const string b)
	{
		if (a.size() == 0) return -1;
		for (int i = 0; i < a.size(); i++)
			if (equals(new_kwords[a[i].left], b)) return i;
		return -1;
	}
	int matchKeyword(const string s)
	{
		if (new_kwords.size() == 0) return -1;
		for (int i = 0; i < new_kwords.size(); i++)
			if (equals(s, new_kwords[i])) return i;
		return -1;
	}
	int matchLeft(const string s)
	{
		if (CombinedLeft.size() == 0) return -1;
		for (int i = 0; i < CombinedLeft.size(); i++)
			if (equals(s, new_kwords[CombinedLeft[i].left])) return i;
		return -1;
	}
private:
	bool equals(const string a, const string b)
	{
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++)
			if (a[i] != b[i]) return false;
		return true;
	}
	void setUnion(set<string> &a, const set<string> b)
	{
		for (set<string>::iterator i = b.begin(); i != b.end(); i++)
			a.insert(*i);
	}
};

#endif