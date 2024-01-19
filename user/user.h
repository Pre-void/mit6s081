struct stat;
struct rtcdate;

/**提前声明**/
struct sysinfo;

/**定user.h 提供了用户程序与底层操作系统之间的接口，
 * 通过这些声明，用户程序可以使用系统调用和库函数来实现各种功能。
 * 用户只能使用在user.h里声明的系统调用**/

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
/**新添加的系统调用trace,函数原型签名**/
int trace(int);
/**新添加的系统调用sysinfo,函数原型签名**/
int sysinfo(struct sysinfo *);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
