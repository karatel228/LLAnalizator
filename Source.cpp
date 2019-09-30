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
	vector<vector<tree*>> child;
	vector<tree*> chosen;
};

struct tree* init(string a)  
{
	struct tree* lst = new tree;
	lst->parent = 0; 
	lst->production = a;

	return(lst);
}

struct tree* addelem(tree* lst, string production)
{
	struct tree* temp = new tree;
	temp->production = production; // сохранение поля данных добавляемого узла
	temp->parent = lst; 
	return(temp);
}
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

int find(vector<pair<string, string>> vect, string symbol) {
	for (int i = 0; i < vect.size(); i++) {
		if (vect[i].first == symbol)
			return i;
	}
	return -1;
}

vector<pair<string, string>> findAll(vector<pair<string, string>> vect, string symbol) {
	vector<pair<string, string>> finded;
	for (int i = 0; i < vect.size(); i++) {
		if (vect[i].first == symbol)
			finded.push_back(vect[i]);
	}

	return finded;
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

vector<vector<pair<string, string>>> follows(vector<vector<string>>  grammar, vector<vector<pair<string, string>>> firsts) {
	vector<vector<pair<string, string>>> follow;
	vector<pair<string, string>> expr;
	follow.push_back(vector <pair<string, string>>());
	follow[0].push_back(pair<string, string>(grammar[0][0], ""));
	follow[0].push_back(pair<string, string>("$", "&"));

	for (auto elem : grammar) {
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
						if (tokens[j + 1].front() == '<') {
							follow[pos].push_back(pair<string, string>(tokens[j + 1], "&"));
						}
						else {
							if (j + 2 != tokens.size())
								follow[pos].push_back(pair<string, string>(tokens[j + 1], tokens[j+2]));
							else
								follow[pos].push_back(pair<string, string>(tokens[j + 1], elem[0]));

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
				for (auto first : firsts) {
					if (first.begin()->first == vect[i].first) {
						for (int k = 1; k < first.size(); k++) {
							if (find(vect, first[k].first) == -1 && first[k].first != "e") {
								vect.push_back(pair<string, string>(first[k].first, vect[i].first));

							}

							}
						if (find(first, "e") != -1) {
							for (auto fol : follow) {
								if (fol[0].first == vect[i].first)
								{
									for (int j = 1; j < fol.size(); j++) {
										if (find(vect, fol[j].first) == -1)
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

void print_tree(tree* initial, ostream& out) {

	out << initial->production;
	struct tree* current = new tree;
	current = initial;

	while (1) {
		for (auto child : current->chosen) {
			out << child->production << "   ";
		}

		if (current->parent != NULL) {
			for (int i = 0; i < current->parent->chosen.size(); i++) {
				if (current->parent->chosen[i] == current) {
					if (i != current->parent->chosen.size() - 1)
						current = current->parent->chosen[i + 1];
					else
						current = current->parent->chosen[0]->chosen[0];
					break;
				}
			}
		}
		else
			current = current->chosen[0];
	}
	
	

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
	vector<tree*> syntax_tree;
	struct tree* root = new tree;;

	for (int i = 0; i < grammar.size(); i++) {

		struct tree* current = new tree;

		gramm.push_back(vector<string>());
		tokens = split(grammar[i], "::=");
		gramm[i].push_back(tokens[0]);
		tokens = split(tokens[1], "|");
		gramm[i].insert(gramm[i].end(), tokens.begin(), tokens.end());
		if (i == 0) {
			root = init(gramm[i].front());
			syntax_tree.push_back(root);
			current = root;
		}
		else {
			for (auto leaf : syntax_tree) {
				if (leaf->production == gramm[i].front()) {
					current = leaf;
					break;
				}

			}
		}
		for (string token : tokens) {
			current->child.push_back(vector<tree*>());
			vector<string> children = split(token, "<");

			for (string child : children) {
				if (child.front() != '<') {
					struct tree* buff = new tree;
					buff = init(child);
					buff->parent = current;
					current->child.back().push_back(buff);
					continue;
				}
				for (auto leaf : syntax_tree) {
					struct tree* buff = new tree;
					if (leaf->production == child) {
						buff = init(leaf->production);
						buff->parent = current;
						for (auto alternat : leaf->child) {
							buff->child.push_back(vector<tree*>());
							for (auto elem : alternat) {
								struct tree* new_child = new tree;
								new_child = init(elem->production);
								new_child->parent = buff;
								buff->child.back().push_back(new_child);
							}
						}
						current->child.back().push_back(buff);
						break;
					}
					if (syntax_tree.back() == leaf && leaf->production != child) {
						buff = init(child);
						buff->parent = current;
						current->child.back().push_back(buff);
						syntax_tree.push_back(buff);
					}


				}
			}


		}

		for (auto leaf : syntax_tree) {
			if (leaf->production == gramm[i].front()) {
				leaf->child = current->child;
			}

		}

	}

	for (int i = 0; i < syntax_tree.size(); i++)
	{
		cout << syntax_tree[i]->production << endl;
		for (auto altern : syntax_tree[i]->child) {
			for (auto leaf : altern) {
				cout << leaf->production << "";
			}
			cout << endl;
		}
		cout << endl;
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
	struct tree* current = new tree;
	current = root;

	int pos = 0;
	int ind = 0;

	ofstream fout("output.txt");
	if (fout.is_open()) {
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
				continue;
			}

			if (current->child.empty()) {
				for (auto leaf : syntax_tree) {
					if (leaf->production == current->production) {
						for (auto alternative : leaf->child) {
							current->child.push_back(vector<tree*>());
							for (auto nonterm : alternative) {
								struct tree* buff = new tree;
								buff = init(nonterm->production);
								buff->parent = current;
								current->child.back().push_back(buff);
							}
						}
						break;
					}
				}
			}
			vector<pair<string, string>> set = find_vector(first, current->production);
			if ((ind = find(set, string(1, expression[pos]))) != -1) {
				if (set[ind].second == "&") {
					struct tree* terminal = new tree;
					terminal = init(set[ind].first);
					current->chosen.push_back(terminal);
					/*fout << current->production << "-";
					for (auto leaf : current->chosen) {
						fout << leaf->production;
					}
					fout << endl;*/
					pos++;
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
					continue;
				}
				for (auto alternative : current->child) {
					if (alternative.front()->production == set[ind].second || alternative.front()->production == set[ind].first) {
						current->chosen = alternative;
						/*fout << current->production << "-";
						for (auto leaf : current->chosen) {
							fout << leaf->production;
						}
						
						<< endl;*/
						alternative.front()->parent = current;
						current = alternative.front();
						break;
					}
				}
			}

			else if (find(set, "e") != -1) {
				for (int i = 0; i < current->parent->chosen.size(); i++) {
					if (current->parent->chosen[i] == current) {
						if (i != 0) {
							set = find_vector(follow, current->parent->chosen[i - 1]->production);
							if (find(set, string(1, expression[pos])) == -1) {
								fout << "Error. Character " << expression[pos] << " is not acceptable. Expression doesn't belong to grammar" << endl;
								return 0;
							}
						}
						if (i != current->parent->chosen.size() - 1)
							current = current->parent->chosen[i + 1];
						else {
							if (current->parent == root) {
								current = current->parent;
								set = find_vector(follow, current->production);
								if (find(set, string(1, expression[pos])) != -1) {
									fout << root->production;
									struct tree* current_1 = new tree;
									current_1 = root;

									while (1) {
										for (auto child : current_1->chosen) {
											fout << child->production << "   ";
										}

										if (current_1->parent != NULL) {
											for (int i = 0; i < current_1->parent->chosen.size(); i++) {
												if (current_1->parent->chosen[i] == current_1) {
													if (i != current_1->parent->chosen.size() - 1)
														current_1 = current_1->parent->chosen[i + 1];
													else {
														current_1 = current_1->parent->chosen[0]->chosen[0];
														fout << endl;
													}
														
													break;
												}
											}
										}
										else {
											current_1 = current_1->chosen[0];
											fout << endl;
										}
											
										
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
				fout << "Error. Character " << expression[pos] << " is not acceptable. Expression doesn't belong to grammar" << endl;
				return 0;
			}
		}
	}
}
	


	


						


	
	/*else {
		cout << "Unable to open file.";
		return 0;
	}

	
	tree* curr = new tree;
	curr = syntax_tree;*/

	
	


