#include "hotpot/hp_writer.h"
#include "hotpot/hp_error.h"

hpint32 write_struct_begin(HPAbstractWriter *self, const char *struct_name)
{
	if(self->write_struct_begin == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_struct_begin(self, struct_name);
}

hpint32 write_struct_end(HPAbstractWriter *self, const char *struct_name)
{
	if(self->write_struct_end == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_struct_end(self, struct_name);
}

hpint32 write_vector_begin(HPAbstractWriter *self)
{
	if(self->write_vector_begin == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_vector_begin(self);
}

hpint32 write_vector_end(HPAbstractWriter *self)
{
	if(self->write_vector_end == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_vector_end(self);
}

hpint32 write_field_begin(HPAbstractWriter *self, const char *var_name, hpuint32 len)
{
	if(self->write_field_begin == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_field_begin(self, var_name, len);
}

hpint32 write_field_end(HPAbstractWriter *self, const char *var_name, hpuint32 len)
{
	if(self->write_field_end == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_field_end(self, var_name, len);
}

hpint32 write_hpint8(HPAbstractWriter *self, const hpint8 val)
{
	if(self->write_hpint8 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpint8(self, val);
}

hpint32 write_hpint16(HPAbstractWriter *self, const hpint16 val)
{
	if(self->write_hpint16 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpint16(self, val);
}

hpint32 write_hpint32(HPAbstractWriter *self, const hpint32 val)
{
	if(self->write_hpint32 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpint32(self, val);
}

hpint32 write_hpint64(HPAbstractWriter *self, const hpint64 val)
{
	if(self->write_hpint64 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpint64(self, val);
}

hpint32 write_hpuint8(HPAbstractWriter *self, const hpuint8 val)
{
	if(self->write_hpuint8 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpuint8(self, val);
}

hpint32 write_hpuint16(HPAbstractWriter *self, const hpuint16 val)
{
	if(self->write_hpuint16 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpuint16(self, val);
}

hpint32 write_hpuint32(HPAbstractWriter *self, const hpuint32 val)
{
	if(self->write_hpuint32 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpuint32(self, val);
}

hpint32 write_hpuint64(HPAbstractWriter *self, const hpuint64 val)
{
	if(self->write_hpuint64 == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpuint64(self, val);
}

hpint32 write_hpchar(HPAbstractWriter *self, const hpchar val)
{
	if(self->write_hpchar == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpchar(self, val);
}

hpint32 write_hpdouble(HPAbstractWriter *self, const hpdouble val)
{
	if(self->write_hpdouble == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpdouble(self, val);
}

hpint32 write_enum(HPAbstractWriter *self, const int val)
{
	if(self->write_enum == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_enum(self, val);
}

hpint32 write_enum_name(HPAbstractWriter *self, const hpint32 val, const hpchar *enum_name)
{
	if(self->write_enum_name == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_enum_name(self, val, enum_name);
}

hpint32 write_hpstring(HPAbstractWriter *self, const hpchar* str)
{
	if(self->write_hpstring == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpstring(self, str);
}

hpint32 write_bytes(HPAbstractWriter *self, const hpbytes bytes)
{
	if(self->write_bytes == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_bytes(self, bytes);
}

hpint32 write_hpbool(HPAbstractWriter *self, const hpbool val)
{
	if(self->write_hpbool == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_hpbool(self, val);
}

hpint32 write_null(HPAbstractWriter *self)
{
	if(self->write_null == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_null(self);
}

hpint32 write_semicolon(HPAbstractWriter *self)
{
	if(self->write_semicolon == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_semicolon(self);
}

hpint32 write_type(HPAbstractWriter *self, const HPType type)
{
	if(self->write_type == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_type(self, type);
}

hpint32 write_vector_item_begin(HPAbstractWriter *self, hpuint32 index)
{
	if(self->write_vector_item_begin == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_vector_item_begin(self, index);
}

hpint32 write_vector_item_end(HPAbstractWriter *self, hpuint32 index)
{
	if(self->write_vector_item_end == NULL)
	{
		return E_HP_NOERROR;
	}
	return self->write_vector_item_end(self, index);
}