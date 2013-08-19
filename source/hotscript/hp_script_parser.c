#include "hotscript/hp_script_parser.h"
#include "hotprotocol/hp_abstract_reader.h"
#include "hotpot/hp_error_code.h"
#include "script_l.h"
#include "script_y.h"
#include <string.h>
#include "hotscript/hp_script_lex.h"
#include "hotpot/hp_platform.h"


hpint32 hotscript_do_text(SCRIPT_PARSER *self, const YYLTYPE *yylloc, const SP_NODE *text)
{

	HotOp *op = hotoparr_get_next_op(&self->hotoparr);
	op->instruct = HOT_ECHO;
	op->arg.echo_arg.bytes = text->bytes;
	return E_HP_NOERROR;
}

hpint32 hotscript_do_literal(SCRIPT_PARSER *self, const YYLTYPE *yylloc, const SP_NODE *text)
{
	HotOp *op = hotoparr_get_next_op(&self->hotoparr);
	op->instruct = HOT_ECHO_LITERAL;
	op->arg.echo_arg.bytes = text->bytes;
	return E_HP_NOERROR;
}

hpint32 hotscript_do_vector_begin(SCRIPT_PARSER *self, const YYLTYPE *yylloc, SP_NODE *identifier)
{
	HotOp *op = NULL;
	
	op = hotoparr_get_next_op(&self->hotoparr);
	op->instruct = HOT_VECTOR_BEGIN;

	identifier->vector_begin_index = op->lineno;

	self->stack[(self->stack_num)++] = E_SP_ARRAY;

	return E_HP_NOERROR;
}

hpint32 hotscript_do_vector_end(SCRIPT_PARSER *self, const YYLTYPE *yylloc, SP_NODE *identifier)
{
	HotOp *op = NULL;
	op = hotoparr_get_next_op(&self->hotoparr);
	op->instruct = HOT_VECTOR_END;

	self->hotoparr.oparr[identifier->vector_begin_index].arg.vector_begin_arg.failed_jmp_lineno = hotoparr_get_next_op_number(&self->hotoparr);

	--(self->stack_num);
	return E_HP_NOERROR;
}

hpint32 hotscript_do_field_begin(SCRIPT_PARSER *self, const YYLTYPE *yylloc, SP_NODE *identifier)
{
	HotOp *op = NULL;

	if(identifier->token == tok_identifier)
	{
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_FIELD_BEGIN;
		op->arg.field_begin_arg.name = identifier->bytes;

		identifier->field_begin_index = op->lineno;
	}	
	else if(identifier->token == tok_integer)
	{
		if((self->stack_num <= 0) || (self->stack[self->stack_num - 1] != E_SP_ARRAY))
		{
			goto ERROR_RET;
		}
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_SET_INDEX;
		op->arg.vector_set_index_arg.index = identifier->ui32;

		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_ITEM_BEGIN;
		identifier->vector_item_begin_index = op->lineno;	
	}
	else if(identifier->token == tok_auto_integer)
	{
		if((self->stack_num <= 0) || (self->stack[self->stack_num - 1] != E_SP_ARRAY))
		{
			goto ERROR_RET;
		}

		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_SET_INDEX;
		if(identifier->token == tok_auto_integer)
		{
			op->arg.vector_set_index_arg.index = identifier->ui32;
		}
		else
		{
			op->arg.vector_set_index_arg.index = 0;
		}
		
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_ITEM_BEGIN;
		identifier->vector_item_begin_index = op->lineno;		
	}
	
	self->stack[(self->stack_num)++] = E_SP_OBJECT;
	return E_HP_NOERROR;
ERROR_RET:
	yyscripterror(yylloc, &self->scanner_stack, NULL);
	return E_HP_ERROR;
}

hpint32 hotscript_do_field_end(SCRIPT_PARSER *self, const YYLTYPE *yylloc, SP_NODE *identifier)
{

	HotOp *op = NULL;
	if(identifier->token == tok_identifier)
	{
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_FIELD_END;
		self->hotoparr.oparr[identifier->field_begin_index].arg.field_begin_arg.lineno_after_field_end = hotoparr_get_next_op_number(&self->hotoparr);
	}
	else if(identifier->token == tok_integer)
	{
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_ITEM_END;

		self->hotoparr.oparr[identifier->vector_item_begin_index].arg.vector_item_begin_arg.failed_jmp_lineno = hotoparr_get_next_op_number(&self->hotoparr);
	}
	else if(identifier->token == tok_auto_integer)
	{
		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_ITEM_END;

		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_VECTOR_INC_INDEX;

		op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_JMP;
		op->arg.jmp_arg.lineno = self->hotoparr.oparr[identifier->vector_item_begin_index].lineno;


		self->hotoparr.oparr[identifier->vector_item_begin_index].arg.vector_item_begin_arg.failed_jmp_lineno = hotoparr_get_next_op_number(&self->hotoparr);
	}

	--self->stack_num;
	return E_HP_NOERROR;
}


hpint32 hotscript_do_echo_field(SCRIPT_PARSER *self, const YYLTYPE *yylloc, SP_NODE *identifier)
{
	if(identifier->token != tok_call_identifier)
	{
		HotOp *op = hotoparr_get_next_op(&self->hotoparr);
		op->instruct = HOT_ECHO_FIELD;
	}
	return E_HP_NOERROR;
}

void yyscripterror(const YYLTYPE *yylloc, SCANNER_STACK *ss, char *s, ...) 
{
	va_list ap;

	va_start(ap, s);
	scanner_stack_errorap(ss, yylloc, E_HP_SYNTAX_ERROR, s, ap);
	va_end(ap);

	return;
}

static hpint32 get_token_yylval(SCRIPT_PARSER *sp, int token, YYSTYPE * yylval)
{
	return E_HP_NOERROR;
}


extern hpint32 script_lex_scan(SCANNER *self, YYLTYPE *yylloc, YYSTYPE * yylval);
int yyscriptlex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , SCANNER_STACK *ss)
{
	SCRIPT_PARSER *sp = HP_CONTAINER_OF(ss, SCRIPT_PARSER, scanner_stack);
	int ret;

	for(;;)
	{
		SCANNER *scanner = scanner_stack_get_scanner(&sp->scanner_stack);		
		
		yylloc_param->file_name = scanner->file_name;
		ret = script_lex_scan(scanner, yylloc_param, yylval_param);
		yylloc_param->last_line = scanner->yylineno;
		yylloc_param->last_column = scanner->yycolumn;
		if(ret == 0)
		{
			if(scanner_stack_get_num(&sp->scanner_stack) <= 1)
			{
				break;
			}
			scanner_stack_pop(&sp->scanner_stack);
		}
		else if(ret == tok_import)
		{
			char file_name[1024];
			size_t len = 0;
			while(scanner->yy_cursor < scanner->yy_limit)
			{
				if(*scanner->yy_cursor == ';')
				{
					break;
				}
				else if((*scanner->yy_cursor == '\n') || (*scanner->yy_cursor == '\t') || (*scanner->yy_cursor == ' '))
				{
					++(scanner->yy_cursor);
				}
				else
				{
					file_name[len++] = *scanner->yy_cursor;
					++(scanner->yy_cursor);
				}
			}

			file_name[len] = 0;
			if(scanner_stack_push_file(&sp->scanner_stack, file_name, yycINITIAL) != E_HP_NOERROR)
			{
				scanner_stack_error(&sp->scanner_stack, yylloc_param, E_HP_ERROR);
				return 0;
			}
		}
		else
		{
			if(get_token_yylval(sp, ret, yylval_param) != E_HP_NOERROR)
			{
				scanner_stack_error(&sp->scanner_stack, yylloc_param, E_HP_ERROR);
				ret = -1;
				break;
			}
			break;
		}
	}


	return ret;
}

extern int yyscriptparse (SCANNER_STACK *sp);
hpint32 script_parser(SCRIPT_PARSER *self, const char* file_name, HPAbstractReader *reader, void *user_data, vm_user_putc uputc, const char* root_dir)
{
	hpint32 ret;

	self->stack_num = 0;

	scanner_stack_init(&self->scanner_stack, root_dir);


	if(scanner_stack_push_file(&self->scanner_stack, file_name, yycINITIAL) != E_HP_NOERROR)
	{
		scanner_stack_error(&self->scanner_stack, NULL, E_HP_ERROR);
		goto ERROR_RET;
	}
	

	hotoparr_init(&self->hotoparr);

	self->reader = reader;
	
	ret = yyscriptparse(&self->scanner_stack);

	if(self->scanner_stack.result_num != 0)
	{
		goto ERROR_RET;
	}

	if(scanner_stack_pop(&self->scanner_stack) != E_HP_NOERROR)
	{
		goto ERROR_RET;
	}

	if(hotvm_execute(&self->hotvm, &self->hotoparr, self->reader, user_data, uputc) != E_HP_NOERROR)
	{
		scanner_stack_error(&self->scanner_stack, NULL, E_HP_ERROR);
	}

	return E_HP_NOERROR;
ERROR_RET:
	return E_HP_ERROR;
}
