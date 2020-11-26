/*
 * Copyright (c) 2020 euhat.com
 */

#ifndef _EUHAT_INI_OP_H_
#define _EUHAT_INI_OP_H_

#ifdef __cplusplus
extern "C"
{
#endif

const char *iniReadStringC(char **base, const char *path, const char *section, const char *key, const char *defaultStr);

int iniWriteStringC(const char *path, const char *section, const char *key, const char *value);

#ifdef __cplusplus
}
#endif

#endif
