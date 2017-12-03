#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_MAX_SIZE 10

void buffer_init(void);
void buffer_insert(int number);
int buffer_extract(void);
void process(int number);

#endif /* BUFFER_H */
