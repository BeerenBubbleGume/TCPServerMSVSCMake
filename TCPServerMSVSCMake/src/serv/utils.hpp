#pragma once
#include "includes.hpp"
#include "utf.hpp"

struct	CPoint;
struct	CSize;
class	CStream;

enum STREAM_MODE{STREAM_READ, STREAM_WRITE, STREAM_ADD, STREAM_MAX, STREAM_ERROR = -2};

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

class CArrayBase
{
protected:
	int				max_existing;
	int				k_existing;
	int*			m_existing;

	int				max_deleted;
	int				k_deleted;
	int*			m_deleted;

	int				max_indexed;
	int*			m_indexed;

public:

	CArrayBase();
	virtual			~CArrayBase();

	void			Serialize(CStream& stream);
	void			IncreaseDeleted(int from, int to);
	void			ClearExistingAndDeleted();
	int				FromDeletedToExisting();
	int				FromDeletedToExisting(int deleted_index);
	void			FromExistingToDeleted(int index);
	void			AddToExisting(int index);
	void			AddToDeleted(int index);
	int				GetIndex(int index) { return m_existing[index]; }
	int				GetIndexCount()		{ return k_existing; }
};	

class CArrayPtr : public CArrayBase
{
protected:
	int				k_ptr;
	void**			m_ptr;

public:
	CArrayPtr();
	virtual			~CArrayPtr();

	void			Clear();
	void*			Get(int index);
	int				Add(void* data);

	bool			Delete(int index);
	int				FindIndex(void* ptr);

	virtual void	SaveToFile(FILE* file);
	virtual void	LoadFromFile(FILE* file);
};

class CStream
{
protected:

	int mode; // read/write/add

	unsigned int bytes;	

	CString name;

public:

	CStream();

	virtual ~CStream();

	virtual void Close() = 0;
	virtual unsigned int GetLength() = 0;	
	virtual unsigned int GetPosition() = 0;
	
	virtual void SetPosition(unsigned int pos) = 0;
	void Seek(unsigned int pos) { SetPosition(pos); }

	virtual void ChangeMode(int mode) = 0;

	bool IsStoring();
	bool IsLoading();

	const char* GetName() { return name.c_str(); }

	// ����� ������
	void SetMode(int mode);
	int GetMode() { return mode; }

	virtual unsigned int
		Write(void* m_data, unsigned int k_data) = 0;
	virtual unsigned int
		Read(void* m_data, unsigned int k_data) = 0;

	void operator<<(bool& value);
	void operator<<(char& value);
	void operator<<(unsigned char& value);
	void operator<<(CString& value);

	void operator<<(int& value);
	void operator<<(short& value);
	void operator<<(unsigned short& value);
	void operator<<(float& value);
	void operator<<(double& value);
	void operator<<(unsigned int& value);
	void operator<<(CSize& value);
	void operator<<(CPoint& value);


	void operator>>(bool& value);
	void operator>>(char& value);
	void operator>>(unsigned char& value);
	void operator>>(CString& value);

	void operator>>(int& value);
	void operator>>(short& value);
	void operator>>(unsigned short& value);
	void operator>>(float& value);
	void operator>>(double& value);
	void operator>>(unsigned int& value);
	void operator>>(CSize& value);
	void operator>>(CPoint& value);
};

class CStreamFile : public CStream
{
protected:
	FILE*			stream;

	unsigned int	bytes;
	FILE*			fopen_file(const char* name, const char* mode);
	const unsigned short* 
					utf8to16(const unsigned char* str_utf8);
public:
	virtual void	Close();
	bool			Open(const char* fileName, int mode);
	
	CStreamFile();
	virtual			~CStreamFile();
	

	virtual unsigned int 
					GetLength();	
	virtual unsigned int 
					GetPosition();

	virtual void	SetPosition(unsigned int pos);

	virtual unsigned int 
					Write(void* m_data, unsigned int k_data);
	virtual unsigned int 
					Read(void* m_data, unsigned int k_data);

	virtual void	ChangeMode(int mode);

};

struct CSize
{
	int cx, cy;

	CSize() { cx = cy = 0; }
	CSize(int x, int y) { cx = x; cy = y; }
};

struct CPoint
{
	int x, y;

	CPoint() { x = y = 0; }
	CPoint(int x, int y) { this->x = x; this->y = y; }
};






