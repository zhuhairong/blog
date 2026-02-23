#ifndef C_UTILS_INI_H
#define C_UTILS_INI_H

#include <stdbool.h>

typedef struct ini_s ini_t;

// 加载与销毁
ini_t* ini_load(const char *filepath);
ini_t* ini_create(void);
void   ini_free(ini_t *ini);

// 读取配置
const char* ini_get(const ini_t *ini, const char *section, const char *key);
int         ini_get_int(const ini_t *ini, const char *section, const char *key, int default_val);
bool        ini_get_bool(const ini_t *ini, const char *section, const char *key, bool default_val);
double      ini_get_double(const ini_t *ini, const char *section, const char *key, double default_val);

// 设置配置
bool        ini_set(ini_t *ini, const char *section, const char *key, const char *value);
bool        ini_set_int(ini_t *ini, const char *section, const char *key, int value);
bool        ini_set_bool(ini_t *ini, const char *section, const char *key, bool value);
bool        ini_set_double(ini_t *ini, const char *section, const char *key, double value);

// 删除配置项
bool        ini_delete(ini_t *ini, const char *section, const char *key);
bool        ini_delete_section(ini_t *ini, const char *section);

// 保存到文件
bool        ini_save(const ini_t *ini, const char *filepath);

// 遍历支持
typedef void (*ini_visit_fn)(const char *section, const char *key, const char *value, void *user_data);
void        ini_foreach(const ini_t *ini, ini_visit_fn visit, void *user_data);
void        ini_foreach_section(const ini_t *ini, const char *section, ini_visit_fn visit, void *user_data);

// 属性获取
bool        ini_has_section(const ini_t *ini, const char *section);
bool        ini_has_key(const ini_t *ini, const char *section, const char *key);

#endif // C_UTILS_INI_H
