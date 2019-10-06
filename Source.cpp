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
	follow[0].push_back(pair<string, string>("$", ""));

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
				for (auto first : firsts) {
					if (first.begin()->first == vect[i].first) {
						for (int k = 1; k < first.size(); k++) {
							if (find(vect, first[k].first) == -1 && first[k].first != "e") {
								vect.push_back(pair<string, string>(first[k].first, ""));

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

vector<tree*> build_table(vector<vector<string>>  grammar) {
	vector<tree*> syntax_tree;
	struct tree* root = new tree;
	struct tree* current = new tree;

	for (int i = 0; i < grammar.size(); i++) {
		
		if (i == 0) {
			root = init(grammar[i].front());
			syntax_tree.push_back(root);
			current = root;
		}
		else {
			for (auto leaf : syntax_tree) {
				if (leaf->production == grammar[i].front()) {
					current = leaf;
					break;
				}

			}
		}
		for (int j = 1; j < grammar[i].size(); j++) {
			current->child.push_back(vector<tree*>());
			vector<string> children = split(grammar[i][j], "<");

			for (string child : children) {
				struct tree* buff = new tree;
				buff = init(child);
				buff->parent = current;
				if (child.front() != '<') {
					current->child.back().push_back(buff);
					continue;
				}
				for (auto leaf : syntax_tree) {
					if (leaf->production == child) {
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
						current->child.back().push_back(buff);
						syntax_tree.push_back(buff);
					}


				}
			}


		}
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
		for (auto elem : output[line]) {
			length_cur += elem.length();
		}
		if (length_cur > length_max)
			length_max = length_cur;
		for (int j = 0; j < line; j++) {
			length = 0;
			for(auto elem : output[j]) {
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
				for (auto elem : output[j]) {
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
				gramm[i].push_back(tokens[0]);
			}	
			tokens = split(tokens[1], "|");
			for (int j = 0; j < tokens.size(); j++) {
				vector<string> elements = split(tokens[j], "<");
				right_side.push_back(elements);
			}
			for (int j = 0; j < right_side.size(); j++) {
				if (right_side[j].front() == gramm[i].front())
					right_side[j].push_back("LR");
				for (int z = j + 1; z < right_side.size(); z++) {
					if (right_side[j].front() == right_side[z].front()) {
						if (right_side[j].back() != "I")
							right_side[j].push_back("I");
						right_side[z].push_back("I");
					}
				}
				if (right_side[j].back() == "LR" || right_side[j].size()>1 && right_side[j][right_side[j].size() - 2] == "LR") {
					string rule = gramm.back().front();
					rule.erase(rule.end() - 1);
					rule += "'>";
					gramm.push_back(vector<string>());
					gramm.back().push_back(rule);
					rule += "::=";
					for (int z = j; z < right_side.size(); z++) {
						if (right_side[z].back() == "I" || z==j) {
							if (right_side[z].back() == "I" && z == j)
								right_side[z].erase(right_side[z].end() - 2, right_side[z].end());
							else
								right_side[z].erase(right_side[z].end() - 1);
							right_side[z].push_back(gramm.back().front());
							right_side[z].erase(right_side[z].begin());
							for (int k = 0; k < right_side[z].size(); k++) {
								rule += right_side[z][k];
							}
							rule += "|";
							right_side[z].push_back("I");
							/*right_side.erase(right_side.begin() + z);
							if (z == j)
								j--;
							z--;*/
						}
						
					}
					
					for (int z = 0; z < right_side.size(); z++) {
						if (z < j) {
							gramm[i].erase(gramm[i].begin() + z + 1);
						}
						if (right_side[z].back() != "I") {

							right_side[z].push_back(gramm.back().front());
						}
						else {
							right_side.erase(right_side.begin() + z);
							if(z==j&&j!=0)
								j--;
							z--;
						}
						

					}

					fout << "Production " << gramm[i].front() << " has left recursion." << endl;
					rule += "e";
					grammar.insert(grammar.begin() + i + 1, rule);
				}
				if (right_side[j].back() == "I") {
					string rule = gramm.back().front();
					rule.erase(rule.end() - 1);
					rule += "'>";
					gramm.push_back(vector<string>());
					gramm.back().push_back(rule);
					rule += "::=";
					gramm[i].push_back(right_side[j].front() + gramm.back().front());
					for (int z = j; z < right_side.size(); z++) {
						if (right_side[z].back() == "I") {
							right_side[z].erase(right_side[z].end() - 1);
							right_side[z].erase(right_side[z].begin());
							if (right_side[z].size() == 0)
								rule += "e";
							else {
								for (string elem : right_side[z]) {
									rule += elem;
								}
							}
							if (z != right_side.size() - 1)
								rule += "|";
							right_side.erase(right_side.begin() + z);
							z--;
						}

					}
					grammar.insert(grammar.begin() + i + 1, rule);


				}
				else {
					gramm[i].push_back("");
					for (int z = 0; z < right_side[j].size(); z++) {
						gramm[i].back() += right_side[j][z];
					}
				}
				if (j == right_side.size()-1  && i!=gramm.size()-1) {
					fout << "Production " << gramm[i].front() << " does not fulfill I grammatical rule. Applying left-factoring and rewriting production:" << endl;
					fout << gramm[i].front() << "::=";
					tokens.clear();
					for (int z = 1; z < gramm[i].size(); z++) {
						tokens.push_back(gramm[i][z]);
						fout << gramm[i][z];
						if (z != gramm[i].size() - 1)
							fout << "|";
					}
					fout << endl;
				}
			}

		}

		vector<tree*> syntax_tree = build_table(gramm);

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

		for (auto production : gramm) {
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
					terminal->parent = current;
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
									/*fout << root->production;
									struct tree* current_1 = new tree;
									current_1 = root;*/

									/*while (1) {
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
									}*/
									vector<vector<string>> output_tree = print_tree(root);
									for (auto line : output_tree) {
										for (string production : line) {
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

	
	


