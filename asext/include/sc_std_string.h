#pragma once

#include <cstring>

#ifdef _WIN32

class msvs_stdstring
{
public:
	char* c_str()
	{
		if (_capacity >= sizeof(_u.strbuf)) {
			return _u.strptr;
		}
		return _u.strbuf;
	}

	void assign(const char* src)
	{
		size_t srclen = strlen(src);
		if (srclen < sizeof(_u.strbuf)) {
			memcpy(_u.strbuf, src, srclen + 1);
			_len = srclen;
			_capacity = sizeof(_u.strbuf) - 1;
		}
	}

	size_t len() const { return _len; }

	size_t capacity() const { return _capacity; }

private:

	union {
		char strbuf[16];
		char* strptr;
	} _u;
	size_t _len;
	size_t _capacity;
};

static_assert(sizeof(msvs_stdstring) == (16 + sizeof(size_t) * 2), "Size Check");

typedef msvs_stdstring sc_stdstring;

#else

class cxx11_std_string {  // 32-bit
public:
	char* c_str()
	{
		return _M_p;
	}

	size_t len() const { return _M_string_length; }

	size_t capacity() const
	{
		if (_M_p == _M_local_buf) {
			return sizeof(_M_local_buf) - 1;
		}
		return _M_allocated_capacity;
	}

	void assign(const char* src)
	{
		size_t srclen = strlen(src);
		if (srclen < sizeof(_M_local_buf)) {
			_M_p = _M_local_buf;
			memcpy(_M_local_buf, src, srclen + 1);
			_M_string_length = srclen;
		}
	}

private:
	char* _M_p;              // +0x00  指向字符数据（SSO 时指向自身 _M_local_buf）
	size_t _M_string_length;  // +0x04  字符串长度（不含 \0）
	union {                   // +0x08
		char   _M_local_buf[16];      // SSO 缓冲区（可存 ≤15 字符 + \0）
		size_t _M_allocated_capacity;  // 堆分配时的容量
	};
};  // sizeof = 24

static_assert(sizeof(cxx11_std_string) == (sizeof(char*) + sizeof(size_t) + 16), "Size Check");

typedef cxx11_std_string sc_stdstring;

#endif
