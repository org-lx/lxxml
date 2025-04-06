#pragma once

typedef struct lxml_attrib {
	char* name;
	char* value;
	struct lxml_attrib* next;
} lxml_attrib;

typedef struct lxml_node {
	char* name;
	char* text;
	lxml_attrib* attributes;
	struct lxml_node* children;
	struct lxml_node* next_sibling;
} lxml_node;

char* lxml_parse_node(char* data, lxml_node** node);


#ifndef LXML_NO_STDIO
// for debugging
void lxml_print_node(lxml_node* node);
#endif
