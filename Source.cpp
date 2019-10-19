#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>


using namespace std;

struct tree
{
	string production;
	struct tree* parent;
	vector<string> children;
	vector<tree*> chosen;
};

struct tree* init(string a)  
{
	struct tree* lst = new tree;
	lst->parent = 0; 
	lst->production = a;

	return(lst);
}

vector<string> split(string str, const string& delimiter) {
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

	if (delimiter == "<") {
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
	return tokens;
}

int find(vector<pair<string, string>>& vect, const string& symbol) {

	for (int i = 0; i < vect.size(); i++) {
		if (vect[i].first == symbol)
			return i;
	}
	return -1;
}


vector<pair<string, string>> find_vector(vector<vector<pair<string, string>>>& table, string& non_term) {
	for (auto vect : table) {
		if (vect.begin()->first == non_term)
			return vect;

	}
	return vector<pair<string, string>>();
}


vector<vector<pair<string, string>>> firsts(vector<vector<string>>&  grammar) {
	vector<vector<pair<string, string>>> first;
	vector<string> tokens;
	vector<pair<string, string>> expr;
	
	for (int i = grammar.size() - 1; i >= 0; i--) {
		first.push_back(vector < pair<string, string>>());
		first.back().push_back(pair<string, string>(grammar[i][0], ""));
		for (int j = 1; j < grammar[i].size(); j++) {
			tokens = split(grammar[i][j], "<");
			if (tokens.size() == 1 && tokens[0].front()!='<') {
				first.back().push_back(pair<string, string>(tokens[0], "&"));
				continue;
			}				
			if (tokens.size() != 1 && tokens[0].front() != '<') 
				first.back().push_back(pair<string, string>(tokens[0],  tokens[1]));			

			else {
				int pos = 0;
				do {
					bool empty_flag = false;
					if (tokens[pos].back() == '>') {
						if (!(expr = find_vector(first, tokens[pos])).empty()) {
							for (int k = 1; k < expr.size(); k++) {
								if (expr[k].first == "e")
									empty_flag = true;
								if (find(first.back(),expr[k].first) == -1)
									first.back().push_back(pair<string, string>(expr[k].first, tokens[pos]));
							}
						}
						else {
							string buffer = tokens[pos];
							for (int p = pos + 1; p < tokens.size(); p++) {
								buffer += tokens[p];
							}
							first.back().push_back(pair<string, string>(buffer, "&"));
						}
						if (empty_flag) {
							pos++;
						}
						else
							break;
					}
					else {
						if (pos + 1 != tokens.size())
							first.back().push_back(pair<string, string>(tokens[pos], tokens[pos + 1]));
						else
							first.back().push_back(pair<string, string>(tokens[pos], "&" ));
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
				int pos = 0;
				bool empty_flag = false;
				do {
					expr = find_vector(first, tokens[pos]);
					vect.erase(vect.begin() + i);
					for (int j = 1; j < expr.size(); j++) {
						if (expr[j].first == "e")
							empty_flag = true;
						if (find(vect, expr[j].first) == -1)
							vect.push_back(pair<string, string>(expr[j].first, expr.begin()->first));
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

vector<vector<pair<string, string>>> follows(vector<vector<string>>&  grammar, vector<vector<pair<string, string>>>& firsts) {
	vector<vector<pair<string, string>>> follow;
	vector<pair<string, string>> expr;
	follow.push_back(vector <pair<string, string>>());
	follow[0].push_back(pair<string, string>(grammar[0][0], ""));
	follow[0].push_back(pair<string, string>("$", ""));

	for (auto& elem : grammar) {
		for (int i = 1; i < elem.size(); i++) {
			vector<string> tokens = split(elem[i], "<");
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
							follow.back().push_back(pair<string, string>(tokens[j], ""));
							pos = follow.size() - 1;
						}
					}
					if (j != tokens.size() - 1) {
						follow[pos].push_back(pair<string, string>(tokens[j + 1], ""));		
					}						
					else
						follow[pos].push_back(pair<string, string>(elem[0], "follow"));

				}
			}

		}

	}

	for (auto &vect : follow) {
		for (int i = 1; i < vect.size(); i++) {
			if (vect[i].second=="follow") {
				expr = find_vector(follow, vect[i].first);
				for (int j = 1; j < expr.size(); j++) {
					if (find(vect, expr[j].first) == -1)
						vect.push_back(expr[j]);	
				}
				vect.erase(vect.begin() + i);
				i--;				
				}
			}
		}


	for (auto& vect : follow) {
		for (int i = 1; i < vect.size(); i++) {
			if (vect[i].first.front() == '<') {
				for (auto& first : firsts) {
					if (first.begin()->first == vect[i].first) {
						for (int k = 1; k < first.size(); k++) {
							if (find(vect, first[k].first) == -1 && first[k].first != "e") {
								vect.push_back(pair<string, string>(first[k].first, ""));

							}
						}
						if (find(first, "e") != -1) {
							for (auto& fol : follow) {
								if (fol[0].first == vect[i].first)
								{
									for (int j = 1; j < fol.size(); j++) {
										if (find(vect, fol[j].first) == -1)
											vect.push_back(fol[j]);
									}
									break;
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

vector<tree*> build_table(vector<vector<string>>&  grammar) {
	vector<tree*> syntax_tree;
	struct tree* current = new tree;

	for (int i = 0; i < grammar.size(); i++) {
	
		current = init(grammar[i].front());
		for (int j = 1; j < grammar[i].size(); j++) {
			current->children.push_back(grammar[i][j]);
		}
		syntax_tree.push_back(current);
	}

	return syntax_tree;

}

vector<vector<string>> print_tree(tree* initial) {

	vector <vector<string>> output;
	output.push_back(vector<string>());
	output.back().push_back(initial->production);
	output.push_back(vector<string>());
	struct tree* current = new tree;
	current = initial->chosen[0];
	int line = 0;
	int length_max = 0;

	while (1) {
		int pos = 0;
		line++;
		int length = 0;
		int length_cur = 0;
		output[line].push_back(current->production);
		output[line].push_back(" ");
		for (auto& elem : output[line]) {
			length_cur += elem.length();
		}
		if (length_cur > length_max)
			length_max = length_cur;
		for (int j = 0; j < line; j++) {
			length = 0;
			for(auto& elem : output[j]) {
				length += elem.length();
			}
			if (length < length_max) {
				for (int i = 0; i < length_max - length; i++) { 
					output[j].push_back(" ");
				} 
			}
		}
		if (current->production.front() != '<') {
			int start = line ;
			if (current->parent->chosen.size() > 1) {
				start = line + 1;
			}
			else
				length_cur = length_max;
			for (int j = start; j < output.size(); j++) {
				length = 0;
				for (auto& elem : output[j]) {
					length += elem.length();
				}
				for (int i = 0; i < length_cur - length; i++)
				{
					output[j].push_back(" ");
				}
			}
			for (int i = 0; i < current->parent->chosen.size(); i++) {
				if (current->parent->chosen[i] == current) {
					if (i != current->parent->chosen.size() - 1) {
						pos = i + 1;
						current = current->parent;
						line = line--;
						break;
					}
					else {
						current = current->parent;
						if (current == initial) {
							return output;
						}
						line--;
						i = -1;
						continue;
					}

				}
			}
		}
		current = current->chosen[pos];
		if (line == output.size() - 1) {
			output.push_back(vector<string>());
			if (output[line].front() == " " || output[line].front().front() != '<') {
				length = 0;
				for (int i = 0; i < output[line].size() - 2; i++) {
					length += output[line][i].length();
				}
				for (int i = 0; i < length; i++) {
					output.back().push_back(" ");
				}
			}	
		}
			
	}

	delete current;
			
}
	
	





int main() {
	string line;
	ifstream myfile;
	myfile.open("grammar.txt");
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
	vector<string> tokens;
	ofstream fout("output.txt");
	if (fout.is_open()) {
		fout << "GRAMMAR" << endl;
		for (int i = 0; i < grammar.size(); i++) {

			fout << grammar[i];
			fout << endl;
			vector<vector<string>> right_side;
			tokens = split(grammar[i], "::=");

			if (gramm.size() == 0 || gramm.back().size() != 1) {
				gramm.push_back(vector<string>());
				gramm.back().push_back(tokens[0]);
			}
			tokens = split(tokens[1], "|");
			for (int j = 0; j < tokens.size(); j++) {
				vector<string> elements = split(tokens[j], "<");
				right_side.push_back(elements);
			}
			string rule = "";
			for (int j = 0; j < right_side.size(); j++) {
				if (right_side[j].front() == gramm[i].front()) {
					right_side[j].push_back(gramm[i].front());
					right_side[j].back().erase(right_side[j].back().size() - 1);
					right_side[j].back() += "'>";
					right_side[j].erase(right_side[j].begin());
					for (int z = 0; z < right_side[j].size(); z++) {
						rule += right_side[j][z];
					}
					rule += "|";
					right_side.erase(right_side.begin() + j);
					j--;
				}
			}

			if (rule != "") {
				gramm.push_back(vector<string>());
				gramm.back().push_back(gramm[i].front());
				gramm.back().back().erase(gramm.back().back().size() - 1);
				gramm.back().back() += "'>";
				rule += "e";
				for (auto &alternate : right_side) {
					alternate.push_back(gramm.back().front());

				}
				fout << "Production " << gramm[i].front() << " has left recursion. Fixed production:" << endl;
				fout << gramm[i].front() <<"::=";
				for (int z = 0; z < right_side.size(); z++) {
					for (int k = 0; k < right_side[z].size(); k++) {
							fout << right_side[z][k];
					}
					if (z != right_side.size() - 1)
						fout << "|";
				}
				fout << endl;
				rule.insert(0, gramm.back().front() + "::=");
				grammar.insert(grammar.begin() + i + (gramm.size() - 1-i), rule);
				rule.clear();
			}

			for (int j = 0; j < right_side.size(); j++) {
				for (int z = j + 1; z < right_side.size(); z++) {
					if (right_side[j].front() == right_side[z].front()) {
						if (rule=="") {
							right_side.push_back(vector<string>());
							right_side.back().push_back(right_side[j].front());
							right_side.back().push_back(gramm.back().front());
							right_side.back().back().erase(right_side.back().back().end());
							right_side.back().back() += "'>";
							gramm.push_back(vector<string>());
							gramm.back().push_back(right_side.back().back());
						}
						if (right_side[z].back() == gramm.back().front()) {
							break;
						}
						right_side[z].erase(right_side[z].begin());
						if (right_side[z].size() == 0)
							rule += "e";
						else {
							for (string token : right_side[z]) {
								rule += token;
							}
						}
						rule += "|";
						right_side.erase(right_side.begin() + z);
						z--;
					}
				}
				if (rule != "") {
					right_side[j].erase(right_side[j].begin());
					if (right_side[j].size() == 0)
						rule.insert(0, "e|");
					else
						rule.insert(0, "|");
					for (int z = 0; z < right_side[j].size(); z++) {
						rule.insert(z, right_side[j][z]);
					}
					right_side.erase(right_side.begin() + j);
					rule.erase(rule.size() - 1);
					rule.insert(0, gramm.back().front() + "::=");
					grammar.insert(grammar.begin() + i + (grammar.size() - i-1), rule);
					fout << "Production " << gramm[i].front() << " does not fulfill I grammatical rule. Applying left-factoring and rewriting production:" << endl;
					fout << gramm[i].front() + "::=";
					for (int z = 0; z < right_side.size(); z++) {
						for (int k = 0; k < right_side[z].size(); k++) {
							fout << right_side[z][k];
						}
						if (z != right_side.size() - 1)
							fout << "|";
					}
					fout << endl;
					
				}
			}
			for (auto& line : right_side) {
				gramm[i].push_back("");
				for (string token : line) {
					gramm[i].back() += token;
				}
			}
		}
	

		vector<tree*> syntax_tree = build_table(gramm);

		/*for (int i = 0; i < syntax_tree.size(); i++)
		{
			cout << syntax_tree[i]->production << endl;
			for (auto altern : syntax_tree[i]->children) {
					cout << altern->production << "";
				
				cout << endl;
			}
			cout << endl;
		}*/


		vector<vector<pair<string, string>>> first = firsts(gramm);

		vector<vector<pair<string, string>>> follow = follows(gramm, first);

		fout << "==============================================================================================================================" << endl;
		fout << "FIRSTS" << endl;
		for (int i = 0; i < first.size(); i++)
		{
			for (auto& it : first[i]) {
				fout << "(" << it.first << "," << it.second << ")";
			}
			fout << endl;
		}
		fout << "==============================================================================================================================" << endl;
		fout << "FOLLOWS" << endl;
		for (int i = 0; i < follow.size(); i++)
		{
			for (auto& it : follow[i]) {
				fout << "(" << it.first << "," << it.second << ")";
			}
			fout << endl;
		}

		for (auto& production : gramm) {
			vector<pair<string, string>> firsts = find_vector(first, production[0]);
			vector<pair<string, string>> follows = find_vector(follow, production[0]);

			for (int i = 1; i < firsts.size(); i++) {
				if (find(follows, firsts[i].first) != -1) {
					if (find(firsts, "e") != -1) {
						fout << "Error. Production " << production[0] << " does not fulfull gramatical rule 2.";
						return 0;
					}
				}
			}
		}

		fout << "==============================================================================================================================" << endl;

		myfile.open("expression.txt");
		string expression;
		if (myfile.is_open())
		{
			getline(myfile, expression);
			myfile.close();
		}
		else cout << "Unable to open file";
		/*cout << "Enter expression to check. At the end press Enter: ";
		cin >> expression;*/
		expression += "$";
		struct tree* root = syntax_tree.front();
		struct tree* current = new tree;
		current = root;

		int pos = 0;
		int ind = 0;


		while (1) {

			if (current->production == string(1, expression[pos])) {
				pos++;
				for (int i = 0; i < current->parent->chosen.size(); i++) {
					if (current->parent->chosen[i] == current) {
						if (i != current->parent->chosen.size() - 1) {
							current = current->parent->chosen[i + 1];
							break;
						}

						else {
							current = current->parent;
							for (int i = 0; i < current->parent->chosen.size(); i++) {
								if (current->parent->chosen[i] == current) {
									if (i != current->parent->chosen.size() - 1) {
										current = current->parent->chosen[i + 1];

										break;
									}
									else {
										current = current->parent;
										i = -1;
										continue;
									}
								}
							}

						}
					}
				}
			}
			vector<pair<string, string>> set = find_vector(first, current->production);
			if ((ind = find(set, string(1, expression[pos]))) != -1) {
				if (set[ind].second == "&") {
					struct tree* buff = new tree;
					buff = init(string(1, expression[pos]));
					buff->parent = current;
					current->chosen.push_back(buff);
					pos++;
					for (int j = 0; j < current->parent->chosen.size(); j++) {
						if (current->parent->chosen[j]->production == current->production) {
							if (j != current->parent->chosen.size() - 1) {
								current = current->parent->chosen[j + 1];
								break;
							}
							else {
								if (current->parent == root)
									break;
								else {
									current = current->parent;
									j = -1;
									continue;
								}
							}
						}
					}
					continue;
				}
				for (auto& child : current->children) {
					if (child.find(set[ind].second) != string::npos) {
						if (child[0] == expression[pos] || child.find(set[ind].second) == 0) {
							vector<string> tokens = split(child, "<");
							for (auto& token : tokens) {
								if (token.front() != '<') {
									struct tree* buff = new tree;
									buff = init(token);
									buff->parent = current;
									current->chosen.push_back(buff);
									continue;
								}
								for (auto& leaf : syntax_tree) {
									if (leaf->production == token) {
										if (leaf->chosen.size()==0 && leaf->parent==NULL) {
											current->chosen.push_back(leaf);
											leaf->parent = current;
										}
										else {
											struct tree* buff = new tree;
											buff = init(leaf->production);
											buff->children.insert(buff->children.begin(), leaf->children.begin(), leaf->children.end());
											buff->parent = current;
											current->chosen.push_back(buff);
										}
										break;
									}
								}
							}
						}
					}
				}
				current = current->chosen.front();
			}

			else if (find(set, "e")!=-1){
				for (int i = 0; i < current->parent->chosen.size(); i++) {
					if (current->parent->chosen[i] == current) {
						if (i != 0) {
							set = find_vector(follow, current->parent->chosen[i - 1]->production);
							if (find(set, string(1, expression[pos])) == -1) {
								fout << "Error. Character " << expression[pos] << " is not acceptable. Expression doesn't belong to grammar1" << endl;
								return 0;
							}
						}
						if (current->chosen.size() == 0) {
							struct tree* terminal = new tree;
							terminal = init("e");
							current->chosen.push_back(terminal);
							terminal->parent = current;
						}
						if (i != current->parent->chosen.size() - 1)
							current = current->parent->chosen[i + 1];
						else {
							if (current->parent == root) {
								current = current->parent;
								set = find_vector(follow, current->production);
								if (find(set, string(1, expression[pos])) != -1) {
									vector<vector<string>> output_tree = print_tree(root);
									for (auto& line : output_tree) {
										for (string& production : line) {
											fout << production;
										}
										fout << endl;
									}
									fout << "String accepted.";
									return 1;
								}
							}
							current = current->parent;
							for (int j = 0; j < current->parent->chosen.size(); j++) {
								if (current->parent->chosen[j]->production == current->production) {
									if (j != current->parent->chosen.size() - 1) {
										current = current->parent->chosen[j + 1];
										break;
									}
									else {
										if (current->parent == root)
											break;
										else {
											current = current->parent;
											j = -1;
											continue;
										}
									}
								}
							}
						}
						break;

					}
				}
			}

			else {
				fout << "Error. Character " << expression[pos] << " is not acceptable. Expression doesn't belong to grammar2" << endl;
				return 0;
			}
		}


	}							
						
}


