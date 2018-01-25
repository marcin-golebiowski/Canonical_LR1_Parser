/***********************************************************
From: Marco Qiaofeng Liu (Email: qiaofengmarco@outlook.com),
	  School of Computer Science and Engineering,
      Southeast University, Jiulonghu Campus,
      Nanjing, China
************************************************************/

#include "LR_Parser.hpp"
#include <fstream>
int main()
{
	ifstream pin("./sample/Language.txt"), pri_in("./sample/Priority.txt"), fin("./sample/Input.txt");
	ofstream fout("./sample/Analysis Result.txt");

	string buff, temp, p1, temp_str;
	int pos, pos1, tot = 0, counter, last;
	
	vector<string> prior;
	while (getline(pri_in, buff))
	{
		prior.push_back(buff);
	}
	pri_in.close();

	vector<string> p;
	vector< vector<string> > kws;
	
	tot = 0;
	while (getline(pin, buff))
	{
		kws.push_back(vector<string>());

		pos = buff.find_first_of("$");
		p1 = buff.substr(0, pos - 1);
		p.push_back(p1);

		pos1 = buff.rfind("$");
		temp = buff.substr(pos + 1, pos1 - 1 - pos);

		counter = 0;
		last = -1;
		while (counter < temp.size())
		{
			if (temp[counter] == ' ')
			{
				kws[tot].push_back(temp.substr(last + 1, counter - last - 1));
				last = counter;
			}
			counter++;
		}
		kws[tot].push_back(temp.substr(last + 1, temp.size() - 1 - last));
		tot++;
	}
	pin.close();

	LR_Parser parser(p, kws, prior); //Canonical LR(1) Parser Definition
	vector<string> ans;

	//parser.displayTable(); //display the Parsing Table

	bool flag;

	int count = 0;
	while (getline(fin, buff))
	{
		count++;
		flag = parser.parseLine(buff, ans); //parse a line (deal with a sample)
		fout << "======================== Analysis Result ";
		if (count < 10) 
			fout << "0";
		fout << count << " =======================" << endl;
		for (int i = 0; i < ans.size(); i++)
			fout << ans[i] << endl;
		for (int i = 0; i < 60; i++)
			fout << "=";
		fout << endl;
		ans.clear();
	}
	fin.close();
	fout.close();

	cout << "Finished syntax analysis!" << endl;
	getchar();

	return 0;
}