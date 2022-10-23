#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

using namespace std;

map<string, vector<string>> grammar;
map<string, vector<string>> grammar_cnf;
map<string, vector<vector<bool>>> cyk;
//vector<string> terminals;
string terminals = " ";
vector<vector<string>> table;
bool visualization = false;
bool visualization_steps = false;
struct Point {
    int x;
    int y;
};

bool operator == (Point l, Point r) {
    if(l.x == r.x && l.y == r.y)
        return true;
    return false;
}

vector<vector<pair<string, Point>>> tree;

struct Node {
    Node* l;
    Node* r;
    string term;
};

void print_table() {
    size_t max_cell = 0;
    vector<vector<string>> table_temp = table;
    for(size_t i = 0; i < table_temp.size(); ++i) {
        for(size_t j = 0; j < table_temp[i].size(); ++j) {
            if(table_temp[i][j].size() > max_cell) {
                max_cell = table_temp[i][j].size();
            }
        }
    }
    for(size_t i = 0; i < table_temp.size() - 1; ++i) {
        cout << table_temp.size() - 1 - i << (table_temp.size() - 1 - i < 10 ? " |" : "|");
        for(size_t j = 1; j < table_temp[i].size(); ++j) {
            if(j > i + 1)
                continue;
            string spaces;
            spaces.assign((max_cell - table_temp[table_temp.size() - 1 - i][j].size() + 1) / 2, ' ');
            table_temp[table_temp.size() - 1 - i][j] = spaces + table_temp[table_temp.size() - 1 - i][j] + spaces;
            if (table_temp[table_temp.size() - 1 - i][j].size() < max_cell + 1)
                table_temp[table_temp.size() - 1 - i][j] = table_temp[table_temp.size() - 1 - i][j] + " ";
            cout << table_temp[table_temp.size() - 1 - i][j] << "|";
        }
        cout << "\n";
    }
    string spaces;
    cout << " ";
    spaces.assign(max_cell / 2, ' ');
    for(size_t i = 0; i < table_temp.size() - 1; ++i) {
        cout << (i < 10 ? " " : "") << spaces << i + 1 << (max_cell % 2 == 1 ? " " : "") << spaces;
    }
    cout << "\n";
}

void resize_cyk(size_t n) {
    table.resize(n + 1, vector<string>(n + 1, ""));
    for(auto &i : grammar_cnf) {
        cyk[i.first].resize(n + 1, vector<bool>(n + 1, false));
    }
}

bool init_cyk(string &test_line, size_t n) {
    stringstream os(test_line);
    string word;
    vector<pair<string, bool>> checker;
    bool error = false;
    for(size_t i = 1; i <= n; ++i) {
        os >> word;
        checker.push_back(make_pair(word, false));
        for(auto &production : grammar_cnf) {
            for(string &right_p : production.second) {
                if(word == right_p) {
                    checker[i - 1].second = true;
                    cyk[production.first][1][i] = true;
                    if(table[1][i].empty())
                        table[1][i] = production.first;
                    else
                        table[1][i] += " " + production.first;
                    Point p = {1, (int)i};
                    tree.push_back({make_pair(production.first, p), make_pair(word, p), make_pair("", p)});
                }
            }
        }
        if(!checker[i - 1].second) {
            if(!error) {
                cout << "Ошибка! В грамматике не найдено: ";
                error = true;
            }
            cout << "\"" + checker[i - 1].first + "\"" << "; ";
        }
    }
    if(error)
        cout << "\n";
    return error;
}

void print_cyk() {
    for(auto &i : cyk) {
        cout << "==== " << i.first << " ====\n";
        for(size_t j = 0; j < i.second.size(); ++j) {
            for(size_t k = 0; k < i.second[j].size(); ++k) {
                cout << (int)i.second[j][k] << " ";
            }
            cout << "\n";
        }
        cout << "===========\n";
    }
}

bool algo_cyk(int n) {
    for(int i = 2; i <= n; ++i) {
        for(int j = 1; j <= n - i + 1; ++j) {
            for(int k = 1; k <= i - 1; ++k) {
                for(auto &production : grammar_cnf) {
                    for(string &pr : production.second) {
                        if(count(pr.begin(), pr.end(), ' ') < 1 || pr.length() < 2)
                            continue;
                        size_t sp = pr.find(' ');
                        if(cyk[pr.substr(0, sp)][k][j] && cyk[pr.substr(sp + 1, pr.length() - sp)][i - k][j + k]) {
                            cyk[production.first][i][j] = true;
                            if(table[i][j].empty())
                                table[i][j] = production.first;
                            else
                                table[i][j] += " " + production.first;
                            Point p0 = {i, j};
                            Point p1 = {k, j};
                            Point p2 = {i - k, j + k};
                            tree.push_back({make_pair(production.first, p0), make_pair(pr.substr(0, sp), p1), make_pair(pr.substr(sp + 1, pr.length() - sp), p2)});
                            if(visualization) {
                                system("clear");
                                cout << production.first << " (" << i << ", " << j << ") " << "<- "
                                << pr.substr(0, sp) << " (" << k << ", " << j << ") + " << pr.substr(sp + 1, pr.length() - sp)
                                << " (" << i - k << ", " << j + k << ")\n";
                                print_table();
                                cout << "Нажмите любую клавишу для продолжения...\n";
                                getchar();
                            }
                        }
                    }
                }
            }
        }
    }
    if(cyk["S0"][n][1])
        return true;
    else
        return false;
}

void print_grammar() {
    cout << "Grammar\n";
    for(auto &i : grammar) {
        cout << i.first << " -> ";
        for(size_t j = 0; j < i.second.size(); ++j) {
            cout << i.second[j];
            if(j < i.second.size() - 1)
                cout << " | ";
        }
        cout << "\n";
    }
}

void print_grammar_cnf() {
    cout << "Grammar_CNF\n";
    for(auto &i : grammar_cnf) {
        cout << i.first << " -> ";
        for(size_t j = 0; j < i.second.size(); ++j) {
            cout << i.second[j];
            if(j < i.second.size() - 1)
                cout << " | ";
        }
        cout << "\n";
    }
}

void print_tree() {
    for(int i = 0; i < tree.size(); ++i) {
        cout << tree[tree.size() - 1 - i][0].first << " -> " << tree[tree.size() - 1 - i][1].first << " + " << tree[tree.size() - 1 - i][2].first << "\n";
    }
}

void build_tree(Node* root, pair<string, Point> &v) {
    root->term = v.first;
    if(v.second.x == 1) {
        Node* l = new Node;
        l->term = v.first;
        l->l = nullptr;
        l->r = nullptr;
        root->l = l;
        for(int i = 0; i < tree.size(); ++i) {
            if(tree[i][0].first == v.first && tree[i][0].second == v.second) {
                l->term = "\"" + tree[i][1].first + "\"";
//                l->term = "(" + tree[i][1].first + ")";
                break;
            }
        }
        return;
    }
    Node* l = new Node;
    l->l = nullptr;
    l->r = nullptr;
    Node* r = new Node;
    r->l = nullptr;
    r->r = nullptr;

    root->l = l;
    root->r = r;
    pair<string, Point> l_t;
    pair<string, Point> r_t;
    for(int i = 0; i < tree.size(); ++i) {
        if(tree[i][0].first == v.first && tree[i][0].second == v.second) {
            l_t = tree[i][1];
            r_t = tree[i][2];
            break;
        }
    }
    build_tree(l, l_t);
    build_tree(r, r_t);
}

void print_tree_node(Node* root) {
    if(root) {
        print_tree_node(root->l);
        cout << root->term << "\n";
        print_tree_node(root->r);
    }
}

//static string ch_hor = "-", ch_ver = "|", ch_ddia = "/", ch_rddia = "\\", ch_udia = "\\", ch_ver_hor = "|-", ch_udia_hor = "\\-", ch_ddia_hor = "/-", ch_ver_spa = "| ";
static string ch_hor = "\u2500" /*─*/, ch_ver = "\u2502" /*│*/, ch_ddia = "\u250C" /*┌*/, ch_rddia = "\u2510" /*┐*/, ch_udia = "\u2514" /*└*/, ch_ver_hor = "\u251C\u2500" /*├─*/, ch_udia_hor = "\u2514\u2500" /*└─*/, ch_ddia_hor = "\u250C\u2500" /*┌─*/, ch_ver_spa = "\u2502 " /*│ */;
void dump0(Node const * node, string const & prefix = "", bool root = true, bool last = true) {
    cout << prefix << (root ? "" : (last ? ch_udia_hor : ch_ver_hor)) << (node ? (node->term) : "") << "\n";
    if (!node || (!node->l && !node->r))
        return;
    vector<Node*> v{node->l, node->r};
    for (size_t i = 0; i < v.size(); ++i)
        dump0(v[i], prefix + (root ? "" : (last ? "  " : ch_ver_spa)), false, i + 1 >= v.size());
}

void dump2(Node const * node, string const & rpref = "", string const & cpref = "", string const & lpref = "") {
    if (!node) return;
    if (node->r)
        dump2(node->r, rpref + "  ", rpref + ch_ddia_hor, rpref + ch_ver_spa);
    cout << cpref << node->term << endl;
    if (node->l)
        dump2(node->l, lpref + ch_ver_spa, lpref + ch_udia_hor, lpref + "  ");
}


void cf_to_cnf() {
    vector<string> eps;

    // Step 1 Создаем новый стартовый нетерминал
    grammar_cnf["S0"].push_back("S");

    // Step 2 Удаляем все неодиночные терминалы в правых частях
    for(auto &i : grammar) {
        for(size_t j = 0; j < i.second.size(); ++j) {
            string comps = i.second[j];
            size_t comp_s = count(comps.begin(), comps.end(), ' ') + 1;
            string comps_cnf = "";
            if(comp_s == 1) {
                if(comps.empty()) {
                    eps.push_back(i.first);
                }
                if(comps[0] < 'A' || comps[0] > 'Z')
                    terminals += comps + " ";
                grammar_cnf[i.first].push_back(comps);
            } else {
                for (size_t k = 0; k < comp_s; ++k) {
                    size_t idx = comps.find(' ');
                    string cut = comps.substr(0, idx);
                    if (cut[0] >= 'A' && cut[0] <= 'Z') {
                        comps_cnf += cut + " ";
                    } else {
                        string NT = "N_" + cut;
                        comps_cnf += NT + " ";
                        if (grammar_cnf.find(NT) == grammar_cnf.end()) {
                            grammar_cnf[NT].push_back(cut);
//                            terminals.push_back(cut);
                            terminals += cut + " ";
                        }
                    }
                    comps.erase(0, idx + 1);
                }
                comps_cnf.erase(comps_cnf.size() - 1, comps_cnf.size());
                grammar_cnf[i.first].push_back(comps_cnf);
            }
        }
    }
    if(visualization_steps) {
        cout << "Шаг 1 и 2. Создание нового стартового терминала и удаление неодиночных терминалов в правых частях\n";
        print_grammar_cnf();
        cout << "Нажмите любую клавишу для продолжения...\n";
        getchar();
    }

    // Step 3 Удалить правила длины > 2
    int counter = 1;
    for(auto &i : grammar_cnf) {
        for(size_t j = 0; j < i.second.size(); ++j) {
            string &comps = i.second[j];
            size_t comp_s = count(comps.begin(), comps.end(), ' ') + 1;
            if(comp_s > 2) {
                for (size_t s = comp_s; s > 2; --s) {
                    size_t idx = comps.rfind(' ');
                    string r = comps.substr(idx + 1, comps.size() - idx);
                    comps.erase(idx, comps.size() - idx);
                    idx = comps.rfind(' ');
                    string l = comps.substr(idx + 1, comps.size() - idx);
                    comps.erase(idx, comps.size() - idx);
                    grammar_cnf["A" + to_string(counter)].push_back(l + " " + r);
                    comps += " A" + to_string(counter);
                    counter++;
                }
            }
        }
    }
    if(visualization_steps) {
        cout << "Шаг 3. Удаление правил длины > 2\n";
        print_grammar_cnf();
        cout << "Нажмите любую клавишу для продолжения...\n";
        getchar();
    }

    // Step 4 Удалить непродуктивные правила
    for(size_t i = 0; i < eps.size(); ++i) {
        for(auto &j : grammar_cnf) {
            for(size_t k = 0; k < j.second.size(); ++k) {
                string comp = j.second[k];
                size_t size = count(comp.begin(), comp.end(), ' ') + 1;
                if(size == 1) {
                    if(comp == eps[i]) {
                        grammar_cnf[j.first].push_back("");
                    }
                    continue;
                }
                size_t idx = comp.find(' ');
                string l = comp.substr(0, idx);
                string r = comp.substr(idx + 1, comp.size() - idx);
                if(l == eps[i]) {
                    grammar_cnf[j.first].push_back(r);
                }
                if(r == eps[i]) {
                    grammar_cnf[j.first].push_back(l);
                }
            }
        }
        grammar_cnf[eps[i]].erase(grammar_cnf[eps[i]].end() - 1, grammar_cnf[eps[i]].end());
    }
    if(visualization_steps) {
        cout << "Шаг 4. Удаление непродуктивных правил\n";
        print_grammar_cnf();
        cout << "Нажмите любую клавишу для продолжения...\n";
        getchar();
    }

    // Step 5 Удалить цепные правила
    for(auto &i : grammar_cnf) {
        bool chain_rule_is_exist = true;
        while(chain_rule_is_exist) {
            chain_rule_is_exist = false;
            for(size_t j = 0; j < i.second.size(); ++j) {
                if(i.second[j].empty() || i.second[j][0] < 'A' || i.second[j][0] > 'Z' || count(i.second[j].begin(), i.second[j].end(), ' ') > 0)
                    continue;
                chain_rule_is_exist = true;
                i.second.insert(i.second.begin() + j + 1, grammar_cnf[i.second[j]].begin(), grammar_cnf[i.second[j]].end());
                i.second.erase(i.second.begin() + j);
            }
        }
    }
    if(visualization_steps) {
        cout << "Шаг 5. Удаление цепных праивл\n";
        print_grammar_cnf();
        cout << "Нажмите любую клавишу для продолжения...\n";
        getchar();
    }
}
void check_line(string &line, size_t n) {
    stringstream os(line);
    string word;
    string new_line = "";
    for(size_t i = 0; i < n; ++i) {
        bool exist = true;
        os >> word;
        if(terminals.find(word) > terminals.size()) {
            string find_word = "";
            word.push_back((char)0);
            for(char &symbol : word) {
                find_word.push_back(symbol);
                if(terminals.find(find_word) <= terminals.size()) {
                    continue;
                } else {
                    char temp_symbol = find_word.back();
                    find_word.pop_back();
                    new_line += find_word + " ";
                    find_word = "";
                    find_word.push_back(temp_symbol);
                }
            }
        } else {
            new_line += word + " ";
        }
    }
    line = new_line;
}

int main(int argc, char *argv[]) {
    cout << "Симуляция работы алгоритма CYK\n";
    cout << "На вход подается КС-грамматика. В правилах между любыми двумя терминалами / нетерминалами ставится пробел. Все нетерминалы начинаются с большой буквы.\n";
    cout << "Правила разделяются знаком ';'. Если для одного нетерминала несколько правил, они записываются через знак '|' без пробелов.\n";
    cout << "Все правила записываются в одну строку. Пример: S->S + S|S * S|N;N->0|1;.\n";
    cout << "Если после знака '|' ничего не писать, то в грамматику будет записана пустая строка (например, S->( S ) S|;).\n";
    cout << "Стартовый нетерминал грамматики обязательно 'S'.\n";
    cout << "На проверку подается строка. Все слова с маленькой буквы. Слова разделяются ровно одним пробелом.\n";
    string vis;
    bool file = false;
    cout << "Хотите открыть файл с тестом? (y - да, n - нет): ";
    getline(cin, vis);
    file = vis == "y" || vis == "Y" || vis == "Yes" || vis == "yes";
    string line;
    if(file) {
        cout << "Введите название файла (e[1..10].txt): ";
        string file_name;
        cin >> file_name;
        string path(argv[0]);
        path = path.substr(0, path.rfind('/'));
        path += + "/examples/" + file_name;
        if (!freopen((path).c_str(), "r", stdin)) {
            cout << "Неверное название файла\n";
            return 0;
        }
        getline(cin, line);
        visualization_steps = false;
        visualization = false;
    } else {
        cout << "Включить визуализацию заполнения таблицы CYK? (y - да, n - нет): ";
        getline(cin, vis);
        visualization = vis == "y" || vis == "Y" || vis == "Yes" || vis == "yes";
        if(visualization)
            cout << "Визуализация включена\n";
        else
            cout << "Визуализация отключена\n";
        cout << "Включить визуализацию перевода КС-грамматики в НФХ? (y - да, n - нет): ";
        getline(cin, vis);
        visualization_steps = vis == "y" || vis == "Y" || vis == "Yes" || vis == "yes";
        if(visualization_steps)
            cout << "Визуализация приведения к НФХ включена\n";
        else
            cout << "Визуализация приведения к НФХ отключена\n";

            cout << "Введите грамматику: ";
            getline(cin, line);
    }
    size_t neterm_size = 0;
    while(line.size()) {
        size_t pos = line.find(";");
        string expr = line.substr(0, pos);
        size_t temp_pos = expr.find("->");
        size_t size_to = count(expr.begin(), expr.end(), '|');
        string or_nter = expr.substr(0, temp_pos);
        grammar[or_nter].resize(size_to + 1);
        neterm_size++;
        expr.erase(0, temp_pos + 2);

        for(size_t i = 0; i < size_to + 1; ++i) {
            temp_pos = expr.find("|");
            if(temp_pos)
                grammar[or_nter][i] = expr.substr(0, temp_pos);
            else
                grammar[or_nter][i] = " ";
            expr.erase(0, temp_pos + 1);
        }
        line.erase(0, pos + 1);
    }
    cf_to_cnf();
    cout << "Введенная грамматика:\n";
    print_grammar();
    cout << "НФХ:\n";
    print_grammar_cnf();
    if(!file)
        cout << "Введите строку для проверки: ";
    string test_line;
    getline(cin, test_line);
    string origin_line = test_line;
    size_t n = count(test_line.begin(), test_line.end(), ' ') + 1;
    check_line(test_line, n);
    n = count(test_line.begin(), test_line.end(), ' ');
    resize_cyk(n);
    if(init_cyk(test_line, n))
        return 0;
    bool correct = algo_cyk(n);
    cout << "Итоговая таблица CYK:\n";
    print_table();
    if(correct) {
        Node *root = new Node;
        build_tree(root, tree[tree.size() - 1][0]);
        cout << "Дерево вывода:\n";
        dump2(root);
        cout << "Из данной грамматики можно вывести строку: " << origin_line << "\n";
    } else
        cout << "Из данной грамматики невозможно вывести строку: " << origin_line << "\n";

    return 0;
}
