/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年09月05日 星期二 23时16分56秒
 @File Name: unp.h
 @Description:
 ******************************************************/

#ifndef UTILS_UNP_H_
#define UTILS_UNP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void Sigfunc(int); /* for signal handlers */

// 成功返回之前处理函数; 出错返回 SIG_ERR
extern Sigfunc* unp_signal(int signo, Sigfunc* func);

#ifdef __cplusplus
}
#endif

#endif /* ifndef UTILS_UNP_H_ */
