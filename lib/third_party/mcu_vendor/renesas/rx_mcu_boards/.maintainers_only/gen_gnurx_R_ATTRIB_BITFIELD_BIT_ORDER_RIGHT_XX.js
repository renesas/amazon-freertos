// Generate R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX() macros for GNURX
// cscript -nologo gen_gnurx_R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX.js > gen_gnurx_R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX.h

var stdout = WScript.StdOut;
var NL = "\r\n";
var str = "";

str += "#if defined(__RX_LITTLE_ENDIAN__)" + NL;
str += NL;

for (var argnum = 2; argnum <= 32; argnum++)
{
	str += "#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_" + argnum + "(";
	for (var i = 1; i <= argnum; i++)
	{
		str += "bf" + i;
		if (i < argnum)
		{
			if (i % 10 == 0)
			{
				str += ", \\" + NL;
				str += "                                            ";
			}
			else
			{
				str += ", ";
			}
		}
		else
		{
			str += ")\\" + NL;
		}
	}
	str += "struct {\\" + NL;
	for (var i = 1; i <= argnum; i++)
	{
		str += "    bf" + i + ";\\" + NL;
	}
	str += "}" + NL;
	str += NL;
}

str += "#else" + NL;
str += NL;

for (var argnum = 2; argnum <= 32; argnum++)
{
	str += "#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_" + argnum + "(";
	for (var i = 1; i <= argnum; i++)
	{
		str += "bf" + i;
		if (i < argnum)
		{
			if (i % 10 == 0)
			{
				str += ", \\" + NL;
				str += "                                            ";
			}
			else
			{
				str += ", ";
			}
		}
		else
		{
			str += ")\\" + NL;
		}
	}
	str += "struct {\\" + NL;
	for (var i = argnum; i >= 1; i--)
	{
		str += "    bf" + i + ";\\" + NL;
	}
	str += "}" + NL;
	str += NL;
}
str += "#endif" + NL;
str += NL;

stdout.Write(str);
