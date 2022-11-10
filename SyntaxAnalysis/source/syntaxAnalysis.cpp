#include "stdafx.h"

#define syntactic_grammar_FILE_PATH "./LR1_txt/GRAMMAR.txt"
#define grammar_input_FILE_PATH "./LR1_txt/INPUT.txt"

vector<string> Vn = { "E", "S", "T", "F"};
vector<string> Vt = { "(", ")", "*", "+", "i", "#" };//Vn中储存语法非终结符  Vt中储存语法终结符

struct grammar         //定义文法  
{				  //e.g. 文法(0)E->S -- left = E || right = S || num = 0 || length = 1 
	string left;  //文法左部
	string right; //文法右部
	int num;      //文法序号
	int length;   //归约长度
};

vector<grammar> grammars;

struct project            //定义项目
{					  //e.g. I0包括E->·S,# || num = 0 || position = 0 || search = '#'
	int num;      	  //第几条文法
	int position;     //圆点位置
	vector<char>  search;  //向前搜索符(即First集)
};

struct project_set      //定义项目集
{
	string name;    //项目集名称
	vector<project> t;  //定义项目集中的项目t
};

vector<project_set> sets;
map<string, string> m;

bool comp(vector<project> v1, vector<project> v2)
{
	for (int i = 0; i < v2.size(); i++)
	{
		if (find(v1.begin(), v1.end(), v2[i]) == v1.end())
			return false;
	}
	return true;
}
bool comp(vector<char> v1, vector<char> v2)
{
	for (int i = 0; i < v2.size(); i++)
	{
		if (find(v1.begin(), v1.end(), v2[i]) == v1.end())
			return false;
	}
	return true;
}
bool operator == (const project& p1, const project& p2)
{
	if (p1.num == p2.num && p1.position == p2.position && comp(p1.search, p2.search) && comp(p2.search, p1.search))
		return true;
	return false;
}
bool operator == (const project_set& c1, const project_set& c2)
{
	if (comp(c1.t, c2.t) && comp(c2.t, c1.t))
		return true;
	else
		return false;
}

vector<char> getFirst(vector<grammar> yufa, string s, vector<string> Vt)    //得到First集
{
	vector<char> search;
	if (s[0]) //如果s[0]不为空
	{
		vector<string> toEmpty;   //能推出空的语法左部
		for (int i = 0; i < yufa.size(); i++)
		{
			if (yufa[i].right == "$")
				toEmpty.push_back(yufa[i].left);
		}

		string t;
		t = s[0];
		vector<string> ::iterator it = find(Vt.begin(), Vt.end(), t);   //判断第一个是不是终结符
		if (it != Vt.end())       	//如果是终结符，将它加入search
		{
			search.push_back(s[0]);
		}
		else						//如果不是终结符，即是非终结符
		{
			it = find(toEmpty.begin(), toEmpty.end(), t);
			if (it == toEmpty.end())   //不能推出空
			{
				for (int i = 0; i < yufa.size(); i++)
				{
					if (t == yufa[i].left) //如果t是语法文法的左部非终结符
					{
						vector<char> s1 = getFirst(yufa, yufa[i].right, Vt); //递归求得所有的语法文法左部非终结符的first集
						for (int j = 0; j < s1.size(); j++)
						{
							if (find(search.begin(), search.end(), s1[j]) == search.end())  //如果first集中没有s1，则将s1加入first集
								search.push_back(s1[j]);
						}
					}
				}
			}
			else   //能推出空
			{
				for (int i = 0; i < yufa.size(); i++)
				{
					if (t == yufa[i].left)  //t和语法文法的左部相等
					{
						if (yufa[i].right[0] != '$')  //且该语法文法的右部不等于空
						{
							vector<char> s1 = getFirst(yufa, yufa[i].right, Vt);//递归求得所有的语法文法左部非终结符的first集
							for (int j = 0; j < s1.size(); j++)
							{
								if (find(search.begin(), search.end(), s1[j]) == search.end())//如果first集中没有s1，则将s1加入first集
									search.push_back(s1[j]);
							}

						}
					}
				}
				string ss;
				for (int i = 1; i < s.length(); i++)
					ss += s[i];
				vector<char> s1 = getFirst(yufa, ss, Vt);
				for (int j = 0; j < s1.size(); j++)
				{
					if (find(search.begin(), search.end(), s1[j]) == search.end()) //如果s1不在search中则加入
						search.push_back(s1[j]);
				}
			}
		}
	}

	return search;
}

void read_Grammar() {
	fstream file;
	char buff[30];
	file.open(syntactic_grammar_FILE_PATH, ios::in);
	if (!file.is_open()) {
		cout << "Can not find the file--'GRAMMAR.txt',please check!" << endl;
	}
	else {
		int line = 0;
		while (file.getline(buff, 30)) {//读入语法的文法							 
										//e.g. 文法(0)E->S -- left = E || right = S || num = 0 || length = 1
			char* s = buff;
			grammar tmp;
			int length = 0;
			tmp.num = line++;
			tmp.left = *s++;
			s++;
			s++;
			if (*s == '$')
			{
				length = 0;
				tmp.right += *s;
			}
			else {
				while (*s) {				
					length++;
					tmp.right += *s++;
				}
			}	
			tmp.length = length;
			grammars.push_back(tmp);
		}
	}
	file.close();

	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "输入的文法为:\n";
	for (int i = 0; i < grammars.size(); i++)
		cout << i << "   " << grammars[i].left << "->" << grammars[i].right << endl;
	cout << "-----------------------------------------------------------------------------" << endl;
}

void getClosure(project_set &clo) {
	project tmp;//定义项目tmp

	for (int i = 0; i < clo.t.size(); i++) {
		if (clo.t[i].position < grammars[clo.t[i].num].right.size()) {//圆点位置不在最后
			string first;
			first = grammars[clo.t[i].num].right[clo.t[i].position]; //取到圆点后面的字符
			if (find(Vn.begin(), Vn.end(), first) != Vn.end()) {	//若该字符为非终结符
				string s;
				vector<char> search;
				for (int j = clo.t[i].position + 1; j < grammars[clo.t[i].num].right.length(); j++) {
					s += grammars[clo.t[i].num].right[j];
				}
				for (int j = 0; j < clo.t[i].search.size(); j++) {
					string t = s;	//t即为(Ba)
					t += clo.t[i].search[j];
					vector<char> search1 = getFirst(grammars, t, Vt);//得到所有的前项搜索符
					for (int k = 0; k < search1.size(); k++)
					{
						if (find(search.begin(), search.end(), search1[k]) == search.end()) {
							search.push_back(search1[k]);  //得到向前搜索符search集合
						}
					}
				}
				tmp.search = search;  //得到项目temp的向前搜索符search集合
				for (int j = 0; j < grammars.size(); j++) {
					if (grammars[clo.t[i].num].right[clo.t[i].position] == grammars[j].left[0]) {	//如果圆点后的字符等于语法左边的字符
						tmp.num = grammars[j].num;
						tmp.position = 0;
						if (find(clo.t.begin(), clo.t.end(), tmp) == clo.t.end()) {	//如果新加的项目不在项目集中，仍需判断是否该文法已经存在，仅是前向搜索符不同
							bool flag = true;
							for (int k = 0; k < clo.t.size(); k++)
							{
								if (clo.t[k].num == tmp.num && clo.t[k].position == tmp.position) //如果是同心集
								{
									flag = 0;
									for (int n = 0; n < tmp.search.size(); n++)
									{
										if (find(clo.t[k].search.begin(), clo.t[k].search.end(), tmp.search[n]) == clo.t[k].search.end())//如果同心集的search中不含当前tmp的字符
										{
											clo.t[k].search.push_back(tmp.search[n]);//就将temp的search加入同心集中
										}
									}
								}
							}
							if (flag) {	//如果新加的项目不重复且不是同心集
								clo.t.push_back(tmp);  //就将项目加入clo
							} 	
						} 
					} 
				}
			}
		}
	}
}

void getSets(project_set& clo) {
	project temp;  //定义项目temp
	for (int i = 0; i < sets.size(); i++)//处理每个项目集
	{
		vector<string> used;//定义用过的输入符used
		for (int j = 0; j < sets[i].t.size(); j++)//当前项目集中每个可能的输入符
		{
			string first;
			first = grammars[sets[i].t[j].num].right[sets[i].t[j].position]; //圆点后的字符
			if (sets[i].t[j].position < grammars[sets[i].t[j].num].right.size() && find(used.begin(), used.end(), first) == used.end())	//如果圆点不在最后且该输入符没有使用过				
			{
				used.push_back(first);  //将该输入符加入used
				for (int k = 0; k < sets[i].t.size(); k++)//用该输入符遍历当前项目集
				{
					string cmp;
					cmp = grammars[sets[i].t[k].num].right[sets[i].t[k].position];//圆点后符号
					if (first == cmp) //如果圆点后的符号和该输入符相同
					{
						temp.num = sets[i].t[k].num;
						temp.position = sets[i].t[k].position + 1;
						temp.search = sets[i].t[k].search;
						clo.t.push_back(temp);//把当前字符产生的核加入clo
					}
				}

				getClosure(clo);

				if (clo.t.size() != 0)
				{
					string key, value;
					value = "S";
					key = sets[i].name + first;
					vector<project_set>::iterator it = find(sets.begin(), sets.end(), clo);
					if (it == sets.end())//如果clo在sets中是不重复的
					{
						clo.name = clo.name[0] + 1;
						value += clo.name;
						sets.push_back(clo);
					}
					else
						value += it->name;
					clo.t.clear();
					m.insert(pair<string, string>(key, value));
				}
			}
		}
		used.clear();
	}
	for (vector<project_set>::iterator it = sets.begin(); it != sets.end(); it++)//填入归约的位置
	{
		cout << "I" << it->name[0] - '0' << endl;
		for (int i = 0; i < it->t.size(); i++)//项目集中每个项目t[i]
		{
			cout << grammars[it->t[i].num].left << "->" << grammars[it->t[i].num].right << "  " << "[·的位置" << grammars[it->t[i].num].right[it->t[i].position] << "]";
			cout << "  ,";
			for (int j = 0; j < it->t[i].search.size(); j++)
				cout << it->t[i].search[j] << " ";//输出向前搜索符search
			cout << endl;
			char t;
			t = grammars[it->t[i].num].right[it->t[i].position];//项目集中每个项目中圆点后的字符
			if (!t)//为空时
			{
				string key, value;
				value = "r";
				if (grammars[it->t[i].num].right[it->t[i].position - 1] == 'S')
					//规定S为语法文法的开始符号,归约到S时结束 
				{
					value = "acc";
					key = it->name;
					key += "#";
					m.insert(pair<string, string>(key, value));
				}
				else
				{
					value += to_string(it->t[i].num);
					key = it->name;
					for (int j = 0; j < it->t[i].search.size(); j++)
						m.insert(pair<string, string>(key + it->t[i].search[j], value));
				}
			}
		}
	}
}

void printAnalyzeTable() {
	cout << "-------------------------------------------------------------------------------------";
	cout << "\nLR(1)分析表:\n\n";
	cout << "  ";
	for (int i = 0; i < Vt.size(); i++)
		cout << setw(8) << Vt[i];
	for (int i = 0; i < Vn.size(); i++)
		cout << setw(8) << Vn[i];
	cout << endl;
	for (int i = 0; i < sets.size(); i++)
	{
		if (sets[i].name[0] - '0' < 10) {
			cout << " " << sets[i].name[0] - '0';
		}
		else {
			cout << sets[i].name[0] - '0';
		}
		cout << "  ";

		for (int j = 0; j < Vt.size(); j++)
		{
			string value = "   ";
			string key;
			key += sets[i].name[0];
			key += Vt[j];
			map<string, string> ::iterator it = m.find(key);
			if (it != m.end())
				value = m[key];
			if (value.size() == 2) {
				if (value[1] - '0' < 10)
					cout << setw(6) << value[0] << value[1] - '0' << " ";
				else {
					cout << setw(6) << value[0] << value[1] - '0';
				}
			}
			else
				cout << setw(8) << value;
		}
		for (int j = 0; j < Vn.size(); j++)
		{
			string value = "   ";
			string key;
			key += sets[i].name[0];
			key += Vn[j];
			map<string, string> ::iterator it = m.find(key);
			if (it != m.end())
				value = m[key];
			if (value.size() == 2) {
				if (value[1] - '0' < 10)
					cout << setw(6) << value[1] - '0' << "  ";
				else {
					cout << setw(6) << value[1] - '0' << "  ";
				}
			}
			else
				cout << setw(8) << value;
		}
		cout << endl;
	}
}

void read_Input(vector <string> &in) {
	char buff[30];
	fstream file;
	file.open(grammar_input_FILE_PATH, ios::in);
	if (!file.is_open())
	{
		cout << "Can not find the file--'语法输入.txt',please check!";
	}
	else
	{
		while (file >> buff)
			in.push_back(buff);
	}
	file.close();
	in.push_back("#");

	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "输入的语句为:\n";
	cout << "#";
	for (int i = 0; i < in.size(); i++)
		cout << in[i];
	cout << endl;
	cout << "-----------------------------------------------------------------------------" << endl;
}

int main(){	
	
	//1.读入2°文法
	read_Grammar();
		
	//2.拓广文法并得到初态项目集合
	project_set clo;
	clo.name = "0";
	project temp = { 0, 0, {'#'} };  //定义和初始化项目temp
	clo.t.push_back(temp);
	getClosure(clo);
	sets.push_back(clo);//得到初态项目集

	clo.t.clear();
	
	//3.构造项目集规范族和分析表
	getSets(clo);	//处理生成所有项目集

	//4.打印分析表
	printAnalyzeTable();
	
	vector <string> states;   //状态栈
	vector <string> symbols;  //符号栈
	vector <string> in;		  //输入符
	states.push_back("0");
	symbols.push_back("#");
	
	//5.读入输入语句
	read_Input(in);
	

	//6.分析语句并判断是否符合
	cout << "分析过程为:\n\n";
	cout << setw(8) << "步骤" << setw(20) << "状态栈" << setw(20) << "符号栈" << setw(20) << "输入符" << setw(8) << "动作";
	cout << endl;

	int count = 1;//步骤数
	for (int i = 0; i < in.size(); i++)
	{
		cout << setw(8) << count++;
		string t1;
		for (int j = 0; j < states.size(); j++)
		{
			t1 += to_string(states[j][0] - '0');
			t1 += " ";
		}
		cout << setw(20) << t1;
		t1 = "";
		for (int j = 0; j < symbols.size(); j++)
		{
			t1 += symbols[j];

		}
		cout << setw(20) << t1;
		t1 = "";
		for (int j = i; j < in.size(); j++)
		{
			t1 += in[j];
		}
		cout << setw(20) << t1;
		string key;   //表中对应的行和列
		string action;//获取表中动作
		char ch;	  //识别进行移进还是归约
		key += states.back();
		key += in[i];
		map<string, string> ::iterator it = m.find(key);
		if (it == m.end())    //表中为空 -> 句子错误
		{
			cout << "\n ERROR:当前状态下错误的输入导致出错！" << endl;
			system("pause");
			return 0;
		}
		action = it->second;
		if (action.size() == 2)
			cout << setw(6) << action[0] << action[1] - '0' << endl;
		else
			cout << setw(8) << action << endl;
		if (action == "acc")    //动作为acc -> 结束
		{
			cout << "语法识别成功\n";
			system("pause");
			return 0;
		}
		ch = action[0];
		if (ch == 'S')   //移进
		{
			char n = action[1];
			string state, symbol;//移入的状态 符号
			state = n;
			symbol = in[i];
			states.push_back(state);
			symbols.push_back(symbol);
		}
		else    //归约
		{
			i--;
			char n = action[1];
			int num = n - '0';
			for (int i = 0; i < grammars[num].length; i++)//移出产生式右边的符号 和对应状态
			{
				symbols.pop_back();
				states.pop_back();
			}
			symbols.push_back(grammars[num].left);//移入左部  并goto
			string key;
			string state;
			key += states.back();
			key += grammars[num].left;
			map<string, string> ::iterator it = m.find(key);
			if (it == m.end())    //表中为空  句子错误
			{
				cout << "\n ERROR:执行归约后无法找到下一状态！" << endl;
				system("pause");
				return 0;
			}
			action = it->second;
			state = action[1];
			states.push_back(state);
		}
	}
	system("pause");
	return 0;
}