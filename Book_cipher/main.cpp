#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <map>
#include <sstream>

using namespace std;

void Encrypt(string inpath, vector<string> vec)  
{
	int n = inpath.length();
    char char_array[n + 1];
    strcpy(char_array, inpath.c_str());
	ifstream in(char_array);
	ofstream out("Encrypted.txt");
    string line;
    
    string crypted;
    
    while (getline(in, line))
    {
        for(int i = 0; i < line.length(); i++)
		{
			char s = line[i];

			for(int j = 0;  j < vec.size(); j++)
				for(int k = 0; k < vec[j].size(); k++)
				{
					if(vec[j][k] ==  s)
					{
						out << j <<"." << k;
						goto end;
					}	
				}
			out <<"_";	
			end:	
			out <<" ";		
		}
		out <<endl;	
    }
}

void Decrypt(string inpath, vector<string> vec)
{
	int n = inpath.length();
    char char_array[n + 1];
    strcpy(char_array, inpath.c_str());
	ifstream in(char_array);
	ofstream out("Decrypted_new.txt");
    string l;   
    string subline;
    stringstream line;

	while (getline(in, l))
	{
		line.clear();
		line << l;
		while (getline(line, subline, ' '))
    	{
    		if(subline.find('_') != std::string::npos)
    		{
	    		out << "_";
	    		continue;
			}
	    	int dot = subline.find('.');
	        string row = subline.substr(0, dot);
	        string col = subline.substr(dot+1);
	
	        int r = atoi(row.c_str());
	        int c = atoi(col.c_str());
	        
	        out << vec[r][c];
	    }
	    out << endl;
	}
    
}

void TextAnalysis(string inpath)
{
	int n = inpath.length();
    char char_array[n + 1];
    strcpy(char_array, inpath.c_str());
	ifstream in(char_array);
	ofstream out("Analysis.txt");
    string line;
    
    map<char, int> letters;

	int all_letters = 0;
    
    while (getline(in, line))
    {
        for(int i = 0; i < line.length(); i++)
		{
			char s = line[i];
			all_letters++;			
			letters[s]++;	
		}	
    }
    
    map<char, int> :: iterator it = letters.begin();

    for (it = letters.begin(); it != letters.end(); it++) 
        out << it->first << " - " << (double)it->second/all_letters << endl;
    
    int max = 0;
    for (it = letters.begin(); it != letters.end(); it++) 
	{ 
        if (it->second > max) 
			max = it->second;   
    }
 
    for (int y = max; y > 0; y--) 
	{
        for (it = letters.begin(); it != letters.end(); it++) 
		{
            if (it->second >= y) 
				cout << '#';
            else cout << ' ';
        }
        cout << endl;
    }
    
    for (it = letters.begin(); it != letters.end(); it++) 
        cout << it->first;
}

void CipherAnalysis(string inpath)
{
	int n = inpath.length();
    char char_array[n + 1];
    strcpy(char_array, inpath.c_str());
	
	ifstream in(char_array);
	ofstream out("Cipher_Analysis.txt");
    
    map<pair<int, int>, int> codes;
    int all_letters = 0;	
    string l;   
    string subline;
    stringstream line;

	while (getline(in, l))
	{
		line.clear();
		line << l;
		while (getline(line, subline, ' '))
    	{
    		if(subline.find('_') != std::string::npos)
    		{
	    		out << "_";
	    		continue;
			}
	    	int dot = subline.find('.');
	        string row = subline.substr(0, dot);
	        string col = subline.substr(dot+1);
	
	        int r = atoi(row.c_str());
	        int c = atoi(col.c_str());
	        all_letters++;	
	        pair<int, int> indexes (r, c);
  			codes[indexes]++; 
	    }
	}
    
    map<pair<int, int>, int> :: iterator it = codes.begin();

    for (it = codes.begin(); it != codes.end(); it++) 
        out << it->first.first << "." << it->first.second << " - " << (double)it->second/all_letters << endl;     
    
    int max = 0;
    for (it = codes.begin(); it != codes.end(); it++) 
	{ 
        if (it->second > max) 
			max = it->second;   
    }
 
	/* 
    for (int y = max; y > 0; y--) 
	{
        for (it = codes.begin(); it != codes.end(); it++) 
		{
            if (it->second >= y) 
				cout << '#';
            else cout << ' ';
        }
        cout << endl;
    }
    */
    
    for (it = codes.begin(); it != codes.end(); it++) 
	{
		cout << it->first.first << "." << it->first.second << " - ";
        for(int i = 0; i < it->second; i++) 
			cout << '#';
				
		cout << endl;	
    }
}

int main() 
{   
    ifstream book("Key-book.txt");
    string line;
    vector<string> vec;
    
    if (book.is_open())
    {
        while (getline(book, line))
        {
        	vec.push_back(line);
        }
    }
    book.close();
    
    int choice;
    GetChoice:
    cout  << "Choose operation:\n (1) Encrypt file \n (2) Decrypt file \n (3) Original FA \n (4) Cipher FA \n";
	cin >> choice;
	cout << "\n";
		
	switch(choice)
	{
		case 1: 
			Encrypt("Decrypted.txt", vec);	
			break;
			 	
		case 2:
			Decrypt("Encrypted.txt", vec);
			break;
		
		case 3:
			TextAnalysis("Decrypted.txt");
			break;
			
		case 4:
			CipherAnalysis("Encrypted.txt");
			break;	
		
		default:
			cout << "I can't understand you. Try again\n";
			goto GetChoice;
	}
	return 0;
}
