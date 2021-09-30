#include "utils.hpp"
#include <cassert>

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
	int len = strlen(s);
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
	Format("%d", v);
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
	m_existing = nullptr;

	max_deleted = 0;
	k_deleted = 0;
	m_deleted = nullptr;

	max_indexed = 0;
	m_indexed = nullptr;
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
