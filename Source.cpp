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

int find(vector<pair<string, string>> vect, string symbol) {
	for (int i = 0; i < vect.size(); i++) {
		if (vect[i].first == symbol)
			return i;
	}
	return -1;
}

vector<pair<string, string>> find_vector(vector<vector<pair<string, string>>> table, string non_term) {
	for (auto vect : table) {
		if (vect.begin()->first == non_term)
			return vect;

	}
	return vector<pair<string, string>>();
}


vector<vector<pair<string, string>>> firsts(vector<vector<string>>  grammar) {
	vector<vector<pair<string, string>>> first;
	vector<string> tokens;
	
	for (int i = grammar.size() - 1; i >= 0; i--) {
		first.push_back(vector < pair<string, string>>());
		first[grammar.size() - 1 - i].push_back(pair<string, string>(grammar[i][0], ""));
		for (int j = 1; j < grammar[i].size(); j++) {
			tokens = split(grammar[i][j], "<");
			if (tokens.size() == 1) {
				first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[0], "&"));
				continue;
			}	
			else {
				for (int t = 0; t < tokens.size(); t++) {
					if (tokens[t] == "") {
						tokens.erase(tokens.begin() + t);
					}
					if (tokens[t].find('>') != string::npos && tokens[t].back() != '>') {
						vector<string> buffer = split(tokens[t], ">");
						tokens.erase(tokens.begin() + t);
						tokens.insert(tokens.begin() + t, buffer.begin(), buffer.end());
						tokens[t] = tokens[t] + ">";
					}
					if (tokens[t].back() == '>')
						tokens[t] = "<" + tokens[t];
				}
			}
			
			if (tokens.size() != 1 && tokens[0].front() != '<') 
				first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[0],  tokens[1]));			

			else {
				int pos = 0;
				do {
					bool empty_flag = false;
					if (tokens[pos].back() == '>') {
						for (int z = first.size() - 1; z >= 0; z--)
						{
							if (first[z].begin()->first == tokens[pos]) {
								for (int k = 1; k < first[z].size(); k++) {
									if (first[z][k].first == "e")
										empty_flag = true;
									if (find(first[grammar.size() - 1 - i], first[z][k].first)==-1)
										first[grammar.size() - 1 - i].push_back(pair<string, string>(first[z][k].first, tokens[pos]));
								}
								break;
							}
							if ((z == 0) && first[z].begin()->first != tokens[pos]) {
								string buffer = tokens[pos];
								for (int p = pos + 1; p < tokens.size(); p++) {
									buffer +=tokens[p];
								}
								first[grammar.size() - 1 - i].push_back(pair<string, string>(buffer, "&"));
							}
						}
						if (empty_flag) {
							pos++;
						}
						else
							break;
					}
					else {
						if (pos + 1 != tokens.size())
							first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[pos], tokens[pos + 1]));
						else
							first[grammar.size() - 1 - i].push_back(pair<string, string>(tokens[pos], "&" ));
						break;
					}

				} while (pos != tokens.size());

			}
		}

	}
	for (auto &vect : first) {
		for (auto i = 1; i < vect.size(); i++) {
			if (vect[i].first.front() == '<' && vect[i].second == "&") {
				tokens = split(vect[i].first, "<");
				tokens.erase(tokens.begin());
				int pos = 0;
				bool empty_flag = false;
				do {
					for (auto expr : first) {
						if (expr.begin()->first == "<" + tokens[pos]) {
							vect.erase(vect.begin() + i);
							for (int j = 1; j < expr.size(); j++) {
								if (expr[j].first == "e")
									empty_flag = true;
								if (find(vect, expr[j].first)==-1)
									vect.push_back(pair<string, string>(expr[j].first, expr.begin()->first));
							}
							break;
						}
					}
						if (empty_flag) 
								pos++;
						else
							break;				
				} while (pos != tokens.size());
			}
		}
	}

		return first;

}

vector<vector<pair<string, string>>> follows(vector<vector<string>>  grammar, vector<vector<pair<string, string>>> firsts) {
	vector<vector<pair<string, string>>> follow;
	follow.push_back(vector <pair<string, string>>());
	follow[0].push_back(pair<string, string>(grammar[0][0], ""));
	follow[0].push_back(pair<string, string>("$", "&"));

	for (auto &elem : grammar) {
		for (int i = 1; i < elem.size(); i++) {
			vector<string> tokens = split(elem[i], "<");
			for (int t = 0; t < tokens.size(); t++) {
				if (tokens[t] == "") {
					tokens.erase(tokens.begin() + t);
				}
				if (tokens[t].find('>') != string::npos && tokens[t].back() != '>') {
					vector<string> buffer = split(tokens[t], ">");
					tokens.erase(tokens.begin() + t);
					tokens.insert(tokens.begin() + t, buffer.begin(), buffer.end());
					tokens[t] = tokens[t] + ">";
				}
				if (tokens[t].back() == '>')
					tokens[t] = "<" + tokens[t];
			}
			for (int j = 0; j < tokens.size(); j++) {

				if (tokens[j].back() == '>') {
					int pos = 0;
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
					if (j != tokens.size() - 1) {
						if (tokens[j + 1].front() == '<') {
							follow[pos].push_back(pair<string, string>(tokens[j + 1], "&"));
						}
						else {
							if (j + 2 != tokens.size())
								follow[pos].push_back(pair<string, string>(tokens[j + 1], tokens[j+2]));
							else
								follow[pos].push_back(pair<string, string>(tokens[j + 1], "&"));

						}
							
					}
						
					else
						follow[pos].push_back(pair<string, string>(elem[0], "follow"));

				}
			}

		}

	}

		for (auto &vect : follow) {
			for (int i = 1; i < vect.size(); i++) {
				if (vect[i].second == "follow") {
					for (auto elem : follow) {
						if (elem[0].first == vect[i].first) {
							vect.erase(vect.begin() + i);
							for (int j = 1; j < elem.size(); j++) {
								if (find(vect, elem[j].first)==-1) 
									vect.push_back(elem[j]);									
								
									
							}
							i--;
							break;
						}
					}
				}
			}
		}

	for (auto &vect : follow) {
		for (int i = 1; i < vect.size(); i++) {
			if (vect[i].first.front() == '<') {
				for (auto first : firsts) {
					if (first.begin()->first == vect[i].first) {
						for (int k = 1; k < first.size(); k++) {
							if (find(vect, first[k].first)==-1 && first[k].first != "e") {
									vect.push_back(pair<string, string>(first[k].first, vect[i].first));
							
							}
								
						}
						if (find(first, "e")!=-1) {
							for (auto fol : follow) {
								if (fol[0].first == vect[i].first)
								{

									for (int j = 1; j < fol.size(); j++) {
										if (find(vect, fol[j].first)==-1)
											vect.push_back(fol[j]);
									}
								}
							}

						}
						vect.erase(vect.begin() + i);
						i--;
						break;
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
	string expression;
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

	cout << "GRAMMAR" << endl;
	for (int i = 0; i < grammar.size(); i++)
	{
		cout << grammar[i];
		cout << endl;
	}

	vector<vector<pair<string, string>>> first = firsts(gramm);

	vector<vector<pair<string, string>>> follow = follows(gramm, first);

	cout << "==============================================================================================================================" << endl;
	cout << "FIRSTS" << endl;
	for (int i = 0; i < first.size(); i++)
	{
		for (auto& it : first[i]) {
			cout << "(" << it.first << "," << it.second << ")";
		}
		cout << endl;
	}
	cout << "==============================================================================================================================" << endl;
	cout << "FOLLOWS" << endl;
	for (int i = 0; i < follow.size(); i++)
	{
		for (auto& it : follow[i]) {
			cout << "(" << it.first << "," << it.second << ")";
		}
		cout << endl;
	}

	cout << "==============================================================================================================================" << endl;
	cout << "Enter expression to check. At the end press Enter: ";
	cin >> expression;
	expression += "$";
	string current = "<expression>";
	vector<vector<string>> tree;
	tree.push_back(vector<string>());
	tree.back().push_back(current);
	int iter = 1;
	int cur_symbol = 0;
	int term_pos = 0;

	ofstream fout("output.txt");
	if (fout.is_open()){
		while (1) {

			for (auto str : tree.back()) {
				fout << str;
			}
			fout << endl;
			vector<string> back = tree.back();
			tree.push_back(vector<string>());
			for (auto elem : back) {
				tree.back().push_back(elem);
			}

			if (current.find('<') == string::npos && current != "$") {
				if (string(1, expression[cur_symbol]) == current) {
					if (term_pos == tree.back().size()) {
						if (find(follow[0], current)) {
							fout << "String accepted";
							fout.close();
							return 1;
						}
						else {
							fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
							fout.close();
							return 0;

						}
					}
					cur_symbol++;
					current = tree.back()[term_pos + 1];
					continue;
				}
				else {
					fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
					fout.close();
					return 0;

				}
			}
			vector<pair<string, string>> production = find_vector(first, current);

			if (expression[cur_symbol] == '$' && current == tree.back().back()) {
				production = find_vector(follow, current);
				if (find(production, string(1, expression[cur_symbol]))) {
					tree.back().erase(tree.back().end() - 1);
					for (auto str : tree.back()) {
						fout << str;
					}
					fout << endl;
					fout << "String accepted";
					fout.close();
					return 1;
				
				}
			}

			if (!production.empty()) {
				int pos = find(production, string(1, expression[cur_symbol]));
				if (pos != -1) {
					if (production[pos].second != "&") {
						for (auto gram : gramm)
							if (gram[0] == current) {
								for (auto token : gram) {
									if (token.find(production[pos].second) != string::npos) {
										vector<string> tokens = split(token, "<");
										for (int i = 0; i < tokens.size(); i++) {
											if (tokens[i] == "")
												tokens.erase(tokens.begin() + i);
											if (tokens[i].find('>') != string::npos && tokens[i].back() != '>') {
												vector<string> buffer = split(tokens[i], ">");
												tokens.erase(tokens.begin() + i);
												tokens.insert(tokens.begin() + i, buffer.begin(), buffer.end());
												tokens[i] = tokens[i] + ">";
											}
											if (tokens[i].back() == '>')
												tokens[i] = "<" + tokens[i];
										}
										for (int i = 0; i < tree.back().size(); i++) {
											if (tree.back()[i] == current) {
												tree.back().erase(tree.back().begin() + i);
												tree.back().insert(tree.back().begin() + i, tokens.begin(), tokens.end());
												break;
											}
										}
										if (tokens[0].find('<') == string::npos) {
											if (string(1, expression[cur_symbol]) == tokens[0])
												cur_symbol++;
											else {
												fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
												fout.close();
												return 0;
											}
										}
										current = production[pos].second;
										break;
									}

								}
								break;
							}

					}
					else {

						for (int i = 0; i < tree.back().size(); i++) {
							if (tree.back()[i] == current) {
								tree.back().erase(tree.back().begin() + i);
								tree.back().insert(tree.back().begin() + i, production[pos].first);
								if (i + 1 != tree.back().size())
									current = tree.back()[i + 1];
								term_pos = i + 1;
								break;
							}
						}
						cur_symbol++;

					}
				}
				else if (find(production, "e")!=-1) {
					int brek_flag = 0;
					for (int j = 0; j < tree.size(); j++) {
						for (int i = 0; i < tree[j].size(); i++) {
							if (tree[j][i] == current) {
								if (i!=0 && tree[j][i - 1].front() == '<') {
									production = find_vector(follow, tree[j][i - 1]);
									if (find(production, string(1, expression[cur_symbol]))) {
										for (int i = 0; i < tree.back().size(); i++) {
											if (tree.back()[i] == current) {
												tree.back().erase(tree.back().begin() + i);
												current = tree.back()[i];
												brek_flag = 1;
												break;
											}
										}
										
									}
									else {
										fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
										fout.close();
										return 0;
									}
									break;

								}

							}
						}
						if (brek_flag != 0)
							break;
					}
				}
				else {
					fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
					fout.close();
					return 0;
				}

			}
			else {
				fout << "Error. Character " << expression[cur_symbol] << " is not acceptable. Expression doesn't belong to grammar" << endl;
				fout.close();
				return 0;
			}
		}
	}

	else {
		cout << "Unable to open file.";
		return 0;
	}

	
	


}