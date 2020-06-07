#pragma once
#include <windows.h>
#include "pch.h"
#include "vsm.h"
#include <string>
#include <cinttypes>
#include "logger.h"

const int YOUNG = 0;
const int MID = 1;
const int OLD = 2;
const int REGISTER_SIZE = 4;
const int NUM_OF_REGISTERS = 16;
const int PIN_I_SIZE = 9;


class K1804BC2 : public IDSIMMODEL
{
	IINSTANCE* _inst;
	IDSIMCKT* _ckt;
	ABSTIME _time;
	int _pos = OLD;
	uint32_t _dbg_counter = 0;
	uint8_t _regs[NUM_OF_REGISTERS];
	uint8_t _reg_q;
	uint8_t _reg_y;
	uint8_t _reg_b;
	bool _reg_z;
	bool _reg_f3;
	bool _reg_c4;
	bool _reg_p_ovr;
	bool _reg_g_n;
	IDSIMPIN* _pin_A[REGISTER_SIZE];
	IDSIMPIN* _pin_B[REGISTER_SIZE];
	IDSIMPIN* _pin_DA[REGISTER_SIZE];
	IDSIMPIN* _pin_DB[REGISTER_SIZE];
	IDSIMPIN* _pin_I[PIN_I_SIZE];
	IDSIMPIN* _pin_OEY;
	IDSIMPIN* _pin_OEB;
	IDSIMPIN* _pin_EA;
	IDSIMPIN* _pin_T;
	IDSIMPIN* _pin_C0;
	IDSIMPIN* _pin_Y[REGISTER_SIZE];
	IDSIMPIN* _pin_C4;
	IDSIMPIN* _pin_Z;
	IDSIMPIN* _pin_F3;
	IDSIMPIN* _pin_P_OVR;
	IDSIMPIN* _pin_G_N;
	IDSIMPIN* _pin_PF0;
	IDSIMPIN* _pin_PQ0;
	IDSIMPIN* _pin_PF3;
	IDSIMPIN* _pin_PQ3;
	IDSIMPIN* _pin_WE;
	IDSIMPIN* _pin_IEN;
	IDSIMPIN* _pin_LSS;
	IDSIMPIN* _pin_W_MSS;
	uint8_t genValue(IDSIMPIN** pins, size_t n, size_t offset = 0);
	bool isHigh(IDSIMPIN* pin);
	bool isLow(IDSIMPIN* pin);
	bool isNegedge(IDSIMPIN* pin);
	bool isPosedge(IDSIMPIN* pin);
	void setState(ABSTIME time, IDSIMPIN* pin, int set);

	struct CommandFields
	{
		uint8_t From;
		uint8_t Alu;
		uint8_t To;
		uint8_t A;
		uint8_t B;
		uint8_t DA;
		uint8_t DB;
		bool I0;
		bool C0;
		int position; // позиция в секции
	};

	CommandFields* getCommand();

	struct Operands
	{
		uint8_t R;
		uint8_t S;
	};

	Operands* getOperands(const CommandFields* cmd, ILogger* log);

	void __download__000(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__001(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__01X(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__100(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__101(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__11X(const CommandFields* cmd, Operands* ops, ILogger* log);

	struct ALUReasult
	{
		uint8_t Y; // Результат Алу
		bool P_OVR; // Переполнение
		bool C4; // Перенос из старшего разряда
		bool F3; // Знак, содержимое старшего разряда АЛУ
		bool Z; // Признак нулевого результата
		bool G_N; // Сигнал генерации переноса из АЛУ
	};

	ALUReasult* ALU(bool c0, uint8_t code, const Operands* ops, ILogger* log);


	//I1-4
	// I0 важно
	void __alu__0000(bool c0, bool i0, const Operands* ops, const uint8_t special_code, ALUReasult* res, ILogger* log);
	void __alu__0001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__0111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__1111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);


	void load(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void special(bool c0, const Operands* ops, const uint8_t code, ALUReasult* res, ILogger* log);

	void __load__0000(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0001(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0010(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0011(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0100(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0101(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0110(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__0111(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1000(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1001(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1010(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1011(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1100(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1101(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1110(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__1111(const CommandFields* cmd, ALUReasult* res, ILogger* log);

	// TODO не все коды есть -> сделать исключения ?? по пинам 
	void __special__0000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__0010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__0100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__0101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__0110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__1000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__1010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__1100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __special__1110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);

	void __special_load__0000(ALUReasult* res, ILogger* log);
	void __special_load__0010(ALUReasult* res, ILogger* log);
	void __special_load__0100(ALUReasult* res, ILogger* log);
	void __special_load__0101(ALUReasult* res, ILogger* log);
	void __special_load__0110(ALUReasult* res, ILogger* log);
	void __special_load__1000(ALUReasult* res, ILogger* log);
	void __special_load__1010(ALUReasult* res, ILogger* log);
	void __special_load__1100(ALUReasult* res, ILogger* log);
	void __special_load__1110(ALUReasult* res, ILogger* log);

	void computeFlags(ALUReasult* res, bool c0, const Operands* ops, uint8_t aluCode);
	// можно наверное поменять структуру
	int getPosition();
public:
	INT isdigital(CHAR* pinname);
	VOID setup(IINSTANCE* inst, IDSIMCKT* dsim);
	VOID runctrl(RUNMODES mode);
	VOID actuate(REALTIME time, ACTIVESTATE newstate);
	BOOL indicate(REALTIME time, ACTIVEDATA* data);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback(ABSTIME time, EVENTID eventid);
};
