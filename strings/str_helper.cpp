/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月24日 星期四 10时14分27秒
 @File Name: str_helper.cpp
 @Description:
 @Reference: jsoncpp (https://github.com/open-source-parsers/jsoncpp)
 ******************************************************/

#include "str_helper.h"

#include <assert.h>
#include <string.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace utils {
namespace str_helper {

static const int64_t minLargestInt = int64_t(~(uint64_t(-1) / 2));
static const int64_t maxLargestInt = int64_t(uint64_t(-1) / 2);
static const uint64_t maxLargestUInt = uint64_t(-1);

enum {
	/// Constant that specify the size of the buffer that must be passed to
	/// uintToString.
	uintToStringBufferSize = 3 * sizeof(uint64_t) + 1
};
typedef char UIntToStringBuffer[uintToStringBufferSize];

/** Converts an unsigned integer to string.
 * @param value Unsigned interger to convert to string
 * @param current Input/Output string buffer.
 *        Must have at least uintToStringBufferSize chars free.
 */
static inline void uintToString(uint64_t value, char*& current) {
	*--current = 0;
	do {
		*--current =
			static_cast<char>(value % 10U + static_cast<unsigned>('0'));
		value /= 10;
	} while (value != 0);
}

/** Change ',' to '.' everywhere in buffer.
 *
 * We had a sophisticated way, but it did not work in WinCE.
 * @see https://github.com/open-source-parsers/jsoncpp/pull/9
 */
static inline void fixNumericLocale(char* begin, char* end) {
	while (begin < end) {
		if (*begin == ',') {
			*begin = '.';
		}
		++begin;
	}
}

static char getDecimalPoint() {
	struct lconv* lc = localeconv();
	return lc ? *(lc->decimal_point) : '\0';
}

static inline void fixNumericLocaleInput(char* begin, char* end) {
	char decimalPoint = getDecimalPoint();
	if (decimalPoint != '\0' && decimalPoint != '.') {
		while (begin < end) {
			if (*begin == '.') {
				*begin = decimalPoint;
			}
			++begin;
		}
	}
}

/// Returns true if ch is a control character (in range [1,31]).
static inline bool isControlCharacter(char ch) { return ch > 0 && ch <= 0x1F; }
static bool containsControlCharacter(const char* str) {
	while (*str) {
		if (isControlCharacter(*(str++))) return true;
	}
	return false;
}

static bool containsControlCharacter0(const char* str, unsigned len) {
	char const* end = str + len;
	while (end != str) {
		if (isControlCharacter(*str) || 0 == *str) return true;
		++str;
	}
	return false;
}

std::string valueToString(int64_t value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	if (value == minLargestInt) {
		uintToString(uint64_t(maxLargestInt) + 1, current);
		*--current = '-';
	} else if (value < 0) {
		uintToString(uint64_t(-value), current);
		*--current = '-';
	} else {
		uintToString(uint64_t(value), current);
	}
	assert(current >= buffer);
	return current;
}

std::string valueToString(uint64_t value) {
	UIntToStringBuffer buffer;
	char* current = buffer + sizeof(buffer);
	uintToString(value, current);
	assert(current >= buffer);
	return current;
}

std::string valueToString(int value) {
	return valueToString(int64_t(value));
}

std::string valueToString(unsigned int value) {
	return valueToString(uint64_t(value));
}

std::string valueToString(double value, bool useSpecialFloats,
							 unsigned int precision) {
	// Allocate a buffer that is more than large enough to store the 16 digits
	// of
	// precision requested below.
	char buffer[36];
	int len = -1;

	char formatString[15];
	snprintf(formatString, sizeof(formatString), "%%.%dg", precision);

	// Print into the buffer. We need not request the alternative representation
	// that always has a decimal point because JSON doesn't distingish the
	// concepts of reals and integers.
	if (std::isfinite(value)) {
		len = snprintf(buffer, sizeof(buffer), formatString, value);
		fixNumericLocale(buffer, buffer + len);

		// try to ensure we preserve the fact that this was given to us as a
		// double on input
		if (!strchr(buffer, '.') && !strchr(buffer, 'e')) {
			strcat(buffer, ".0");
		}

	} else {
		// IEEE standard states that NaN values will not compare to themselves
		if (value != value) {
			len = snprintf(buffer, sizeof(buffer),
						   useSpecialFloats ? "NaN" : "null");
		} else if (value < 0) {
			len = snprintf(buffer, sizeof(buffer),
						   useSpecialFloats ? "-Infinity" : "-1e+9999");
		} else {
			len = snprintf(buffer, sizeof(buffer),
						   useSpecialFloats ? "Infinity" : "1e+9999");
		}
	}
	assert(len >= 0);
	return buffer;
}

std::string valueToString(double value) {
	return valueToString(value, false, 17);
}

std::string valueToString(bool value) { return value ? "true" : "false"; }

std::string valueToQuotedString(const char* value) {
	if (value == NULL) return "";
	// Not sure how to handle unicode...
	if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL &&
		!containsControlCharacter(value))
		return std::string("\"") + value + "\"";
	// We have to walk value and escape any special characters.
	// Appending to std::string is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	std::string::size_type maxsize =
		strlen(value) * 2 + 3;  // allescaped+quotes+NULL
	std::string result;
	result.reserve(maxsize);  // to avoid lots of mallocs
	result += "\"";
	for (const char* c = value; *c != 0; ++c) {
		switch (*c) {
			case '\"':
				result += "\\\"";
				break;
			case '\\':
				result += "\\\\";
				break;
			case '\b':
				result += "\\b";
				break;
			case '\f':
				result += "\\f";
				break;
			case '\n':
				result += "\\n";
				break;
			case '\r':
				result += "\\r";
				break;
			case '\t':
				result += "\\t";
				break;
			// case '/':
			// Even though \/ is considered a legal escape in JSON, a bare
			// slash is also legal, so I see no reason to escape it.
			// (I hope I am not misunderstanding something.
			// blep notes: actually escaping \/ may be useful in javascript to
			// avoid </
			// sequence.
			// Should add a flag to allow this compatibility mode and prevent
			// this
			// sequence from occurring.
			default:
				if (isControlCharacter(*c)) {
					std::ostringstream oss;
					oss << "\\u" << std::hex << std::uppercase
						<< std::setfill('0') << std::setw(4)
						<< static_cast<int>(*c);
					result += oss.str();
				} else {
					result += *c;
				}
				break;
		}
	}
	result += "\"";
	return result;
}

// https://github.com/upcaste/upcaste/blob/master/src/upcore/src/cstring/strnpbrk.cpp
static char const* strnpbrk(char const* s, char const* accept, size_t n) {
	assert((s || !n) && accept);

	char const* const end = s + n;
	for (char const* cur = s; cur < end; ++cur) {
		int const c = *cur;
		for (char const* a = accept; *a; ++a) {
			if (*a == c) {
				return cur;
			}
		}
	}
	return NULL;
}

std::string valueToQuotedStringN(const char* value, unsigned length) {
	if (value == NULL) return "";
	// Not sure how to handle unicode...
	if (strnpbrk(value, "\"\\\b\f\n\r\t", length) == NULL &&
		!containsControlCharacter0(value, length))
		return std::string("\"") + value + "\"";
	// We have to walk value and escape any special characters.
	// Appending to std::string is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	std::string::size_type maxsize =
		length * 2 + 3;  // allescaped+quotes+NULL
	std::string result;
	result.reserve(maxsize);  // to avoid lots of mallocs
	result += "\"";
	char const* end = value + length;
	for (const char* c = value; c != end; ++c) {
		switch (*c) {
			case '\"':
				result += "\\\"";
				break;
			case '\\':
				result += "\\\\";
				break;
			case '\b':
				result += "\\b";
				break;
			case '\f':
				result += "\\f";
				break;
			case '\n':
				result += "\\n";
				break;
			case '\r':
				result += "\\r";
				break;
			case '\t':
				result += "\\t";
				break;
			// case '/':
			// Even though \/ is considered a legal escape in JSON, a bare
			// slash is also legal, so I see no reason to escape it.
			// (I hope I am not misunderstanding something.)
			// blep notes: actually escaping \/ may be useful in javascript to
			// avoid </
			// sequence.
			// Should add a flag to allow this compatibility mode and prevent
			// this
			// sequence from occurring.
			default:
				if ((isControlCharacter(*c)) || (*c == 0)) {
					std::ostringstream oss;
					oss << "\\u" << std::hex << std::uppercase
						<< std::setfill('0') << std::setw(4)
						<< static_cast<int>(*c);
					result += oss.str();
				} else {
					result += *c;
				}
				break;
		}
	}
	result += "\"";
	return result;
}
}
}
