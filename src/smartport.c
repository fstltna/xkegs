const char rcsid_smartport_c[] = "@(#)$KmKId: smartport.c,v 1.39 2021-01-16 04:00:19+00 kentd Exp $";

/************************************************************************/
/*			KEGS: Apple //gs Emulator			*/
/*			Copyright 2002-2020 by Kent Dickey		*/
/*									*/
/*	This code is covered by the GNU GPL v3				*/
/*	See the file COPYING.txt or https://www.gnu.org/licenses/	*/
/*	This program is provided with no warranty			*/
/*									*/
/*	The KEGS web page is kegs.sourceforge.net			*/
/*	You may contact the author at: kadickey@alumni.princeton.edu	*/
/************************************************************************/

#include "defc.h"

extern int Verbose;
extern int Halt_on;
extern int g_rom_version;
extern int g_io_amt;
extern int g_highest_smartport_unit;

int g_cycs_in_io_read = 0;

extern Engine_reg engine;

extern Iwm g_iwm;

#define LEN_SMPT_LOG	16
STRUCT(Smpt_log) {
	word32	start_addr;
	int	cmd;
	int	rts_addr;
	int	cmd_list;
	int	extras;
	int	unit;
	int	buf;
	int	blk;
};

Smpt_log g_smpt_log[LEN_SMPT_LOG];
int	g_smpt_log_pos = 0;

void
smartport_error(void)
{
	int	pos;
	int	i;

	pos = g_smpt_log_pos;
	printf("Smartport log pos: %d\n", pos);
	for(i = 0; i < LEN_SMPT_LOG; i++) {
		pos--;
		if(pos < 0) {
			pos = LEN_SMPT_LOG - 1;
		}
		printf("%d:%d: t:%04x, cmd:%02x, rts:%04x, "
			"cmd_l:%04x, x:%d, unit:%d, buf:%04x, blk:%04x\n",
			i, pos,
			g_smpt_log[pos].start_addr,
			g_smpt_log[pos].cmd,
			g_smpt_log[pos].rts_addr,
			g_smpt_log[pos].cmd_list,
			g_smpt_log[pos].extras,
			g_smpt_log[pos].unit,
			g_smpt_log[pos].buf,
			g_smpt_log[pos].blk);
	}
}
void
smartport_log(word32 start_addr, int cmd, int rts_addr, int cmd_list)
{
	int	pos;

	pos = g_smpt_log_pos;
	if(start_addr != 0) {
		g_smpt_log[pos].start_addr = start_addr;
		g_smpt_log[pos].cmd = cmd;
		g_smpt_log[pos].rts_addr = rts_addr;
		g_smpt_log[pos].cmd_list = cmd_list;
		g_smpt_log[pos].extras = 0;
		g_smpt_log[pos].unit = 0;
		g_smpt_log[pos].buf = 0;
		g_smpt_log[pos].blk = 0;
	} else {
		pos--;
		if(pos < 0) {
			pos = LEN_SMPT_LOG - 1;
		}
		g_smpt_log[pos].extras = 1;
		g_smpt_log[pos].unit = cmd;
		g_smpt_log[pos].buf = rts_addr;
		g_smpt_log[pos].blk = cmd_list;
	}
	pos++;
	if(pos >= LEN_SMPT_LOG) {
		pos = 0;
	}
	g_smpt_log_pos = pos;
}

void
do_c70d(word32 arg0)
{
	int	cmd;
	int	cmd_list_lo, cmd_list_mid, cmd_list_hi;
	int	rts_lo, rts_hi;
	word32	rts_addr;
	word32	cmd_list;
	int	unit;
	int	param_cnt;
	int	status_ptr_lo, status_ptr_mid, status_ptr_hi;
	int	buf_ptr_lo, buf_ptr_hi;
	int	buf_ptr;
	int	block_lo, block_mid, block_hi;
	int	block;
	word32	status_ptr;
	int	status_code;
	int	ctl_ptr_lo, ctl_ptr_hi;
	int	ctl_ptr;
	int	ctl_code;
	int	mask;
	int	stat_val;
	int	size;
	int	ret;
	int	ext;
	int	i;

	set_memory_c(0x7f8, 0xc7, 0);

	if((engine.psr & 0x100) == 0) {
		disk_printf("c70d called in native mode!\n");
		if((engine.psr & 0x30) != 0x30) {
			halt_printf("c70d called native, psr: %03x!\n",
							engine.psr);
		}
	}

	engine.stack = ((engine.stack + 1) & 0xff) + 0x100;
	rts_lo = get_memory_c(engine.stack, 0);
	engine.stack = ((engine.stack + 1) & 0xff) + 0x100;
	rts_hi = get_memory_c(engine.stack, 0);
	rts_addr = (rts_lo + (256*rts_hi) + 1) & 0xffff;
	disk_printf("rts_addr: %04x\n", rts_addr);

	cmd = get_memory_c(rts_addr, 0);
	cmd_list_lo = get_memory_c((rts_addr + 1) & 0xffff, 0);
	cmd_list_mid = get_memory_c((rts_addr + 2) & 0xffff, 0);
	cmd_list_hi = 0;
	mask = 0xffff;
	if(cmd & 0x40) {
		/* extended */
		mask = 0xffffff;
		cmd_list_hi = get_memory_c((rts_addr + 3) & 0xffff, 0);
	}

	cmd_list = cmd_list_lo + (256*cmd_list_mid) + (65536*cmd_list_hi);

	disk_printf("cmd: %02x, cmd_list: %06x\n", cmd, cmd_list);
	param_cnt = get_memory_c(cmd_list, 0);

	ext = 0;
	if(cmd & 0x40) {
		ext = 2;
	}

	smartport_log(0xc70d, cmd, rts_addr, cmd_list);

	switch(cmd & 0x3f) {
	case 0x00:	/* Status == 0x00 and 0x40 */
		if(param_cnt != 3) {
			disk_printf("param_cnt %d is != 3!\n", param_cnt);
			exit(8);
		}
		unit = get_memory_c((cmd_list+1) & mask, 0);
		status_ptr_lo = get_memory_c((cmd_list+2) & mask, 0);
		status_ptr_mid = get_memory_c((cmd_list+3) & mask, 0);
		status_ptr_hi = 0;
		if(cmd & 0x40) {
			status_ptr_hi = get_memory_c((cmd_list+4) & mask, 0);
		}

		status_ptr = status_ptr_lo + (256*status_ptr_mid) +
			(65536*status_ptr_hi);
		if(cmd & 0x40) {
			status_code = get_memory_c((cmd_list+6) & mask, 0);
		} else {
			status_code = get_memory_c((cmd_list+4) & mask, 0);
		}

		smartport_log(0, unit, status_ptr, status_code);

		disk_printf("unit: %02x, status_ptr: %06x, code: %02x\n",
			unit, status_ptr, status_code);
		if(unit == 0 && status_code == 0) {
			/* Smartport driver status */
			/* see technotes/smpt/tn-smpt-002 */
			set_memory_c(status_ptr, g_highest_smartport_unit+1, 0);
			set_memory_c(status_ptr+1, 0xff, 0); /* interrupt stat*/
			set_memory16_c(status_ptr+2, 0x0002, 0); /* vendor id */
			set_memory16_c(status_ptr+4, 0x1000, 0); /* version */
			set_memory16_c(status_ptr+6, 0x0000, 0);

			engine.xreg = 8;
			engine.yreg = 0;
			engine.acc &= 0xff00;
			engine.psr &= ~1;
			engine.kpc = (rts_addr + 3 + ext) & mask;
			return;
		} else if(unit > 0 && status_code == 0) {
			/* status for unit x */
			if((unit > MAX_C7_DISKS) ||
					(g_iwm.smartport[unit-1].fd < 0)) {
				stat_val = 0x80;
				size = 0;
			} else {
				stat_val = 0xf8;
				size = g_iwm.smartport[unit-1].image_size;
				size = (size+511) / 512;
			}
			set_memory_c(status_ptr, stat_val, 0);
			set_memory24_c(status_ptr +1, size, 0);
			engine.xreg = 4;
			if(cmd & 0x40) {
				set_memory_c(status_ptr + 4,
						(size >> 16) & 0xff, 0);
				engine.xreg = 5;
			}
			engine.yreg = 0;
			engine.acc &= 0xff00;
			engine.psr &= ~1;
			engine.kpc = (rts_addr + 3 + ext) & mask;

			disk_printf("just finished unit %d, stat 0\n", unit);
			return;
		} else if(status_code == 3) {
			if((unit > MAX_C7_DISKS) ||
					(g_iwm.smartport[unit-1].fd < 0)) {
				stat_val = 0x80;
				size = 0;
			} else {
				stat_val = 0xf8;
				size = g_iwm.smartport[unit-1].image_size;
				size = (size+511) / 512;
			}
			if(cmd & 0x40) {
				disk_printf("extended for stat_code 3!\n");
			}
			/* DIB for unit 1 */
			set_memory_c(status_ptr, stat_val, 0);
			set_memory24_c(status_ptr +1, size, 0);
			if(cmd & 0x40) {
				set_memory_c(status_ptr + 4,
						(size >> 24) & 0xff, 0);
				status_ptr++;
			}
			set_memory_c(status_ptr +4, 4, 0);
			for(i = 5; i < 21; i++) {
				set_memory_c(status_ptr +i, 0x20, 0);
			}
			set_memory_c(status_ptr +5, 'K', 0);
			set_memory_c(status_ptr +6, 'E', 0);
			set_memory_c(status_ptr +7, 'G', 0);
			set_memory_c(status_ptr +8, 'S', 0);

			/* hard disk supporting extended calls */
			set_memory16_c(status_ptr + 21, 0xa002, 0);
			set_memory16_c(status_ptr + 23, 0x0000, 0);

			if(cmd & 0x40) {
				engine.xreg = 26;
			} else {
				engine.xreg = 25;
			}
			engine.yreg = 0;
			engine.acc &= 0xff00;
			engine.psr &= ~1;
			engine.kpc = (rts_addr + 3 + ext) & 0xffff;

			disk_printf("Just finished unit %d, stat 3\n", unit);
			if(unit == 0 || unit > MAX_C7_DISKS) {
				engine.acc |= 0x28;
				engine.psr |= 1;
			}
			return;
		}
		printf("cmd: 00, unknown unit/status code!\n");
		break;
	case 0x01:	/* Read Block == 0x01 and 0x41 */
		if(param_cnt != 3) {
			halt_printf("param_cnt %d is != 3!\n", param_cnt);
			return;
		}
		unit = get_memory_c((cmd_list+1) & mask, 0);
		buf_ptr_lo = get_memory_c((cmd_list+2) & mask, 0);
		buf_ptr_hi = get_memory_c((cmd_list+3) & mask, 0);

		buf_ptr = buf_ptr_lo + (256*buf_ptr_hi);
		if(cmd & 0x40) {
			buf_ptr_lo = get_memory_c((cmd_list+4) & mask, 0);
			buf_ptr_hi = get_memory_c((cmd_list+5) & mask, 0);
			buf_ptr += ((buf_ptr_hi*256) + buf_ptr_lo)*65536;
			cmd_list += 2;
		}
		block_lo = get_memory_c((cmd_list+4) & mask, 0);
		block_mid = get_memory_c((cmd_list+5) & mask, 0);
		block_hi = get_memory_c((cmd_list+6) & mask, 0);
		block = ((block_hi*256) + block_mid)*256 + block_lo;
		disk_printf("smartport read unit %d of block %04x into %04x\n",
			unit, block, buf_ptr);
		if(unit < 1 || unit > MAX_C7_DISKS) {
			halt_printf("Unknown unit #: %d\n", unit);
		}

		smartport_log(0, unit - 1, buf_ptr, block);

		ret = do_read_c7(unit - 1, buf_ptr, block);
		engine.xreg = 0;
		engine.yreg = 2;
		engine.acc = (engine.acc & 0xff00) | (ret & 0xff);
		engine.psr &= ~1;
		if(ret != 0) {
			engine.psr |= 1;
		}
		engine.kpc = (rts_addr + 3 + ext) & 0xffff;

		return;
		break;
	case 0x02:	/* Write Block == 0x02 and 0x42 */
		if(param_cnt != 3) {
			halt_printf("param_cnt %d is != 3!\n", param_cnt);
			return;
		}
		unit = get_memory_c((cmd_list+1) & mask, 0);
		buf_ptr_lo = get_memory_c((cmd_list+2) & mask, 0);
		buf_ptr_hi = get_memory_c((cmd_list+3) & mask, 0);

		buf_ptr = buf_ptr_lo + (256*buf_ptr_hi);
		if(cmd & 0x40) {
			buf_ptr_lo = get_memory_c((cmd_list+4) & mask, 0);
			buf_ptr_hi = get_memory_c((cmd_list+5) & mask, 0);
			buf_ptr += ((buf_ptr_hi*256) + buf_ptr_lo)*65536;
			cmd_list += 2;
		}
		block_lo = get_memory_c((cmd_list+4) & mask, 0);
		block_mid = get_memory_c((cmd_list+5) & mask, 0);
		block_hi = get_memory_c((cmd_list+6) & mask, 0);
		block = ((block_hi*256) + block_mid)*256 + block_lo;
		disk_printf("smartport write unit %d of block %04x from %04x\n",
			unit, block, buf_ptr);
		if(unit < 1 || unit > MAX_C7_DISKS) {
			halt_printf("Unknown unit #: %d\n", unit);
		}

		smartport_log(0, unit - 1, buf_ptr, block);

		ret = do_write_c7(unit - 1, buf_ptr, block);
		engine.xreg = 0;
		engine.yreg = 2;
		engine.acc = (engine.acc & 0xff00) | (ret & 0xff);
		engine.psr &= ~1;
		if(ret != 0) {
			engine.psr |= 1;
		}
		engine.kpc = (rts_addr + 3 + ext) & 0xffff;

		HALT_ON(HALT_ON_C70D_WRITES, "c70d Write done\n");
		return;
		break;
	case 0x03:	/* Format == 0x03 and 0x43 */
		if(param_cnt != 1) {
			halt_printf("param_cnt %d is != 1!\n", param_cnt);
			return;
		}
		unit = get_memory_c((cmd_list+1) & mask, 0);

		if(unit < 1 || unit > MAX_C7_DISKS) {
			halt_printf("Unknown unit #: %d\n", unit);
		}

		smartport_log(0, unit - 1, 0, 0);

		ret = do_format_c7(unit - 1);
		engine.xreg = 0;
		engine.yreg = 2;
		engine.acc = (engine.acc & 0xff00) | (ret & 0xff);
		engine.psr &= ~1;
		if(ret != 0) {
			engine.psr |= 1;
		}
		engine.kpc = (rts_addr + 3 + ext) & 0xffff;

		HALT_ON(HALT_ON_C70D_WRITES, "c70d Format done\n");
		return;
		break;
	case 0x04:	/* Control == 0x04 and 0x44 */
		if(cmd == 0x44) {
			halt_printf("smartport code 0x44 not supported\n");
		}
		if(param_cnt != 3) {
			halt_printf("param_cnt %d is != 3!\n", param_cnt);
			return;
		}
		unit = get_memory_c((cmd_list+1) & mask, 0);
		ctl_ptr_lo = get_memory_c((cmd_list+2) & mask, 0);
		ctl_ptr_hi = get_memory_c((cmd_list+3) & mask, 0);
		ctl_ptr = (ctl_ptr_hi << 8) + ctl_ptr_lo;
		if(cmd & 0x40) {
			ctl_ptr_lo = get_memory_c((cmd_list+4) & mask, 0);
			ctl_ptr_hi = get_memory_c((cmd_list+5) & mask, 0);
			ctl_ptr += ((ctl_ptr_hi << 8) + ctl_ptr_lo) << 16;
			cmd_list += 2;
		}

		ctl_code = get_memory_c((cmd_list +4) & mask, 0);

		switch(ctl_code) {
		case 0x00:
			printf("Performing a reset on unit %d\n", unit);
			break;
		default:
			halt_printf("control code: %02x unknown!\n", ctl_code);
		}

		engine.xreg = 0;
		engine.yreg = 2;
		engine.acc &= 0xff00;
		engine.psr &= ~1;
		engine.kpc = (rts_addr + 3 + ext) & 0xffff;
		return;
		break;
	default:	/* Unknown command! */
		/* set acc = 1, and set carry, and set kpc */
		engine.xreg = (rts_addr) & 0xff;
		engine.yreg = (rts_addr >> 8) & 0xff;
		engine.acc = (engine.acc & 0xff00) + 0x01;
		engine.psr |= 0x01;	/* set carry */
		engine.kpc = (rts_addr + 3 + ext) & 0xffff;
		if(cmd != 0x4a && cmd != 0x48) {
			/* Finder does 0x4a call before formatting disk */
			/* Many things do 0x48 call to see online drives */
			halt_printf("Just did smtport cmd:%02x rts_addr:%04x, "
				"cmdlst:%06x\n", cmd, rts_addr, cmd_list);
		}
		return;
	}

	halt_printf("Unknown smtport cmd:%02x, cmd_list:%06x, rts_addr:%06x\n",
		cmd, cmd_list, rts_addr);
}

void
do_c70a(word32 arg0)
{
	int	cmd, unit;
	int	buf_lo, buf_hi;
	int	blk_lo, blk_hi;
	int	blk, buf;
	int	prodos_unit;
	int	size;
	int	ret;

	set_memory_c(0x7f8, 0xc7, 0);

	cmd = get_memory_c((engine.direct + 0x42) & 0xffff, 0);
	prodos_unit = get_memory_c((engine.direct + 0x43) & 0xffff, 0);
	buf_lo = get_memory_c((engine.direct + 0x44) & 0xffff, 0);
	buf_hi = get_memory_c((engine.direct + 0x45) & 0xffff, 0);
	blk_lo = get_memory_c((engine.direct + 0x46) & 0xffff, 0);
	blk_hi = get_memory_c((engine.direct + 0x47) & 0xffff, 0);

	blk = (blk_hi << 8) + blk_lo;
	buf = (buf_hi << 8) + buf_lo;
	disk_printf("cmd: %02x, pro_unit: %02x, buf: %04x, blk: %04x\n",
		cmd, prodos_unit, buf, blk);

	if((prodos_unit & 0x7f) == 0x70) {
		unit = 0 + (prodos_unit >> 7);
	} else if((prodos_unit & 0x7f) == 0x40) {
		unit = 2 + (prodos_unit >> 7);
	} else {
		halt_printf("Unknown prodos_unit: %d\n", prodos_unit);
		return;
	}

	smartport_log(0xc70a, cmd, blk, buf);

	engine.psr &= ~1;	/* clear carry */
	if(g_rom_version >= 3) {
		engine.kpc = 0xc764;
	} else {
		engine.kpc = 0xc765;
	}

	ret = 0x27;	/* I/O error */
	if(cmd == 0x00) {
		size = g_iwm.smartport[unit].image_size;
		size = (size+511) / 512;

		smartport_log(0, unit, size, 0);

		ret = 0;
		engine.xreg = size & 0xff;
		engine.yreg = size >> 8;
	} else if(cmd == 0x01) {
		smartport_log(0, unit, buf, blk);
		ret = do_read_c7(unit, buf, blk);
	} else if(cmd == 0x02) {
		smartport_log(0, unit, buf, blk);
		ret = do_write_c7(unit, buf, blk);
	} else if(cmd == 0x03) {	/* format */
		smartport_log(0, unit, buf, blk);
		ret = do_format_c7(unit);
	}

	engine.acc = (engine.acc & 0xff00) | (ret & 0xff);
	if(ret != 0) {
		engine.psr |= 1;
	}
	return;
}

int
do_read_c7(int unit_num, word32 buf, int blk)
{
	byte	local_buf[0x200];
	Disk	*dsk;
	byte	*bptr;
	register word32 start_time;
	register word32 end_time;
	long	image_start, image_size, ret;
	word32	val;
	int	len, fd;
	int	i;

	if(unit_num < 0 || unit_num > MAX_C7_DISKS) {
		halt_printf("do_read_c7: unit_num: %d\n", unit_num);
		smartport_error();
		return 0x28;
	}

	dsk = &(g_iwm.smartport[unit_num]);
	fd = dsk->fd;
	image_start = dsk->image_start;
	image_size = dsk->image_size;
	if(fd < 0) {
		printf("c7_fd == %d!\n", fd);
#if 0
		if(blk != 2 && blk != 0) {
			/* don't print error if only reading directory */
			smartport_error();
			halt_printf("Read unit:%02x blk:%04x\n", unit_num, blk);
		}
#endif
		return 0x2f;
	}
	if(((blk + 1) * 0x200) > (image_start + image_size)) {
		halt_printf("Tried to read past %08lx on disk (blk:%04x)\n",
			image_start + image_size, blk);
		smartport_error();
		return 0x27;
	}

	if(fd == 0) {
		// image was compressed and is in dsk->raw_data
		bptr = dsk->raw_data + image_start + (blk*0x200);
		for(i = 0; i < 0x200; i++) {
			local_buf[i] = bptr[i];
		}
	} else {
		ret = lseek(fd, image_start + blk*0x200, SEEK_SET);
		if(ret != (image_start + blk*0x200)) {
			halt_printf("lseek ret %08lx, errno: %d\n", ret, errno);
			smartport_error();
			return 0x27;
		}

		len = (int)read(fd, &local_buf[0], 0x200);
		if(len != 0x200) {
			printf("read returned %08x, errno:%d, blk:%04x, unit:"
				"%02x\n", len, errno, blk, unit_num);
			halt_printf("name: %s\n", dsk->name_ptr);
			smartport_error();
			return 0x27;
		}
	}

	g_io_amt += 0x200;

	if(buf >= 0xfc0000) {
		disk_printf("reading into ROM, just returning\n");
		return 0;
	}

	GET_ITIMER(start_time);

	for(i = 0; i < 0x200; i += 2) {
		val = (local_buf[i+1] << 8) + local_buf[i];
		set_memory16_c(buf + i, val, 0);
	}

	GET_ITIMER(end_time);

	g_cycs_in_io_read += (end_time - start_time);

	return 0;
}

int
do_write_c7(int unit_num, word32 buf, int blk)
{
	byte	local_buf[0x200];
	Disk	*dsk;
	long	ret, image_start, image_size;
	int	len, fd;
	int	i;

	if(unit_num < 0 || unit_num > MAX_C7_DISKS) {
		halt_printf("do_write_c7: unit_num: %d\n", unit_num);
		smartport_error();
		return 0x28;
	}

	dsk = &(g_iwm.smartport[unit_num]);
	fd = dsk->fd;
	image_start = dsk->image_start;
	image_size = dsk->image_size;
	if(fd < 0) {
		halt_printf("c7_fd == %d!\n", fd);
		smartport_error();
		return 0x28;
	}

	for(i = 0; i < 0x200; i++) {
		local_buf[i] = get_memory_c(buf + i, 0);
	}

	if(dsk->write_prot || dsk->raw_data) {
		printf("Write, but %s is write protected!\n", dsk->name_ptr);
		return 0x2b;
	}

	if(dsk->write_through_to_unix == 0) {
		halt_printf("Write to %s, but not wr_thru!\n", dsk->name_ptr);
		return 0x00;
	}

	ret = lseek(fd, image_start + blk*0x200, SEEK_SET);
	if(ret != (image_start + blk*0x200)) {
		halt_printf("lseek returned %08x, errno: %d\n", ret, errno);
		smartport_error();
		return 0x27;
	}

	if(ret >= (image_start + image_size)) {
		halt_printf("Tried to write to %08x\n", ret);
		smartport_error();
		return 0x27;
	}

	len = (int)write(fd, &local_buf[0], 0x200);
	if(len != 0x200) {
		halt_printf("write ret %08x bytes, errno: %d\n", len, errno);
		smartport_error();
		return 0x27;
	}

	g_io_amt += 0x200;

	return 0;
}

int
do_format_c7(int unit_num)
{
	byte	local_buf[0x1000];
	Disk	*dsk;
	long	image_start, image_size, ret, total, sum;
	int	len, max, fd;
	int	i;

	if(unit_num < 0 || unit_num > MAX_C7_DISKS) {
		halt_printf("do_format_c7: unit_num: %d\n", unit_num);
		smartport_error();
		return 0x28;
	}

	dsk = &(g_iwm.smartport[unit_num]);
	fd = dsk->fd;
	image_start = dsk->image_start;
	image_size = dsk->image_size;
	if(fd < 0) {
		halt_printf("c7_fd == %d!\n", fd);
		smartport_error();
		return 0x28;
	}

	if(dsk->write_prot || dsk->raw_data) {
		printf("Format, but %s is write protected!\n", dsk->name_ptr);
		return 0x2b;
	}

	if(dsk->write_through_to_unix == 0) {
		printf("Format of %s ignored\n", dsk->name_ptr);
		return 0x00;
	}

	for(i = 0; i < 0x1000; i++) {
		local_buf[i] = 0;
	}

	ret = lseek(fd, image_start, SEEK_SET);
	if(ret != image_start) {
		halt_printf("lseek returned %08lx, errno: %d\n", ret, errno);
		smartport_error();
		return 0x27;
	}

	sum = 0;
	total = image_size;

	while(sum < total) {
		max = (int)MY_MIN(0x1000, total-sum);
		len = (int)write(fd, &local_buf[0], max);
		if(len != max) {
			halt_printf("write ret %08x, errno:%d\n", len, errno);
			smartport_error();
			return 0x27;
		}
		sum += len;
	}

	return 0;
}


void
do_c700(word32 ret)
{
	disk_printf("do_c700 called, ret: %08x\n", ret);

	ret = do_read_c7(0, 0x800, 0);

	set_memory_c(0x7f8, 7, 0);
	set_memory16_c(0x42, 0x7001, 0);
	set_memory16_c(0x44, 0x0800, 0);
	set_memory16_c(0x46, 0x0000, 0);
	engine.xreg = 0x70;
	engine.kpc = 0x801;

	if(ret != 0) {
		printf("Failure reading boot disk in s7d1!\n");
		engine.kpc = 0xff59;	/* Jump to monitor, fix $36-$39 */
	}
}

