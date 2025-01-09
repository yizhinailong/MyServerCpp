#include "util.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 根据条件触发错误处理并退出程序
 *
 * 当给定的条件为真时，该函数会打印错误消息并终止程序执行
 * 这通常用于检查程序中的错误条件，例如参数验证失败或资源分配失败
 *
 * @param condition 如果为真，则触发错误处理
 * @param err_msg 错误消息，用于描述错误条件
 */
void errif(bool condition, const char* err_msg) {
    if (condition) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}
