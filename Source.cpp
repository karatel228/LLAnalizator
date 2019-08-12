#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
using namespace std;

vector<string> split(string str, string delimiter) {
	size_t pos = 0;
	string token;
	vector<string> tokens;
	//str.erase(0, 3);


	while ((pos = str.find(delimiter)) != string::npos) {
		tokens.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	if (str.find(delimiter) == string::npos)
		tokens.push_back(str);
	return tokens;
}

bool find(vector<pair<string, string>> vect, string symbol) {
	for (auto &elem : vect) {
		if (elem.first == symbol)
			return true;
	}
	return false;
}

vector<vector<pair<string, string>>> firsts(vector<vector<string>>  grammar) {
	vector<vector<pair<string, string>>> first;
	
	for (int i = grammar.size() - 1; i >= 0; i--) {
		first.push_back(vector < pair<string, string>>());
		first[grammar.size() - 1 - i].push_back(pair<string, string>(grammar[i][0], ""));
		for (int j = 1; j < grammar[i].size(); j++) {
			vector<string> tokens = split(grammar[i][j], "<");
			if (tokens.size() == 1)
				first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[0], "e"));
			else if (tokens[0] != "")
				first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[0], "<" + tokens[1]));
			else {
				int pos = 1;
				do {
					bool empty_flag = false;
					if (tokens[pos].back() == '>') {
						tokens[pos] = "<" + tokens[pos];
						for (int z = first.size() - 1; z >= 0; z--)
						{
							if (first[z].begin()->first == tokens[pos]) {
								for (int k = 1; k < first[z].size(); k++) {
									if (first[z][k].first == "e")
										empty_flag = true;
									if (!find(first[grammar.size() - 1 - i], first[z][k].first))
										first[grammar.size() - 1 - i].push_back(pair<string, string>(first[z][k].first, tokens[pos]));
								}
								break;
							}
							if ((z == 0) && first[z].begin()->first != tokens[pos])
								first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[pos], "e"));
						}
						if (empty_flag) {
							pos++;
							continue;
						}
						else
							break;
					}
					else {
						if (pos + 1 != tokens.size())
							first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[pos], "<" + tokens[pos + 1]));
						else
							first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[pos], "e" ));
						break;
					}

				} while (pos != tokens.size());

			}
		}

	}
	for (auto &vect : first) {
		for (auto i = 1; i < vect.size(); i++) {
			if (vect[i].first.front() == '<'&&vect[i].second == "e")
				for (auto expr : first) {
					if (expr.begin()->first == vect[i].first) {
						vect.erase(vect.begin() + i);
						for (int j = 1; j < expr.size(); j++) {
							if(!find(vect, expr[j].first))
								vect.push_back(pair<string, string>(expr[j].first, expr.begin()->first));
						}
					}
				}
		}
	}

		return first;

}

vector<vector<pair<string, string>>> follows(vector<vector<string>>  grammar) {
	vector<vector<pair<string, string>>> follow;
	follow.push_back(vector <pair<string, string>>());
	follow[0].push_back(pair<string, string>(grammar[0][0], ""));
	follow[0].push_back(pair<string, string>("$", "e"));

	for (auto &elem : grammar) {
		for (int i = 1; i < elem.size(); i++) {
			vector<string> tokens = split(elem[i], "<");
			for (int j = 0; j < tokens.size(); j++) {
				if (tokens[j] == "") {
					tokens.erase(tokens.begin() + j);
				}
				if (tokens[j].find('>')!= string::npos && tokens[j].back() != '>') {
					vector<string> buffer = split(tokens[j], ">");
					tokens.erase(tokens.begin() + j);
					tokens.insert(tokens.begin() + j, buffer.begin(), buffer.end());
					tokens[j] = tokens[j] + ">";
				}
				if (tokens[j].back() == '>') {
					int pos = 0;
					tokens[j] = "<" + tokens[j];
					for (int y = follow.size() - 1; y >= 0; y--) {
						if (follow[y].begin()->first == tokens[j]) {
							pos = y;
							break;
						}
						if ((y == 0) && follow[y].begin()->first != tokens[j]) {
							follow.push_back(vector <pair<string, string>>());
							follow[follow.size() - 1].push_back(pair<string, string>(tokens[j], ""));
							pos = follow.size() - 1;
						}
					}
					if (j != tokens.size() - 1) 
						follow[pos].push_back(pair<string, string>((tokens[j+1].back()=='>' ? "<" + tokens[j + 1] : tokens[j + 1]), "e"));							
					else
						for (int z = 0; z < follow.size(); z++) {
							if (follow[z][0].first == elem[0])
								for (int k = 1; k < follow[z].size(); k++) {
									follow[pos].push_back(follow[z][k]);
								}
						}

				}
			}

		}

	}

	return follow;

}





int main() {
	string line;
	ifstream myfile("grammar.txt");
	vector<string> grammar;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			grammar.push_back(line);
		}
		myfile.close();
	}

	else cout << "Unable to open file";


	vector<vector<string>> gramm;
	regex re("(<.*?>)");
	smatch match;

	vector<string> tokens;

	for (int i = 0; i < grammar.size(); i++) {

		gramm.push_back(vector<string>());
		regex_search(grammar[i], match, re);
		gramm[i].push_back(match.str(1));
		string right_side = match.suffix();
		right_side.erase(0, 3);
		tokens = split(right_side, "|");
		gramm[i].insert(gramm[i].end(), tokens.begin(), tokens.end());

	}

	vector<vector<pair<string, string>>> first = firsts(gramm);
		
	vector<vector<pair<string, string>>> follow = follows(gramm);

	for (int i = 0; i < first.size(); i++)
	{
		for (auto &it:first[i]) {
			cout <<"("<<it.first<<","<<it.second<<")";
		}
		cout << endl;
	}

	for (int i = 0; i < follow.size(); i++)
	{
		for (auto &it : follow[i]) {
			cout << "(" << it.first << "," << it.second << ")";
		}
		cout << endl;
	}

	getchar();
	return 0;
}