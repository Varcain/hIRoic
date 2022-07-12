#include "protocolparser.h"
#include <QDebug>

ProtocolParser::ProtocolParser()
{
	mLength = 0;
	mCmdId = CMD_ID_INVALID;
	mState = STATE_BEGIN;
	mPayLoadIdx = 0;
	mChksum = 0;
}

void ProtocolParser::reset()
{
	mLength = 0;
	mCmdId = CMD_ID_INVALID;
	mState = STATE_BEGIN;
	mPayLoadIdx = 0;
	mChksum = 0;
}

ProtocolParser::E_PARSE_RES ProtocolParser::parse(unsigned char *&buff, int *length)
{
	ProtocolParser::E_PARSE_RES res;

	//qDebug() << "Parsing " << *length << "bytes";

	if (buff == 0) {
		qDebug() << "PARSE_ERROR: buff = NULL";
		return PARSE_ERROR;
	}
	if (length == 0) {
		qDebug() << "PARSE_ERROR: length = NULL";
		return PARSE_ERROR;
	}
	if (*length == 0) {
		qDebug() << "PARSE_ERROR: length = 0";
		return PARSE_ERROR;
	}
	if (mState == STATE_ERROR) {
		qDebug() << "PARSE_ERROR: STATE_ERROR";
		return PARSE_ERROR;
	}
	if (mState == STATE_END) {
		qDebug() << "STATE_END: parse complete";
		return PARSE_COMPLETE;
	}

	do {
		(*length)--;
		//qDebug() << "Received char " << QString("%1").arg(*buff , 0, 16);
		switch (mState) {
		case STATE_BEGIN:
		case STATE_TOKEN1:
			if (*buff == TOKEN_1) {
				mState = STATE_TOKEN2;
			} else {
				mState = STATE_ERROR;
			}
			break;

		case STATE_TOKEN2:
			if (*buff == TOKEN_2) {
				mState = STATE_ID;
			} else {
				mState = STATE_ERROR;
			}
			break;

		case STATE_ID:
			E_CMD_ID cmd;
			cmd = (ProtocolParser::E_CMD_ID)*buff;
			switch (cmd) {
			case CMD_ID_SET_PIXEL:
			case CMD_ID_FILL_AREA:
			case CMD_ID_CONSOLE:
			case CMD_ID_SCREEN_CONTROL:
			case CMD_ID_UI_CONTROL:
				mCmdId = cmd;
				mState = STATE_LEN1;
				break;
			default:
				mState = STATE_ERROR;
			}

			break;

		case STATE_LEN1:
			mLength = *buff & 0xFF;
			mPayloadLenChksum = *buff;
			mState = STATE_LEN2;
			break;

		case STATE_LEN2:
			mLength += (*buff << 8) & 0xFF00;
			mPayloadLenChksum ^= *buff;
			//qDebug() << "Payload length " << mLength;
			mState = STATE_LEN_CHKSUM;
			break;

		case STATE_LEN_CHKSUM:
			if (mPayloadLenChksum == *buff) {
				mState = STATE_PAYLOAD;
			} else {
				qDebug() << "Bad len checksum: got " << mPayloadLenChksum << "expected" << *(buff-1);
				mState = STATE_ERROR;
			}
			break;

		case STATE_PAYLOAD:
			if (mPayLoadIdx < (mLength - 1)) {
				mPayload[mPayLoadIdx++] = *buff;
			} else {
				mPayload[mPayLoadIdx++] = *buff;
				mState = STATE_CHKSUM;
			}
			break;

		case STATE_CHKSUM:
			if (mChksum == (*buff++)) {
				mState = STATE_END;
				return PARSE_COMPLETE;
			} else {
				qDebug() << "Bad checksum: got " << mChksum << "expected" << *(buff-1);
				mState = STATE_ERROR;
			}
			break;

		case STATE_END:
			return PARSE_COMPLETE;
			break;

		default:
			return PARSE_ERROR;
		}

		mChksum ^= (*buff);
		buff++;

	} while ((*length) > 0 && mState != STATE_ERROR);

	if (mState == STATE_ERROR) {
		qDebug() << "Bad byte: " << *(buff-1);
		res = PARSE_ERROR;
	} else if (mState == STATE_END) {
		res = PARSE_COMPLETE;
	} else {
		res = PARSE_OK;
	}
	return res;
}

ProtocolParser::E_CMD_ID ProtocolParser::getCmdId()
{
	return mCmdId;
}

unsigned short ProtocolParser::getLength()
{
	return mLength;
}

unsigned char* ProtocolParser::getPayload()
{
	return mPayload;
}
