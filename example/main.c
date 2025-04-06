#include "lxxml.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	char* data;
	if (argc <= 1) {
		data = "<person name=\"Alice Campbell\"><age>30</age></person>";
	} else {
		FILE* file = fopen(argv[1], "rb");
		if (!file) return 1;
		
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		rewind(file);

		data = (char*)malloc(size+1);
		if (!data) {
			fclose(file);
			return 1;
		}

		fread(data, 1, size, file);
		fclose(file);

		data[size] = '\0';
	}

	lxml_node* root = 0;
	lxml_parse_node(data, &root);
	lxml_print_node(root);

	if (argc > 1) {
		free(data);
	}
}
