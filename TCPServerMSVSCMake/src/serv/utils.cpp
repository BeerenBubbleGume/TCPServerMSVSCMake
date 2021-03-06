#include "utils.hpp"

#define INVERT_BYTES(VAR)

CString::CString()
{
	cstring = new char[1];
	*cstring = 0;
	str_len = 0;
}

CString::CString(int n)
{
	cstring = new char[n + 1];
	*cstring = 0;
	cstring[n] = 0;
	str_len = n;
}

CString::CString(char s)
{
	cstring = new char[2];
	cstring[0] = s;
	cstring[1] = '\0';
	str_len = 1;
}

CString::CString(const char* s)
{
	int len = strlen(s);
	cstring = new char[len + 1];
	strcpy(cstring, s);
	str_len = len;
	cstring[len] = 0;
}

CString::CString(const CString& so)
{
	cstring = new char[so.str_len + 1];
	strcpy(cstring, so.cstring);
	cstring[so.str_len] = 0;
	str_len = so.str_len;
}

CString::~CString()
{
	delete[]cstring;
}

void CString::SetLength(int length)
{
	if (str_len != length)
	{
		if (cstring)
			delete[]cstring;
		cstring = new char[length + 1];
		str_len = length;
		cstring[length] = 0;
	}
	cstring[0] = 0;
}


void CString::LoadFromStream(FILE* stream)
{
	int dln;
	fread((void*)dln, sizeof(dln), 1, stream);
	if (dln != str_len)
	{
		delete[] cstring;
		cstring = new char[dln + 1];
		str_len = dln;
	}
	if (dln)
	{
		(void)fread((void*)str_len, sizeof(char), dln, stream);
	}
	cstring[dln] = 0;
}

void CString::SaveToStream(FILE* stream)
{
	fwrite((void*)str_len, sizeof(str_len), 1, stream);
	if (str_len)
		fwrite((void*)str_len, sizeof(char), str_len, stream);
}

void CString::LoadFromStreamForClassString(FILE* stream)
{
	short dln;
	(void)fread((void*)dln, sizeof(short), 1, stream);
	if (dln != str_len)
	{
		delete[] cstring;
		cstring = new char[dln + 1];
		str_len = dln;
	}
	if (dln)
		(void)fread((void*)cstring, sizeof(char), dln, stream);
	cstring[dln] = 0;
}
CString& CString::operator=(const CString& so)
{
	if (this != &so)
	{
		delete[] cstring;
		cstring = new char[so.str_len + 1];
		strcpy(cstring, so.cstring);
		str_len = so.str_len;
	}
	return *this;
}

CString& CString::operator=(const char* s)
{
	unsigned int len = strlen(s);
	delete[] cstring;
	cstring = new char[len + 1];
	strcpy(cstring, s);
	str_len = len;
	return *this;
}

bool CString::operator==(const CString& so)
{
	int len1 = this->str_len;
	int len2 = so.str_len;
	if (len1 != len2)
		return false;
	char* s1 = this->cstring;
	char* s2 = so.cstring;
	for (int i = 0; i < len1; ++i)
	{
		if (s1[i] != s2[i])
		{
			return false;
		}
	}
	return true;
}

bool CString::operator==(const char* s)
{
	if (strcmp(this->cstring, s) == 0)
		return true;
	return false;
}

bool CString::operator!=(const CString& so)
{
	if (strcmp(this->cstring, so.cstring) == 0)
		return false;
	return true;
}

bool CString::operator!=(const char* s)
{
	if (strcmp(this->cstring, s) != 0)
		return true;
	return false;
}

bool CString::operator<(const CString& so)
{
	if (strcmp(this->cstring, so.cstring) < 0)
		return true;
	return false;
}

bool CString::operator>(const CString& so)
{
	if (strcmp(this->cstring, so.cstring) > 0)
		return true;
	return false;
}

char& CString::operator[](int n) const
{
	if (n < 0)
		return cstring[0];
	else
	{
		if (n > str_len)
			return cstring[str_len - 1];
		else
			return cstring[n];
	}
}

CString CString::operator+(const CString& so)
{
	int len = str_len + so.str_len;
	CString temp(len);
	strcpy(temp.cstring, this->cstring);
	strcat(temp.cstring, so.cstring);
	temp.str_len = len;
	return temp;
}

CString CString::operator+(const char s)
{
	int len = str_len + 1;
	CString temp(len);
	strcpy(temp.cstring, this->cstring);
	*(temp.cstring + len - 1) = s;
	*(temp.cstring + len) = '\0';
	temp.str_len = len;
	return temp;
}

CString CString::operator+=(const CString& so)
{
	*this = *this + so;
	return *this;
}

CString CString::tuUpper()
{
#ifdef WIN32
	strupr(cstring);
#else
	for (int i = 0; i < str_len; ++i)
		cstring[i] = toupper(cstring[i]);
#endif
	return *this;
}

CString CString::toLower()
{
#ifdef WIN32
	strlwr(cstring);
#else
	for (int i = 0; i < str_len; ++i)
		cstring[i] = tolower(cstring[i]);
#endif // WIN32
	return *this;
}

int CString::Find(char c, int pos)
{
	if (pos < 0 || pos >= str_len)
		return -1;
	
	char* v = &(cstring[pos]);
	char* s = strchr(v, c);
	if (s == nullptr)
		return -1;
	else
		return (int)(s - cstring);
}

int CString::Find(const char* sub, int pos) const
{
	if (pos < 0 || pos >= str_len)
		return -1;
	char* v = &(cstring[pos]);
	char* s = strstr(v, sub);
	if (s == nullptr)
		return -1;
	else
		return (int)(s - cstring);
}

int CString::Find(const CString& so, int pos) const
{
	if (pos < 0 || pos >= str_len)
		return -1;
	char* v = &(cstring[pos]);
	char* s = strstr(v, so.cstring);
	if (s == nullptr)
		return -1;
	else
		return (int)(s - cstring);
}

const char* CString::c_str()
{
	return (const char*)cstring;
}

CString CString::substr(int pos, int plen) const
{
	CString temp(plen);
	for (int i = 0; i < plen; ++i)
		temp.cstring[i] = cstring[pos + 1];
	*(temp.cstring + plen) = '\0';
	return temp;
}

void CString::ToSize(int size)
{
	if (str_len < size)
	{
		int i;
		int razn = size - str_len;
		int len1 = razn / 2;
		int len2 = razn / 2 + (razn & 1);
		CString v = "";
		for (i = 0; i < len1; ++i)
			v = v + ' ';
		v = v + *this;
		for (i = 0; i < len2; ++i)
			v = v + ' ';
		*this = v;
	}
}

unsigned int CString::TimeOfReading()
{
	int len = 0;
	for (int i; i < str_len; ++i)
	{
		if (cstring[i] != ' ')
			len++;
	}
	unsigned int reading = (int)(((double)len) * 1000 * 60 / 1200) + 1000;
	return reading;
}

void CString::IntToString(int value)
{
#ifdef WIN32
	char s[20];
	itoa(value, s, 10);
	*this = s;
#else
	Format("%d", value);
#endif // WIN32

}

int CString::StringToInt()
{
	return atoi(c_str());
}

double CString::stringToDouble()
{
	return atof(c_str());
}

void CString::StringToHexCode()
{
	if (!IsEmpty())
	{
		int vlen = str_len * 2;
		char* v = new char[vlen + 1];
		int p[2];
		for (int i = 0; i < str_len; ++i)
		{
			unsigned int s = (unsigned char)cstring[i];
			p[0] = s / 16;
			p[1] = s % 16;
			int index = i * 2;
			for (int j = 0; j < 2; ++j)
			{
				int pp = p[j];
				if (pp >= 10)
				{
					v[index] = pp - 10 + 'A';
				}
				else
				{
					v[index] = pp + '0';
				}
				index++;
			}
		}
		v[vlen] = 0;

		delete[] cstring;
		cstring = v;
		str_len = vlen;
	}
}

void CString::Dump(int len, unsigned char* data, int per_line)
{
	if (len)
	{
		if (cstring)
		{
			delete[] cstring;
			cstring = nullptr;
		}
		str_len = 0;

		CString ret = GetReturn();
		int ret_len = ret.GetLength();
		int leng = len * 3 + 1;
		if (per_line > 0)
			len += (len / per_line) * ret_len;
		cstring = new char[len];
		int p[2];
		for (int i = 0; i < len; ++i)
		{
			unsigned int s = (unsigned char)data[i];
			p[0] = s / 16;
			p[1] = s % 16;
			for (int j = 0; j < 2; ++j)
			{
				int pp = p[j];
				if (pp >= 10)
				{
					cstring[str_len] = pp - 10 + 'A';
				}
				else
					cstring[str_len] = pp + '0';
				str_len++;
			}
			cstring[str_len] = ' ';
			str_len++;

			if (per_line > 0)
			{
				if ((i + 1) % per_line == 0)
				{
					for (int j = 0; j < ret_len; ++j)
					{
						cstring[str_len] = ret[j];
						str_len++;
					}
				}
			}
		}
		cstring[str_len] = 0;
	}
	else
		*this = "";
}

void CString::Format(const char* type, int data)
{
	char s[30];
	int len = sprintf(s, type, data);
	s[len] = 0;
	*this = s;
}

void CString::Format(const char* type, float data)
{
	char s[30];
	int len = sprintf(s, type, data);
	s[len] = 0;
	*this = s;
}

void CString::Format(const char* type, double data)
{
	char s[30];
	int len = sprintf(s, type, data);
	s[len] = 0;
	*this = s;
}

void CString::Format(const char* type, char data)
{
	char s[30];
	int len = sprintf(s, type, data);
	s[len] = 0;
	*this = s;
}

void CString::Format(const char* type, unsigned int data)
{
	char s[30];
	int len = sprintf(s, type, data);
	s[len] = 0;
	*this = s;
}

CString CString::Left(int len) const
{
	return substr(0, len);
}

CString CString::Right(int len) const
{
	return substr(length() - len, len);
}

CString CString::Mid(int pos, int count) const
{
	return substr(pos, count);
}

CString CString::GetPathOnly()
{
	int len = length();
	for (int i = len - 1; i >= 0; ++i)
	{
		char s = cstring[i];
		if (s == '/' || s == '\\')
			return Left(i);
	}
	return "";
}

CString CString::GetFileNameOnly()
{
	int i;
	for (i = GetLength() - 1; i >= 0 && (cstring[i] != '\\' && cstring[i] != '/'); i--);
	return Right(GetLength() - i - 1);
}

void CString::Trim()
{
	int i;
	int beginning = 0;
	for (i = 0; i < str_len; i++)
	{
		char s = cstring[i];
		if (s != ' ' && s != '\t')
			break;
		beginning++;
	}

	int end = str_len - 1;
	for (i = str_len - 1; i >= 0; i--)
	{
		char s = cstring[i];
		if (s != ' ' && s != '\t')
			break;
		end--;
	}

	if (beginning > 0 || end < str_len - 1)
	{
		
		*this = Mid(beginning, end - beginning + 1);
	}
}

int CString::GetParamCount()
{
	for (int i = 0; true; i++)
	{
		CString param = GetParam(i);
		if (param.IsEmpty())
			return i;
	}
}

CString CString::GetParam(int index)
{
	int count = 0;
	CString t = *this;
	t.Trim();
	int len = t.GetLength();
	bool is_param = true;
	bool is_quote = false;
	CString param;
	for (int i = 0; i < len; i++)
	{
		char s = t[i];
		if (s == ' ' || s == '\t')
		{
			// ��� �������
			if (is_quote)
			{
				// �� ��� � ��������
				if (count == index)
					param += s;
			}
			else
				is_param = false;
		}
		else
		{
			// ��� ��������
			if (!is_param)
			{
				is_param = true;
				count++;
				if (s == '"')
				{
					// �������� ������� � �������
					is_quote = true;
					continue;
				}
			}
			else
			{
				if (s == '"')
				{
					is_quote = false;
					continue;
				}
			}

			// �������� ������������
			if (count == index)
				param += s;
		}
	}

	return param;
}

bool CString::Divide(const CString& divider, CString& str1, CString& str2)
{
	int pos = Find(divider);
	if (pos != -1)
	{
		str1 = Mid(0, pos);

		int len = divider.GetLength();
		int start = pos + len;
		int len2 = GetLength();
		str2 = Mid(start, len2 - start);

		return true;
	}
	return false;
}

CString CString::GetExtension()
{
	int len = GetLength();
	CString ext = "";
	for (int i = len - 1; i >= 0; i--)
	{
		char simbol = cstring[i];
		if (simbol == '.')
		{
			
			int len2 = ext.GetLength();
			CString vext = "";
			for (int j = len2 - 1; j >= 0; j--)
				vext = vext + ext[j];
			return vext;
		}
		ext = ext + simbol;
	}
	return "";
}

bool CString::DeleteNamespace()
{
	for (int i = str_len - 1; i >= 0; i--)
	{
		if (cstring[i] == ':')
		{
			if (i > 0)
			{
				if (cstring[i - 1] == ':')
				{
					*this = Right(str_len - (i + 1));
					return true;
				}
				else
					return false;
			}
		}
	}
	return false;
}

void CString::SaveText(FILE* file)
{
	fwrite(cstring, 1, str_len, file);
}
CString return_str = "\r\n";
CString CString::GetReturn()
{
	return return_str;
}

void CString::SetReturn(const char* s)
{
	return_str = s;
}

int CString::Delete(int nIndex, int nCount)
{
	int i;
	int len = str_len - nCount;
	char* v = new char[len + 1];
	for (i = 0; i < nIndex; i++)
		v[i] = cstring[i];
	for (i = nIndex; i < len; i++)
		v[i] = cstring[i + nCount];
	delete[] cstring;
	cstring = v;
	str_len = len;
	cstring[len] = 0;
	return len;
}

void CString::DeleteLastSlash()
{
	while (str_len)
	{
		int index = str_len - 1;
		char s = cstring[index];
		if (s == '\\' || s == '/')
			*this = Left(index);
		else
			break;
	}
}

bool CString::IsVariableName()
{
	if (str_len > 0)
	{
		for (int i = 0; i < str_len; i++)
		{
			char ss = cstring[i];
			if (!((ss >= '0' && ss <= '9') || (ss >= 'a' && ss <= 'z') || (ss >= 'A' && ss <= 'Z') || ss == '_'))
				return false;
		}
		return true;
	}
	return false;
}


CArrayBase::CArrayBase()
{
	max_existing = 0;
	k_existing = 0;
	m_existing = NULL;

	max_deleted = 0;
	k_deleted = 0;
	m_deleted = NULL;

	max_indexed = 0;
	m_indexed = NULL;
}

CArrayBase::~CArrayBase()
{
	max_existing = k_existing = 0;
	if (m_existing)
	{
		delete[]m_existing;
		m_existing = NULL;
	}

	max_deleted = k_deleted = 0;
	if (m_deleted)
	{
		delete[]m_deleted;
		m_deleted = NULL;
	}

	max_indexed = 0;
	if (m_indexed)
	{
		delete[]m_indexed;
		m_indexed = 0;
	}
}

void CArrayBase::ClearExistingAndDeleted()
{
	max_existing = k_existing = 0;
	if (m_existing)
	{
		delete[]m_existing;
		m_existing = NULL;
	}

	max_deleted = k_deleted = 0;
	if (m_deleted)
	{
		delete[]m_deleted;
		m_deleted = NULL;
	}

	max_indexed = 0;
	if (m_indexed)
	{
		delete[]m_indexed;
		m_indexed = 0;
	}
}

void CArrayBase::IncreaseDeleted(int from, int to)
{
	int count = to - from + 1;
	for (int i = 0; i < count; i++)
		AddToDeleted(from + i);
}

int CArrayBase::FromDeletedToExisting()
{
	int index = -1;
	if (k_deleted)
	{
		k_deleted--;
		index = m_deleted[k_deleted];
		AddToExisting(index);
	}
	return index;
}

int CArrayBase::FromDeletedToExisting(int deleted_index)
{
	int index = -1;
	for (int i = 0; i < k_deleted; i++)
	{
		if (m_deleted[i] == deleted_index)
		{
			m_deleted[i] = m_deleted[k_deleted - 1];
			k_deleted--;
			AddToExisting(deleted_index);
			index = deleted_index;
			break;
		}
	}
	return index;
}

void CArrayBase::FromExistingToDeleted(int index)
{
	assert(max_indexed > index);
	int from = m_indexed[index];
	assert(from >= 0);

	m_indexed[index] = -1;
	if (from != k_existing)
	{
		m_existing[from] = m_existing[k_existing];
		m_indexed[k_existing] = from;
	}
	AddToDeleted(index);
}

void CArrayBase::AddToExisting(int index)
{
	int i;

	if (max_indexed <= index)
	{
		int index1 = index + 1;
		int max_indexed2 = index1 + index1 / 4;
		int* vm_indexed = new int[max_indexed2];
		for (i = 0; i < max_indexed; i++)
			vm_indexed[i] = m_indexed[i];
		for (i = max_indexed; i < max_indexed2; i++)
			vm_indexed[i] = -1;
		if (m_indexed)
			delete[]m_indexed;
		m_indexed = vm_indexed;
		max_indexed = max_indexed2;
	}
	m_indexed[index] = k_existing;

	if (max_existing == k_existing)
	{
		int step = max_existing / 4;
		if (step < 4)
			step = 4;
		int vmax_existing = max_existing + step;
		int* vm_existing = new int[vmax_existing];
		for (i = 0; i < max_existing; i++)
			vm_existing[i] = m_existing[i];
		if (m_existing)
			delete[]m_existing;
		m_existing = vm_existing;
		max_existing = vmax_existing;
	}
	m_existing[k_existing] = index;
	k_existing++;
}

void CArrayBase::Serialize(CStream& stream)
{
	//int i;
	if (stream.IsStoring())
	{
		stream << max_existing;
		stream << k_existing;
		if (k_existing)
			stream.Write(m_existing, k_existing * sizeof(int));
		//for (i=0;i<k_existing;i++)
		//	stream<<m_existing[i];

		stream << max_deleted;
		stream << k_deleted;
		if (k_deleted)
			stream.Write(m_deleted, k_deleted * sizeof(int));
		//for (i=0;i<k_deleted;i++)
		//	stream<<m_deleted[i];

		stream << max_indexed;
		if (max_indexed)
			stream.Write(m_indexed, max_indexed * sizeof(int));
		//for (i=0;i<max_indexed;i++)
		//	stream<<m_indexed[i];
	}
	else
	{
		ClearExistingAndDeleted();

		stream >> max_existing;
		stream >> k_existing;
		if (max_existing)
		{
			m_existing = new int[max_existing];
			if (k_existing)
				stream.Read(m_existing, k_existing * sizeof(int));
			//for (i=0;i<k_existing;i++)
			//	stream>>m_existing[i];
		}

		stream >> max_deleted;
		stream >> k_deleted;
		if (max_deleted)
		{
			m_deleted = new int[max_deleted];
			if (k_deleted)
				stream.Read(m_deleted, k_deleted * sizeof(int));
			//for (i=0;i<k_deleted;i++)
			//	stream>>m_deleted[i];
		}

		stream >> max_indexed;
		if (max_indexed)
		{
			m_indexed = new int[max_indexed];
			stream.Read(m_indexed, max_indexed * sizeof(int));
			//for (i=0;i<max_indexed;i++)
			//	stream>>m_indexed[i];
		}
	}
}

void CArrayBase::AddToDeleted(int index)
{
	if (max_indexed > index)
	{
		m_indexed[index] = -1;
	}
	if (max_deleted == k_deleted)
	{
		int step = max_deleted / 4;
		if (step < 4)
		{
			step = 4;
		}
		int vmax_deleted = max_deleted + step;
		int* vm_deleted = new int[vmax_deleted];
		int i;
		for (i = 0; i < max_deleted; ++i)
		{
			vm_deleted[i] = m_deleted[i];
		}
		if (m_deleted)
			delete[] m_deleted;
		m_deleted = vm_deleted;
		max_deleted = vmax_deleted;
	}
	m_deleted[k_deleted] = index;
	k_deleted++;
}

CStreamFile::CStreamFile()
{
	mode = STREAM_READ;
	bytes = 0;
	name = "";
	stream = nullptr;
}

CStreamFile::~CStreamFile()
{
	if (stream)
	{
		fclose(stream);
		stream = nullptr;
	}
	bytes = 0;
	if (name)
	{
		delete name;
		name = nullptr;
	}
	mode = STREAM_READ;
}

FILE* CStreamFile::fopen_file(const char* name, const char* mode)
{
	FILE* stream = NULL;
#ifdef WIN32
	// ��� Windows ���������� �������������� ������ utf8 � utf16
	wchar_t* wfile = (wchar_t*)utf8to16((unsigned const char*)name);
	wchar_t* wm = (wchar_t*)utf8to16((unsigned const char*)mode);
	stream = _wfopen(wfile, wm);
	delete[]wfile;
	delete[]wm;
#else
	stream = fopen(name, mode);
#endif
	return stream;
}

const unsigned short* CStreamFile::utf8to16(const unsigned char* str_utf8)
{
	unsigned short* str16 = NULL;
	if (str_utf8)
	{
		// ���������� ����� �������� ������, ������, ��� ������ ������������ �� 0
		int length = 0;
		while (true)
		{
			short s = str_utf8[length];
			length++;
			if (!s)
				break;
		}

		int factor = 5;

		int end = length * factor;
		str16 = new unsigned short[end];

		// ���������� ����� ���������� ��� ������ �������, ��� ��� ������� ����������� ������ ������
		const UTF8* vstr_utf8 = str_utf8;
		const UTF8** source = &vstr_utf8;
		UTF16* vstr16 = str16;
		UTF16** target = &vstr16;

		// ConversionResult result = __ConvertUTF8toUTF16(
		// 	source, (const UTF8*)(&(str_utf8[length])),
		// 	target, &(str16[end]), lenientConversion);

		// if (result != conversionOK)
		// {
		// 	delete[]str16;
		// 	str16 = NULL;
		// }
	}

	return str16;
}

void CStreamFile::Close()
{
	if (stream)
	{
		fclose(stream);
		stream = nullptr;
		name = "";
	}
}

bool CStreamFile::Open(const char* fileName, int mode)
{
	name = fileName;
	ChangeMode(mode);
	if (!stream)
	{
		this->mode = STREAM_READ;
		name = "";
	}
	return stream != nullptr;
}

unsigned int CStreamFile::GetLength()
{
	unsigned int len = 0;
	if (stream)
	{
		unsigned int pos = ftell(stream);
		fseek(stream, 0, SEEK_END);
		len = ftell(stream);
		fseek(stream, pos, SEEK_SET);
	}
	return len;
}

unsigned int CStreamFile::GetPosition()
{
	unsigned int pos = 0;
	if (stream)
	{
		pos = ftell(stream);
	}
	return pos;
}

void CStreamFile::SetPosition(unsigned int pos)
{
	if (stream && IsLoading())
	{
		fseek(stream, pos, SEEK_SET);
	}
}

unsigned int CStreamFile::Write(void* m_data, unsigned int k_data)
{
	bytes = 0;
	if (stream && IsStoring())
	{
		bytes = fwrite(m_data, 1, k_data, stream);
	}
	return bytes;
}

unsigned int CStreamFile::Read(void* m_data, unsigned int k_data)
{
	bytes = 0;
	if (stream && IsLoading())
		bytes = fread(m_data, 1, k_data, stream);
	return bytes;
}

void CStreamFile::ChangeMode(int mode)
{
	CString vname = name;
	Close();
	name = vname;

	CString smode;
	switch (mode)
	{
	case STREAM_READ:
		smode = "rb";
		break;
	case STREAM_WRITE:
		smode = "wb";
		break;

	case STREAM_ADD:
		smode = "ab";
		break;

	default:
		return;
	}

	this->mode = mode;
	
	stream = fopen_file(name, smode.c_str());
}

bool CStream::IsStoring()
{
	return (mode != STREAM_READ);
}

bool CStream::IsLoading()
{
	return (mode == STREAM_READ);
}

void CStream::SetMode(int mode)
{
	if (mode != this->mode && mode >= STREAM_READ && mode <= STREAM_ADD)
		ChangeMode(mode);
}

void CStream::operator<<(bool& value)
{
	char* v = (char*)value;
	Write(v, 1);
}

void CStream::operator<<(char& value)
{
	char* v = (char*)value;
	Write(v, 1);
}

void CStream::operator<<(unsigned char& value)
{
	char* v = (char*)&value;
	Write(v, 1);
}

void CStream::operator<<(CString& value)
{
	int len = value.GetLength();
	if (len >= 0xff)
	{
		unsigned char vlen = 0xff;
		Write((char*)&value, 1);
		unsigned short vlen2 = len;
		*this << vlen2;
	}
	else
	{
		unsigned char vlen = (unsigned char)len;
		Write((char*)&vlen, 1);
	}

	if (len)
	{
		char* v = (char*)(const char*)value;
		Write(v, len);
	}
}

void CStream::operator<<(int& value)
{
	int v2 = value;
	INVERT_BYTES(2);
	char* v = (char*)&v2;
	Write(v, 2);
}

void CStream::operator<<(short& value)
{
	short v2 = value;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 2);
}

void CStream::operator<<(unsigned short& value)
{
	short v2 = value;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 2);
}

void CStream::operator<<(float& value)
{
	float v2 = value;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 4);
}

void CStream::operator<<(double& value)
{
	double v2 = value;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 8);
}

void CStream::operator<<(unsigned int& value)
{
	unsigned int v2 = value;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 4);
}

void CStream::operator<<(CSize& value)
{
	int v2 = value.cx;
	INVERT_BYTES(v2);
	char* v = (char*)&v2;
	Write(v, 4);

	v2 = value.cy;
	INVERT_BYTES(v2);
	v = (char*)&v2;
	Write(v, 4);
}

void CStream::operator<<(CPoint& value)
{
	int v2 = value.x;
	INVERT_BYTES(v2);
	char* v = (char*)(&v2);
	Write(v, 4);

	v2 = value.y;
	INVERT_BYTES(v2);
	v = (char*)(&v2);
	Write(v, 4);
}

void CStream::operator>>(bool& value)
{
	char* v = (char*)(&value);
	Read(v, 1);
}

void CStream::operator>>(char& value)
{
}

void CStream::operator>>(unsigned char& value)
{
	char* v = (char*)(&value);
	Read(v, 1);
}

void CStream::operator>>(CString& value)
{
	int len;
	unsigned char vlen;
	Read((char*)&vlen, 1);
	if (vlen == 0xff)
	{
		unsigned short vlen2;
		(*this) >> vlen2;
		len = vlen2;
	}
	else
		len = vlen;

	if (len)
	{
		value.SetLength(len);
		char* m = (char*)value.c_str();
		Read(m, len);
		m[len] = 0;
	}
	else
		value = "";
}

void CStream::operator>>(int& value)
{
	char* v = (char*)(&value);
	Read(v, 4);
	INVERT_BYTES(value);
}

void CStream::operator>>(short& value)
{
	char* v = (char*)(&value);
	Read(v, 2);
	INVERT_BYTES(value);
}

void CStream::operator>>(unsigned short& value)
{
	char* v = (char*)(&value);
	Read(v, 2);
	INVERT_BYTES(value);
}

void CStream::operator>>(float& value)
{
	char* v = (char*)(&value);
	Read(v, 4);
	INVERT_BYTES(value);
}

void CStream::operator>>(double& value)
{
	char* v = (char*)(&value);
	Read(v, 8);
	INVERT_BYTES(value);
}

void CStream::operator>>(unsigned int& value)
{
	char* v = (char*)(&value);
	Read(v, 4);
	INVERT_BYTES(value);
}

void CStream::operator>>(CSize& value)
{
	char* v = (char*)(&value.cx);
	Read(v, 4);
	INVERT_BYTES(value.cx);
	v = (char*)(&value.cy);
	Read(v, 4);
	INVERT_BYTES(value.cy);
}

void CStream::operator>>(CPoint& value)
{
	char* v = (char*)(&value.x);
	Read(v, 4);
	INVERT_BYTES(value.x);
	v = (char*)(&value.y);
	Read(v, 4);
	INVERT_BYTES(value.y);
}

CArrayPtr::CArrayPtr()
{
	k_ptr = 0;
	m_ptr = nullptr;
}

CArrayPtr::~CArrayPtr()
{
}

void CArrayPtr::Clear()
{
	if (m_ptr)
	{
		free(m_ptr);
		m_ptr = NULL;
	}

	k_ptr = 0;

	ClearExistingAndDeleted();
}

void* CArrayPtr::Get(int index)
{
	if (index >= 0 && index < k_ptr)
		return m_ptr[index];
	return nullptr;
}

int CArrayPtr::Add(void* data)
{
	int index = FromDeletedToExisting();

	if (index == -1)
	{
		int size = sizeof(void*);
		int step = k_ptr / 4;
		if (step < 4)
			step = 4;
		int k_ptr2 = k_ptr + step;
		m_ptr = (void**)realloc(m_ptr, size * (k_ptr2));

		for (int i = k_ptr; i < k_ptr2; i++)
			m_ptr[i] = NULL;

		IncreaseDeleted(k_ptr, k_ptr2 - 1);
		index = FromDeletedToExisting();

		k_ptr = k_ptr2;
	}

	m_ptr[index] = data;

	return index;
}

bool CArrayPtr::Delete(int index)
{
	if (index >= 0 && index < k_ptr)
	{
		if (m_ptr[index])
		{
			m_ptr[index] = NULL;
			FromExistingToDeleted(index);

			return true;
		}
	}
	return false;
}

int CArrayPtr::FindIndex(void* ptr)
{
	int index = -1;

	for (int i = 0; i < k_existing; i++)
	{
		int ind = m_existing[i];
		if (m_ptr[ind] == ptr)
		{
			index = ind;
			break;
		}
	}

	return index;
}

void CArrayPtr::SaveToFile(FILE* file)
{
}

void CArrayPtr::LoadFromFile(FILE* file)
{
}

CStream::CStream()
{
	bytes = 0;
	mode = STREAM_READ;
	name = "";
}

CStream::~CStream()
{

}

CStringArray::CStringArray()
{
	k_str = 0;
	max_str = 100;
	m_str = NULL;
}

CStringArray::~CStringArray()
{
	Clear();
}

void CStringArray::Clear()
{
	for (int i = 0; i < k_str; i++)
	{
		delete[] m_str[i];
		m_str[i] = NULL;
	}

	if (m_str)
	{
		delete[]m_str;
		m_str = NULL;
	}

	k_str = max_str = 0;
}

void CStringArray::Serialize(CStream& ar)
{
	if (ar.IsStoring())
	{
		ar << max_str;
		ar << k_str;
		for (int i = 0; i < k_str; i++)
			ar << (*(m_str[i]));
	}
	else
	{
		Clear();

		ar >> max_str;
		if (max_str)
		{
			m_str = new CString * [max_str];
			ar >> k_str;
			for (int i = 0; i < k_str; i++)
			{
				m_str[i] = new CString;
				ar >> (*(m_str[i]));
			}
		}
	}
}

int CStringArray::Add(CString* str)
{
	if (k_str == max_str)
	{
		// ���������� ���������� ���������� �������
		int step = max_str / 4;
		if (step < 10)
			step = 10;

		int max = max_str + step;
		CString** vm_str = new CString * [max];
		for (int i = 0; i < max_str; i++)
			vm_str[i] = m_str[i];
		max_str = max;
		if (m_str)
			delete[]m_str;
		m_str = vm_str;
	}
	if (k_str < 0)
		k_str = 0;
	m_str[k_str] = new CString;
	*(m_str[k_str]) = *str;
	k_str++;
	return k_str - 1;
}

CString* CStringArray::Get(int index)
{
	if (index >= 0 && index < k_str)
		return m_str[index];
	return nullptr;
}

void CStringArray::FromString(const char* str)
{
	Clear();

	CString from = str;
	from.Trim();

	if (!from.IsEmpty())
	{
		CString ss;
		int len = from.GetLength();
		for (int i = 0; i < len; i++)
		{
			char s = from[i];
			if (s == ' ' || s == '\t')
			{
				if (!ss.IsEmpty())
				{
					Add(&ss);
					ss = "";
				}
			}
			else
				ss += s;
		}
		if (!ss.IsEmpty())
			Add(&ss);
	}
}

CStreamMemory::CStreamMemory() : CStream()
{
	buffer = nullptr;
	position = 0;
	length = 0;
	max_length = 0;
	is_created_buffer = false;
}

CStreamMemory::~CStreamMemory()
{
	if (buffer)
	{
		if (is_created_buffer)
			free(buffer);
		buffer = NULL;
		position = 0;
		length = 0;
		max_length = 0;
	}
}

bool CStreamMemory::Open(const char* buffer, unsigned int length, int mode)
{
	Close();

	if (mode == STREAM_ADD && ((!buffer) || length <= 0))
		mode = STREAM_WRITE;

	this->mode = mode;

	switch (mode)
	{
	case STREAM_READ:
		this->buffer = (char*)buffer;
		length = length;
		max_length = length;
		is_created_buffer = false;
		break;

	case STREAM_WRITE:
		if (buffer || length > 0)
			return false;
		max_length = STREAM_MEMORY_START_SIZE;
		this->buffer = (char*)malloc(max_length);
		is_created_buffer = true;
		break;

	case STREAM_ADD:
		if ((!buffer) || length <= 0)
		{
			// ������� �������� ������
			max_length = STREAM_MEMORY_START_SIZE;
			this->buffer = (char*)malloc(max_length);
		}
		else
		{
			// �������� ������ � ����� �����
			max_length = STREAM_MEMORY_START_SIZE + length;
			this->buffer = (char*)malloc(max_length);
			for (unsigned int i = 0; i < length; i++)
				this->buffer[i] = buffer[i];
			length = length;
			position = length;
		}
		is_created_buffer = true;
		break;

	default:
		return false;
	}

	return true;
}

void CStreamMemory::Close()
{
	if (buffer)
	{
		if (is_created_buffer)
			free(buffer);
		buffer = NULL;
		position = 0;
		length = 0;

		max_length = STREAM_MEMORY_START_SIZE;
		buffer = (char*)malloc(max_length);
		is_created_buffer = true;
	}
}

unsigned int CStreamMemory::GetLength()
{
	if (buffer)
		return length;
	return 0;
}

unsigned int CStreamMemory::GetPosition()
{
	if (buffer)
		return position;
	return 0;
}

void CStreamMemory::SetPosition(unsigned int pos)
{
	if (buffer && IsLoading())
	{
		if (length && pos > length)
			pos = length;
		position = pos;
	}
}

unsigned int CStreamMemory::Write(void* m_data, unsigned int k_data)
{
	bytes = 0;
	if (buffer && IsStoring())
	{
		unsigned int new_position = position + k_data;
		if (new_position > max_length)
		{
			// ��������� ������
			max_length = new_position + new_position / 4;
			buffer = (char*)realloc(buffer, max_length);
		}

		char* d = (char*)m_data;

		for (unsigned int i = 0; i < k_data; i++)
		{
			buffer[position] = d[i];
			position++;
		}

		if (position > length)
			length = position;
	}

	return bytes;
}

unsigned int CStreamMemory::Read(void* m_data, unsigned int k_data)
{
	bytes = 0;
	if (buffer && IsLoading())
	{
		unsigned int new_position = position + k_data;
		bytes = k_data;
		if (length && new_position > length)
			bytes = length - position;

		char* d = (char*)m_data;

		for (unsigned int i = 0; i < bytes; i++)
		{
			d[i] = buffer[position];
			position++;
		}
	}

	return bytes;
}

void CStreamMemory::ChangeMode(int mode)
{
	switch (mode)
	{
	case STREAM_READ:
		position = 0;
		break;

	case STREAM_WRITE:
		Close();
		max_length = STREAM_MEMORY_START_SIZE;
		buffer = (char*)malloc(max_length);
		is_created_buffer = true;
		break;

	case STREAM_ADD:
		position = length;
		break;
	}

	this->mode = mode;
}

CStringData::CStringData()
{
	data.length = 0;
	data.data = NULL;
	index = -1;
}

CStringData::~CStringData()
{
	if (data.data)
	{
		delete[]data.data;
		data.data = NULL;
		data.length = 0;
	}
}

void CStringData::SetData(int length, unsigned char* data)
{
	if (this->data.length != length)
	{
		if (this->data.data)
		{
			delete[]this->data.data;
			this->data.data = NULL;
			this->data.length = 0;
		}

		if (length)
		{
			this->data.length = length;
			this->data.data = new unsigned char[length];
		}
	}
	memcpy(this->data.data, data, length);
}

void CStringData::Serialize(CStream& stream)
{
	CString* ms = this;
	if (stream.IsStoring())
	{
		stream << *ms;
		stream << data.length;
		if (data.length > 0)
			stream.Write(data.data, data.length);
	}
	else
	{
		if (data.data)
		{
			delete[]data.data;
			data.data = NULL;
			data.length = 0;
		}

		stream >> *ms;

		stream >> data.length;
		if (data.length > 0)
		{
			data.data = new unsigned char[data.length];
			stream.Read(data.data, data.length);
		}
	}
}

CStringDataArray::CStringDataArray()
{
	k_str = 0;
	max_str = 0;
	m_str = nullptr;
}

CStringDataArray::~CStringDataArray()
{
	Clear();
}

void CStringDataArray::Clear()
{
	for (int i = 0; i < k_str; i++)
	{
		delete m_str[i];
		m_str[i] = NULL;
	}

	if (m_str)
	{
		delete[]m_str;
		m_str = NULL;
	}

	k_str = max_str = 0;
}

void CStringDataArray::Serialize(CStream& ar)
{
	if (ar.IsStoring())
	{
		ar << max_str;
		ar << k_str;
		for (int i = 0; i < k_str; i++)
			m_str[i]->Serialize(ar);
	}
	else
	{
		Clear();

		ar >> max_str;
		if (max_str)
		{
			m_str = new CStringData * [max_str];
			ar >> k_str;
			for (int i = 0; i < k_str; i++)
			{
				m_str[i] = new CStringData;
				m_str[i]->Serialize(ar);
			}
		}
	}
}

int CStringDataArray::Add(CString* str, MEM_DATA* data)
{
	if(k_str == max_str)
	{
		// ���������� ���������� ���������� �������
		int step = max_str / 4;
		if (step < 10)
			step = 10;

		int max = max_str + step;
		CStringData** vm_str = new CStringData * [max];
		for (int i = 0; i < max_str; i++)
			vm_str[i] = m_str[i];
		max_str = max;
		if (m_str)
			delete[]m_str;
		m_str = vm_str;
	}

	CStringData* md = new CStringData;
	m_str[k_str] = md;

	CString* vmd = md;

	*vmd = *str;
	if (data)
		md->SetData(data->length, data->data);

	k_str++;
	return k_str - 1;
}

int CStringDataArray::Delete(int index)
{
	if (index >= 0 && index < k_str)
	{
		int index_fast = m_str[index]->GetIndex();

		delete m_str[index];

		for (int i = index + 1; i < k_str; i++)
			m_str[i - 1] = m_str[i];

		k_str--;
		m_str[k_str] = NULL;

		return index_fast;
	}
	return -1;
}

CStringData* CStringDataArray::Get(int index)
{
	if (index >= 0 && index < k_str)
		return m_str[index];
	return nullptr;
}

__STRING_TABLE_3::__STRING_TABLE_3()
{
	k_data = 0;
	m_data = nullptr;
}

__STRING_TABLE_3::~__STRING_TABLE_3()
{
	Clear();
}

void __STRING_TABLE_3::Clear()
{
	for (int i = 0; i < k_data; i++)
	{
		if (m_data[i])
		{
			delete m_data[i];
			m_data[i] = NULL;
		}
	}

	if (m_data)
	{
		delete[]m_data;
		m_data = NULL;
		k_data = 0;
	}
}

void __STRING_TABLE_3::SetLength(int len)
{
	if (k_data < len)
	{
		CStringDataArray** vm_data = new CStringDataArray * [len];
		for (int i = 0; i < k_data; i++)
			vm_data[i] = m_data[i];
		for (int i = k_data; i < len; i++)
			vm_data[i] = NULL;
		delete[]m_data;
		m_data = vm_data;
		k_data = len;
	}
}

__STRING_TABLE_2::__STRING_TABLE_2()
{
	k_data = 0;
	m_data = nullptr;
}

__STRING_TABLE_2::~__STRING_TABLE_2()
{
	Clear();
}

void __STRING_TABLE_2::Clear()
{
	for (int i = 0; i < k_data; i++)
	{
		if (m_data[i])
		{
			delete m_data[i];
			m_data[i] = NULL;
		}
	}

	if (m_data)
	{
		delete[]m_data;
		m_data = NULL;
		k_data = 0;
	}
}

void __STRING_TABLE_2::SetLength(int len)
{
	if (k_data < len)
	{
		__STRING_TABLE_3** vm_data = new __STRING_TABLE_3 * [len];
		for (int i = 0; i < k_data; i++)
			vm_data[i] = m_data[i];
		for (int i = k_data; i < len; i++)
			vm_data[i] = NULL;
		delete[]m_data;
		m_data = vm_data;
		k_data = len;
	}
}

__STRING_TABLE_1::__STRING_TABLE_1()
{
	k_data = 0;
	m_data = NULL;
}

__STRING_TABLE_1::~__STRING_TABLE_1()
{
	Clear();
}

void __STRING_TABLE_1::Clear()
{
	for (int i = 0; i < k_data; i++)
	{
		if (m_data[i])
		{
			delete m_data[i];
			m_data[i] = NULL;
		}
	}

	if (m_data)
	{
		delete[]m_data;
		m_data = NULL;
		k_data = 0;
	}
}

void __STRING_TABLE_1::SetLength(int len)
{
	if (k_data < len)
	{
		__STRING_TABLE_2** vm_data = new __STRING_TABLE_2 * [len];
		for (int i = 0; i < k_data; i++)
			vm_data[i] = m_data[i];
		for (int i = k_data; i < len; i++)
			vm_data[i] = NULL;
		delete[]m_data;
		m_data = vm_data;
		k_data = len;
	}
}

CStringTable::CStringTable()
{
	max_ptr = 0;
	k_ptr = 0;
	m_ptr = NULL;
}

CStringTable::~CStringTable()
{
	Clear();
}

void CStringTable::Clear()
{
	table.Clear();
	if (m_ptr)
	{
		delete[]m_ptr;
		m_ptr = NULL;
	}
	max_ptr = 0;
	k_ptr = 0;
}

CStringData* CStringTable::Find(CString& name)
{
	int first, middle, last;
	if (GetSymbols(name, first, middle, last))
	{
		if (table.k_data > first)
		{
			__STRING_TABLE_2* table2 = table.m_data[first];
			if (table2)
			{
				if (table2->k_data > last)
				{
					__STRING_TABLE_3* table3 = table2->m_data[last];
					if (table3)
					{
						if (table3->k_data > middle)
						{
							CStringDataArray* str_array = table3->m_data[middle];
							if (str_array)
							{
								int k = str_array->GetCount();
								for (int i = 0; i < k; i++)
								{
									CStringData* str = str_array->Get(i);
									if (name == *str)
										return str;
								}
							}
						}
					}
				}
			}
		}
	}
	return nullptr;
}

bool CStringTable::Add(CString& name, MEM_DATA* data)
{
	if (!Find(name))
	{
		int first, middle, last;
		if (GetSymbols(name, first, middle, last))
		{
			if (table.k_data <= first)
				table.SetLength(first + 1);
			__STRING_TABLE_2* table2 = table.m_data[first];
			if (!table2)
			{
				table.m_data[first] = new __STRING_TABLE_2;
				table2 = table.m_data[first];
			}
			if (table2->k_data <= last)
				table2->SetLength(last + 1);
			__STRING_TABLE_3* table3 = table2->m_data[last];
			if (!table3)
			{
				table2->m_data[last] = new __STRING_TABLE_3;
				table3 = table2->m_data[last];
			}
			if (table3->k_data <= middle)
				table3->SetLength(middle + 1);
			CStringDataArray* str_array = table3->m_data[middle];
			if (!str_array)
			{
				str_array = new CStringDataArray;
				table3->m_data[middle] = str_array;
			}
			int k = str_array->GetCount();
			for (int i = 0; i < k; i++)
			{
				CString* str = str_array->Get(i);
				if (name == *str)
				{
					return false;
				}
			}
			int index = str_array->Add(&name, data);
			CStringData* added_str = str_array->Get(index);

			if (max_ptr == k_ptr)
			{
				int step = max_ptr / 4;
				if (step < 4)
					step = 4;
				max_ptr += step;
				CStringData** vm_ptr = new CStringData * [max_ptr];
				if (m_ptr)
				{
					for (int i = 0; i < k_ptr; i++)
						vm_ptr[i] = m_ptr[i];

					delete[]m_ptr;
				}
				m_ptr = vm_ptr;
			}
			m_ptr[k_ptr] = added_str;
			added_str->SetIndex(k_ptr);
			k_ptr++;
			return true;
		}
	}
	return false;
}

bool CStringTable::Delete(CString& name)
{
	if (Find(name))
	{
		int first, middle, last;
		if (GetSymbols(name, first, middle, last))
		{
			__STRING_TABLE_2* table2 = table.m_data[first];
			__STRING_TABLE_3* table3 = table2->m_data[last];
			CStringDataArray* str_array = table3->m_data[middle];
			int k = str_array->GetCount();
			for (int i = 0; i < k; i++)
			{
				CStringData* str = str_array->Get(i);
				if (name == *str)
				{
					int index = str_array->Delete(i);
					k_ptr--;
					if (index != k_ptr)
					{
						m_ptr[index] = m_ptr[k_ptr];
						m_ptr[index]->SetIndex(index);
					}
					break;
				}
			}
		}

		return true;
	}
	return false;
}

void CStringTable::Serialize(CStream& stream, bool count_yes)
{
	if (stream.IsStoring())
	{
		if (count_yes)
			stream << k_ptr;
		for (int i = 0; i < k_ptr; i++)
			m_ptr[i]->Serialize(stream);
	}
	else
	{
		Clear();

		CStringData d;
		if (count_yes)
		{
			stream >> k_ptr;
			for (int i = 0; i < k_ptr; i++)
			{
				d.Serialize(stream);
				Add(d, d.GetData());
			}
		}
		else
		{
			// ���������� ������ ����� �� �����
			unsigned int length = stream.GetLength();
			unsigned int pos = 0;
			while (pos < length)
			{
				d.Serialize(stream);
				Add(d, d.GetData());
				pos = stream.GetPosition();
			}
		}
	}
}

bool CStringTable::GetSymbols(CString& name, int& first, int& middle, int& last)
{
	int len = name.GetLength();
	if (len > 0)
	{
		unsigned char v = name[0];
		first = v;
		v = name[len / 2];
		middle = v;
		v = name[len - 1];
		last = v;
		return true;
	}
	first = middle = last = -1;
	return false;
}

CAddressString::CAddressString(sockaddr_in const& addr)
{
	fVal = new char[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr, fVal, INET_ADDRSTRLEN);
}

CAddressString::CAddressString(in_addr const& addr)
{
	fVal = new char[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr, fVal, INET_ADDRSTRLEN);
}

CAddressString::CAddressString(sockaddr_in6 const& addr)
{
	fVal = new char[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &addr, fVal, INET6_ADDRSTRLEN);
}

CAddressString::CAddressString(in6_addr const& addr)
{
	fVal = new char[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &addr, fVal, INET6_ADDRSTRLEN);
}

CAddressString::CAddressString(sockaddr_storage const& addr)
{
	switch (addr.ss_family) {
	case AF_INET: {
		fVal = new char[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &addr, fVal, INET6_ADDRSTRLEN);
		break;
	}
	case AF_INET6: {
		fVal = new char[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &addr, fVal, INET6_ADDRSTRLEN);
		break;
	}
	default: {
		fVal = new char[200]; // more than enough for this error message
		sprintf(fVal, "(unknown address family %d)", addr.ss_family);
		break;
	}
	}
}

CAddressString::~CAddressString()
{
	delete[] fVal;
}

char* strDup(char const* str) {
	if (str == NULL) return NULL;
	size_t len = strlen(str) + 1;
	char* copy = new char[len];

	if (copy != NULL) {
		memcpy(copy, str, len);
	}
	return copy;
}

char const* dateHeader() {
	static char buf[200];
#if !defined(_WIN32_WCE)
	time_t tt = time(NULL);
	strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
#else
	// WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
	// so generate the "Date:" header a different, WinCE-specific way.
	// (Thanks to Pierre l'Hussiez for this code)
	// RSF: But where is the "Date: " string?  This code doesn't look quite right...
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
	WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
	WCHAR inBuf[200];
	DWORD locale = LOCALE_NEUTRAL;

	int ret = GetDateFormat(locale, 0, &SystemTime,
		(LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof inBuf);
	inBuf[ret - 1] = ' ';
	ret = GetTimeFormat(locale, 0, &SystemTime,
		(LPTSTR)timeFormat,
		(LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
	wcstombs(buf, inBuf, wcslen(inBuf));
#endif
	return buf;
}