#include "generate/tlibc_reader.h"
#include "generate/tlibc_reader_header.h"
#include "generator.h"
#include "version.h"
#include "symbols.h"

#include <stdio.h>
#include <string.h>



static TD_ERROR_CODE on_document_begin(GENERATOR *super, const char *file_name)
{
	char header[MAX_PACKAGE_NAME_LENGTH];	

	generator_open(super, file_name, TLIBC_READER_SUFFIX);

	generator_print(super, "/**\n");
    generator_print(super, " * Autogenerated by TData Compiler (%s)\n", TDATA_VERSION);
    generator_print(super, " *\n");
    generator_print(super, " * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING\n");
    generator_print(super, " *  @generated\n");
    generator_print(super, " */\n");
	generator_print(super, "\n");

	
	//包含header的头文件
	strncpy(header, file_name, MAX_PACKAGE_NAME_LENGTH);
	header[MAX_PACKAGE_NAME_LENGTH - 1] = 0;
	generator_replace_extension(header, MAX_PACKAGE_NAME_LENGTH, TLIBC_READER_HEADER_SUFFIX);
	generator_print(super, "#include \"%s\"\n", header);

	generator_print(super, "#include \"platform/tlibc_platform.h\"\n");

	generator_print(super, "\n");
	generator_print(super, "\n");
	return E_TD_NOERROR;
}

static TD_ERROR_CODE on_document_end(GENERATOR *super, const char *file_name)
{	
	generator_print(super, "\n");

	generator_close(super);
	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_import(TLIBC_READER_GENERATOR *self, const ST_Import *de_import)
{
	char name[MAX_PACKAGE_NAME_LENGTH];	
	strncpy(name, de_import->package_name, MAX_PACKAGE_NAME_LENGTH);
	name[MAX_PACKAGE_NAME_LENGTH - 1] = 0;
	generator_replace_extension(name, MAX_PACKAGE_NAME_LENGTH, TLIBC_READER_HEADER_SUFFIX);
	generator_print(&self->super, "#include \"%s\"\n", name);

	return E_TD_NOERROR;
}

static void _on_enum_name(TLIBC_READER_GENERATOR *self, const ST_ENUM *de_enum)
{
	tuint32 i;

	generator_print(&self->super, "\n");
	generator_print(&self->super, "TLIBC_ERROR_CODE read_%s_name(TLIBC_ABSTRACT_READER *self, %s *data)\n", de_enum->name, de_enum->name);
	generator_print(&self->super, "{\n");
	generator_print(&self->super, "\tchar name[TLIBC_MAX_IDENTIFIER_LENGTH];\n");
	generator_print(&self->super, "\tif(read_enum_name(self, name, TLIBC_MAX_IDENTIFIER_LENGTH) != E_TLIBC_NOERROR) goto ERROR_RET;\n");

	for(i = 0; i < de_enum->enum_def_list_num; ++i)
	{
		generator_print(&self->super, "\tif(strcmp(name, \"%s\") == 0)\n", de_enum->enum_def_list[i].identifier);
		generator_print(&self->super, "\t{\n");
		generator_print(&self->super, "\t\t*data = %s;\n", de_enum->enum_def_list[i].identifier);
		generator_print(&self->super, "\t\tgoto done;\n");
		generator_print(&self->super, "\t}\n");
	}

	generator_print(&self->super, "\n");
	generator_print(&self->super, "\n");
	generator_print(&self->super, "ERROR_RET:\n");
	generator_print(&self->super, "\treturn E_TLIBC_ERROR;\n");
	generator_print(&self->super, "done:\n");
	generator_print(&self->super, "\treturn E_TLIBC_NOERROR;\n");

	generator_print(&self->super, "}\n");
}

static void _on_enum_number(TLIBC_READER_GENERATOR *self, const ST_ENUM *de_enum)
{
	generator_print(&self->super, "\n");
	generator_print(&self->super, "TLIBC_ERROR_CODE read_%s_number(TLIBC_ABSTRACT_READER *self, %s *data)\n", de_enum->name, de_enum->name);
	generator_print(&self->super, "{\n");
	generator_print(&self->super, "\treturn read_enum_number(self, (tint32*)data);\n");
	generator_print(&self->super, "}\n");
}

static TD_ERROR_CODE _on_enum(TLIBC_READER_GENERATOR *self, const ST_ENUM *de_enum)
{	
	generator_print(&self->super, "\n");
	generator_print(&self->super, "TLIBC_ERROR_CODE read_%s(TLIBC_ABSTRACT_READER *self, %s *data)\n", de_enum->name, de_enum->name);
	generator_print(&self->super, "{\n");
	generator_print(&self->super, "\tif(read_%s_name(self, data) == E_TLIBC_NOERROR) goto done;\n", de_enum->name);
	generator_print(&self->super, "\tif(read_%s_number(self, data) == E_TLIBC_NOERROR) goto done;\n", de_enum->name);
	generator_print(&self->super, "\treturn E_TLIBC_ERROR;\n");
	generator_print(&self->super, "done:\n");
	generator_print(&self->super, "\treturn E_TLIBC_NOERROR;\n");	
	generator_print(&self->super, "}\n");


	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_struct(TLIBC_READER_GENERATOR *self, const ST_STRUCT *de_struct)
{
	tuint32 i;
	generator_print(&self->super, "\n");

	generator_print(&self->super, "TLIBC_ERROR_CODE read_%s(TLIBC_ABSTRACT_READER *self, %s *data)\n", de_struct->name, de_struct->name);

	generator_print(&self->super, "{\n");
	generator_print(&self->super, "\tif(read_struct_begin(self, \"%s\") != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->name);
	for(i = 0; i < de_struct->field_list.field_list_num; ++i)
	{
		generator_print(&self->super, "\n");
		//condition
		if(de_struct->field_list.field_list[i].condition.oper != E_EO_NON)
		{
			const char *op = NULL;
			switch(de_struct->field_list.field_list[i].condition.oper)
			{
			case E_EO_AND:
				op = "&";		
				break;
			case E_EO_EQUAL:		
				op = "==";
				break;
			case E_EO_UNEQUAL:
				op = "!=";
				break;
			}
			generator_print(&self->super, "\tif(data->%s %s ", de_struct->field_list.field_list[i].condition.op0, op);
			generator_print_value(&self->super, &de_struct->field_list.field_list[i].condition.op1);
			generator_print(&self->super, ")\n");
		}
		else
		{
			generator_print(&self->super, "\t//if(TRUE)\n");
		}
		generator_print(&self->super, "\t{\n");

		generator_print(&self->super, "\t\tif(read_field_begin(self, \"%s\") != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier);
		if(de_struct->field_list.field_list[i].type.type == E_SNT_CONTAINER)
		{
			if(de_struct->field_list.field_list[i].type.ct.ct == E_CT_VECTOR)
			{
				const ST_SIMPLE_TYPE *vector_type = symbols_get_real_type(self->super.symbols, &de_struct->field_list.field_list[i].type.ct.vector_type);
				generator_print(&self->super, "\t\t{//vector begin\n");
				generator_print(&self->super, "\t\t\tuint32 i;\n");
				generator_print(&self->super, "\t\t\tif(read_vector_begin(self) != E_TLIBC_NOERROR) goto ERROR_RET;\n");
				generator_print(&self->super, "\t\t\tfor(i = 0; i < %s; ++i)\n", de_struct->field_list.field_list[i].type.ct.vector_length);
				generator_print(&self->super, "\t\t\t{\n");
				generator_print(&self->super, "\t\t\t\tif(i == data->%s_num) break;\n", de_struct->field_list.field_list[i].identifier);
				generator_print(&self->super, "\t\t\t\tif(read_vector_item_begin(self, i) != E_TLIBC_NOERROR) goto ERROR_RET;\n");
				if(vector_type->st == E_ST_STRING)
				{
					generator_print(&self->super, "\t\tif(read_tstring(self, data->%s, %s) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier, vector_type->string_length);
				}
				else
				{
					generator_print(&self->super, "\t\t\t\tif(read_");
					generator_print_simple_type(&self->super, vector_type);
					generator_print(&self->super, "(self, &data->%s[i]) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier);
				}
				generator_print(&self->super, "\t\t\t\tif(read_vector_item_end(self, i) != E_TLIBC_NOERROR) goto ERROR_RET;\n");
				generator_print(&self->super, "\t\t\t}\n");
				generator_print(&self->super, "\t\t\tif(read_vector_end(self) != E_TLIBC_NOERROR) goto ERROR_RET;\n");
				generator_print(&self->super, "\t\t}//vector end\n");
			}
		}
		else if(de_struct->field_list.field_list[i].type.type == E_SNT_SIMPLE)
		{
			if(de_struct->field_list.field_list[i].type.st.st == E_ST_STRING)
			{
				generator_print(&self->super, "\t\tif(read_tstring(self, data->%s, %s) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier, de_struct->field_list.field_list[i].type.st.string_length);
			}
			else
			{
				generator_print(&self->super, "\t\tif(read_");
				generator_print_simple_type(&self->super, &de_struct->field_list.field_list[i].type.st);
				generator_print(&self->super, "(self, &data->%s[i]) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier);
			}
		}

		generator_print(&self->super, "\t\tif(read_field_end(self, \"%s\") != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->field_list.field_list[i].identifier);

		generator_print(&self->super, "\t}\n");
	}
	generator_print(&self->super, "\n");
	generator_print(&self->super, "\tif(read_struct_end(self, \"%s\") != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_struct->name);

	generator_print(&self->super, "\n");
	generator_print(&self->super, "\treturn E_TLIBC_NOERROR;\n");
	generator_print(&self->super, "ERROR_RET:\n");
	generator_print(&self->super, "\treturn E_TLIBC_ERROR;\n");
	generator_print(&self->super, "}\n");

	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_union(TLIBC_READER_GENERATOR *self, const ST_UNION *de_union)
{
	tuint32 i;

	generator_print(&self->super, "\n");

	generator_print(&self->super, "TLIBC_ERROR_CODE read_%s(TLIBC_ABSTRACT_READER *self, %s *data, %s selector)\n", de_union->name, de_union->name, de_union->parameters.par_list[0].type.st_refer);
	generator_print(&self->super, "{\n");
	generator_print(&self->super, "\tswitch(selector)\n");
	generator_print(&self->super, "\t{\n");
	for(i = 0; i < de_union->union_field_list.union_field_list_num; ++i)
	{
		generator_print(&self->super, "\t\tcase %s:\n", de_union->union_field_list.union_field_list[i].key);
		if(de_union->union_field_list.union_field_list[i].simple_type.st == E_ST_STRING)
		{
			generator_print(&self->super, "\t\t\tif(read_tstring(self, data->%s, %s) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_union->union_field_list.union_field_list[i].name, de_union->union_field_list.union_field_list[i].simple_type.string_length);
		}
		else
		{
			generator_print(&self->super, "\t\t\tif(read_");
			generator_print_simple_type(&self->super, &de_union->union_field_list.union_field_list[i].simple_type);
			generator_print(&self->super, "(self, &data->%s[i]) != E_TLIBC_NOERROR) goto ERROR_RET;\n", de_union->union_field_list.union_field_list[i].name);
		}

		generator_print(&self->super, "\t\t\tbreak;\n");		
	}
	generator_print(&self->super, "\t\tdefault:\n");
	generator_print(&self->super, "\t\t\tgoto ERROR_RET;\n");		

	generator_print(&self->super, "\t}\n");

	generator_print(&self->super, "\n");
	generator_print(&self->super, "\treturn E_TLIBC_NOERROR;\n");
	generator_print(&self->super, "ERROR_RET:\n");
	generator_print(&self->super, "\treturn E_TLIBC_ERROR;\n");
	generator_print(&self->super, "}\n");

	return E_TD_NOERROR;
}

static TD_ERROR_CODE on_definition(GENERATOR *super, const ST_DEFINITION *definition)
{
	TLIBC_READER_GENERATOR *self = TLIBC_CONTAINER_OF(super, TLIBC_READER_GENERATOR, super);
	switch(definition->type)
	{
		case E_DT_IMPORT:
			return _on_import(self, &definition->definition.de_import);				
		case E_DT_CONST:
			return E_TD_NOERROR;
		case E_DT_ENUM:
			_on_enum_name(self, &definition->definition.de_enum);
			_on_enum_number(self, &definition->definition.de_enum);
			return _on_enum(self, &definition->definition.de_enum);
		case E_DT_STRUCT:
			return _on_struct(self, &definition->definition.de_struct);
		case E_DT_UNION:
			return _on_union(self, &definition->definition.de_union);
		case E_DT_TYPEDEF:
			return E_TD_NOERROR;
		case E_DT_UNIX_COMMENT:
			return E_TD_NOERROR;
		default:
			return E_TD_ERROR;
	}
}

void tlibc_writer_generator_init(TLIBC_READER_GENERATOR *self, const SYMBOLS *symbols)
{
	generator_init(&self->super, symbols);

	self->super.on_document_begin = on_document_begin;
	self->super.on_document_end = on_document_end;
	self->super.on_definition = on_definition;
}
