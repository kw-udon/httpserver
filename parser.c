#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


//remove CR-LF
void chomp (char* c) {
	int len = strlen(c);
	if (len >= 2 && (c[len-2] == '\r' && c[len-1] == '\n')) {
		c[len-2] = c[len-1] = '\0';
	}else if (len >= 1 && (c[len-1] == '\r' || c[len-1] == '\n')) {
		c[len-1] = '\0';
	}
	return;
}

void errorReq (reqinfo* r, int num) {
	r -> error = num;
	switch (num) {
	case 11:
		fprintf (stderr, "Header doesn't start from \"Host:\"\n");
		break;
	case 100:
		fprintf(stderr, "method is too long\n");
		break;
	default:
		fprintf(stderr, "Error\n");
	}
	return;
}


/*
	m = "GET /index.php HTTP/1.1"
 */
void parseMethod (reqinfo* r, char* m){
	char *u;
	char *v;
	int i;
	r->error = 0;
	chomp(m);
	if (strlen(m) > 256) {
		errorReq(r, 100);
		return;
	}
	while(m[i] != ' ') {
		i++;
		if (m[i] == '\0') {
			errorReq(r, 1);
			return;
		}
	}
	u = m+i+1;
	while(*u != ' '&& *u != '\0') u++;
	m[i] = '\0';
	if (strcmp(m, "GET") == 0) {
		r -> method = 0;
	} else if (strcmp(m, "HEAD") == 0) {
		r -> method = 1;
	} else {
		errorReq(r, 2);
		return;
	}

	i = 0;
	while(u[i] != ' ') {
		if (u[i] == '\0') {
			errorReq(r, 3);
			return;
		}
		i++;
	}
	v = u+i+1;
	while(*v != ' '&& *v != '\0') v++;
	u[i] = '\0';
	r -> uri = (char*) malloc(strlen(u) * sizeof(char));
	strcpy(r -> uri, u);


	i = 0;
	while(v[i] != ' ') i++;
	v[i] = '\0';
	if (strcmp(v, "HTTP/1.0") == 0) {
		r -> version = 0;
	} else if (strcmp(v, "HTTP/1.1") == 0) {
		r -> version = 1;
	} else {
		errorReq(r, 4);
		return;
	}
	return;
}

void parseHost (reqinfo* r, char* m){
	char* h;
	int i=0;
	while (m[i] != ' ') {
		if (m[i] == '\0') {
			errorReq(r, 10);
			return;
		}
		i++;
	}
	h=m+i+1;
	m[i] = '\0';
	if (strcmp(m, "Host:") == 0) {
		errorReq(r, 11);
		return;
	}
	r -> host = (char*) malloc(strlen(h) * sizeof(char));
	strcpy(r -> host, h);
	return;
}
