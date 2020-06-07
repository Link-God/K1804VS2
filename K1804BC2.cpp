// MT1804BC1.cpp: определяет экспортированные функции для приложения DLL.
//

#include "pch.h"
#include "K1804BC2.h"
#include "logger.h"

uint8_t K1804BC2::genValue(IDSIMPIN** pins, size_t n, size_t offset)
{
	uint8_t res = 0;
	for (size_t i = 0; i < n; ++i)
		res |= isHigh(pins[offset + i]) << i;
	return res;
}


// линковка выходов схемы в Proteus и в коде
VOID K1804BC2::setup(IINSTANCE* instance, IDSIMCKT* dsimckt)
{
	_inst = instance;
	_ckt = dsimckt;

	char buffer[16];
	for (size_t i = 0; i < REGISTER_SIZE; ++i)
	{
		sprintf_s(buffer, "A%d", i);
		_pin_A[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < REGISTER_SIZE; ++i)
	{
		sprintf_s(buffer, "B%d", i);
		_pin_B[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < REGISTER_SIZE; ++i)
	{
		sprintf_s(buffer, "DA%d", i);
		_pin_DA[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < REGISTER_SIZE; ++i)
	{
		sprintf_s(buffer, "DB%d", i);
		_pin_DB[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < PIN_I_SIZE; ++i)
	{
		sprintf_s(buffer, "I%d", i);
		_pin_I[i] = _inst->getdsimpin(buffer, true);
	}

	_pin_PQ0 = _inst->getdsimpin(const_cast<CHAR*>("PQ0"), true);
	_pin_PQ3 = _inst->getdsimpin(const_cast<CHAR*>("PQ3"), true);
	_pin_PF0 = _inst->getdsimpin(const_cast<CHAR*>("PF0"), true);
	_pin_PF3 = _inst->getdsimpin(const_cast<CHAR*>("PF3"), true);
	_pin_T = _inst->getdsimpin(const_cast<CHAR*>("T"), true);
	_pin_OEB = _inst->getdsimpin(const_cast<CHAR*>("OEB"), true);
	_pin_EA = _inst->getdsimpin(const_cast<CHAR*>("EA"), true);
	_pin_IEN = _inst->getdsimpin(const_cast<CHAR*>("IEN"), true);
	_pin_C0 = _inst->getdsimpin(const_cast<CHAR*>("C0"), true);
	_pin_OEY = _inst->getdsimpin(const_cast<CHAR*>("OEY"), true);
	_pin_LSS = _inst->getdsimpin(const_cast<CHAR*>("LSS"), true);
	_pin_W_MSS = _inst->getdsimpin(const_cast<CHAR*>("W/MSS"), true);
	_pin_WE = _inst->getdsimpin(const_cast<CHAR*>("WE"), true);


	for (size_t i = 0; i < REGISTER_SIZE; ++i)
	{
		sprintf_s(buffer, "Y%d", i);
		_pin_Y[i] = _inst->getdsimpin(buffer, true);
		//setState(0, _pin_Y[i], false);
	}
	// выход флагов
	_pin_C4 = _inst->getdsimpin(const_cast<CHAR*>("C4"), true);
	setState(0, _pin_C4, 0);
	_pin_Z = _inst->getdsimpin(const_cast<CHAR*>("Z"), true);
	setState(0, _pin_Z, 0);
	_pin_P_OVR = _inst->getdsimpin(const_cast<CHAR*>("P/OVR"), true);
	setState(0, _pin_P_OVR, 0);
	_pin_G_N = _inst->getdsimpin(const_cast<CHAR*>("G/N"), true);
	setState(0, _pin_G_N, 0);
}

// получение данных со всех входов
K1804BC2::CommandFields* K1804BC2::getCommand()
{
	CommandFields* cmd = new CommandFields();
	cmd->From = (isHigh(_pin_EA) << 2) | (isHigh(_pin_I[0]) << 1) | isHigh(_pin_OEB);
	cmd->Alu = genValue(_pin_I, 5, 1);
	cmd->To = genValue(_pin_I, 9, 5);
	cmd->A = genValue(_pin_A, REGISTER_SIZE);
	cmd->B = genValue(_pin_B, REGISTER_SIZE);
	cmd->DA = genValue(_pin_DA, REGISTER_SIZE);
	cmd->DB = genValue(_pin_DB, REGISTER_SIZE);
	cmd->C0 = isHigh(_pin_C0);
	cmd->I0 = isHigh(_pin_I[0]);
	_pos = cmd->position = getPosition();
	return cmd;
}

// R = POH(A), S = POH(B)
void K1804BC2::__download__000(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = _regs[cmd->A];
	ops->S = _regs[cmd->B];
	if (log != nullptr)
	{
		log->log("From: R=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->R) + ", S=POH(A=" + std::to_string(cmd->B) + ")="
			+ std::to_string(ops->S));
	}
}

// R = POH(A), S = DB
void K1804BC2::__download__001(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = _regs[cmd->A];
	ops->S = cmd->DB;
	if (log != nullptr)
	{
		log->log("From: R=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->R) + ", S=DB=" + std::to_string(ops->S));
	}
}

// 010: R=POH(A), S=Q
void K1804BC2::__download__01X(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = _regs[cmd->A];
	ops->S = _reg_q;
	if (log != nullptr)
	{
		log->log("From: R=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->R) + ", S=PQ=" + std::to_string(ops->S));
	}
}

// R = DA, S = POH(B)
void K1804BC2::__download__100(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = cmd->DA;
	ops->S = _regs[cmd->B];
	if (log != nullptr)
	{
		log->log("From: R=DA=" + std::to_string(ops->R) + ", S=POH(B="
			+ std::to_string(cmd->B) + ")=" + std::to_string(ops->S));
	}
}

// R = DA, S = DB
void K1804BC2::__download__101(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = cmd->DA;
	ops->S = cmd->DB;
	if (log != nullptr)
	{
		log->log("From: R=DA=" + std::to_string(ops->R) + ", S=DB=" + std::to_string(ops->S));
	}
}

// R = DA, S = Q
void K1804BC2::__download__11X(const CommandFields* cmd, Operands* ops, ILogger* log)
{
	if (cmd == nullptr || ops == nullptr)
	{
		return;
	}
	ops->R = cmd->DA;
	ops->S = _reg_q;
	if (log != nullptr)
	{
		log->log("From: R=DA=" + std::to_string(ops->R) + ", S=PQ=" + std::to_string(ops->S));
	}
}

// Загрузка операндов
K1804BC2::Operands* K1804BC2::getOperands(const CommandFields* cmd, ILogger* log)
{
	if (cmd == nullptr)
	{
		return nullptr;
	}
	Operands* ops = new Operands();
	switch (cmd->From)
	{
		// 000: R=POH(A), S=POH(B)
	case 0:
		__download__000(cmd, ops, log);
		break;
		// 001: R=POH(A), S=DB
	case 1:
		__download__001(cmd, ops, log);
		break;
		// 010: R=POH(A), S=Q
	case 2:
		__download__01X(cmd, ops, log);
		break;
		// 011: R=POH(A), S=Q
	case 3:
		__download__01X(cmd, ops, log);
		break;
		// 100: R=DA, S=POH(B)
	case 4:
		__download__100(cmd, ops, log);
		break;
		// 101: R=DA, S=DB
	case 5:
		__download__101(cmd, ops, log);
		break;
		// 110: R=DA, S=Q
	case 6:
		__download__11X(cmd, ops, log);
		break;
		// 111: R=DA, S=Q
	case 7:
		__download__11X(cmd, ops, log);
		break;
	}
	return ops;
}

// I0 = 0 -> специальный функции
// I0 = 1 -> 1111
void K1804BC2::__alu__0000(bool c0, bool i0, const Operands* ops, const uint8_t special_code, ALUReasult* res,
                           ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	if (i0 == 0)
	{
		special(c0, ops, special_code, res, log);
	}
	else
	{
		res->F = res->Y = 0b1111;
		if (log != nullptr)
		{
			log->log("ALU: Y=1111");
		}
		//computeFlags(res, c0, ops, 0);
	}
}

// S - R - 1 + C0
void K1804BC2::__alu__0001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->S - ops->R - 1 + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=S-R-1+C0=" + std::to_string(ops->S) + "-"
			+ std::to_string(ops->R) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	//computeFlags(res, c0, ops, 0);
}

//R - S - 1 + C0
void K1804BC2::__alu__0010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R - ops->S - 1 + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=R-S-1+C0=" + std::to_string(ops->R) + "-"
			+ std::to_string(ops->S) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// R + S + C0
void K1804BC2::__alu__0011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R + ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=R+S+C0=" + std::to_string(ops->R) + "+"
			+ std::to_string(ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// S + C0
void K1804BC2::__alu__0100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=S+C0=" + std::to_string(ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// ~S + C0
void K1804BC2::__alu__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=~S+C0=" + std::to_string(~ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// R + C0
void K1804BC2::__alu__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=R+C0=" + std::to_string(ops->R) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// ~R + C0
void K1804BC2::__alu__0111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~ops->R + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=~S+C0=" + std::to_string(~ops->R) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

// 0000
void K1804BC2::__alu__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = 0b0000;
	if (log != nullptr)
	{
		log->log("ALU: Y=0000");
	}
}

// ~R & S
void K1804BC2::__alu__1001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~ops->R & ops->S;
	if (log != nullptr)
	{
		log->log("ALU: Y=~R&S=" + std::to_string(~ops->R) + "&" + std::to_string(ops->S) + "="
			+ std::to_string(res->Y));
	}
}

//~(R ^ S)
void K1804BC2::__alu__1010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~(ops->R ^ ops->S);
	if (log != nullptr)
	{
		log->log("ALU: Y=~(R^S)=~(" + std::to_string(ops->R) + "^" + std::to_string(ops->S) + ")="
			+ std::to_string(res->Y));
	}
}

//R ^ S
void K1804BC2::__alu__1011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R ^ ops->S;
	if (log != nullptr)
	{
		log->log("ALU: Y=~(R^S)=" + std::to_string(ops->R) + "^" + std::to_string(ops->S) + "="
			+ std::to_string(res->Y));
	}
}

//R & S
void K1804BC2::__alu__1100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R & ops->S;
	if (log != nullptr)
	{
		log->log("ALU: Y=R&S=" + std::to_string(ops->R) + "&" + std::to_string(ops->S) + "="
			+ std::to_string(res->Y));
	}
}

//~(R | S)
void K1804BC2::__alu__1101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~(ops->R | ops->S);
	if (log != nullptr)
	{
		log->log("ALU: Y=~(R|S)=~(" + std::to_string(ops->R) + "|" + std::to_string(ops->S) + ")="
			+ std::to_string(res->Y));
	}
}

// ~(R & S)
void K1804BC2::__alu__1110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ~(ops->R & ops->S);
	if (log != nullptr)
	{
		log->log("ALU: Y=~(R&S)=~(" + std::to_string(ops->R) + "&" + std::to_string(ops->S) + ")="
			+ std::to_string(res->Y));
	}
}

// R | S
void K1804BC2::__alu__1111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->F = res->Y = ops->R | ops->S;
	if (log != nullptr)
	{
		log->log("ALU: Y=R|S=" + std::to_string(ops->R) + "|" + std::to_string(ops->S) + "="
			+ std::to_string(res->Y));
	}
}

K1804BC2::ALUReasult* K1804BC2::ALU(bool c0, uint8_t code, const Operands* ops, ILogger* log)
{
	if (ops == nullptr)
	{
		return nullptr;
	}
	auto res = new ALUReasult();
	auto special_code = genValue(_pin_I, 9, 5);
	auto I0 = isHigh(_pin_I[0]);

	switch (code)
	{
		// I0 = 0 -> специальный функции
		// I0 = 1 -> 1111
	case 0:
		__alu__0000(c0, I0, ops, special_code, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//S - R - 1 + C0
	case 1:
		__alu__0001(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R - S - 1 + C0
	case 2:
		__alu__0010(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R + S + C0
	case 3:
		__alu__0011(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//S + C0
	case 4:
		__alu__0100(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~S + C0
	case 5:
		__alu__0101(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R + C0
	case 6:
		__alu__0110(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~R + C0
	case 7:
		__alu__0111(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//0000
	case 8:
		__alu__1000(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~R & S
	case 9:
		__alu__1001(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~(R ^ S)
	case 10:
		__alu__1010(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R ^ S
	case 11:
		__alu__1011(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R & S
	case 12:
		__alu__1100(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~(R | S)
	case 13:
		__alu__1101(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//~(R & S)
	case 14:
		__alu__1110(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
		//R | S
	case 15:
		__alu__1111(c0, ops, res, log);
		computeFlags(false, res, c0, ops, code);
		break;
	}
	return res;
}


void K1804BC2::__load__0000(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y;
	if (cmd->position == OLD)
	{
		isHigh(_pin_PF3) ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0001 : y &= 0b1110;
	}
	else
	{
		isHigh(_pin_PF3) ? y |= 0b1000 : y &= 0b0111;
		res->Y & 0b1000 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0001 : y &= 0b1110;
	}
	res->Y & 0b0001 ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	res->Y = y & 0b1111;
	if (log != nullptr)
	{
		log->log("Load: Y->F/2=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__0001(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y >> 1;
	isHigh(_pin_PF3) ? y |= 0b1000 : y &= 0b0111;
	res->Y & 0b0001 ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	res->Y = (y) & 0b1111;
	if (log != nullptr)
	{
		log->log("Load: Y->F/2=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__0010(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y;
	if (cmd->position == OLD)
	{
		isHigh(_pin_PF3) ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0001 : y &= 0b1110;
	}
	else
	{
		isHigh(_pin_PF3) ? y |= 0b1000 : y &= 0b0111;
		res->Y & 0b1000 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0001 : y &= 0b1110;
	}
	res->Y & 0b0001 ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);

	res->Y = y & 0b1111;

	_reg_q & 0b0001 ? setState(_time, _pin_PQ0, 1) : setState(_time, _pin_PQ0, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q >> 1;
		isHigh(_pin_PQ3) ? pq |= 0b1000 : pq & 0b0111;
		_reg_q = pq & 0b1111;
	}

	if (log != nullptr)
	{
		log->log("Load: Y->F/2=" + std::to_string(res->Y & 0b1111) +
			" Q -> Q/2=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__0011(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y >> 1;
	isHigh(_pin_PF3) ? y |= 0b1000 : y &= 0b0111;
	res->Y & 0b0001 ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);
	res->Y = y & 0b1111;

	_reg_q & 0b0001 ? setState(_time, _pin_PQ0, 1) : setState(_time, _pin_PQ0, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q >> 1;
		isHigh(_pin_PQ3) ? pq |= 0b1000 : pq & 0b0111;
		_reg_q = pq & 0b1111;
	}

	if (log != nullptr)
	{
		log->log("Load: Y->F/2=" + std::to_string(res->Y & 0b1111) +
			" Q -> Q/2=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__0100(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	bool parity;
	if (isHigh(_pin_PF3))
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != true;
	}
	else
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != false;
	}
	parity == true ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);

	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__0101(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = true;
	bool parity;
	if (isHigh(_pin_PF3))
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != true;
	}
	else
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != false;
	}
	parity == true ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);

	_reg_q & 0b0001 ? setState(_time, _pin_PQ0, 1) : setState(_time, _pin_PQ0, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q >> 1;
		isHigh(_pin_PQ3) ? pq |= 0b1000 : pq & 0b0111;
		_reg_q = (pq) & 0b1111;
	}

	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) +
			" Q -> Q/2=" + std::to_string(_reg_q));
	}
}


void K1804BC2::__load__0110(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = true;
	bool parity;
	if (isHigh(_pin_PF3))
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != true;
	}
	else
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != false;
	}
	parity == true ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);

	if (isLow(_pin_IEN))
	{
		_reg_q = (res->Y) & 0b1111;
	}
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);

	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) + " Y->Q=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__0111(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	bool parity;
	if (isHigh(_pin_PF3))
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != true;
	}
	else
	{
		parity = static_cast<bool>(res->Y & 0b1000) != static_cast<bool>(res->Y & 0b0100) != static_cast<bool>(res->Y &
				0b0010)
			!= static_cast<bool>(res->Y & 0b0001) != false;
	}
	parity == true ? setState(_time, _pin_PF0, 1) : setState(_time, _pin_PF0, -1);

	if (isLow(_pin_IEN))
	{
		_reg_q = (res->Y) & 0b1111;
	}
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) + " Y->Q=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__1000(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y;
	if (cmd->position == OLD)
	{
		isHigh(_pin_PF0) ? y |= 0b0001 : y &= 0b1110;
		res->Y & 0b0001 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	}
	else
	{
		isHigh(_pin_PF0) ? y |= 0b0001 : y &= 0b1110;
		res->Y & 0b0001 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b1000 : y &= 0b0111;
		res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	}
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	res->Y = (y) & 0b1111;
	if (log != nullptr)
	{
		log->log("Load: Y->2Y=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__1001(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y << 1;
	res->Y = y & 0b1111;
	res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	if (log != nullptr)
	{
		log->log("Load: Y->2Y=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__1010(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y;
	if (cmd->position == OLD)
	{
		isHigh(_pin_PF0) ? y |= 0b0001 : y &= 0b1110;
		res->Y & 0b0001 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	}
	else
	{
		isHigh(_pin_PF0) ? y |= 0b0001 : y &= 0b1110;
		res->Y & 0b0001 ? y |= 0b0010 : y &= 0b1101;
		res->Y & 0b0010 ? y |= 0b0100 : y &= 0b1011;
		res->Y & 0b0100 ? y |= 0b1000 : y &= 0b0111;
		res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	}
	res->Y = y & 0b1111;

	_reg_q & 0b1000 ? setState(_time, _pin_PQ3, 1) : setState(_time, _pin_PQ3, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q << 1;
		isHigh(_pin_PQ0) ? pq |= 0b0001 : pq & 0b1110;
		_reg_q = (pq) & 0b1111;
	}

	if (log != nullptr)
	{
		log->log("Load: Y->2Y=" + std::to_string(res->Y & 0b1111) +
			" Q->2Q=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__1011(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	uint8_t y = res->Y << 1;
	res->Y = (y) & 0b1111;
	res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);

	_reg_q & 0b1000 ? setState(_time, _pin_PQ3, 1) : setState(_time, _pin_PQ3, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q << 1;
		isHigh(_pin_PQ0) ? pq |= 0b0001 : pq & 0b1110;
		_reg_q = (pq) & 0b1111;
	}

	if (log != nullptr)
	{
		log->log("Load: Y->2Y=" + std::to_string(res->Y & 0b1111) +
			" Q->2Q=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__1100(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = true;
	res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	setState(_time, _pin_PF0, 0);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__1101(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = true;

	_reg_q & 0b1000 ? setState(_time, _pin_PQ3, 1) : setState(_time, _pin_PQ3, -1);
	if (isLow(_pin_IEN))
	{
		uint8_t pq = _reg_q << 1;
		isHigh(_pin_PQ0) ? pq |= 0b0001 : pq & 0b1110;
		_reg_q = (pq) & 0b1111;
	}
	res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	setState(_time, _pin_PF0, 0);

	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) +
			" Q->2Q=" + std::to_string(_reg_q));
	}
}

void K1804BC2::__load__1110(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	// PR0 -> Y[0,1,2,3] Q-hold
	uint8_t y = res->Y;
	isHigh(_pin_PF0) ? y &= 0b1111 : y &= 0b0000;
	isHigh(_pin_PF0) ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);

	if (log != nullptr)
	{
		log->log("Load: PR0->Y[0,1,2,3]" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

void K1804BC2::__load__1111(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	if (cmd == nullptr || res == nullptr)
	{
		return;
	}
	res->W = false;
	setState(_time, _pin_PF0, 0);
	res->Y & 0b1000 ? setState(_time, _pin_PF3, 1) : setState(_time, _pin_PF3, -1);
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
	if (log != nullptr)
	{
		log->log("Load: Y=" + std::to_string(res->Y & 0b1111) + " Q - hold");
	}
}

// S+C0, если Z=0; R+S+C0, если Z=1;
void K1804BC2::__special__0000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	std::string msg;
	// PQ0=Q0
	if (_reg_q & 0b0001)
	{
		setState(_time, _pin_PQ0, 1); // PQ0 = 1
		msg += "PQ0=1,";
	}
	else
	{
		setState(_time, _pin_PQ0, -1); // PQ0 = 0
		msg += "PQ0=0,";
	}
	if (isLow(_pin_IEN))
	{
		// Q=Q/2
		_reg_q = _reg_q >> 1;
		// Q3 = PQ3
		if (isHigh(_pin_PQ3))
		{
			_reg_q |= 0b1000; // Q3 = 1
			msg += "Q3=PQ3=1,";
		}
		msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	}
	if (log != nullptr)
		log->log(msg);

	if (_pos == YOUNG ? !static_cast<bool>(_reg_q & 0b0001) : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ops->R + ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=R+S+C0=" + std::to_string(ops->R) +
				"+" + std::to_string(ops->S) + "+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
}

// Лог. F/2->Y, Лог. Q/2->Q; старшая МПС: Y3 = C4
void K1804BC2::__special_load__0000(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	std::string msg = "LOAD SP: Лог. F/2->Y, Лог. Q/2->Q\n";
	// PF0=F0
	if (res->F & 0b0001)
	{
		setState(_time, _pin_PF0, 1); // PF0 = 1
		msg += "PF0=1,";
	}
	else
	{
		setState(_time, _pin_PF0, -1); // PF0 = 0
		msg += "PF0=0,";
	}
	res->Y = res->F >> 1; // Y=F/2
	// Если не старшая МПС, то PF3 вход
	if (_pos != OLD)
	{
		// Y3 = PF3
		if (isHigh(_pin_PF3))
		{
			res->Y |= 0b1000; // Y3 = 1
			msg += "F3=PF3=1,";
		}
	}
	else
		setState(_time, _pin_PF3, 0);
	msg += "Y=F/2=" + std::to_string(res->Y) + "\n";
	// // PQ0=Q0
	// if (_reg_q & 0b0001)
	// {
	// 	setState(_time, _pin_PQ0, 1); // PQ0 = 1
	// 	msg += "PQ0=1,";
	// }
	// else
	// {
	// 	setState(_time, _pin_PQ0, -1); // PQ0 = 0
	// 	msg += "PQ0=0,";
	// }
	// if (isLow(_pin_IEN))
	// {
	// 	// Q=Q/2
	// 	_reg_q = _reg_q >> 1;
	// 	// Q3 = PQ3
	// 	if (isHigh(_pin_PQ3))
	// 	{
	// 		_reg_q |= 0b1000; // Q3 = 1
	// 		msg += "Q3=PQ3=1,";
	// 	}
	// 	msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	// }
	if (log != nullptr)
		log->log(msg);
}

// S+C0, если Z=0; R+S+C0, если Z=1;
void K1804BC2::__special__0010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	std::string msg;
	// PQ0=Q0
	if (_reg_q & 0b0001)
	{
		setState(_time, _pin_PQ0, 1); // PQ0 = 1
		msg += "PQ0=1,";
	}
	else
	{
		setState(_time, _pin_PQ0, -1); // PQ0 = 0
		msg += "PQ0=0,";
	}
	if (isLow(_pin_IEN))
	{
		// Q=Q/2
		_reg_q = _reg_q >> 1;
		// Q3 = PQ3
		if (isHigh(_pin_PQ3))
		{
			_reg_q |= 0b1000; // Q3 = 1
			msg += "Q3=PQ3=1,";
		}
		msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	}
	if (log != nullptr)
		log->log(msg);

	if (_pos == YOUNG ? !static_cast<bool>(_reg_q & 0b0001) : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ops->R + ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=R+S+C0=" + std::to_string(ops->R) +
				"+" + std::to_string(ops->S) + "+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
}

// Лог. F/2->Y, Лог. Q/2->Q; старшая МПС: Y3 = F3 xor OVR
void K1804BC2::__special_load__0010(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	std::string msg = "LOAD SP: Лог. F/2->Y, Лог. Q/2->Q\n";
	// PF0=F0
	if ((res->F & 0b0001))
	{
		setState(_time, _pin_PF0, 1); // PF0 = 1
		msg += "PF0=1,";
	}
	else
	{
		setState(_time, _pin_PF0, -1); // PF0 = 0
		msg += "PF0=0,";
	}
	// Y=F/2
	res->Y = res->F >> 1;
	// Если не старшая МПС, то PF3 вход
	if (_pos != OLD)
	{
		// Y3 = PF3
		if (isHigh(_pin_PF3))
		{
			res->Y |= 0b1000; // F3 = 1
			msg += "F3=PF3=1,";
		}
	}
	else
		setState(_time, _pin_PF3, 0);

	msg += "Y=F/2=" + std::to_string(res->Y) + "\n";
	// // PQ0=Q0
	// if (_reg_q & 0b0001)
	// {
	// 	setState(_time, _pin_PQ0, 1); // PQ0 = 1
	// 	msg += "PQ0=1,";
	// }
	// else
	// {
	// 	setState(_time, _pin_PQ0, -1); // PQ0 = 0
	// 	msg += "PQ0=0,";
	// }
	// if (isLow(_pin_IEN))
	// {
	// 	// Q=Q/2
	// 	_reg_q = _reg_q >> 1;
	// 	// Q3 = PQ3
	// 	if (isHigh(_pin_PQ3))
	// 	{
	// 		_reg_q |= 0b1000; // Q3 = 1
	// 		msg += "Q3=PQ3=1,";
	// 	}
	// 	msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	// }
	if (log != nullptr)
		log->log(msg);
}

// S+1+C0
void K1804BC2::__special__0100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	res->F = res->Y = ops->S + 1 + c0;

	if (log != nullptr)
	{
		log->log("ALU_SPECIAL: Y=S+1+C0=" + std::to_string(ops->S) +
			"+1+" + std::to_string(c0) + "=" + std::to_string(res->Y));
	}
}

// F->Y, PF0 = чётность
void K1804BC2::__special_load__0100(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	bool PF3 = isHigh(_pin_PF3);
	// PF0 = чётность = PF3 xor F3 xor F2 xor F1 xor F0
	if (PF3 != static_cast<bool>(res->F & 0b1000) != static_cast<bool>(res->F & 0b0100) != static_cast<bool>(res->F &
			0b0010)
		!= static_cast<bool>(res->F & 0b0001))
	{
		setState(_time, _pin_PF0, 1); // PF0 = 1
	}
	else
	{
		setState(_time, _pin_PF0, -1); // PF0 = 0
	}
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
}

// S+C0, если Z=0; ~S+C0, если Z=1; старшая МПС: Y3 = S3 xor F3;
void K1804BC2::__special__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	if (_pos == OLD ? !static_cast<bool>(ops->S & 0b1000) : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ~ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=~S+C0=~" + std::to_string(ops->S) +
				"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
		}
	}

	// это должно по факту быть в загрузке, но S3 тут.
	// старшая МПС : Y3 = S3 xor F3
	if (_pos == OLD)
	{
		auto Y3 = (ops->S & 0b1000) ^ (res->Y & 0b1000);
		res->Y &= 0b0111;
		res->Y |= Y3;
	}
}

// F->Y; PF0 = чётность
void K1804BC2::__special_load__0101(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	bool PF3 = isHigh(_pin_PF3);
	// PF0 = чётность = PF3 xor F3 xor F2 xor F1 xor F0
	if (PF3 != static_cast<bool>(res->F & 0b1000) != static_cast<bool>(res->Y & 0b0100)
		!= static_cast<bool>(res->Y & 0b0010) != static_cast<bool>(res->Y & 0b0001))
	{
		setState(_time, _pin_PF0, 1); // PF0 = 1
	}
	else
	{
		setState(_time, _pin_PF0, -1); // PF0 = 0
	}
	setState(_time, _pin_PQ0, 0);
	setState(_time, _pin_PQ3, 0);
}

// S+C0, если Z=0; S-R-1+C0, если Z=1
void K1804BC2::__special__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	std::string msg;
	// PQ0=Q0
	if ((_reg_q & 0b0001))
	{
		setState(_time, _pin_PQ0, 1); // PQ0 = 1
		msg += "PQ0=1,";
	}
	else
	{
		setState(_time, _pin_PQ0, -1); // PQ0 = 0
		msg += "PQ0=0,";
	}
	if (isLow(_pin_IEN))
	{
		// Q=Q/2
		_reg_q = _reg_q >> 1;
		// Q3 = PQ3
		if (isHigh(_pin_PQ3))
		{
			_reg_q |= 0b1000; // Q3 = 1
			msg += "Q3=PQ3=1,";
		}
		msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	}
	if (log != nullptr)
		log->log(msg);

	if (_pos == YOUNG ? !static_cast<bool>(_reg_q & 0b0001) : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ops->S - ops->R - 1 + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=S-R-1+C0=~" + std::to_string(ops->S) +
				"-" + std::to_string(ops->R) + "-1+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
}

// Лог. F/2->Y, Лог. Q/2->Q
void K1804BC2::__special_load__0110(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	std::string msg = "LOAD SP: Лог. F/2->Y, Лог. Q/2->Q\n";
	// PF0=F0
	if ((res->Y & 0b0001))
	{
		setState(_time, _pin_PF0, 1); // PF0 = 1
		msg += "PF0=1,";
	}
	else
	{
		setState(_time, _pin_PF0, -1); // PF0 = 0
		msg += "PF0=0,";
	}
	// Y=F/2
	res->Y = res->F >> 1;
	// Если не старшая МПС, то PF3 вход
	if (_pos != OLD)
	{
		// Y3 = PF3
		if (isHigh(_pin_PF3))
		{
			res->Y |= 0b1000; // F3 = 1
			msg += "F3=PF3=1,";
		}
	}
	else
		setState(_time, _pin_PF3, 0);
	msg += "Y=F/2=" + std::to_string(res->Y) + "\n";
	// // PQ0=Q0
	// if ((_reg_q & 0b0001))
	// {
	// 	setState(_time, _pin_PQ0, 1); // PQ0 = 1
	// 	msg += "PQ0=1,";
	// }
	// else
	// {
	// 	setState(_time, _pin_PQ0, -1); // PQ0 = 0
	// 	msg += "PQ0=0,";
	// }
	// if (isLow(_pin_IEN))
	// {
	// 	// Q=Q/2
	// 	_reg_q = _reg_q >> 1;
	// 	// Q3 = PQ3
	// 	if (isHigh(_pin_PQ3))
	// 	{
	// 		_reg_q |= 0b1000; // Q3 = 1
	// 		msg += "Q3=PQ3=1,";
	// 	}
	// 	msg += "Q=Q/2=" + std::to_string(_reg_q) + "\n";
	// }
	if (log != nullptr)
		log->log(msg);
}

// S+C0
void K1804BC2::__special__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	res->F = res->Y = ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU_SPECIAL: Y=S+C0=" + std::to_string(ops->S) +
			"+" + std::to_string(c0) + "=" + std::to_string(res->Y));
	}
}

// F->Y, PF3=F3, Лог. 2Q->Q
void K1804BC2::__special_load__1000(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	// PF3=F3
	if (res->F & 0b1000)
	{
		setState(_time, _pin_PF3, 1);
	}
	else
	{
		setState(_time, _pin_PF3, -1);
	}
	// PQ3 = Q3
	if (_reg_q & 0b1000)
	{
		setState(_time, _pin_PQ3, 1); //  PQ3 = 1
	}
	else
	{
		setState(_time, _pin_PQ3, -1); //  PQ3 = 0
	}
	if (isLow(_pin_IEN))
	{
		// 2Q->Q
		_reg_q = (_reg_q << 1) & 0b1111;
		if (isHigh(_pin_PQ0))
		{
			_reg_q |= 0b0001; // Q0 = PQ0 = 1
		}
	}
	setState(_time, _pin_PF0, 0);
}

// S+C0
void K1804BC2::__special__1010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	res->F = res->Y = ops->S + c0;
	log->log("ALU_SPECIAL: Y=S+C0=" + std::to_string(ops->S) +
		"+" + std::to_string(c0) + "=" + std::to_string(res->Y));

	// Если старшая МПС: PF3 = R3 xor F3
	if (_pos == OLD)
	{
		if (static_cast<bool>(ops->R & 0b1000) != static_cast<bool>(res->F & 0b1000))
		{
			setState(_time, _pin_PF3, 1);
		}
		else
		{
			setState(_time, _pin_PF3, -1);
		}
	} // PF3 =F3
	else
	{
		if (res->F & 0b1000)
		{
			setState(_time, _pin_PF3, 1);
		}
		else
		{
			setState(_time, _pin_PF3, -1);
		}
	}
	if (isLow(_pin_IEN))
		_reg_sign = !(static_cast<bool>(res->F & 0b1000) != static_cast<bool>(ops->R & 0b1000));
}

// Лог. 2F->Y, Лог. 2Q->Q
void K1804BC2::__special_load__1010(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	// 2F->Y
	res->Y = res->Y << 1;
	if (isHigh(_pin_PF0))
	{
		res->Y |= 0b0001; // F0=PF0=1
	}
	// PF3 выставлен в special_1010
	// PQ3 = Q3
	if (_reg_q & 0b1000)
	{
		setState(_time, _pin_PQ3, 1); //  PQ3 = 1
	}
	else
	{
		setState(_time, _pin_PQ3, -1); //  PQ3 = 0
	}
	if (isLow(_pin_IEN))
	{
		// 2Q->Q
		_reg_q = (_reg_q << 1) & 0b1111;
		if (isHigh(_pin_PQ0))
		{
			_reg_q |= 0b0001; // Q0 = PQ0 = 1
		}
	}
}

// S+R+C0, если Z=0; S-R-1+C0, если Z=1
void K1804BC2::__special__1100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	if (_pos == OLD ? !_reg_sign : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + ops->R + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+R+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(ops->R) + "+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ops->S - ops->R - 1 + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=S-R-1+C0=" + std::to_string(ops->S) +
				"-" + std::to_string(ops->R) + "-1+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
	// Если старшая МПС: PF3 = ~(R3 xor F3)
	if (_pos == OLD)
	{
		if ((ops->R & 0b1000) ^ (res->Y & 0b1000))
		{
			setState(_time, _pin_PF3, -1);
		}
		else
		{
			setState(_time, _pin_PF3, 1);
		}
	} // PF3 =F3
	else
	{
		if (res->F & 0b1000)
		{
			setState(_time, _pin_PF3, 1);
		}
		else
		{
			setState(_time, _pin_PF3, -1);
		}
	}
	if (isLow(_pin_IEN))
		_reg_sign = !(static_cast<bool>(res->F & 0b1000) != static_cast<bool>(ops->R & 0b1000));
}

// Лог. 2F->Y, Лог. 2Q->Q
void K1804BC2::__special_load__1100(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	// 2F->Y
	res->Y = res->Y << 1;
	if (isHigh(_pin_PF0))
	{
		res->Y |= 0b0001; // F0=PF0=1
	}
	// PF3 выставлен в special_1100
	// PQ3 = Q3
	if (_reg_q & 0b1000)
	{
		setState(_time, _pin_PQ3, 1); //  PQ3 = 1
	}
	else
	{
		setState(_time, _pin_PQ3, -1); //  PQ3 = 0
	}
	if (isLow(_pin_IEN))
	{
		// 2Q->Q
		_reg_q = (_reg_q << 1) & 0b1111;
		if (isHigh(_pin_PQ0))
		{
			_reg_q |= 0b0001; // Q0 = PQ0 = 1
		}
	}
}

// S+R+C0, если Z=0; S-R-1+C0, если Z=1
void K1804BC2::__special__1110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
		return;

	if (_pos == OLD ? !_reg_sign : isLow(_pin_Z))
	{
		res->F = res->Y = ops->S + ops->R + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=0, Y=S+R+C0=" + std::to_string(ops->S) +
				"+" + std::to_string(ops->R) + "+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
	else
	{
		res->F = res->Y = ops->S - ops->R - 1 + c0;
		if (log != nullptr)
		{
			log->log("ALU_SPECIAL: Z=1, Y=S-R-1+C0=" + std::to_string(ops->S) +
				"-" + std::to_string(ops->R) + "-1+" + std::to_string(c0) +
				"=" + std::to_string(res->Y));
		}
	}
}

// F->Y, Лог. 2Q->Q
void K1804BC2::__special_load__1110(ALUReasult* res, ILogger* log)
{
	if (res == nullptr)
		return;
	// PF3=F3
	if (res->F & 0b1000)
	{
		setState(_time, _pin_PF3, 1);
	}
	else
	{
		setState(_time, _pin_PF3, -1);
	}
	// PQ3 = Q3
	if (_reg_q & 0b1000)
	{
		setState(_time, _pin_PQ3, 1); //  PQ3 = 1
	}
	else
	{
		setState(_time, _pin_PQ3, -1); //  PQ3 = 0
	}
	if (isLow(_pin_IEN))
	{
		// 2Q->Q
		_reg_q = (_reg_q << 1) & 0b1111;
		if (isHigh(_pin_PQ0))
		{
			_reg_q |= 0b0001; // Q0 = PQ0 = 1
		}
	}
}

void K1804BC2::load(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	setState(_time, _pin_PF3, 0);
	setState(_time, _pin_PF0, 0);
	setState(_time, _pin_PQ3, 0);
	setState(_time, _pin_PQ0, 0);
	switch (cmd->To)
	{
		//arith F/2 -> Y 
	case 0:
		__load__0000(cmd, res, log);
		break;
		//log F/2 -> Y
	case 1:
		__load__0001(cmd, res, log);
		break;
		//arith F/2 -> Y  log Q/2 -> Q 
	case 2:
		__load__0010(cmd, res, log);
		break;
		//log F/2 -> Y  log Q/2 -> Q 
	case 3:
		__load__0011(cmd, res, log);
		break;
		// F -> Y Q-hold
	case 4:
		__load__0100(cmd, res, log);
		break;
		// F -> Y log Q/2 -> Q
	case 5:
		__load__0101(cmd, res, log);
		break;
		// F -> Y F -> Q (W = 1)
	case 6:
		__load__0110(cmd, res, log);
		break;
		// F -> Y F -> Q (W = 0)
	case 7:
		__load__0111(cmd, res, log);
		break;
		//arith 2F -> Y Q-hold
	case 8:
		__load__1000(cmd, res, log);
		break;
		// log 2F -> Y Q-hold
	case 9:
		__load__1001(cmd, res, log);
		break;
		// arith 2F -> Y  log 2Q -> Q
	case 10:
		__load__1010(cmd, res, log);
		break;
		// log 2F -> Y log 2Q -> Q
	case 11:
		__load__1011(cmd, res, log);
		break;
		// F -> Y log Q-hold
	case 12:
		__load__1100(cmd, res, log);
		break;
		// F -> Y log 2Q -> Q
	case 13:
		__load__1101(cmd, res, log);
		break;
		// PR0 -> Y[0,1,2,3] Q-hold
	case 14:
		__load__1110(cmd, res, log);
		break;
		// F - > Y Q-hold
	case 15:
		__load__1111(cmd, res, log);
		break;
	}
	// в res->Y лежит всегда нужный выход
	_reg_y = res->Y & 0b1111;
}

void K1804BC2::special(bool c0, const Operands* ops, const uint8_t code, ALUReasult* res, ILogger* log)
{
	_special = true;
	// W = 0;
	if (isLow(_pin_LSS))
		setState(_time, _pin_W_MSS, -1);
	switch (code)
	{
	case 0:
		__special__0000(c0, ops, res, log);
		__special_load__0000(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 2:
		__special__0010(c0, ops, res, log);
		__special_load__0010(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 4:
		__special__0100(c0, ops, res, log);
		__special_load__0100(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 5:
		__special__0101(c0, ops, res, log);
		__special_load__0101(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 6:
		__special__0110(c0, ops, res, log);
		__special_load__0110(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 8:
		__special__1000(c0, ops, res, log);
		__special_load__1000(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 10:
		__special__1010(c0, ops, res, log);
		__special_load__1010(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 12:
		__special__1100(c0, ops, res, log);
		__special_load__1100(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	case 14:
		__special__1110(c0, ops, res, log);
		__special_load__1110(res, log);
		computeFlags(_special, res, c0, ops, code);
		break;
	default:
		log->log("Not defined");
		break;
	}
}


VOID K1804BC2::simulate(ABSTIME time, DSIMMODES mode)
{
	auto log = new Logger();
	_time = time;

	if (isPosedge(_pin_T))
	{
		// получение микрокоманды
		auto cmd = getCommand();
		log->log("CMD #" + std::to_string(_dbg_counter++));

		// загрузка оператов
		auto ops = getOperands(cmd, log);
		// вычисление в АЛУ
		auto result = ALU(cmd->C0, cmd->Alu, ops, log);
		// выставление флагов
		// Z - I/O
		_reg_z = result->Z;
		_reg_c4 = result->C4;
		_reg_p_ovr = result->P_OVR;
		_reg_g_n = result->G_N;
		_reg_y = result->Y;
		// загрузка результов если не были спец функции(в них свой load)
		if (!_special)
		{
			load(cmd, result, log);
		}
		_special = false;
		// логирование
		_inst->log(const_cast<CHAR*>(log->getLog().c_str()));

		if (isHigh(_pin_IEN))
			setState(time, _pin_W_MSS, 1);
		else
			setState(time, _pin_W_MSS, result->W ? 1 : -1);

		delete result;
		delete ops;
		delete cmd;
	}

	if (isLow(_pin_OEY))
	{
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i) ? 1 : -1);
		setState(time, _pin_Z, _reg_z ? 1 : -1);
		setState(time, _pin_C4, _reg_c4 ? 1 : -1);
		setState(time, _pin_P_OVR, _reg_p_ovr ? 1 : -1);
		setState(time, _pin_G_N, _reg_g_n ? 1 : -1);
	}
	else
	{
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], 0);
	}

	if (isLow(_pin_T) && isLow(_pin_WE))
	{
		auto pin_b = genValue(_pin_B, REGISTER_SIZE);
		auto pin_y = genValue(_pin_Y, REGISTER_SIZE);
		_regs[pin_b] = pin_y;
	}

	if (isLow(_pin_OEB))
	{
		auto pin_b = genValue(_pin_B, REGISTER_SIZE);
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_DB[i], _regs[pin_b] & (1 << i) ? 1 : -1);
	}

	delete log;
}

int K1804BC2::getPosition()
{
	if (islow(_pin_LSS->istate()))
		return YOUNG; //младшая
	else
	{
		if (ishigh(_pin_W_MSS->istate()))
			return MID; // средняя
		else
			return OLD; // старшая
	}
}

bool K1804BC2::isHigh(IDSIMPIN* pin)
{
	return ishigh(pin->istate());
}

bool K1804BC2::isLow(IDSIMPIN* pin)
{
	return islow(pin->istate());
}

bool K1804BC2::isNegedge(IDSIMPIN* pin)
{
	return pin->isnegedge();
}

bool K1804BC2::isPosedge(IDSIMPIN* pin)
{
	return pin->isposedge();
}

void K1804BC2::setState(ABSTIME time, IDSIMPIN* pin, int set)
{
	switch (set)
	{
	case 1:
		pin->setstate(time, 1, SHI);
		break;
	case -1:
		pin->setstate(time, 1, SLO);
		break;
	case 0:
		pin->setstate(time, 1, FLT);
		break;
	}
}

bool K1804BC2::compute_G(uint8_t G, uint8_t P)
{
	return G & 0b1000 || (G & 0100 && P & 0b1000) || (G & 0b0010 && P & 0b0100 && P & 0b1000) ||
		(G & 0b0001 && P & 0b0010 && P & 0b0100 && P & 0b1000);
}

bool K1804BC2::compute_P(uint8_t P)
{
	return P & 0b0001 && P & 0b0010 && P & 0b0100 && P & 0b1000;
}

bool K1804BC2::compute_C3(bool c0, uint8_t G, uint8_t P)
{
	return G & 0b0100 || G & 0b0010 && P & 0b0100 || G & 0b0001 && P & 0b0010 && P & 0b0100 ||
		c0 && P & 0b0001 && P & 0b0010 && P & 0b0100;
}


void K1804BC2::computeFlags(bool special, ALUReasult* res, bool c0, const Operands* ops, uint8_t code)
{
	uint8_t G;
	uint8_t P;
	if (!special)
	{
		switch (code)
		{
		case 0:
			G = 0b0000;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, 0b1111);
			res->Z = !(res->Y & 0b1111);
			break;
		case 1:
			G = ~ops->R & ops->S;
			P = ~ops->R | ops->S;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 2:
			G = ops->R & ~ops->S;
			P = ops->R | ~ops->S;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 3:
			G = ops->R & ops->S;
			P = ops->R | ops->S;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 4:
			G = 0b0000;
			P = ops->S;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 5:
			G = 0b0000;
			P = ~ops->S;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;

		case 6:
			G = 0b0000;
			P = ops->R;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 7:
			G = 0b0000;
			P = ~ops->R;
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(res->Y & 0b1111);
			break;
		case 8:
			G = 0b0000;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 9:
			G = ~ops->R & ops->S;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 10:
			G = ops->R & ops->S;
			P = ops->R | ops->S;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 11:
			G = ~ops->R & ops->S;
			P = ~ops->R | ops->S;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 12:
			G = ops->R & ops->S;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 13:
			G = ~ops->R & ~ops->S;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 14:
			G = ops->R & ops->S;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		case 15:
			G = ~ops->R & ~ops->S;
			P = 0b1111;
			res->C4 = false;
			res->P_OVR = false;
			if (_pos == OLD)
				res->G_N = res->F & 0b1000;
			else
				res->G_N = !compute_G(G, P);
			res->Z = !(res->Y & 0b1111);
			break;
		}
	}
	else
	{
		switch (code)
		{
		case 0:
			if (_pos == YOUNG)
				res->Z = _reg_q & 0b0001;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				G = 0b0000;
				P = ops->S;
			}
			else
			{
				G = ops->R & ops->S;
				P = ops->R | ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			if (_pos == OLD)
				res->C4 ? res->Y |= 0b1000 : res->Y &= 0b0111;
			break;
		case 2:
			if (_pos == YOUNG)
				res->Z = _reg_q & 0b0001;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				G = 0b0000;
				P = ops->S;
			}
			else
			{
				G = ops->R & ops->S;
				P = ops->R | ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			if (_pos == OLD)
				(static_cast<bool>(res->F & 0b1000) != res->P_OVR) ? res->Y |= 0b1000 : res->Y &= 0b0111;
			break;
		case 4:
			if (isLow(_pin_LSS))
			{
				G = ops->S & 0b0001;
				P = ops->S | 0b0001;
			}
			else
			{
				G = 0b0000;
				P = ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			break;
		case 5:
			G = 0b0000;
			if (_pos == OLD)
				res->Z = ops->S & 0b1000;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				P = ops->S;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				P = ~ops->S;
				res->G_N = static_cast<bool>(res->F & 0b1000) != static_cast<bool>(ops->S & 0b1000);
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			break;
		case 6:
			if (_pos == YOUNG)
				res->Z = _reg_q & 0b0001;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				G = 0b0000;
				P = ops->S;
			}
			else
			{
				G = ~ops->R & ops->S;
				P = ~ops->R | ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			if (_pos == OLD)
				(static_cast<bool>(res->F & 0b1000) != res->P_OVR) ? res->Y |= 0b1000 : res->Y &= 0b0111;
			break;
		case 8:
			G = 0b0000;
			P = ops->S;
			if (_pos == OLD)
			{
				res->C4 = static_cast<bool>(_reg_q & 0b1000) != static_cast<bool>(_reg_q & 0b0100);
				res->P_OVR = static_cast<bool>(_reg_q & 0b0010) != static_cast<bool>(_reg_q & 0b0001);
				res->G_N = _reg_q & 0b01000;
			}
			else
			{
				res->C4 = compute_G(G, P) || compute_P(P) && c0;
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !(_reg_q & 0b1111);
			break;
		case 10:
			G = 0b0000;
			P = ops->S;
			if (_pos == OLD)
			{
				res->C4 = static_cast<bool>(res->F & 0b1000) != static_cast<bool>(res->F & 0b0100);
				res->P_OVR = static_cast<bool>(res->F & 0b0010) != static_cast<bool>(res->F & 0b0001);
				res->G_N = res->F & 0b01000;
			}
			else
			{
				res->C4 = compute_G(G, P) || compute_P(P) && c0;
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			res->Z = !((_reg_q & 0b1111) | (res->F & 0b1111));
			break;
		case 12:
			if (_pos == OLD)
				res->Z = _reg_sign;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				G = ops->R & ops->S;
				P = ops->R | ops->S;
			}
			else
			{
				G = ~ops->R & ops->S;
				P = ~ops->R | ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			break;
		case 14:
			if (_pos == OLD)
				res->Z = _reg_sign;
			else
				res->Z = isHigh(_pin_Z);
			if (!res->Z)
			{
				G = ops->R & ops->S;
				P = ops->R | ops->S;
			}
			else
			{
				G = ~ops->R & ops->S;
				P = ~ops->R | ops->S;
			}
			res->C4 = compute_G(G, P) || compute_P(P) && c0;
			if (_pos == OLD)
			{
				res->P_OVR = compute_C3(c0, G, P) != res->C4;
				res->G_N = res->F & 0b1000;
			}
			else
			{
				res->P_OVR = !compute_P(P);
				res->G_N = !compute_G(G, P);
			}
			break;
		}
	}
}

INT K1804BC2::isdigital(CHAR* pinname)
{
	return 1;
}

VOID K1804BC2::runctrl(RUNMODES mode)
{
}

VOID K1804BC2::actuate(REALTIME time, ACTIVESTATE newstate)
{
}

VOID K1804BC2::callback(ABSTIME time, EVENTID eventid)
{
}

BOOL K1804BC2::indicate(REALTIME time, ACTIVEDATA* data)
{
	return FALSE;
}
