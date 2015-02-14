/* Copyright (c) 2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "USBAPI.h"

#if defined(USBCON)
#ifdef HID_ENABLED

//#define RAWHID_ENABLED

//	Singletons for mouse and keyboard

Mouse_ Mouse;
Keyboard_ Keyboard;

//================================================================================
//================================================================================

//	HID report descriptor

#define LSB(_x) ((_x) & 0xFF)
#define MSB(_x) ((_x) >> 8)

#define RAWHID_USAGE_PAGE	0xFFC0
#define RAWHID_USAGE		0x0C00
#define RAWHID_TX_SIZE 64
#define RAWHID_RX_SIZE 64

extern const u8 _hidReportDescriptor[] PROGMEM;
const u8 _hidReportDescriptor[] = {
	
	//	Mouse
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)	// 54
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION

	//	Keyboard
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)	// 47
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
   
	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    
	0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    
//ADDED to include LED report
	0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (1)
    0x29, 0x05,                    //   USAGE_MAXIMUM (5)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs) // LED report
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x01,                    //   OUTPUT (Constant) // padding 

	0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    
	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION

#ifdef RAWHID_ENABLED
	//	RAW HID
	0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE),	// 30
	0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE),

	0xA1, 0x01,				// Collection 0x01
    0x85, 0x03,             // REPORT_ID (3)
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,		// logical maximum = 255

	0x95, 64,				// report count TX
	0x09, 0x01,				// usage
	0x81, 0x02,				// Input (array)

	0x95, 64,				// report count RX
	0x09, 0x02,				// usage
	0x91, 0x02,				// Output (array)
	0xC0					// end collection
#endif
};

extern const HIDDescriptor _hidInterface PROGMEM;
const HIDDescriptor _hidInterface =
{
	D_INTERFACE(HID_INTERFACE,1,3,0,0),
	D_HIDREPORT(sizeof(_hidReportDescriptor)),
	D_ENDPOINT(USB_ENDPOINT_IN (HID_ENDPOINT_INT),USB_ENDPOINT_TYPE_INTERRUPT,0x40,0x01)
};

//================================================================================
//================================================================================
//	Driver

u8 _hid_protocol = 1;
u8 _hid_idle = 1;

#define WEAK __attribute__ ((weak))

int WEAK HID_GetInterface(u8* interfaceNum)
{
	interfaceNum[0] += 1;	// uses 1
	return USB_SendControl(TRANSFER_PGM,&_hidInterface,sizeof(_hidInterface));
}

int WEAK HID_GetDescriptor(int /* i */)
{
	return USB_SendControl(TRANSFER_PGM,_hidReportDescriptor,sizeof(_hidReportDescriptor));
}

void WEAK HID_SendReport(u8 id, const void* data, int len)
{
	USB_Send(HID_TX, &id, 1);
	USB_Send(HID_TX | TRANSFER_RELEASE,data,len);
}

bool WEAK HID_Setup(Setup& setup)
{
	u8 r = setup.bRequest;
	u8 requestType = setup.bmRequestType;
	if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType)
	{
		if (HID_GET_REPORT == r)
		{
			//HID_GetReport();
			return true;
		}
		if (HID_GET_PROTOCOL == r)
		{
			//Send8(_hid_protocol);	// TODO
			return true;
		}
	}
	
	if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType)
	{
		if (HID_SET_PROTOCOL == r)
		{
			_hid_protocol = setup.wValueL;
			return true;
		}

		if (HID_SET_IDLE == r)
		{
			_hid_idle = setup.wValueL;
			return true;
		}
		//ADDED To handle ledstatus
		if (HID_SET_REPORT == r)
		{
			if(setup.wLength == 2)
			{
				uint8_t data[2];
				if(2 == USB_RecvControl(data,2))
				{
					Keyboard.setLedStatus(data[1]);
				}
			}
		}
	}
	return false;
}

//================================================================================
//================================================================================
//	Mouse

Mouse_::Mouse_(void) : _buttons(0)
{
}

void Mouse_::begin(void) 
{
}

void Mouse_::end(void) 
{
}

void Mouse_::click(uint8_t b)
{
	_buttons = b;
	move(0,0,0);
	_buttons = 0;
	move(0,0,0);
}

void Mouse_::move(signed char x, signed char y, signed char wheel)
{
	u8 m[4];
	m[0] = _buttons;
	m[1] = x;
	m[2] = y;
	m[3] = wheel;
	HID_SendReport(1,m,4);
}

void Mouse_::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		move(0,0,0);
	}
}

void Mouse_::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void Mouse_::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool Mouse_::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
		return true;
	return false;
}

//================================================================================
//================================================================================
//	Keyboard

Keyboard_::Keyboard_(void) 
{
}

void Keyboard_::begin(void) 
{
}

void Keyboard_::end(void) 
{
}

void Keyboard_::sendReport(KeyReport* keys)
{
	HID_SendReport(2,keys,sizeof(KeyReport));
}

extern
const uint8_t _asciimap[128] PROGMEM;

#define SHIFT 0x80
const uint8_t _asciimap[128] =
{
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK  
	0x00,             // BEL
	0x2a,			// BS	Backspace
	0x2b,			// TAB	Tab
	0x28,			// LF	Enter
	0x00,             // VT 
	0x00,             // FF 
	0x00,             // CR 
	0x00,             // SO 
	0x00,             // SI 
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM 
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS 
	0x00,             // GS 
	0x00,             // RS 
	0x00,             // US 

	0x2c,		   //  ' '
	0x1e|SHIFT,	   // !
	0x34|SHIFT,	   // "
	0x20|SHIFT,    // #
	0x21|SHIFT,    // $
	0x22|SHIFT,    // %
	0x24|SHIFT,    // &
	0x34,          // '
	0x26|SHIFT,    // (
	0x27|SHIFT,    // )
	0x25|SHIFT,    // *
	0x2e|SHIFT,    // +
	0x36,          // ,
	0x2d,          // -
	0x37,          // .
	0x38,          // /
	0x27,          // 0
	0x1e,          // 1
	0x1f,          // 2
	0x20,          // 3
	0x21,          // 4
	0x22,          // 5
	0x23,          // 6
	0x24,          // 7
	0x25,          // 8
	0x26,          // 9
	0x33|SHIFT,      // :
	0x33,          // ;
	0x36|SHIFT,      // <
	0x2e,          // =
	0x37|SHIFT,      // >
	0x38|SHIFT,      // ?
	0x1f|SHIFT,      // @
	0x04|SHIFT,      // A
	0x05|SHIFT,      // B
	0x06|SHIFT,      // C
	0x07|SHIFT,      // D
	0x08|SHIFT,      // E
	0x09|SHIFT,      // F
	0x0a|SHIFT,      // G
	0x0b|SHIFT,      // H
	0x0c|SHIFT,      // I
	0x0d|SHIFT,      // J
	0x0e|SHIFT,      // K
	0x0f|SHIFT,      // L
	0x10|SHIFT,      // M
	0x11|SHIFT,      // N
	0x12|SHIFT,      // O
	0x13|SHIFT,      // P
	0x14|SHIFT,      // Q
	0x15|SHIFT,      // R
	0x16|SHIFT,      // S
	0x17|SHIFT,      // T
	0x18|SHIFT,      // U
	0x19|SHIFT,      // V
	0x1a|SHIFT,      // W
	0x1b|SHIFT,      // X
	0x1c|SHIFT,      // Y
	0x1d|SHIFT,      // Z
	0x2f,          // [
	0x31,          // bslash
	0x30,          // ]
	0x23|SHIFT,    // ^
	0x2d|SHIFT,    // _
	0x35,          // `
	0x04,          // a
	0x05,          // b
	0x06,          // c
	0x07,          // d
	0x08,          // e
	0x09,          // f
	0x0a,          // g
	0x0b,          // h
	0x0c,          // i
	0x0d,          // j
	0x0e,          // k
	0x0f,          // l
	0x10,          // m
	0x11,          // n
	0x12,          // o
	0x13,          // p
	0x14,          // q
	0x15,          // r
	0x16,          // s
	0x17,          // t
	0x18,          // u
	0x19,          // v
	0x1a,          // w
	0x1b,          // x
	0x1c,          // y
	0x1d,          // z
	0x2f|SHIFT,    // 
	0x31|SHIFT,    // |
	0x30|SHIFT,    // }
	0x35|SHIFT,    // ~
	0				// DEL
};

//Added to implement IT layout mapping
extern
const uint8_t _asciimapIT[128] PROGMEM;
#define ALTGR 0x40
const uint8_t _asciimapIT[128] =
{
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK  
	0x00,             // BEL
	0x2a,			// BS	Backspace
	0x2b,			// TAB	Tab
	0x28,			// LF	Enter
	0x00,             // VT 
	0x00,             // FF 
	0x00,             // CR 
	0x00,             // SO 
	0x00,             // SI 
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM 
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS 
	0x00,             // GS 
	0x00,             // RS 
	0x00,             // US 

	0x2c,		   //  ' ' IT_LAYOUT
	0x1e|SHIFT,	   // ! IT_LAYOUT
	0x1f|SHIFT,	   // " IT_LAYOUT
	0x34|ALTGR,    // # IT_LAYOUT (KEY_QUOTE + KEYCODE_RIGHT_ALT)
	0x21|SHIFT,    // $ IT_LAYOUT (KEY2 + SHIFT_MASK)
	0x22|SHIFT,    // % IT_LAYOUT KEY5|SHIFT
	0x23|SHIFT,    // & IT_LAYOUT KEY6|SHIFT
	0x2d,          // ' IT_LAYOUT KEY_MINUS
	0x25|SHIFT,    // ( IT_LAYOUT KEY8+SHIFT
	0x26|SHIFT,    // ) IT_LAYOUT 
	0x30|SHIFT,    // * IT_LAYOUT KEY_RIGHT_BRACE + SHIFT
	0x30,    	   // + IT_LAYOUT
	0x36,          // , IT_LAYOUT 
	0x38,          // - IT_LAYOUT KEY_SLASH
	0x37,          // . IT_LAYOUT
	0x24|SHIFT,    // / IT_LAYOUT KEY7 + SHIFT
	0x27,          // 0 IT_LAYOUT
	0x1e,          // 1 IT_LAYOUT
	0x1f,          // 2 IT_LAYOUT
	0x20,          // 3 IT_LAYOUT
	0x21,          // 4 IT_LAYOUT
	0x22,          // 5 IT_LAYOUT
	0x23,          // 6 IT_LAYOUT
	0x24,          // 7 IT_LAYOUT
	0x25,          // 8 IT_LAYOUT
	0x26,          // 9 IT_LAYOUT
	0x37|SHIFT,      // : IT_LAYOUT
	0x36|SHIFT,      // ; IT_LAYOUT KEY_COMMA + SHIFT MASK 
	0x64,      // <  IT_LAYOUT 
	0x27|SHIFT,          // = IT_LAYOUT 
	0x64|SHIFT,      // > IT_LAYOUT  notice that 0x64 is the only keycode that has the first leftmost four bits higher than 3..this is an issue when we encode ALTGR as 0x40 
	0x2d|SHIFT,      // ? IT_LAYOUT
	0x33|ALTGR,       // @ IT_LAYOUT KEY_SEMICOLON+ALTGR_MASK
	0x04|SHIFT,      // A IT_LAYOUT
	0x05|SHIFT,      // B IT_LAYOUT 
	0x06|SHIFT,      // C IT_LAYOUT
	0x07|SHIFT,      // D IT_LAYOUT
	0x08|SHIFT,      // E IT_LAYOUT
	0x09|SHIFT,      // F IT_LAYOUT
	0x0a|SHIFT,      // G IT_LAYOUT 
	0x0b|SHIFT,      // H IT_LAYOUT 
	0x0c|SHIFT,      // I IT_LAYOUT 
	0x0d|SHIFT,      // J IT_LAYOUT 
	0x0e|SHIFT,      // K IT_LAYOUT 
	0x0f|SHIFT,      // L IT_LAYOUT 
	0x10|SHIFT,      // M IT_LAYOUT
	0x11|SHIFT,      // N IT_LAYOUT 
	0x12|SHIFT,      // O IT_LAYOUT
	0x13|SHIFT,      // P IT_LAYOUT
	0x14|SHIFT,      // Q IT_LAYOUT 
	0x15|SHIFT,      // R IT_LAYOUT 
	0x16|SHIFT,      // S IT_LAYOUT 
	0x17|SHIFT,      // T IT_LAYOUT 
	0x18|SHIFT,      // U IT_LAYOUT 
	0x19|SHIFT,      // V IT_LAYOUT
	0x1a|SHIFT,      // W IT_LAYOUT
	0x1b|SHIFT,      // X IT_LAYOUT
	0x1c|SHIFT,      // Y IT_LAYOUT 
	0x1d|SHIFT,      // Z IT_LAYOUT 
	0x2f|ALTGR,      // [ IT_LAYOUT KEY_LEFT_BRACE + ALTGR_MASK
	0x35,          // bslash IT_LAYOUT KEY_TILDE
	0x30|ALTGR,     // ] IT_LAYOUT KEY_RIGHT_BRACE + ALTGR_MASK
	0x2e|SHIFT,    // ^ IT_LAYOUT KEY_EQUAL + SHIFT_MASK
	0x38|SHIFT,    // _ IT_LAYOUT KEY_SLASH + SHIFT_MASK
	0x35,          // `
	0x04,          // a IT_LAYOUT
	0x05,          // b IT_LAYOUT
	0x06,          // c IT_LAYOUT
	0x07,          // d IT_LAYOUT
	0x08,          // e IT_LAYOUT
	0x09,          // f IT_LAYOUT
	0x0a,          // g IT_LAYOUT
	0x0b,          // h IT_LAYOUT
	0x0c,          // i IT_LAYOUT
	0x0d,          // j IT_LAYOUT
	0x0e,          // k IT_LAYOUT
	0x0f,          // l IT_LAYOUT
	0x10,          // m IT_LAYOUT
	0x11,          // n IT_LAYOUT
	0x12,          // o IT_LAYOUT
	0x13,          // p IT_LAYOUT
	0x14,          // q IT_LAYOUT
	0x15,          // r IT_LAYOUT
	0x16,          // s IT_LAYOUT
	0x17,          // t IT_LAYOUT
	0x18,          // u IT_LAYOUT
	0x19,          // v IT_LAYOUT
	0x1a,          // w IT_LAYOUT
	0x1b,          // x IT_LAYOUT
	0x1c,          // y IT_LAYOUT
	0x1d,          // z IT_LAYOUT
	0x2f|SHIFT|ALTGR,  // { IT_LAYOUT 
	0x35|SHIFT,    // | IT_LAYOUT KEY_TILDE + SHIFT
	0x30|SHIFT|ALTGR, // } IT_LAYOUT 
	0x35|SHIFT,    // ~
	0				// DEL
};

uint8_t USBPutChar(uint8_t c);

// pressKeycode() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way 
// USB HID works, the host acts like the key remains pressed until we 
// call releaseKeycode(), releaseAll(), or otherwise clear the report and resend.
// When send is set to FALSE (= 0x00) no sendReport() is executed. This comes in
// handy when combining key presses (e.g. SHIFT+A).
size_t Keyboard_::pressKeycode(uint8_t k, uint8_t send) 
{
	uint8_t index = 0x00;
	uint8_t done = 0x00;
	
	if ((k >= KEYCODE_LEFT_CTRL) && (k <= KEYCODE_RIGHT_GUI)) {
		// it's a modifier key
		_keyReport.modifiers |= (0x01 << (k - KEYCODE_LEFT_CTRL));
	} else {
		// it's some other key:
		// Add k to the key report only if it's not already present
		// and if there is an empty slot.
		for (index = 0x00; index < KEYREPORT_KEYCOUNT; index++) {
			if (_keyReport.keys[index] != k) { // is k already in list?
				if (0x00 == _keyReport.keys[index]) { // have we found an empty slot?
					_keyReport.keys[index] = k;
					done = 0x01;
					break;
				}
			} else {
				done = 0x01;
				break;
			}
			
		}
		
		// use separate variable to check if slot was found
		// for style reasons - we do not know how the compiler
		// handles the for() index when it leaves the loop
		if (0x00 == done) {
			setWriteError();
			return 0x00;
		}
	}
	
	if (0x00 != send) {
		sendReport(&_keyReport);
	}
	return 0x01;
}

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way 
// USB HID works, the host acts like the key remains pressed until we 
// call release(), releaseAll(), or otherwise clear the report and resend.
size_t Keyboard_::press(uint8_t k) 
{
	uint8_t keylayout = getKeyLayout();
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers |= (1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		//we need to consider which layout are we using
		if (keylayout == US_LAYOUT) {
			k = pgm_read_byte(_asciimap + k);
		}
		else if (keylayout == IT_LAYOUT) {
			//we need to check only for two ASCII characters that cannot
			//be produced in an IT keyboard with one single key and a combination
			//of modifiers...
			//In case we have to press a grave accent
			if (k == 96)
			{
				Keyboard.pressKeycode(KEYCODE_LEFT_ALT, false);
				Keyboard.pressKeycode(0x61, false);
				Keyboard.pressKeycode(0x5e, true);
				return 1;
			}
			//If we have to press tilde
			else if(k == 126)
			{
				Keyboard.pressKeycode(KEYCODE_LEFT_ALT, false);
				Keyboard.pressKeycode(0x59, false);
				Keyboard.pressKeycode(0x5a, false);
				Keyboard.pressKeycode(0x5e, true);
				return 1;
			}
			k = pgm_read_byte(_asciimapIT + k);
		}
		if (!k) {
			setWriteError();
			return 0;
		}
		if (k & 0x80) {						// it's a capital letter or other character reached with shift
			_keyReport.modifiers |= 0x02;	// the left shift modifier
			k &= 0x7F;
		}
		//it is a character reached with ALTGR but it is not < (0x64) 
		//which is the only character that higher than 0x3F
		if((k & 0x40) && (k != 0x64)) {
			//the ALTGR modifier which is different from the ALTGR
			//keycode that is 0xE6
			_keyReport.modifiers |= 0x40;
			//we clean the ALTGR mask
			k &= 0x3F;
		}
	}
	
	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k && 
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {
		
		for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			setWriteError();
			return 0;
		}	
	}
	sendReport(&_keyReport);
	return 1;
}

// releaseKeycode() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
// When send is set to FALSE (= 0x00) no sendReport() is executed. This comes in
// handy when combining key releases (e.g. SHIFT+A).
size_t Keyboard_::releaseKeycode(uint8_t k, uint8_t send) 
{
	uint8_t indexA;
	uint8_t indexB;
	uint8_t count;
	
	if ((k >= KEYCODE_LEFT_CTRL) && (k <= KEYCODE_RIGHT_GUI)) {
		// it's a modifier key
		_keyReport.modifiers = _keyReport.modifiers & (~(0x01 << (k - KEYCODE_LEFT_CTRL)));
	} else {
		// it's some other key:
		// Test the key report to see if k is present.  Clear it if it exists.
		// Check all positions in case the key is present more than once (which it shouldn't be)
		for (indexA = 0x00; indexA < KEYREPORT_KEYCOUNT; indexA++) {
			if (_keyReport.keys[indexA] == k) {
				_keyReport.keys[indexA] = 0x00;
			}
		}
		
		// finally rearrange the keys list so that the free (= 0x00) are at the
		// end of the keys list - some implementations stop for keys at the
		// first occurence of an 0x00 in the keys list
		// so (0x00)(0x01)(0x00)(0x03)(0x02)(0x00) becomes (0x01)(0x03)(0x02)(0x00)(0x00)(0x00)
		count = 0x00; // holds the number of zeros we've found
		indexA = 0x00;
		while ((indexA + count) < KEYREPORT_KEYCOUNT) {
			if (0x00 == _keyReport.keys[indexA]) {
				count++; // one more zero
				for (indexB = indexA; indexB < KEYREPORT_KEYCOUNT-count; indexB++) {
					_keyReport.keys[indexB] = _keyReport.keys[indexB+0x01];
				}
				_keyReport.keys[KEYREPORT_KEYCOUNT-count] = 0x00;
			} else {
				indexA++; // one more non-zero
			}
		}
	}		
	
	if (0x00 != send) {
		sendReport(&_keyReport);
	}
	return 0x01;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t Keyboard_::release(uint8_t k) 
{
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers &= ~(1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = pgm_read_byte(_asciimap + k);
		if (!k) {
			return 0;
		}
		if (k & 0x80) {							// it's a capital letter or other character reached with shift
			_keyReport.modifiers &= ~(0x02);	// the left shift modifier
			k &= 0x7F;
		}
		if (k & 0x40) {
			//we clean also the case in which ALTGR is pressed
			_keyReport.modifiers &= ~(0x40);
			k &= 0x3F;
		}
		//TO DO - handle cases for IT_LAYOUT grave accent and tilde
	}
	
	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i=0; i<6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
}

void Keyboard_::releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;	
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;	
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;	
	_keyReport.modifiers = 0;
	sendReport(&_keyReport);
}

//this function has been modified because it is used by print methods and
//we want them to be parametrized by keyboard layout too
size_t Keyboard_::write(uint8_t c)
{	
	uint8_t p = press(c);  // Keydown
	//release(c);            // Keyup
	//we use releaseAll() because we encountered problems with print and println function
	//they don't print consecutive backslash / /
	//for instance if we issue the command Keyboard.print(F("cmd /T:01 /K"));
	//the output is "cmd /T:01 K"
	releaseAll();
	return p;              // just return the result of press() since release() almost always returns 1
}

void Keyboard_::setLedStatus(uint8_t s)
{
	_ledStatus = s;
}

uint8_t Keyboard_::getLedStatus(void)
{
	return _ledStatus;
}

//added to introduce parametric keyboard layout
void Keyboard_::setKeyLayout(uint8_t s)
{
	_keyLayout = s;	
}
uint8_t Keyboard_::getKeyLayout(void)
{
	return _keyLayout;
}

//added to check if capsLock is on or off
bool Keyboard_::isCapsLockOn()
{
	return ((getLedStatus() & 2) == 2) ? true : false;
}
#endif

#endif /* if defined(USBCON) */
