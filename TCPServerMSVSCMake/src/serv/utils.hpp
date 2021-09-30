#pragma once
#include <iostream>

class CString
{
protected:
	char* cstring;
	int str_len;
	
public:
	CString();
	CString(int n);
	CString(char s);
	CString(const char* s);
	CString(const CString& so);

	virtual ~CString();

	operator const char* ()
	{
		return c_str();
	}

	void SetLength(int length);
	int GetLength() const
	{
		return str_len;
	}
	int length() const
	{
		return str_len;
	}
	
	CString& operator=(const CString&);
	CString& operator=(const char* s);

	void LoadFromStream(FILE* stream);
	void SaveToStream(FILE* stream);

	void LoadFromStreamForClassString(FILE* stream);

	bool operator==(const CString&);
	bool operator==(const char* s);

	bool operator!=(const CString&);
	bool operator!=(const char* s);

	bool operator<(const CString&);
	bool operator>(const CString&);

	bool IsEmpty() const { return (strlen == 0); }
	char& operator[](int n) const;

	CString operator+(const CString& so);
	CString operator+(const char s);
	
	CString operator+=(const CString& so);
	
	CString tuUpper();
	CString toLower();

	int Find(char c, int pos = 0);

	int Find(const char* sub, int pos = 0) const;
	int Find(const CString& so, int pos = 0) const;

	const char* c_str();

	CString substr(int pos, int plen) const;

	void ToSize(int size);
	unsigned int TimeOfReading();

	void IntToString(int value);
	int StringToInt();

	double stringToDouble();
	void StringToHexCode();
	void Dump(int len, unsigned char* data, int per_line);

	void Format(const char* type, int data);
	void Format(const char* type, float data);
	void Format(const char* type, double data);
	void Format(const char* type, char data);
	void Format(const char* type, unsigned int data);

	CString Left(int len) const;
	CString Right(int len) const;
	CString Mid(int pos, int count) const;

	CString GetPathOnly();
	CString GetFileNameOnly();

	void Trim();
	int GetParamCount();

	CString GetParam(int index);

	bool Divide(const CString& divider, CString& str1, CString& str2);
	CString GetExtension();

	bool DeleteNamespace();
	void SaveText(FILE* file);

	static CString GetReturn();

	static void SetReturn(const char* s);

	int Delete(int nIndex, int nCount = 1);

	void DeleteLastSlash();
	bool IsVariableName();

};


