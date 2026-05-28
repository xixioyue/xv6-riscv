// System calls available to user programs.
void print(const char *);
int fork(void);
void exit(int) __attribute__((noreturn));
int wait(int *);
int exec(int);
int getpid(void);
