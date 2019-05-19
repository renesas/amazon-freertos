// Generate R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX() macros for ICCRX
// cscript -nologo gen_iccrx_R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX.js > gen_iccrx_R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_XX.h

var stdout = WScript.StdOut;
var NL = "\r\n";
var str = "";

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
	str += "R_PRAGMA(bitfields=default)\\" +NL;
	str += "    struct {\\" + NL;
	for (var i = 1; i <= argnum; i++)
	{
		str += "        bf" + i + ";\\" + NL;
	}
	str += "    };\\" + NL;
	str += "R_PRAGMA(bitfields=reversed_disjoint_types)\\" + NL;
	str += "}" + NL;
	str += NL;
}

stdout.Write(str);
