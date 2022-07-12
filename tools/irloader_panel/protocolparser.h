#ifndef PROTOCOLPARSER_H
#define PROTOCOLPARSER_H

/*
2-byte token (0xDA, 0xBA)
1-byte ID (0x01 - console, 0x02 - bitmap, 0x03 - screen control, 0x04 - ui control)
2-byte length
length-bytes payload
<optional> 2-bytes crc

examples:
0xDA 0xBA 0x01 0x02 0x00 0x30 0x31	- prints "01" on the console
0xDA 0xBA 0x02 0x80 0x25 0xXX ...	- example bitmap for 320x240 screen size
*/

class ProtocolParser
{
public:
	typedef enum {
		PARSE_OK,
		PARSE_COMPLETE,
		PARSE_ERROR
	} E_PARSE_RES;

	typedef enum {
		CMD_ID_CONSOLE,
		CMD_ID_SET_PIXEL,
		CMD_ID_FILL_AREA,
		CMD_ID_SCREEN_CONTROL,
		CMD_ID_UI_CONTROL,
		CMD_ID_INVALID
	} E_CMD_ID;

	ProtocolParser();
	E_PARSE_RES parse(unsigned char *&buff, int *length);
	void reset();
	E_CMD_ID getCmdId();
	unsigned short getLength();
	unsigned char* getPayload();
private:
	enum {
		TOKEN_1 = 0xDA,
		TOKEN_2 = 0xBA
	};

	typedef enum {
		STATE_BEGIN,
		STATE_TOKEN1,
		STATE_TOKEN2,
		STATE_ID,
		STATE_LEN1,
		STATE_LEN2,
		STATE_LEN_CHKSUM,
		STATE_PAYLOAD,
		STATE_CHKSUM,
		STATE_END,
		STATE_ERROR
	} E_PARSE_STATE;

	unsigned char mPayload[65536];
	unsigned short mPayLoadIdx;
	unsigned short mLength;
	unsigned char mPayloadLenChksum;
	unsigned char mChksum;
	E_CMD_ID mCmdId;
	E_PARSE_STATE mState;
};

#endif // PROTOCOLPARSER_H
