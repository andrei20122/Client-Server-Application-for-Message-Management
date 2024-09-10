#ifndef _UTILS_H
#define _UTILS_H 1

#define BUFFLEN 1600
#define STDIN 0
#define TOPIC_SIZE 50
#define MAX_CLIENTS 30

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)


#endif