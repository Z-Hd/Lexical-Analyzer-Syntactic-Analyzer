#include "stdafx.h"

#define syntactic_grammar_FILE_PATH "./LR1_txt/GRAMMAR.txt"
#define grammar_input_FILE_PATH "./LR1_txt/INPUT.txt"

vector<string> Vn = { "E", "S", "T", "F"};
vector<string> Vt = { "(", ")", "*", "+", "i", "#" };//Vn�д����﷨���ս��  Vt�д����﷨�ս��

struct grammar         //�����ķ�  
{				  //e.g. �ķ�(0)E->S -- left = E || right = S || num = 0 || length = 1 
	string left;  //�ķ���
	string right; //�ķ��Ҳ�
	int num;      //�ķ����
	int length;   //��Լ����
};

vector<grammar> grammars;

struct project            //������Ŀ
{					  //e.g. I0����E->��S,# || num = 0 || position = 0 || search = '#'
	int num;      	  //�ڼ����ķ�
	int position;     //Բ��λ��
	vector<char>  search;  //��ǰ������(��First��)
};

struct project_set      //������Ŀ��
{
	string name;    //��Ŀ������
	vector<project> t;  //������Ŀ���е���Ŀt
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

vector<char> getFirst(vector<grammar> yufa, string s, vector<string> Vt)    //�õ�First��
{
	vector<char> search;
	if (s[0]) //���s[0]��Ϊ��
	{
		vector<string> toEmpty;   //���Ƴ��յ��﷨��
		for (int i = 0; i < yufa.size(); i++)
		{
			if (yufa[i].right == "$")
				toEmpty.push_back(yufa[i].left);
		}

		string t;
		t = s[0];
		vector<string> ::iterator it = find(Vt.begin(), Vt.end(), t);   //�жϵ�һ���ǲ����ս��
		if (it != Vt.end())       	//������ս������������search
		{
			search.push_back(s[0]);
		}
		else						//��������ս�������Ƿ��ս��
		{
			it = find(toEmpty.begin(), toEmpty.end(), t);
			if (it == toEmpty.end())   //�����Ƴ���
			{
				for (int i = 0; i < yufa.size(); i++)
				{
					if (t == yufa[i].left) //���t���﷨�ķ����󲿷��ս��
					{
						vector<char> s1 = getFirst(yufa, yufa[i].right, Vt); //�ݹ�������е��﷨�ķ��󲿷��ս����first��
						for (int j = 0; j < s1.size(); j++)
						{
							if (find(search.begin(), search.end(), s1[j]) == search.end())  //���first����û��s1����s1����first��
								search.push_back(s1[j]);
						}
					}
				}
			}
			else   //���Ƴ���
			{
				for (int i = 0; i < yufa.size(); i++)
				{
					if (t == yufa[i].left)  //t���﷨�ķ��������
					{
						if (yufa[i].right[0] != '$')  //�Ҹ��﷨�ķ����Ҳ������ڿ�
						{
							vector<char> s1 = getFirst(yufa, yufa[i].right, Vt);//�ݹ�������е��﷨�ķ��󲿷��ս����first��
							for (int j = 0; j < s1.size(); j++)
							{
								if (find(search.begin(), search.end(), s1[j]) == search.end())//���first����û��s1����s1����first��
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
					if (find(search.begin(), search.end(), s1[j]) == search.end()) //���s1����search�������
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
		while (file.getline(buff, 30)) {//�����﷨���ķ�							 
										//e.g. �ķ�(0)E->S -- left = E || right = S || num = 0 || length = 1
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
	cout << "������ķ�Ϊ:\n";
	for (int i = 0; i < grammars.size(); i++)
		cout << i << "   " << grammars[i].left << "->" << grammars[i].right << endl;
	cout << "-----------------------------------------------------------------------------" << endl;
}

void getClosure(project_set &clo) {
	project tmp;//������Ŀtmp

	for (int i = 0; i < clo.t.size(); i++) {
		if (clo.t[i].position < grammars[clo.t[i].num].right.size()) {//Բ��λ�ò������
			string first;
			first = grammars[clo.t[i].num].right[clo.t[i].position]; //ȡ��Բ�������ַ�
			if (find(Vn.begin(), Vn.end(), first) != Vn.end()) {	//�����ַ�Ϊ���ս��
				string s;
				vector<char> search;
				for (int j = clo.t[i].position + 1; j < grammars[clo.t[i].num].right.length(); j++) {
					s += grammars[clo.t[i].num].right[j];
				}
				for (int j = 0; j < clo.t[i].search.size(); j++) {
					string t = s;	//t��Ϊ(Ba)
					t += clo.t[i].search[j];
					vector<char> search1 = getFirst(grammars, t, Vt);//�õ����е�ǰ��������
					for (int k = 0; k < search1.size(); k++)
					{
						if (find(search.begin(), search.end(), search1[k]) == search.end()) {
							search.push_back(search1[k]);  //�õ���ǰ������search����
						}
					}
				}
				tmp.search = search;  //�õ���Ŀtemp����ǰ������search����
				for (int j = 0; j < grammars.size(); j++) {
					if (grammars[clo.t[i].num].right[clo.t[i].position] == grammars[j].left[0]) {	//���Բ�����ַ������﷨��ߵ��ַ�
						tmp.num = grammars[j].num;
						tmp.position = 0;
						if (find(clo.t.begin(), clo.t.end(), tmp) == clo.t.end()) {	//����¼ӵ���Ŀ������Ŀ���У������ж��Ƿ���ķ��Ѿ����ڣ�����ǰ����������ͬ
							bool flag = true;
							for (int k = 0; k < clo.t.size(); k++)
							{
								if (clo.t[k].num == tmp.num && clo.t[k].position == tmp.position) //�����ͬ�ļ�
								{
									flag = 0;
									for (int n = 0; n < tmp.search.size(); n++)
									{
										if (find(clo.t[k].search.begin(), clo.t[k].search.end(), tmp.search[n]) == clo.t[k].search.end())//���ͬ�ļ���search�в�����ǰtmp���ַ�
										{
											clo.t[k].search.push_back(tmp.search[n]);//�ͽ�temp��search����ͬ�ļ���
										}
									}
								}
							}
							if (flag) {	//����¼ӵ���Ŀ���ظ��Ҳ���ͬ�ļ�
								clo.t.push_back(tmp);  //�ͽ���Ŀ����clo
							} 	
						} 
					} 
				}
			}
		}
	}
}

void getSets(project_set& clo) {
	project temp;  //������Ŀtemp
	for (int i = 0; i < sets.size(); i++)//����ÿ����Ŀ��
	{
		vector<string> used;//�����ù��������used
		for (int j = 0; j < sets[i].t.size(); j++)//��ǰ��Ŀ����ÿ�����ܵ������
		{
			string first;
			first = grammars[sets[i].t[j].num].right[sets[i].t[j].position]; //Բ�����ַ�
			if (sets[i].t[j].position < grammars[sets[i].t[j].num].right.size() && find(used.begin(), used.end(), first) == used.end())	//���Բ�㲻������Ҹ������û��ʹ�ù�				
			{
				used.push_back(first);  //�������������used
				for (int k = 0; k < sets[i].t.size(); k++)//�ø������������ǰ��Ŀ��
				{
					string cmp;
					cmp = grammars[sets[i].t[k].num].right[sets[i].t[k].position];//Բ������
					if (first == cmp) //���Բ���ķ��ź͸��������ͬ
					{
						temp.num = sets[i].t[k].num;
						temp.position = sets[i].t[k].position + 1;
						temp.search = sets[i].t[k].search;
						clo.t.push_back(temp);//�ѵ�ǰ�ַ������ĺ˼���clo
					}
				}

				getClosure(clo);

				if (clo.t.size() != 0)
				{
					string key, value;
					value = "S";
					key = sets[i].name + first;
					vector<project_set>::iterator it = find(sets.begin(), sets.end(), clo);
					if (it == sets.end())//���clo��sets���ǲ��ظ���
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
	for (vector<project_set>::iterator it = sets.begin(); it != sets.end(); it++)//�����Լ��λ��
	{
		cout << "I" << it->name[0] - '0' << endl;
		for (int i = 0; i < it->t.size(); i++)//��Ŀ����ÿ����Ŀt[i]
		{
			cout << grammars[it->t[i].num].left << "->" << grammars[it->t[i].num].right << "  " << "[����λ��" << grammars[it->t[i].num].right[it->t[i].position] << "]";
			cout << "  ,";
			for (int j = 0; j < it->t[i].search.size(); j++)
				cout << it->t[i].search[j] << " ";//�����ǰ������search
			cout << endl;
			char t;
			t = grammars[it->t[i].num].right[it->t[i].position];//��Ŀ����ÿ����Ŀ��Բ�����ַ�
			if (!t)//Ϊ��ʱ
			{
				string key, value;
				value = "r";
				if (grammars[it->t[i].num].right[it->t[i].position - 1] == 'S')
					//�涨SΪ�﷨�ķ��Ŀ�ʼ����,��Լ��Sʱ���� 
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
	cout << "\nLR(1)������:\n\n";
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
		cout << "Can not find the file--'�﷨����.txt',please check!";
	}
	else
	{
		while (file >> buff)
			in.push_back(buff);
	}
	file.close();
	in.push_back("#");

	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "��������Ϊ:\n";
	cout << "#";
	for (int i = 0; i < in.size(); i++)
		cout << in[i];
	cout << endl;
	cout << "-----------------------------------------------------------------------------" << endl;
}

int main(){	
	
	//1.����2���ķ�
	read_Grammar();
		
	//2.�ع��ķ����õ���̬��Ŀ����
	project_set clo;
	clo.name = "0";
	project temp = { 0, 0, {'#'} };  //����ͳ�ʼ����Ŀtemp
	clo.t.push_back(temp);
	getClosure(clo);
	sets.push_back(clo);//�õ���̬��Ŀ��

	clo.t.clear();
	
	//3.������Ŀ���淶��ͷ�����
	getSets(clo);	//��������������Ŀ��

	//4.��ӡ������
	printAnalyzeTable();
	
	vector <string> states;   //״̬ջ
	vector <string> symbols;  //����ջ
	vector <string> in;		  //�����
	states.push_back("0");
	symbols.push_back("#");
	
	//5.�����������
	read_Input(in);
	

	//6.������䲢�ж��Ƿ����
	cout << "��������Ϊ:\n\n";
	cout << setw(8) << "����" << setw(20) << "״̬ջ" << setw(20) << "����ջ" << setw(20) << "�����" << setw(8) << "����";
	cout << endl;

	int count = 1;//������
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
		string key;   //���ж�Ӧ���к���
		string action;//��ȡ���ж���
		char ch;	  //ʶ������ƽ����ǹ�Լ
		key += states.back();
		key += in[i];
		map<string, string> ::iterator it = m.find(key);
		if (it == m.end())    //����Ϊ�� -> ���Ӵ���
		{
			cout << "\n ERROR:��ǰ״̬�´�������뵼�³���" << endl;
			system("pause");
			return 0;
		}
		action = it->second;
		if (action.size() == 2)
			cout << setw(6) << action[0] << action[1] - '0' << endl;
		else
			cout << setw(8) << action << endl;
		if (action == "acc")    //����Ϊacc -> ����
		{
			cout << "�﷨ʶ��ɹ�\n";
			system("pause");
			return 0;
		}
		ch = action[0];
		if (ch == 'S')   //�ƽ�
		{
			char n = action[1];
			string state, symbol;//�����״̬ ����
			state = n;
			symbol = in[i];
			states.push_back(state);
			symbols.push_back(symbol);
		}
		else    //��Լ
		{
			i--;
			char n = action[1];
			int num = n - '0';
			for (int i = 0; i < grammars[num].length; i++)//�Ƴ�����ʽ�ұߵķ��� �Ͷ�Ӧ״̬
			{
				symbols.pop_back();
				states.pop_back();
			}
			symbols.push_back(grammars[num].left);//������  ��goto
			string key;
			string state;
			key += states.back();
			key += grammars[num].left;
			map<string, string> ::iterator it = m.find(key);
			if (it == m.end())    //����Ϊ��  ���Ӵ���
			{
				cout << "\n ERROR:ִ�й�Լ���޷��ҵ���һ״̬��" << endl;
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