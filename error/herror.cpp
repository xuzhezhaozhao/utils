/*******************************************************
 @Author: zhezhaoxu (摘录自 Unix网络编程卷I)
 @Created Time : 2017年08月21日 星期一 23时27分38秒
 @File Name: herror.cpp
 @Description:
 ******************************************************/

#include "herror.h"

#include <errno.h>
#include <stdarg.h> /* ANSI C header file */
#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <string.h>
#include <syslog.h> /* for syslog() */
#include <execinfo.h>
#include <unistd.h> /* for STDERR_FILENO */


namespace utils {
namespace herror {

/******************* unp ***********************************/

static void err_doit(int, int, const char *, va_list);

static const int MAXLINE = 4096; /* max text line length */
static const int MAX_PRINT_STACK_SIZE = 4096;

/* Nonfatal error related to system call
 * Print message and return */
void err_ret(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to system call
 * Print message and terminate */
void err_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error related to system call
 * Print message, dump core, and terminate */
void err_dump(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort(); /* dump core and terminate */
	exit(1); /* shouldn't get here */
}

/* Nonfatal error unrelated to system call
 * Print message and return */
void err_msg(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to system call
 * Print message and terminate */
void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */
static void err_doit(int errnoflag, int /* level unused*/, const char *fmt, va_list ap) {
	int errno_save, n;
	char buf[MAXLINE + 1];
	errno_save = errno;				  /* value caller might want printed */
	vsnprintf(buf, MAXLINE, fmt, ap); /* safe */
	n = (int)strlen(buf);
	if (errnoflag) {
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	}
	strcat(buf, "\n");

	fflush(stdout); /* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(stderr);
	return;
}

/******************* unp end *******************************/

std::string err_str(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char buf[MAXLINE + 1];
	int errno_save, n;
	errno_save = errno;				  /* value caller might want printed */
	vsnprintf(buf, MAXLINE, fmt, ap); /* safe */
	va_end(ap);
	n = (int)strlen(buf);
	snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	return std::string(buf);
}

void print_stack() {
	void *buffer[MAX_PRINT_STACK_SIZE + 1];
	int nptrs = backtrace(buffer, MAX_PRINT_STACK_SIZE);
	char **symbol = backtrace_symbols(buffer, nptrs);
	if (!symbol) {
		perror("backtrace_symbols");
		return;
	}
	// don't print self
	for (int i = 1; i < nptrs; ++i) {
		printf("%s\n", *(symbol + i));
	}
	free(symbol);
}

void print_stack_fd(int fd) {
	void *buffer[MAX_PRINT_STACK_SIZE + 1];
	int nptrs = backtrace(buffer, MAX_PRINT_STACK_SIZE);
	// don't print self
	backtrace_symbols_fd(buffer+1, nptrs, fd);
}

std::string get_stackinfo(){
	std::string stackinfo;
	void *buffer[MAX_PRINT_STACK_SIZE + 1];
	int nptrs = backtrace(buffer, MAX_PRINT_STACK_SIZE);
	char **symbol = backtrace_symbols(buffer, nptrs);
	if (!symbol) {
		return std::string("get_stackinfo failed, backtrace_symbols: ")
			+ std::string(strerror(errno));
	}
	// don't print self
	for (int i = 1; i < nptrs; ++i) {
		stackinfo += *(symbol + i);
		stackinfo += '\n';
	}
	free(symbol);
	return stackinfo;
}


}  // namespace herror
}  // namespace utils
