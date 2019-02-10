#include <stdio.h>
#include <time.h>
void getNowTime(char *str)
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    sprintf(str, "%04d%02d%02d%02d%02d%02d", nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, 
            nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
}

