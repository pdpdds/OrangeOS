#include "Hardware.h"

bool HalpInitPIC(void)
{
	/* master 8259 */
	OutPortByte(0x20, 0x11); /* edge trig, cascade mode, ICW4 */
	OutPortByte(0x21, 0x20); /* start at INT vector 20h */
	OutPortByte(0x21, 0x04); /* #IR2 pin has a slave */
	OutPortByte(0x21, 0x01); /* not sp, nonbuffered mode, normal EOI, for x86 */
	OutPortByte(0x21, 0x00);

	/* slave 8259 */
	OutPortByte(0xa0, 0x11); /* edge trig, cascade mode, ICW4 */
	OutPortByte(0xa1, 0x28); /* start at INT vector 28h */
	OutPortByte(0xa1, 0x02); /* slave 8259 has been linked to master's #IR2 pin */
	OutPortByte(0xa1, 0x01); /* not sp, nonbuffered mode, normal EOI, for x86 */
	OutPortByte(0xa1, 0x00);

	return TRUE;
}

bool HalpInitSysTimer(BYTE timeoutPerSecond)
{
	WORD timeout = (WORD)(1193180 / timeoutPerSecond);

	OutPortByte(0x43, 0x34);
	OutPortByte(0x40, (UCHAR)(timeout & 0xff));
	OutPortByte(0x40, (UCHAR)(timeout >> 8));

	return TRUE;
}