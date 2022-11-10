#include "stdafx.h"

#define KEYWORD_FILE_PATH "./lex_txt/KEYWORD.txt"
#define GRAMMAR_FILE_PATH "./lex_txt/GRAMMAR.txt"
#define SOURCES_FILE_PATH "./lex_txt/SOURCES.txt"
#define OUTPUT_FILE_PATH "./lex_txt/OUTPUT.txt"

char str_file[1000];  //用于保存读取文件的内容

string ch = "";       //用于保存输出LOG的内容

vector<string> keyword;  //用于保存KEYWORD.txt中的keyword的内容
vector<string> inchar;	//用于保存INCHAR.txt中的可输入字符
vector<string> type;	//用于存放对应单词的类别
vector<string> token;	//用于存放解析源文件时分离出的单词
vector<string> _type = { "O","I","A", "C", "B", "G", "L","K" };
vector<string> to_type = { "运算符", "标识符", "整数常数", "小数常数", "小数常数", "复数常数", "限制符", "关键字" };
int line = 0;	//正规文法行数

struct NFA_sturct     //定义NFA三元组 "当前状态|接收字符|下一个状态"
{
	string now;
	string input;
	string next;
};

struct DFA            //定义DFA————开始状态、结束状态
{
	string startState;
	vector<string> endState;
	map<string, string> f;
};

struct NFA            //定义NFA————开始状态、结束状态
{
	string startState;
	vector<string> endState;
	NFA_sturct f[200];
};

DFA dfa = { "S" };
NFA nfa = { "S" };

struct closure        //定义闭包closure
{
	string name;
	vector<string> t;
};

string keywordState = "K";   //定义关键字的符号
string symbolState = "I";    //定义标识符的符号

bool compare(vector<string> v1, vector<string> v2)
{
	for (int i = 0; i < v2.size(); i++)
	{
		if (find(v1.begin(), v1.end(), v2[i]) == v1.end())
			return false;
	}
	return true;
}

bool operator == (const closure& c1, const closure& c2)
{
	if (compare(c1.t, c2.t) && compare(c2.t, c1.t))
		return true;
	else
		return false;
}

//用于字符串分割
vector<string> split(const string& s, const string& seperator)	
{
	vector<string> result;
	int index_1 = 0;

	while (index_1 < s.size()) {
		//查找字符串中首个不为分隔符的字符 index_1为该字符在s中的下标
		bool flag = false;
		while (index_1 < s.size() && !flag) {
			flag = true;
			for (int i = 0; i < seperator.size(); ++i) {
				if (s[index_1] == seperator[i]) {
					++index_1;
					flag = false;
					break;
				}
			}
		}

		flag = false;

		//查找下一个分割符，以确定所需内容的范围
		int index_2 = index_1;
		while (index_2 < s.size() && !flag) {
			for (int i = 0; i < seperator.size(); ++i) {
				if (s[index_2] == seperator[i]) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				++index_2;
			}
		}
		
		//将识别出的字符串存入
		if (index_1 != index_2) {	//若index_1 = index_2,则表示s中全部都是分隔符，此时index = s.size()
			result.push_back(s.substr(index_1, index_2 - index_1 ));
			index_1 = index_2;
		}
	}
	return result;
}

//读取KEYWORD文件内容
void readKeywords()
{
	fstream file;
	file.open(KEYWORD_FILE_PATH, ios::in);
	if (!file.is_open()) {
		cout << "Find_File_ERROR!";
	}
	else {
		//不要跳过间隔符
		file.unsetf(ios::skipws);
		//按行读取
		while (file.getline(str_file, 1000))
		{
			keyword.push_back(str_file);
		}
		//用于将每行数据以" "分隔开
		for (int i = 0; i < keyword.size(); i++)
		{
			//以" "相分隔
			vector<string> v = split(keyword[i], " ");
			keyword[i] = v[2];
		}
		file.close();
	}
}

//读取INCHAR文件内容
void readInChar() {
	fstream file;
	file.open("./lex_txt/INCHAR.txt", ios::in);
	if (!file.is_open())
	{
		cout << "Find_File_ERROR!";
	}
	else
	{
		while (file >> str_file)
			inchar.push_back(str_file);
	}
	file.close();
}

//读正规文法入NFA
void readGrammar() {
	fstream file;
	
	file.open(GRAMMAR_FILE_PATH, ios::in);
	if (!file.is_open())
	{
		cout << "Find_File_ERROR!";
	}
	else
	{
		while (file.getline(str_file, 10))//读取文件中每一行的文法,eg: S->=O ，并将它们输入到NFA中 
		{
			char* s = str_file;
			nfa.f[line].now = *s++;
			
			s++;
			s++;
			if (*s == '$')	//说明now状态已是终态
			{
				nfa.endState.push_back(nfa.f[line].now);
				dfa.endState.push_back(nfa.f[line].now);
			}
			else
			{
				nfa.f[line].input = *s++;
				nfa.f[line].next = *s;
			}
			line++;
		}
	}
	file.close();
}

//读取源文件
void readSource() {
	fstream file;
	file.open(SOURCES_FILE_PATH, ios::in);
	if (!file.is_open())
	{
		cout << "Find_File_ERROR!";
	}
	else
	{
		cout << "\n---------------------------------------\n";
		cout << "           读取到的SOURCES内容           \n";
		while (file >> str_file)	//按照分隔符,逐个读入到str_file,逐个处理 
		{
			string state = dfa.startState;
			char* s = str_file;
			cout << std::left << setw(10) << str_file << endl;
			string tmp;			//存储token 
			while (*s)
			{
				char input = *s;
				string key;   		//构造dfa转换f的key 
				key += state;
				key += input;
				map<string, string>::iterator it;
				it = dfa.f.find(key);    //查找能否转换到下一状态 
				if (it != dfa.f.end())   //可以则转换状态,将输入加入tmp 
				{
					state = it->second;
					tmp += input;
				}
				else                   	 //不可以则判断状态是否为终态 
				{
					vector<string>::iterator it;
					it = find(dfa.endState.begin(), dfa.endState.end(), state);
					if (it == dfa.endState.end())     //不是终态则出错 
					{
						cout << "error";
						system("pause");
						return ;
					}
					else                //是终态则存入token和type 
					{
						if (state == symbolState)    //如果是标识符则判断是否为关键字 
						{
							vector<string>::iterator it = find(keyword.begin(), keyword.end(), tmp);
							if (it != keyword.end())
								state = keywordState;
						}
						token.push_back(tmp);
						type.push_back(state);
						tmp = "";
						state = dfa.startState;
						continue;
					}
				}
				s++;
				if (!*s)			//如果str_file遍历完了，此时state中存入的新状态应为终态 否则出错；
				{
					vector<string>::iterator it;
					it = find(dfa.endState.begin(), dfa.endState.end(), state);
					if (it == dfa.endState.end())     //不是终态则出错 
					{
						cout << "error";
						system("pause");
						return ;
					}
					else                //是终态则存入token和type 
					{
						if (state == symbolState)
						{
							vector<string>::iterator it = find(keyword.begin(), keyword.end(), tmp);
							if (it != keyword.end())
								state = keywordState;
						}
						token.push_back(tmp);
						type.push_back(state);
						tmp = "";
						state = dfa.startState;
					}
				}

			}
		}
		file.close();
	}
}

//NFA->DFA
void transNFAToDFA() {
	dfa.endState.assign(nfa.endState.begin(), nfa.endState.end());  //将nfa.endState赋给dfa.endState
	dfa.startState = nfa.startState;

	vector<closure> states;
	vector<int> flags;     //flags作为标志位，0即未被标记
	closure tmp;
	tmp.t.push_back(nfa.startState);
	tmp.name = nfa.startState;
	states.push_back(tmp);	//将初始的闭包推入集合
	flags.push_back(0);
	tmp.t.clear();
	tmp.name = "0";

	vector<int>::iterator it = find(flags.begin(), flags.end(), 0);
	while (it != flags.end()) {
		int pos = distance(flags.begin(), it);	//确认是第几个子集;
		flags[pos] = 1;	//该子集的标志位设为"1"

		for (int i = 0; i < inchar.size(); i++) {
			string ch = inchar[i];
			for (int j = 0; j < states[pos].t.size(); j++) {
				for (int k = 0; k < line; k++) {
					if (ch == nfa.f[k].input && states[pos].t[j] == nfa.f[k].now) {
						tmp.t.push_back(nfa.f[k].next);
					}
				}
			}
			if (tmp.t.size() != 0) {	//若此时的闭包不为空
				vector<closure>::iterator cit = find(states.begin(), states.end(), tmp);
				if (cit == states.end()) {	//若此时tmp不在states中
					tmp.name[0]++;
					string t = tmp.name;
					for (int i = 0; i < nfa.endState.size(); i++) {
						vector<string>::iterator _it = find(tmp.t.begin(), tmp.t.end(), nfa.endState[i]);
						if (_it != tmp.t.end()) {
							tmp.name = *_it;
						}
					}
					states.push_back(tmp);
					flags.push_back(0);
					string key = states[pos].name;
					key += ch;
					dfa.f.insert(pair<string, string>(key, tmp.name));	//key：当前状态名+输入字符 value：得到的下一个状态；
					tmp.name = t;
				}
				else {
					int p = distance(states.begin(), cit);
					string key = states[pos].name;
					key += ch;
					dfa.f.insert(pair<string, string>(key, states[p].name));
				}
				tmp.t.clear();
			}
		}
		it = find(flags.begin(), flags.end(), 0);
	}
}

//分析
void lexAnalysis() {
	for (int i = 0; i < token.size(); i++)
	{
		for (int j = 0; j < _type.size(); j++)
		{
			if (type[i].compare(_type[j]) == 0)
			{
				type[i].assign(to_type[j]);
				break;
			}
		}
	}
}

//输出函数
void output() {
	cout << "\n---------------------------------------\n";
	cout << "            分析出的词法结果             \n";
	for (int i = 0; i < token.size(); i++)
		cout << std::left << setw(12) << token[i] << "     -->     " << std::right << setw(14) << type[i] << endl;

	fstream file;
	file.open(OUTPUT_FILE_PATH, ios::out);

	for (int i = 0; i < token.size(); i++)
	{
		ch += token[i] + " ---> " + type[i] + "\n";
		file << ch;
		ch = "";
	}
	file.close();

	cout << "\n生成的词法结果已保存到'./OUTPUT.txt'\n\n";
	system("pause");
}

int main() {
	readKeywords();
	readInChar();
	readGrammar();
	transNFAToDFA();
	readSource();
	lexAnalysis();
	output();

	/*for (map<string, string>::iterator it = dfa.f.begin(); it != dfa.f.end(); ++it) {
		cout << it->first << it->second << endl;
	}*/

	/*for (int i = 0; i < keyword.size(); i++) {
		cout << i << " " << keyword[i] << endl;
	}*/

	/*vector<string> res = split("   int a = 123", " ");
	for (int i = 0; i < res.size(); i++) {
		cout <<i << " " << res[i] << "%" << endl;
	}*/
	return 0;
}

