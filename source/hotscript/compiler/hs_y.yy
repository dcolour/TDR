%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "hotpot/hp_platform.h"
#include "hotpot/hp_error.h"


#include "xml_parser.h"

#define YYERROR_VERBOSE

void yyerror(const YYLTYPE *yylloc, yyscan_t *yyscan, char *s, ...) 
{
	va_list ap;
	va_start(ap, s);

	if(yylloc->first_line)
	{
		fprintf(stderr, "%d.%d-%d.%d: error: ", yylloc->first_line, yylloc->first_column, yylloc->last_line, yylloc->last_column);
	}
	vfprintf(stderr, s, ap);
	printf("\n");
	va_end(ap);

	return;
ERROR_RET:
	return;
}
//这里的代码生成在自身的文件中
#define GET_XML_PARSER XML_PARSER *xp = HP_CONTAINER_OF(arg, XML_PARSER, scanner);
%}
%locations

%code requires
{
#include "globals.h"

#define YYMALLOC
#define YYFREE
#define YYLEX_PARAM *arg

}//code requires end

%define api.pure
%parse-param { yyscan_t *arg }
%pure_parser

%token tok_begin_tag 
%token tok_end_tag
%token tok_content


%union
{
	char content[MAX_TOKEN_LENGTH];
	char begin_tag[MAX_TOKEN_LENGTH];
	char end_tag[MAX_TOKEN_LENGTH];
}

%type<content>							tok_content
%type<begin_tag>						tok_begin_tag
%type<end_tag>							tok_end_tag

%start XML

%%

XML :
	Element
	{
		GET_XML_PARSER

		xp->element.first_child_index = xp->stack[xp->stack_num].first_element_index;
		xp->element.text[0] = 0;

		xp->tree.element_list[xp->tree.element_list_num] = xp->element;
		++(xp->tree.element_list_num);
	}


Content :
	Content Element
	{
		GET_XML_PARSER;
		xp->tree.element_list[xp->tree.element_list_num] = xp->element;

		xp->tree.element_list[xp->stack[xp->stack_num - 1].last_element_index].next_sibling_index = xp->tree.element_list_num;
		xp->stack[xp->stack_num - 1].last_element_index = xp->tree.element_list_num;

		++(xp->tree.element_list_num);

		printf("append\n");
		printf("\t%s\n",xp->element.name);
	}
|	Element
	{
		GET_XML_PARSER;
		
		xp->tree.element_list[xp->tree.element_list_num] = xp->element;

		xp->stack[xp->stack_num].first_element_index = xp->tree.element_list_num;
		xp->stack[xp->stack_num].last_element_index = xp->tree.element_list_num;

		++(xp->tree.element_list_num);
		++(xp->stack_num);

		printf("push\n");
		printf("\t%s\n",xp->element.name);
	}


Element :
	tok_begin_tag tok_content tok_end_tag
	{
		GET_XML_PARSER

		if(strcmp($1, $3) != 0)
		{
			GET_XML_PARSER
			xp->result = E_HP_ERROR;
			return -1;
		}

		strncpy(xp->element.name, $1, MAX_TOKEN_LENGTH);		
		strncpy(xp->element.text, $2, MAX_TOKEN_LENGTH);
		xp->element.first_child_index = -1;
		xp->element.next_sibling_index = -1;
	}
|
	tok_begin_tag Content tok_end_tag
	{
		GET_XML_PARSER
		strncpy(xp->element.name, $1, MAX_TOKEN_LENGTH);


		xp->element.text[0] = 0;
		xp->element.next_sibling_index = -1;
		xp->element.first_child_index = xp->stack[xp->stack_num - 1].first_element_index;

		--(xp->stack_num);
		printf("pop: %s\n", $1);
	}
|
	tok_begin_tag tok_end_tag
	{
		GET_XML_PARSER
		strncpy(xp->element.name, $1, MAX_TOKEN_LENGTH);
		xp->element.first_child_index = -1;
		xp->element.next_sibling_index = -1;
		xp->element.text[0] = 0;
	}

%%

