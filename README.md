============================================
软件课程设计(II)代码说明文档
============================================
第一部分：词法分析器

LexicalAnalysis.vcxproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件，
    其中包含生成该文件的 Visual C++ 的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

LexicalAnalysis.vcxproj.filters相似扩
    这是使用“应用程序向导”生成的 VC++ 项目筛选器文件。
    它包含有关项目文件与筛选器之间的关联信息。在 IDE 中，通过这种关联，在特定节点下以分组形式显示具有展名的文件。例如，“.cpp”文件与“源文件”筛选器关联。

stdafx.h
    这是应用程序的主头文件,
    其中包括开发过程中使用的各类头文件。

LexicalAnalysis.cpp
    这是包含主应用程序源文件。
///////////////////////////////////////////////////////////////////////////////////
设计思路：根据用户输入的正规文法，生成NFA，再确定化生成DFA，
根据DFA编写识别token的程序，从头到尾从左至右识别用户输入的源代码，
生成token列表。
///////////////////////////////////////////////////////////////////////////////////
1.数据结构：
char str_file[1000];  //用于保存读取文件的内容
string ch = "";       //用于保存输出LOG的内容
vector<string> keyword;  //用于保存KEYWORD.txt中的keyword的内容
vector<string> inchar;	//用于保存INCHAR.txt中的可输入字符
vector<string> type;	//用于存放对应单词的类别
vector<string> token;	//用于存放解析源文件时分离出的单词
vector<string> _type = { "O","I","A", "C", "B", "G", "L","K" };
vector<string> to_type = { "运算符", "标识符", "整数常数", "小数常数", "小数常数", "复数常数", "限制符", "关键字" };
vector<int> tokenLine;
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

2.宏定义：
#define KEYWORD_FILE_PATH "./lex_txt/KEYWORD.txt"
#define GRAMMAR_FILE_PATH "./lex_txt/GRAMMAR.txt"
#define SOURCES_FILE_PATH "./lex_txt/SOURCES.txt"
#define OUTPUT_FILE_PATH "./lex_txt/OUTPUT.txt"
用于读取各文件路径。

3.函数：
bool compare(vector<string> v1, vector<string> v2)；比较两个string数组中的内容是否一致。
bool operator == (const closure& c1, const closure& c2)；操作符重载，用于closure的比较。
vector<string> split(const string& s, const string& seperator)；用于字符串分割。
void readKeywords()；读取KEYWORD文件内容。
void readInChar()；读取INCHAR文件内容。
void readGrammar()；读正规文法入NFA。
void readSource()；读取源文件内容。
void transNFAToDFA()；执行NFA转DFA的操作。
void matchType()；匹配token类型。
void output()；输出结果。
============================================
第二部分：语法分析器

SyntaxAnalysis.vcxproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件，
    其中包含生成该文件的 Visual C++ 的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

SyntaxAnalysis.vcxproj.filters相似扩
    这是使用“应用程序向导”生成的 VC++ 项目筛选器文件。
    它包含有关项目文件与筛选器之间的关联信息。在 IDE 中，通过这种关联，在特定节点下以分组形式显示具有展名的文件。例如，“.cpp”文件与“源文件”筛选器关联。

stdafx.h
    这是应用程序的主头文件,
    其中包括开发过程中使用的各类头文件。

SyntaxAnalysis.cpp
    这是包含主应用程序源文件。
///////////////////////////////////////////////////////////////////////////////////
设计思路：根据用户输入的2º型文法，生成Action及Goto表，
		设计合适的数据结构，判断token序列（用户输入的源程序转换）。
///////////////////////////////////////////////////////////////////////////////////
1.数据结构：
vector<string> Vn 语法非终结符。
vector<string> Vt 语法终结符。
struct grammar{ 
	string left;  //文法左部
	string right; //文法右部
	int num;      //文法序号
	int length;   //归约长度
}; 定义文法。
struct project{ 
	int num;      	//第几条文法
	int position;     //圆点位置
	vector<char>  search;  //向前搜索符(即First集)
}; 定义项目。
struct project_set {
	string name;    //项目集名称
	vector<project> t;  //定义项目集中的项目数组t
}; 定义项目集。
vector<grammar> grammars; 文法集合。
vector<project_set> sets; 项目集集合。
map<string, string> m;  key为当前状态+输入符号，value为执行的动作。
	vector <string> states 状态栈。
	vector <string> symbols 符号栈。
vector <string> in; 输入语句。

2.宏定义:
#define syntactic_grammar_FILE_PATH "./LR1_txt/GRAMMAR.txt"
#define grammar_input_FILE_PATH "./LR1_txt/INPUT.txt"

3.函数：
bool comp(vector<project> v1, vector<project> v2)；比较两项目集是否相同。
bool comp(vector<char> v1, vector<char> v2)；比较两字符串是否相同。
bool operator == (const project& p1, const project& p2)；重载操作符，判断两项目是否相同。
bool operator == (const project_set& c1, const project_set& c2)；重载操作符，判断两项目集是否相同。
vector<char> getFirst(vector<grammar> yufa, string s, vector<string> Vt)；求得First集合。
void read_Grammar()；读入文法。
void getClosure(project_set &clo)；求得当前项目集闭包。
void getSets(project_set& clo)；求得所有项目集和构造分析表。
void printAnalyzeTable()；打印分析表。
void read_Input(vector <string> &in)；读入输入串。
============================================

此文档与课程设计报告互补，
各函数具体实现思路以及调用逻辑见设计报告与源程序，
效果实例见设计报告展示以及exe文件实际运行结果。

Author：朱竑典
Date：2022.05.12