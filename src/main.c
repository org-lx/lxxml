#include <stdlib.h>
#include <string.h>

#include "lxxml.h"

char* skip_whitespace(char* s) {
	while (*s && (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r')) s++;
	return s;
}

// xml declaration isn't used yet, so we just skip it
char* skip_xml_declaration(char* s) {
	s = skip_whitespace(s);
	if (s[0] == '<' && s[1] == '?') {
		s += 2;
		while (*s && !(s[0] == '?' && s[1] == '>')) s++;
		if (*s) s+= 2;
	}
	return s;
}

char* skip_comments(char* s) {
	s = skip_whitespace(s);
	if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-') {
		s += 4;
		while (*s && !(s[0] == '-' && s[1] == '-' && s[2] == '>')) s++;
		if (*s) s+= 3;
	}
	return s;
}

char* skip_misc(char* s) {
	s = skip_whitespace(s);
	s = skip_xml_declaration(s);
	while (1) {
		char* old = s;
		s = skip_comments(s);
		s = skip_whitespace(s);
		if (s == old) break;
	}
	return s;
}

char* strndup(char *str, int chars) {
    char *buffer;
    int n;

    buffer = (char*)malloc(chars +1);
    if (buffer) {
        for (n = 0; ((n < chars) && (str[n] != 0)); n++) buffer[n] = str[n];
        buffer[n] = 0;
    }

    return buffer;
}

char* parse_tag_name(char* s, char** out_name) {
	char* start = s;
	while (*s && (*s != ' ' && *s != '>' && *s != '/')) s++;
	int len = s - start;
	*out_name = strndup(start, len);
	return s;
}

char* parse_attributes(char* s, lxml_attrib** attr_list) {
	lxml_attrib* head = NULL;
	lxml_attrib* tail = NULL;

	while (*s && *s != '>' && *s != '/') {
		s = skip_whitespace(s);
		char* key_start = s;
		while (*s && *s != '=') s++;
		if (*s != '=') break;

		int key_len = s - key_start;
		char* key = strndup(key_start, key_len);
		s++;

		if (*s == '"') s++;
		char* val_start = s;
		while(*s && *s != '"') s++;
		int val_len = s-val_start;
		char* val = strndup(val_start, val_len);
		if (*s == '"') s++;

		lxml_attrib* attr = malloc(sizeof(lxml_attrib));
		attr->name = key;
		attr->value = val;
		attr->next = NULL;

		if (!head) head = tail = attr;
		else tail = tail->next = attr;
	}

	*attr_list = head;
	return s;
}

char* lxml_parse_node(char* s, lxml_node** out_node) {
	s = skip_misc(s);
	if (*s != '<') return NULL;
	s++;

	char* name = NULL;
	s = parse_tag_name(s, &name);
	lxml_attrib* attrs = NULL;
	s = parse_attributes(s, &attrs);
	s = skip_whitespace(s);

	int self_closing = 0;
	if (*s == '/' && *(s+1) == '>') {
		self_closing = 1;
		s += 2;
	} else if (*s == '>') {
		s++;
	}

	lxml_node* node = malloc(sizeof(lxml_node));
	node->name = name;
	node->text = NULL;
	node->attributes = attrs;
	node->children = NULL;
	node->next_sibling = NULL;

	if (!self_closing) {
		lxml_node* first_child = NULL;
		lxml_node* last_child = NULL;

		while (*s) {
			s = skip_misc(s);
			if (*s == '<') {
				if (*(s + 1) == '/') break;
		
				lxml_node* child = NULL;
				s = lxml_parse_node(s, &child);
				if (child) {
					if (!first_child) {
						first_child = last_child = child;
					} else {
						last_child->next_sibling = child;
						last_child = child;
					}
				}
			} else {
				char* text_start = s;
				while (*s && *s != '<') s++;
				int text_len = s - text_start;
				if (text_len > 0) {
					char* text = strndup(text_start, text_len);
					if (node->text) {
						char* old = node->text;
						int old_len = strlen(old);
						node->text = malloc(old_len + text_len + 1);
						memcpy(node->text, old, old_len);
						memcpy(node->text + old_len, text, text_len);
						node->text[old_len + text_len] = 0;
						free(old);
						free(text);
					} else {
						node->text = text;
					}
				}
			}
		}
		node->children = first_child;

		if (*s == '<' && *(s+1) == '/') {
			s += 2;
			while (*s && *s != '>') s++;
			if (*s == '>') s++;
		}
	}

	*out_node = node;
	return s;
}

#ifndef LXML_NO_STDIO
#include <stdio.h>

void print_node(lxml_node* node, int depth) {
    for (int i = 0; i < depth; i++) printf("\t");
    printf("<%s", node->name);
    for (lxml_attrib* a = node->attributes; a; a = a->next)
        printf("\t%s=\"%s\"", a->name, a->value);
    printf(">\n");
    if (node->text) printf("\t\t\t%s", node->text);
    printf("\n");

    for (lxml_node* c = node->children; c; c = c->next_sibling)
        print_node(c, depth + 1);

    for (int i = 0; i < depth; i++) printf("\t");
    printf("</%s>\n", node->name);
}

void lxml_print_node(lxml_node* node) {
	print_node(node, 0);
}
#endif
