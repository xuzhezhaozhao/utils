/*******************************************************
 @Author: zhezhaoxu
 @Created Time : Thu 26 Oct 2017 10:12:05 PM CST
 @File Name: str2num.c
 @Description: from lua/src/lobject.c (5.3.0)
 ******************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* 返回字符的16进制大小 */
static int luaO_hexavalue(int c) {
	if (isdigit(c))
		return c - '0';
	else
		return tolower(c) - 'a' + 10;
}

/**
 * 首地址为 *s 的字符串转成整数是否为负数
 */
static int isneg(const char **s) {
	if (**s == '-') {
		(*s)++;
		return 1;
	} else if (**s == '+')
		(*s)++;
	return 0;
}

const char *l_str2d(const char *s, double *result) {
	char *endptr;
	if (strpbrk(s, "nN")) /* reject 'inf' and 'nan' */
		return NULL;
	else
		*result = strtod(s, &endptr);
	if (endptr == s) return 0; /* nothing recognized */
	while (isspace((unsigned char)(*endptr))) endptr++;
	return (*endptr == '\0' ? endptr : NULL); /* OK if no trailing characters */
}

const char *l_str2int(const char *s, long long *result) {
	unsigned long long a = 0;
	int empty = 1;
	int neg;
	while (isspace((unsigned char)(*s))) s++; /* skip initial spaces */
	neg = isneg(&s);
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { /* hex? */
		s += 2;										   /* skip '0x' */
		for (; isxdigit((unsigned char)(*s)); s++) {
			a = a * 16 + luaO_hexavalue(*s);
			empty = 0;
		}
	} else { /* decimal */
		for (; isdigit((unsigned char)(*s)); s++) {
			a = a * 10 + *s - '0';
			empty = 0;
		}
	}
	while (isspace((unsigned char)(*s))) s++; /* skip trailing spaces */
	if (empty || *s != '\0')
		return NULL; /* something wrong in the numeral */
	else {
		*result = (long long)((neg) ? 0u - a : a);
		return s;
	}
}
