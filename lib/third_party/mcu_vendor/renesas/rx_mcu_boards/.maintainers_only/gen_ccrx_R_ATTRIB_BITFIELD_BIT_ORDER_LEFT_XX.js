// Generate R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX() macros for CC-RX
// cscript -nologo gen_ccrx_R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX.js > gen_ccrx_R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_XX.h

var stdout = WScript.StdOut;
var NL = "\r\n";
var str = "";

for (var argnum = 2; argnum <= 32; argnum++)
{
	str += "#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_" + argnum + "(";
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
	str += "R_PRAGMA(bit_order left)\\" + NL;
	str += "    struct {\\" + NL;
	for (var i = 1; i <= argnum; i++)
	{
		str += "        bf" + i + ";\\" + NL;
	}
	str += "    };\\" + NL;
	str += "R_PRAGMA(bit_order)\\" + NL;
	str += "}" + NL;
	str += NL;
}

stdout.Write(str);
