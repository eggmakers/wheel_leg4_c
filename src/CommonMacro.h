#ifndef __COMMONMACRO_H
#define __COMMONMACRO_H

/*********************************/
/*          通用宏定义库         */
/*  提升代码的精简程度以及可读怿 */
/*    Designed by _VIFEXTech     */
/*********************************/

// Finish  2019-03-21 v1.0 添加注释
// Upgrade 2019-03-21 v1.1 添加__ValueCloseTo
// Upgrade 2019-05-16 v1.2 添加__ExecuteFuncWithTimeout
// Upgrade 2019-05-16 v1.3 添加__ValueStep
// Upgrade 2019-09-25 v1.4 添加__ExecuteOnce
// Upgrade 2020-01-27 v1.5 添加__SemaphoreTake
// Upgrade 2020-03-10 v1.6 添加__ValuePlus

/**
 * @brief  变量监视器，当变量改变时触发一个事仿
 * @param  now:被监控的变量(整形)
 * @param  func:事件触发回调函数(可以是赋值等其他语句)
 * @retval 旿
 */
#define __ValueMonitor(now, func) \
  do                              \
  {                               \
    static int last = (now);      \
    if (last != (now))            \
      func, last = (now);         \
  } while (0)
#define __EventMonitor(now, func) __ValueMonitor((now), func) // 兼容旧代砿

/**
 * @brief  让一个变量以设计的步近接近指定倿
 * @param  src:被控变量
 * @param  dest:被接近的倿
 * @param  step:步长
 * @retval 旿
 */
#define __ValueCloseTo(src, dest, step) \
  do                                    \
  {                                     \
    if ((src) < (dest))                 \
      (src) += (step);                  \
    else if ((src) > (dest))            \
      (src) -= (step);                  \
  } while (0)

/**
 * @brief  让一个变量增加或者减去一个值，在大于等于最大值后仿0开始，低于0后从最大值开姿
 * @param  src:被控变量
 * @param  step:增加或者减少的倿
 * @param  max:最大倿
 * @retval 旿
 */
#define __ValueStep(src, step, max) ((src) = (((step) >= 0) ? (((src) + (step)) % (max)) : (((src) + (max) + (step)) % (max))))

/**
 * @brief  让一个变量增加或者减去一个值，在大于最大值后从最小值开始，小于最小值后从最大值开姿
 * @param  src:被控变量
 * @param  plus:增加的倿
 * @param  min:最小倿
 * @param  max:最大倿
 * @retval 旿
 */
#define __ValuePlus(src, plus, min, max) \
  do                                     \
  {                                      \
    int __value_temp = (src);            \
    __value_temp += (plus);              \
    if (__value_temp < (min))            \
      __value_temp = (max);              \
    else if (__value_temp > (max))       \
      __value_temp = (min);              \
    (src) = __value_temp;                \
  } while (0)

/**
 * @brief  非阻塞式间隔指定时间执行一个函敿
 * @param  func:被执行函敿(可以是赋值等其他语句)
 * @param  time:设定间隔时间(ms)
 * @retval 旿
 */
#define __IntervalExecute(func, time)  \
  do                                   \
  {                                    \
    static unsigned long lasttime = 0; \
    if (millis() - lasttime >= (time)) \
      func, lasttime = millis();       \
  } while (0)

/**
 * @brief  将一个函数重复调用指定次敿
 * @param  func:被调用函敿(可以是赋值等其他语句)
 * @param  n:重复调用次数
 * @retval 旿
 */
#define __LoopExecute(func, n)            \
  for (unsigned long i = 0; i < (n); i++) \
  func

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

/**
 * @brief  将一个值限制在一个范围内
 * @param  x:被限制的倿(任意类型)
 * @param  min:最小倿(任意类型)
 * @param  max:最大倿(任意类型)
 * @retval 旿
 */
#define __LimitValue(x, min, max) ((x) = constrain((x), (min), (max)))

/**
 * @brief  将一个值的变化区间线性映射到另一个区闿
 * @param  x:被映射的倿(任意类型)
 * @param  in_min:被映射的值的最小倿
 * @param  in_max:被映射的值的最大倿
 * @param  out_min:被映射的值的最小倿
 * @param  out_max:被映射的值的最大倿
 * @retval 映射值输凿
 */
#define __Map(x, in_min, in_max, out_min, out_max) \
  (((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

/**
 * @brief  获取一个数组的元素个数
 * @param  arr:数组吿(任意类型)
 * @retval 这个数组的元素个敿
 */
#define __Sizeof(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief  将一个值强制按指定类型解释，常用于结构体拷贿
 * @param  type:类型吿(任意类型)
 * @param  data:被解释的数据(任意类型)
 * @retval 解释输出
 */
#define __TypeExplain(type, data) (*((type *)(&(data))))

/**
 * @brief  执行一个函数在不超时的情况下直到函数的返回值为指定倿
 * @param  func:被调用函敿
 * @param  n:希望函数的返回倿
 * @param  timeout:超时时间
 * @param  flag:外部提供变量，用于检查是否超旿
 * @retval 旿
 */
#define __ExecuteFuncWithTimeout(func, n, timeout, flag) \
  do                                                     \
  {                                                      \
    volatile unsigned long start = millis();             \
    (flag) = false;                                      \
    while (millis() - start < (timeout))                 \
    {                                                    \
      if (func == (n))                                   \
      {                                                  \
        (flag) = true;                                   \
        break;                                           \
      }                                                  \
    }                                                    \
  } while (0)

/**
 * @brief  函数只执行一次，常用于初始化
 * @param  func:被调用函敿(也可以是赋值等其他语句)
 * @retval 旿
 */
#define __ExecuteOnce(func)     \
  do                            \
  {                             \
    static bool isInit = false; \
    if (!isInit)                \
    {                           \
      func, isInit = true;      \
    }                           \
  } while (0)

/**
 * @brief  获取信号量，当sem为true时执行一次func
 * @param  sem:信号釿(bool类型)
 * @param  func:被调用函敿(也可以是赋值等其他语句)
 * @retval 旿
 */
#define __SemaphoreTake(sem, func) \
  do                               \
  {                                \
    if ((sem))                     \
    {                              \
      func, (sem) = false;         \
    }                              \
  } while (0)

#endif
