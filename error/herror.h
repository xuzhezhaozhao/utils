/*******************************************************
 @Author: zhezhaoxu (摘录自 Unix网络编程卷I)
 @Created Time : 2017年08月21日 星期一 23时04分02秒
 @File Name: herror.h
 @Description:
 ******************************************************/

#include <string>
#include <unistd.h>

namespace utils {
namespace herror {

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

std::string err_str(const char *, ...);

// 在出错信息中加入文件名和行号
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define err_atstr(msg) err_str("%s: %s", AT, msg)

void print_stack();
void print_stack_fd(int fd = STDERR_FILENO);
std::string get_stackinfo();

}
}
