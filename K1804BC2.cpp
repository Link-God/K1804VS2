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
	// TODO проверить 
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
	_pin_PR0 = _inst->getdsimpin(const_cast<CHAR*>("PR0"), true);
	_pin_PR3 = _inst->getdsimpin(const_cast<CHAR*>("PR3"), true);
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
	_pin_F3 = _inst->getdsimpin(const_cast<CHAR*>("F3"), true);
	setState(0, _pin_F3, 0);
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
	cmd->position = getPosition();
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
	// TODO проверить так ли нужно вызывавать
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
	{
		return;
	}
	if (i0 == 0)
	{
		special(c0, ops, special_code, res, log);
	}
	else
	{
		res->Y = 0b1111;
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
	res->Y = ops->S - ops->R - 1 + c0;
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
	res->Y = ops->R - ops->S - 1 + c0;
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
	res->Y = ops->R + ops->S + c0;
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
	res->Y = ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=S+C0=" + std::to_string(ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

//~S + C0
void K1804BC2::__alu__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->Y = ~ops->S + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=~S+C0=" + std::to_string(~ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

//R + C0
void K1804BC2::__alu__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->Y = ops->R + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=R+C0=" + std::to_string(ops->R) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

//~R + C0
void K1804BC2::__alu__0111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->Y = ~ops->R + c0;
	if (log != nullptr)
	{
		log->log("ALU: Y=~S+C0=" + std::to_string(~ops->R) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
}

//0000
void K1804BC2::__alu__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	res->Y = 0b0000;
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
	res->Y = ~ops->R & ops->S;
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
	res->Y = ~(ops->R ^ ops->S);
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
	res->Y = ops->R ^ ops->S;
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
	res->Y = ops->R & ops->S;
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
	res->Y = ~(ops->R | ops->S);
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
	res->Y = ~(ops->R & ops->S);
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
	res->Y = ops->R | ops->S;
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
	// TODO : исключения в некторых случаях в зависимотси от I0
	switch (code)
	{
		// I0 = 0 -> специальный функции
		// I0 = 1 -> 1111
	case 0:
		__alu__0000(c0, I0, ops, special_code, res, log);
		break;
		//S - R - 1 + C0
	case 1:
		__alu__0001(c0, ops, res, log);
		break;
		//R - S - 1 + C0
	case 2:
		__alu__0010(c0, ops, res, log);
		break;
		//R + S + C0
	case 3:
		__alu__0011(c0, ops, res, log);
		break;
		//S + C0
	case 4:
		__alu__0100(c0, ops, res, log);
		break;
		//~S + C0
	case 5:
		__alu__0101(c0, ops, res, log);
		break;
		//R + C0
	case 6:
		__alu__0110(c0, ops, res, log);
		break;
		//~R + C0
	case 7:
		__alu__0111(c0, ops, res, log);
		break;
		//0000
	case 8:
		__alu__1000(c0, ops, res, log);
		break;
		//~R & S
	case 9:
		__alu__1001(c0, ops, res, log);
		break;
		//~(R ^ S)
	case 10:
		__alu__1010(c0, ops, res, log);
		break;
		//R ^ S
	case 11:
		__alu__1011(c0, ops, res, log);
		break;
		//R & S
	case 12:
		__alu__1100(c0, ops, res, log);
		break;
		//~(R | S)
	case 13:
		__alu__1101(c0, ops, res, log);
		break;
		//~(R & S)
	case 14:
		__alu__1110(c0, ops, res, log);
		break;
		//R | S
	case 15:
		__alu__1111(c0, ops, res, log);
		break;
	}
	return res;
}


void K1804BC2::__load__0000(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0001(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0010(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0011(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0100(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0101(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}


void K1804BC2::__load__0110(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__0111(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1000(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1001(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1010(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1011(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1100(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1101(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1110(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__load__1111(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	// TODO
}

void K1804BC2::__special__0000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + c0;
	else
		F = ops->R + ops->S + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__0010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + c0;
	else
		F = ops->R + ops->S + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__0100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F = ops->S + 1 + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + c0;
	else
		F = ~ops->S + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + c0;
	else
		F = ops->S - ops->R - 1 + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F = ops->S + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__1010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F = ops->S + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__1100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + ops->R + c0;
	else
		F = ops->S - ops->R - 1 + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}

void K1804BC2::__special__1110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log)
{
	if (ops == nullptr || res == nullptr)
	{
		return;
	}
	uint8_t F;
	if (isLow(_pin_Z))
		F = ops->S + ops->R + c0;
	else
		F = ops->S - ops->R - 1 + c0;

	// TODO load (сдвиги и тд)
	// TODO логи
}


void K1804BC2::load(const CommandFields* cmd, ALUReasult* res, ILogger* log)
{
	setState(_time, _pin_PR3, 0);
	setState(_time, _pin_PR0, 0);
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
	switch (code)
	{
	case 0:
		__special__0000(c0, ops, res, log);
		break;
	case 2:
		__special__0010(c0, ops, res, log);
		break;
	case 4:
		__special__0100(c0, ops, res, log);
		break;
	case 5:
		__special__0101(c0, ops, res, log);
		break;
	case 6:
		__special__0110(c0, ops, res, log);
		break;
	case 8:
		__special__1000(c0, ops, res, log);
		break;
	case 10:
		__special__1010(c0, ops, res, log);
		break;
	case 12:
		__special__1100(c0, ops, res, log);
		break;
	case 14:
		__special__1110(c0, ops, res, log);
		break;
	default:
		// TODO throw ??
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
		//_reg_z = result->Z;
		_reg_f3 = result->F3;
		_reg_c4 = result->C4;
		_reg_p_ovr = result->P_OVR;
		_reg_g_n = result->G_N;
		// загрузка результов
		load(cmd, result, log);
		// логирование
		_inst->log(const_cast<CHAR*>(log->getLog().c_str()));

		delete result;
		delete ops;
		delete cmd;
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

void K1804BC2::computeFlags(ALUReasult* res, bool c0, const Operands* ops, uint8_t aluCode)
{
	// TODO
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
