#include <iostream>
#include <stdlib.h>
using namespace std;
const int memspace = 0x3fffff;
int pc;
int r[32];
unsigned char memory[memspace];
int offset;
int rs1, rs2, res, rd_index, inst, imm, func3, func7;
int signedextend(int x, int bits)
{
	if (x>> (bits-1) == 1)
	{
		x |= ((0xffffffff >> bits) << bits);
	}
	return x;
}
void read()
{
	/*FILE *file;
	file = fopen("testdata.data", "r");
	if (file == NULL)
	{
		cout << "打开文件错误!" << endl;
		exit(0);
	}*/
	char ch;
	int a = 0;
	int b = 0;
	int s[10];
	s[0] = 1;
	for (int i = 1; i <= 8; ++i)
	{
		s[i] = s[i - 1] * 16;
	}
	while (cin.get(ch))
	//while(fread(&ch,sizeof(unsigned char),1,file)==1)
	{
		if (ch == '@')
		{
			int tmp = 0;
			for (int i = 7; i >= 0; --i)
			{
				cin.get(ch);
				//fread(&ch, sizeof(unsigned char), 1, file);
				if(ch>='0'&&ch<='9')
				tmp += (ch-'0') * s[i];
				else
					tmp+= (ch - 'A'+10) * s[i];
			}
			offset = tmp;
			continue;
		}
		if (ch == ' ' || ch == '\n')
		{
			a = 0;
			b = 0;
			continue;
		}
		if (ch >= '0'&&ch <= '9')
		{
			if (b == 0)
			{
				a += (ch - '0') * 16;
				++b;
			}
			else
			{
				a += (ch - '0');
				memory[offset++] = (unsigned char)a;
			}
			continue;
		}
		if (ch >= 'A'&&ch <= 'F')
		{
			if (b == 0)
			{
				a += (ch - 'A' + 10) * 16;
				++b;
			}
			else
			{
				a += (ch - 'A'+10);
				memory[offset++] = (unsigned char)a;
			}
			continue;
		}
	}
	//fclose(file);
}
void IF()
{

	int index = pc;

	inst = 0;
	for (int i = 0; i < 4; ++i)
	{
		int k = 1;
		for (int j = 1; j <= i; ++j)
			k *= 256;
		inst += k * (int)memory[index + i];
	}
	/*cout << "pc: " << hex<< pc << '\n' ;
	cout << "rs2: " << dec << ((inst >> 20) & 31) << '\n';
	cout << "rs1: " << dec << ((inst >> 15) & 31) << '\n';
	cout << "rd: " << dec << ((inst >> 7) & 31) << '\n';*/
	pc += 4;
	return;
}
void ID()
{
	switch (inst & 0x7f)//读取操作码
	{
	case 51://r-type
	{
		func3 = (inst >> 12) & 7;
		rs2 = r[(inst >> 20) & 31];
		rs1 = r[(inst >> 15) & 31];
		rd_index = (inst >> 7) & 31;
		func7 = (inst >> 25) & 127;
		return;
	}
	case 3:case 103://i-type
	{
		imm = (inst >> 20)&0xfff; 
		rs1 = r[(inst >> 15) & 31];
		rd_index = (inst >> 7) & 31;
		func3 = (inst >> 12) & 7;
		imm = signedextend(imm, 12);
		return;
	}
	case 35://s-type
	{
		imm = inst >> 25;
		imm = (imm << 5 )|((inst >> 7) & 31);
		rs2 = r[(inst >> 20) & 31];
		rs1 = r[(inst >> 15) & 31];
		func3 = (inst >> 12) & 7;
		imm = signedextend(imm, 12);
		return;
	}
	case 99://b-type
	{
		imm = inst >> 31;
		imm = (imm << 1) |((inst >> 7) & 1);
		imm = (imm << 6 )|((inst >> 25) & 63);
		imm = (imm << 4 )|((inst >> 8) & 15);
		imm <<= 1;
		imm = signedextend(imm, 13);
		rs2 = r[(inst >> 20) & 31];
		rs1 = r[(inst >> 15) & 31];
		func3 = (inst >> 12) & 7;
		return;
	}
	case 55:case 23://u-type
	{
		imm = inst >> 12;
		imm <<= 12;
		rd_index = (inst >> 7) & 31;
		return;
	}
	case 111://j-type
	{
		imm = ((inst >> 31)&1);
		imm = (imm << 8) + ((inst >> 12) & 255);
		imm = (imm << 1) + ((inst >> 20) & 1);
		imm = (imm << 10) + ((inst >> 21) & 1023);
		imm <<= 1;
		rd_index = (inst >> 7) & 31;
		imm = signedextend(imm, 21);
		return;
	}
	case 19://?-type
	{
		func3 = (inst >> 12) & 7;
		if (func3 == 1 || func3 == 5)//r-type(not)
		{
			//rs2 = r[(inst >> 20) & 31];
			rs2 = (inst >> 20) & 31;//(not actually rs2)
			rs1 = r[(inst >> 15) & 31];
			rd_index = (inst >> 7) & 31;
			func7 = (inst >> 25) & 127;
			return;
		}
		else//i-type
		{
			imm = inst >> 20;
			rs1 = r[(inst >> 15) & 31];
			rd_index = (inst >> 7) & 31;
			func3 = (inst >> 12) & 7;
			imm = signedextend(imm, 12);
			return;
		}
	}
	default:
		/*if (inst == 0)
		{
			cout << r[10];
			exit(0);
		}*/
		cout << "ID error:" << (inst & 0x7f) << endl;
		return;
	}
}
void EX()
{
	//cout << "imm: "<<hex<< imm << '\n' << '\n';
	switch (inst & 0x7f)//读取操作码
	{
	case 51://r-type
	{
		switch (func3)
		{
		case 0://add and sub
		{
			if (func7 == 0)//add
			{
				res = rs1 + rs2;
				return;
			}
			if (func7 == 32)//sub
			{
				res = rs1 - rs2;
				return;
			}
			cout << "error: EX case 51";
			return;
		}
		case 1://sll
		{
			res = (unsigned)rs1 << (unsigned)(rs2 & 31);
			return;
		}
		case 2://slt
		{
			res = rs1 < rs2 ? 1 : 0;
			return;
		}
		case 3://sltu
		{
			res = (unsigned)rs1 < (unsigned)rs2 ? 1 : 0;
			return;
		}
		case 4://xor
		{
			res = rs1 xor rs2;
			return;
		}
		case 5://srl and sra
		{
			if (func7 == 0)//srl
			{
				res = (unsigned)rs1 >> (unsigned)(rs2 & 31);
				return;
			}
			if (func7 == 32)//sra
			{
				res = rs1 >> (rs2 & 31);
				return;
			}
			cout << "error: EX case 51";
			return;
		}
		case 6://or
		{
			res = rs1 | rs2;
			return;
		}
		case 7://and
		{
			res = rs1 & rs2;
			return;
		}
		default:
			cout << "error: EX case 51";
			return;
		}
		return;
	}
	case 103://(i-type)jalr
	{
		res = pc;
		pc = rs1 + imm;
		pc >>= 1;
		pc <<= 1;
		return;
	}
	case 3://(i-type)lb lh lw lbu lhu
	{
		res = rs1 + imm;
		return;
	}
	case 35://(s-type)sb sh sw
	{
		res = rs1 + imm;
		return;
	}
	case 99://b-type
	{
		switch (func3)
		{
		case 0://beq
		{
			if (rs1 == rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		case 1://bne
		{
			if (rs1 != rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		case 4://blt
		{
			if (rs1 < rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		case 5://bge
		{
			if (rs1 >= rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		case 6://bltu
		{
			if ((unsigned)rs1 < (unsigned)rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		case 7://bgeu
		{
			if ((unsigned)rs1 >= (unsigned)rs2)
			{
				pc = pc - 4 + imm;
			}
			return;
		}
		default:
			cout << "error: EX case 99";
			return;
		}
	}
	case 55://(u-type)lui
	{
		res = imm;
		return;
	}
	case 23://(u-type)auipc
	{
		res = pc - 4 + imm;
		return;
	}
	case 111://(j-type)jal
	{
		res = pc;
		pc = pc + imm - 4;
		return;
	}
	case 19://with constant
	{
		switch (func3)
		{
		case 0://addi
		{
			res = rs1 + imm;
			return;
		}
		case 2://slti
		{
			res = rs1 < imm ? 1 : 0;
			return;
		}
		case 3://sltui
		{
			res = (unsigned)rs1 < (unsigned)imm ? 1 : 0;
			return;
		}
		case 4://xori
		{
			res = rs1 xor imm;
			return;
		}
		case 6://ori
		{
			res = rs1 | imm;
			return;
		}
		case 7://andi
		{
			res = rs1 & imm;
			return;
		}
		case 1://slli
		{
			res = rs1 << rs2;//(not actuall rs2)
			return;
		}
		case 5:
		{
			if (func7 == 0)//srli
			{
				res = (unsigned)rs1 >> (unsigned)rs2;//(not actuall rs2)
				return;
			}
			if (func7 == 32)//srai
			{
				res = rs1 >> rs2;//(not actuall rs2)
				return;
			}
			cout << "error: EX case 19";
			return;
		}
		default:
			cout << "error: EX case 19";
			return;
		}
	}
	}
}
//load and store
void MEM()
{
	switch (inst & 0x7f)
	{
	case 3://(i-type)load
	{
		switch (func3)
		{
		case 0://lb
		{
			res = memory[res];
			if (res & 0x80) res |= 0xffffff00;
			return;
		}
		case 1://lh
		{
			res = (memory[res]&255) |((memory[res + 1]&255) << 8);
			res = signedextend(res, 16);
			return;
		}
		case 2://lw
		{
			res = (memory[res]&255) |((memory[res + 1] & 255) << 8 )| ((memory[res + 2] & 255) << 16) |((memory[res + 3] & 255) << 24);
			return;
		}
		case 4://lbu
		{
			res = memory[res]&255;
			return;
		}
		case 5://lhu
		{
			res = memory[res] |((memory[res + 1] & 255) << 8);
			return;
		}
		default:
			cout << "error: MEM load";
			return;
		}
	}
	case 35://(s-type)store
	{
		if (func3 == 0)//sb
		{
			memory[res] = rs2 & 255;
			if (res == 0x30004)
			{
				cout << (unsigned)(r[10]&255);
				exit(0);
			}
			return;
		}
		if (func3 == 1)//sh;
		{
			memory[res] = rs2 & 255;
			memory[res + 1] = (rs2 >> 8) & 255;
			
			return;
		}
		if (func3 == 2)//sw
		{
			memory[res] = rs2 & 255;
			memory[res + 1] = (rs2 >> 8) & 255;
			memory[res + 2] = (rs2 >> 16) & 255;
			memory[res + 3] = (rs2 >> 24) & 255;
			
			return;
		}
		cout << "error: MEM store";
		return;
	}
	}
	return;
}
void WB()
{
	switch (inst & 0x7f)//读取操作码
	{
	case 99:case 35://store and b-type
		return;
	default:
		if (rd_index == 0)
			return;
		r[rd_index] = res;
		return;
	}
}

int main()
{
	read();
	while (1)
	{
		IF();
		ID();
		EX();
		MEM();
		WB();
	}
}

