#include "stdafx.h"

#define KEYWORD_FILE_PATH "./lex_txt/KEYWORD.txt"
#define GRAMMAR_FILE_PATH "./lex_txt/GRAMMAR.txt"
#define SOURCES_FILE_PATH "./lex_txt/SOURCES.txt"
#define OUTPUT_FILE_PATH "./lex_txt/OUTPUT.txt"

char str_file[1000];  //���ڱ����ȡ�ļ�������

string ch = "";       //���ڱ������LOG������

vector<string> keyword;  //���ڱ���KEYWORD.txt�е�keyword������
vector<string> inchar;	//���ڱ���INCHAR.txt�еĿ������ַ�
vector<string> type;	//���ڴ�Ŷ�Ӧ���ʵ����
vector<string> token;	//���ڴ�Ž���Դ�ļ�ʱ������ĵ���
vector<string> _type = { "O","I","A", "C", "B", "G", "L","K" };
vector<string> to_type = { "�����", "��ʶ��", "��������", "С������", "С������", "��������", "���Ʒ�", "�ؼ���" };
int line = 0;	//�����ķ�����

struct NFA_sturct     //����NFA��Ԫ�� "��ǰ״̬|�����ַ�|��һ��״̬"
{
	string now;
	string input;
	string next;
};

struct DFA            //����DFA����������ʼ״̬������״̬
{
	string startState;
	vector<string> endState;
	map<string, string> f;
};

struct NFA            //����NFA����������ʼ״̬������״̬
{
	string startState;
	vector<string> endState;
	NFA_sturct f[200];
};

DFA dfa = { "S" };
NFA nfa = { "S" };

struct closure        //����հ�closure
{
	string name;
	vector<string> t;
};

string keywordState = "K";   //����ؼ��ֵķ���
string symbolState = "I";    //�����ʶ���ķ���

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

//�����ַ����ָ�
vector<string> split(const string& s, const string& seperator)	
{
	vector<string> result;
	int index_1 = 0;

	while (index_1 < s.size()) {
		//�����ַ������׸���Ϊ�ָ������ַ� index_1Ϊ���ַ���s�е��±�
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

		//������һ���ָ������ȷ���������ݵķ�Χ
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
		
		//��ʶ������ַ�������
		if (index_1 != index_2) {	//��index_1 = index_2,���ʾs��ȫ�����Ƿָ�������ʱindex = s.size()
			result.push_back(s.substr(index_1, index_2 - index_1 ));
			index_1 = index_2;
		}
	}
	return result;
}

//��ȡKEYWORD�ļ�����
void readKeywords()
{
	fstream file;
	file.open(KEYWORD_FILE_PATH, ios::in);
	if (!file.is_open()) {
		cout << "Find_File_ERROR!";
	}
	else {
		//��Ҫ���������
		file.unsetf(ios::skipws);
		//���ж�ȡ
		while (file.getline(str_file, 1000))
		{
			keyword.push_back(str_file);
		}
		//���ڽ�ÿ��������" "�ָ���
		for (int i = 0; i < keyword.size(); i++)
		{
			//��" "��ָ�
			vector<string> v = split(keyword[i], " ");
			keyword[i] = v[2];
		}
		file.close();
	}
}

//��ȡINCHAR�ļ�����
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

//�������ķ���NFA
void readGrammar() {
	fstream file;
	
	file.open(GRAMMAR_FILE_PATH, ios::in);
	if (!file.is_open())
	{
		cout << "Find_File_ERROR!";
	}
	else
	{
		while (file.getline(str_file, 10))//��ȡ�ļ���ÿһ�е��ķ�,eg: S->=O �������������뵽NFA�� 
		{
			char* s = str_file;
			nfa.f[line].now = *s++;
			
			s++;
			s++;
			if (*s == '$')	//˵��now״̬������̬
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

//��ȡԴ�ļ�
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
		cout << "           ��ȡ����SOURCES����           \n";
		while (file >> str_file)	//���շָ���,������뵽str_file,������� 
		{
			string state = dfa.startState;
			char* s = str_file;
			cout << std::left << setw(10) << str_file << endl;
			string tmp;			//�洢token 
			while (*s)
			{
				char input = *s;
				string key;   		//����dfaת��f��key 
				key += state;
				key += input;
				map<string, string>::iterator it;
				it = dfa.f.find(key);    //�����ܷ�ת������һ״̬ 
				if (it != dfa.f.end())   //������ת��״̬,���������tmp 
				{
					state = it->second;
					tmp += input;
				}
				else                   	 //���������ж�״̬�Ƿ�Ϊ��̬ 
				{
					vector<string>::iterator it;
					it = find(dfa.endState.begin(), dfa.endState.end(), state);
					if (it == dfa.endState.end())     //������̬����� 
					{
						cout << "error";
						system("pause");
						return ;
					}
					else                //����̬�����token��type 
					{
						if (state == symbolState)    //����Ǳ�ʶ�����ж��Ƿ�Ϊ�ؼ��� 
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
				if (!*s)			//���str_file�������ˣ���ʱstate�д������״̬ӦΪ��̬ �������
				{
					vector<string>::iterator it;
					it = find(dfa.endState.begin(), dfa.endState.end(), state);
					if (it == dfa.endState.end())     //������̬����� 
					{
						cout << "error";
						system("pause");
						return ;
					}
					else                //����̬�����token��type 
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
	dfa.endState.assign(nfa.endState.begin(), nfa.endState.end());  //��nfa.endState����dfa.endState
	dfa.startState = nfa.startState;

	vector<closure> states;
	vector<int> flags;     //flags��Ϊ��־λ��0��δ�����
	closure tmp;
	tmp.t.push_back(nfa.startState);
	tmp.name = nfa.startState;
	states.push_back(tmp);	//����ʼ�ıհ����뼯��
	flags.push_back(0);
	tmp.t.clear();
	tmp.name = "0";

	vector<int>::iterator it = find(flags.begin(), flags.end(), 0);
	while (it != flags.end()) {
		int pos = distance(flags.begin(), it);	//ȷ���ǵڼ����Ӽ�;
		flags[pos] = 1;	//���Ӽ��ı�־λ��Ϊ"1"

		for (int i = 0; i < inchar.size(); i++) {
			string ch = inchar[i];
			for (int j = 0; j < states[pos].t.size(); j++) {
				for (int k = 0; k < line; k++) {
					if (ch == nfa.f[k].input && states[pos].t[j] == nfa.f[k].now) {
						tmp.t.push_back(nfa.f[k].next);
					}
				}
			}
			if (tmp.t.size() != 0) {	//����ʱ�ıհ���Ϊ��
				vector<closure>::iterator cit = find(states.begin(), states.end(), tmp);
				if (cit == states.end()) {	//����ʱtmp����states��
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
					dfa.f.insert(pair<string, string>(key, tmp.name));	//key����ǰ״̬��+�����ַ� value���õ�����һ��״̬��
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

//����
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

//�������
void output() {
	cout << "\n---------------------------------------\n";
	cout << "            �������Ĵʷ����             \n";
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

	cout << "\n���ɵĴʷ�����ѱ��浽'./OUTPUT.txt'\n\n";
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

