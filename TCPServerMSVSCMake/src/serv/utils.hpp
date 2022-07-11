#pragma once
#include "includes.hpp"
#include "utf.hpp"

#define STREAM_MEMORY_START_SIZE 4096

struct	CPoint;
struct	CSize;
class	CStream;

enum STREAM_MODE{STREAM_READ, STREAM_WRITE, STREAM_ADD, STREAM_MAX, STREAM_ERROR = -2};

static char* strDup(const char* str);

struct MEM_DATA
{
	unsigned char* data;
	int length;

	bool operator==(const MEM_DATA& d)
	{
		if (length == d.length)
		{
			for (int i = 0; i < length; i++)
				if (data[i] != d.data[i])
					return false;
			return true;
		}
		return false;
	}
};

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

class CAddressString : public CString {
public:
	// IPv4 input:
	CAddressString(struct sockaddr_in const& addr);
	CAddressString(struct in_addr const& addr);
	//AddressString(ipv4AddressBits const& addr); // "addr" is assumed to be in network byte order

	// IPv6 input:
	CAddressString(struct sockaddr_in6 const& addr);
	CAddressString(struct in6_addr const& addr);
	//AddressString(ipv6AddressBits const& addr);

	// IPv4 or IPv6 input:
	CAddressString(struct sockaddr_storage const& addr);

	virtual ~CAddressString();

	char const* val() const { return fVal; }

private:
	//void init(ipv4AddressBits const& addr); // used to implement the IPv4 constructors
	//void init(ipv6AddressBits const& addr); // used to implement the IPv6 constructors

private:
	char* fVal; // The result ASCII string: allocated by the constructor; deleted by the destructor
};

class CStringArray
{
protected:
	int k_str;
	int max_str;
	CString** m_str;

public:
	CStringArray();
	virtual ~CStringArray();

	void Clear();

	void Serialize(CStream& ar);

	int Add(CString* str);

	CString* Get(int index);

	int GetCount() { return k_str; }

	// ����� ������ �� ��������� �������� ������ �� �������� � ����������
	void FromString(const char* str);
	void IncrementStr() { k_str++; }
};

class CStringData : public CString
{
protected:
	MEM_DATA data;
	int index;

public:
	CStringData();
	virtual ~CStringData();

	MEM_DATA* GetData() { return &data; }
	void SetData(int length, unsigned char* data);

	void SetIndex(int index) { this->index = index; }
	int GetIndex() { return index; }

	void Serialize(CStream& stream);
};

class CStringDataArray
{
protected:
	int k_str;
	int max_str;
	CStringData** m_str;

public:
	CStringDataArray();
	virtual ~CStringDataArray();

	void Clear();

	void Serialize(CStream& ar);

	int Add(CString* str, MEM_DATA* data = NULL);
	int Delete(int index);

	CStringData* Get(int index);

	int GetCount() { return k_str; }
};

struct __STRING_TABLE_3
{
	int k_data;
	CStringDataArray** m_data;

	__STRING_TABLE_3();
	~__STRING_TABLE_3();

	void Clear();

	void SetLength(int len);
};

struct __STRING_TABLE_2
{
	int k_data;
	__STRING_TABLE_3** m_data;

	__STRING_TABLE_2();
	~__STRING_TABLE_2();

	void Clear();

	void SetLength(int len);
};

struct __STRING_TABLE_1
{
	int k_data;
	__STRING_TABLE_2** m_data;

	__STRING_TABLE_1();
	~__STRING_TABLE_1();

	void Clear();

	void SetLength(int len);
};

class CStringTable
{
	__STRING_TABLE_1 table;

	int max_ptr;
	int k_ptr;
	CStringData** m_ptr;

public:
	CStringTable();
	virtual ~CStringTable();

	void Clear();

	CStringData* Find(CString& name);

	bool Add(CString& name, MEM_DATA* data = NULL);

	bool Delete(CString& name);

	void Serialize(CStream& stream, bool count_yes = true);

	int GetCount() { return k_ptr; }
	CStringData* Get(int index) { return m_ptr[index]; }


protected:
	bool GetSymbols(CString& name, int& first, int& middle, int& last);
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

class CStreamMemory : public CStream
{
protected:
	char* buffer;
	unsigned int position;
	unsigned int length;
	unsigned int max_length;
	bool is_created_buffer;

public:
	CStreamMemory();
	virtual ~CStreamMemory();

	bool Open(const char* buffer, unsigned int length, int mode);

	virtual void Close();

	virtual unsigned int GetLength();

	virtual unsigned int GetPosition();
	virtual void SetPosition(unsigned int pos);

	virtual unsigned int Write(void* m_data, unsigned int k_data);
	virtual unsigned int Read(void* m_data, unsigned int k_data);

	virtual void ChangeMode(int mode);

	char* GetBuffer() { return buffer; }
};

class CMemReader : public CStreamMemory
{
public:
	CMemReader() : CStreamMemory()
	{
		Open((const char*)0x1, 1, STREAM_READ);
	}

	void Start(void* ptr)
	{
		buffer = (char*)ptr;
		length = 10000000;
		max_length = 10000000;
		position = 0;
	}

	void Finish(MEM_DATA& buf)
	{
		buf.data = (unsigned char*)GetBuffer();
		buf.length = GetPosition();
	}
};

class CMemWriter : public CStreamMemory
{
public:
	CMemWriter() : CStreamMemory()
	{
		Open(NULL, 0, STREAM_WRITE);
	}

	void Start()
	{
		position = 0;
	}

	void Finish(MEM_DATA& buf)
	{
		buf.data = (unsigned char*)GetBuffer();
		buf.length = GetPosition();
	}

	void SetPositionIndirect(unsigned int position)
	{
		this->position = position;
	}

	void SetLengthIndirect(unsigned int length)
	{
		this->length = length;
	}
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