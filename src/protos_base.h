/************************************************************************/
/*			KEGS: Apple //gs Emulator			*/
/*			Copyright 2002-2021 by Kent Dickey		*/
/*									*/
/*	This code is covered by the GNU GPL v3				*/
/*	See the file COPYING.txt or https://www.gnu.org/licenses/	*/
/*	This program is provided with no warranty			*/
/*									*/
/*	The KEGS web page is kegs.sourceforge.net			*/
/*	You may contact the author at: kadickey@alumni.princeton.edu	*/
/************************************************************************/

#ifdef INCLUDE_RCSID_C
const char rcsid_protos_base_h[] = "@(#)$KmKId: protos_base.h,v 1.51 2021-01-23 22:46:05+00 kentd Exp $";
#endif

/* xdriver.c and macdriver.c and windriver.c */
int win_nonblock_read_stdin(int fd, char *bufptr, int len);

/* special scc_macdriver.c prototypes */
int scc_serial_mac_init(int port);
void scc_serial_mac_change_params(int port);
void scc_serial_mac_fill_readbuf(int port, int space_left, double dcycs);
void scc_serial_mac_empty_writebuf(int port);

/* special scc_windriver.c prototypes */
int scc_serial_win_init(int port);
void scc_serial_win_change_params(int port);
void scc_serial_win_fill_readbuf(int port, int space_left, double dcycs);
void scc_serial_win_empty_writebuf(int port);

/* special joystick_driver.c prototypes */
void joystick_init(void);
void joystick_update(double dcycs);
void joystick_update_buttons(void);

/* END_HDR */

/* adb.c */
int adb_get_hide_warp_info(Kimage *kimage_ptr, int *warpptr);
void adb_nonmain_check(void);
void adb_init(void);
void adb_reset(void);
void adb_log(word32 addr, int val);
void show_adb_log(void);
void adb_error(void);
void adb_add_kbd_srq(void);
void adb_clear_kbd_srq(void);
void adb_add_data_int(void);
void adb_add_mouse_int(void);
void adb_clear_data_int(void);
void adb_clear_mouse_int(void);
void adb_send_bytes(int num_bytes, word32 val0, word32 val1, word32 val2);
void adb_send_1byte(word32 val);
void adb_response_packet(int num_bytes, word32 val);
void adb_kbd_reg0_data(int a2code, int is_up);
void adb_kbd_talk_reg0(void);
void adb_set_config(word32 val0, word32 val1, word32 val2);
void adb_set_new_mode(word32 val);
int adb_read_c026(void);
void adb_write_c026(int val);
void do_adb_cmd(void);
int adb_read_c027(void);
void adb_write_c027(int val);
int read_adb_ram(word32 addr);
void write_adb_ram(word32 addr, int val);
int adb_get_keypad_xy(int get_y);
int adb_update_mouse(Kimage *kimage_ptr, int x, int y, int button_states, int buttons_valid);
int mouse_read_c024(double dcycs);
void mouse_compress_fifo(double dcycs);
void adb_paste_update_state(void);
int adb_paste_add_buf(word32 key);
void adb_key_event(int a2code, int is_up);
word32 adb_read_c000(void);
word32 adb_access_c010(void);
word32 adb_read_c025(void);
int adb_is_cmd_key_down(void);
int adb_is_option_key_down(void);
void adb_increment_speed(void);
void adb_physical_key_update(Kimage *kimage_ptr, int a2code, int is_up, int shift_down, int ctrl_down, int lock_down);
void adb_virtual_key_update(int a2code, int is_up);
void adb_kbd_repeat_off(void);


/* engine_c.c */
void check_breakpoints(word32 addr);
word32 get_memory8_io_stub(word32 addr, byte *stat, double *fcycs_ptr, double fplus_x_m1);
word32 get_memory16_pieces_stub(word32 addr, byte *stat, double *fcycs_ptr, Fplus *fplus_ptr, int in_bank);
word32 get_memory24_pieces_stub(word32 addr, byte *stat, double *fcycs_ptr, Fplus *fplus_ptr, int in_bank);
void set_memory8_io_stub(word32 addr, word32 val, byte *stat, double *fcycs_ptr, double fplus_x_m1);
void set_memory16_pieces_stub(word32 addr, word32 val, double *fcycs_ptr, double fplus_1, double fplus_x_m1, int in_bank);
void set_memory24_pieces_stub(word32 addr, word32 val, double *fcycs_ptr, Fplus *fplus_ptr, int in_bank);
word32 get_memory_c(word32 addr, int cycs);
word32 get_memory16_c(word32 addr, int cycs);
word32 get_memory24_c(word32 addr, int cycs);
void set_memory_c(word32 addr, word32 val, int cycs);
void set_memory16_c(word32 addr, word32 val, int cycs);
void set_memory24_c(word32 addr, word32 val, int cycs);
word32 do_adc_sbc8(word32 in1, word32 in2, word32 psr, int sub);
word32 do_adc_sbc16(word32 in1, word32 in2, word32 psr, int sub);
void fixed_memory_ptrs_init(void);
word32 get_itimer(void);
void engine_recalc_events(void);
void set_halt_act(int val);
void clr_halt_act(void);
word32 get_remaining_operands(word32 addr, word32 opcode, word32 psr, Fplus *fplus_ptr);
int enter_engine(Engine_reg *engine_ptr);


/* clock.c */
double get_dtime(void);
int micro_sleep(double dtime);
void clk_bram_zero(void);
void clk_bram_set(int bram_num, int offset, int val);
void clk_setup_bram_version(void);
void clk_write_bram(FILE *fconf);
void update_cur_time(void);
void clock_update(void);
void clock_update_if_needed(void);
void clock_write_c034(word32 val);
void do_clock_data(void);


/* compile_time.c */


/* config.c */
void config_init_menus(Cfg_menu *menuptr);
void config_init(void);
void cfg_exit(void);
void cfg_toggle_config_panel(void);
void cfg_set_config_panel(int panel);
void cfg_text_screen_dump(void);
void config_vbl_update(int doit_3_persec);
void config_parse_option(char *buf, int pos, int len, int line);
void config_parse_bram(char *buf, int pos, int len);
void config_load_roms(void);
void config_parse_config_kegs_file(void);
void config_generate_config_kegs_name(char *outstr, int maxlen, Disk *dsk, int with_extras);
void config_write_config_kegs_file(void);
void insert_disk(int slot, int drive, const char *name, int ejected, const char *partition_name, int part_num);
int cfg_get_fd_size(int fd);
int cfg_partition_read_block(int fd, void *buf, long blk, int blk_size);
int cfg_partition_find_by_name_or_num(Disk *dsk, const char *partnamestr, int part_num);
int cfg_partition_make_list(int fd);
int cfg_maybe_insert_disk(int slot, int drive, const char *namestr);
int cfg_stat(char *path, struct stat *sb);
word32 cfg_get_be_word16(word16 *ptr);
word32 cfg_get_be_word32(word32 *ptr);
void config_file_to_pipe(Disk *dsk, const char *cmd_ptr, const char *name_ptr);
void cfg_htab_vtab(int x, int y);
void cfg_home(void);
void cfg_cleol(void);
void cfg_putchar(int c);
void cfg_printf(const char *fmt, ...);
void cfg_print_num(int num, int max_len);
void cfg_get_disk_name(char *outstr, int maxlen, int type_ext, int with_extras);
void cfg_parse_menu(Cfg_menu *menuptr, int menu_pos, int highlight_pos, int change);
void cfg_get_base_path(char *pathptr, const char *inptr, int go_up);
void cfg_file_init(void);
void cfg_free_alldirents(Cfg_listhdr *listhdrptr);
void cfg_file_add_dirent(Cfg_listhdr *listhdrptr, const char *nameptr, int is_dir, int size, int image_start, int part_num);
int cfg_dirent_sortfn(const void *obj1, const void *obj2);
int cfg_str_match(const char *str1, const char *str2, int len);
int cfg_strlcat(char *dstptr, const char *srcptr, int dstsize);
char *cfg_strncpy(char *dstptr, const char *srcptr, int dstsize);
void cfg_file_readdir(const char *pathptr);
char *cfg_shorten_filename(const char *in_ptr, int maxlen);
void cfg_fix_topent(Cfg_listhdr *listhdrptr);
void cfg_file_draw(void);
void cfg_partition_selected(void);
void cfg_file_update_ptr(char *str);
void cfg_file_selected(void);
void cfg_file_handle_key(int key);
void cfg_draw_menu(void);
int cfg_control_panel_update(void);


/* debugger.c */
void debugger_init(void);
int debugger_run_16ms(void);
void dbg_log_info(double dcycs, word32 info1, word32 info2);
void debugger_update_list_kpc(void);
void debugger_key_event(int a2code, int is_up, int shift_down, int ctrl_down, int lock_down);
void debugger_page_updown(int isup);
void debugger_redraw_screen(Kimage *kimage_ptr);
void debug_draw_debug_line(Kimage *kimage_ptr, int line, int vid_line);
void debugger_help(void);
void dbg_help_show_strs(int help_depth, const char *str, const char *help_str);
const char *debug_find_cmd_in_table(const char *line_ptr, Dbg_longcmd *longptr, int help_depth);
void do_debug_cmd(const char *in_str);
word32 dis_get_memory_ptr(word32 addr);
void show_one_toolset(FILE *toolfile, int toolnum, word32 addr);
void show_toolset_tables(word32 a2bank, word32 addr);
word32 debug_getnum(const char **str_ptr);
void debug_help(const char *str);
void debug_bp(const char *str);
void debug_bp_set(const char *str);
void debug_bp_clear(const char *str);
void debug_bp_clear_all(const char *str);
void debug_bp_setclr(const char *str, int is_set_clear);
void debug_logpc(const char *str);
void debug_logpc_on(const char *str);
void debug_logpc_off(const char *str);
void debug_logpc_out_data(FILE *pcfile, Data_log *log_data_ptr, double start_dcycs);
void debug_logpc_save(const char *cmd_str);
void set_bp(word32 addr, word32 end_addr);
void show_bp(void);
void delete_bp(word32 addr, word32 end_addr);
int do_blank(int mode, int old_mode);
void do_go(void);
void do_step(void);
void xam_mem(int count);
void show_hex_mem(word32 startbank, word32 start, word32 endbank, word32 end, int count);
void do_debug_list(void);
void dis_do_memmove(void);
void dis_do_pattern_search(void);
void dis_do_compare(void);
const char *do_debug_unix(const char *str, int old_mode);
void do_debug_load(void);
char *do_dis(word32 kpc, int accsize, int xsize, int op_provided, word32 instr, int *size_ptr);
int debug_get_view_line(int back);
int debug_add_output_line(char *in_str, int len);
void debug_add_output_string(char *in_str, int len);
void debug_add_output_chars(char *str);
int dbg_printf(const char *fmt, ...);
int dbg_vprintf(const char *fmt, va_list args);
void halt_printf(const char *fmt, ...);
void halt2_printf(const char *fmt, ...);


/* scc.c */
void scc_init(void);
void scc_reset(void);
void scc_hard_reset_port(int port);
void scc_reset_port(int port);
void scc_regen_clocks(int port);
void scc_port_init(int port);
void scc_try_to_empty_writebuf(int port, double dcycs);
void scc_try_fill_readbuf(int port, double dcycs);
void scc_update(double dcycs);
void do_scc_event(int type, double dcycs);
void show_scc_state(void);
void scc_log(int regnum, word32 val, double dcycs);
void show_scc_log(void);
word32 scc_read_reg(int port, double dcycs);
void scc_write_reg(int port, word32 val, double dcycs);
void scc_maybe_br_event(int port, double dcycs);
void scc_evaluate_ints(int port);
void scc_maybe_rx_event(int port, double dcycs);
void scc_maybe_rx_int(int port, double dcycs);
void scc_clr_rx_int(int port);
void scc_handle_tx_event(int port, double dcycs);
void scc_maybe_tx_event(int port, double dcycs);
void scc_clr_tx_int(int port);
void scc_set_zerocnt_int(int port);
void scc_clr_zerocnt_int(int port);
void scc_add_to_readbuf(int port, word32 val, double dcycs);
void scc_add_to_readbufv(int port, double dcycs, const char *fmt, ...);
void scc_transmit(int port, word32 val, double dcycs);
void scc_add_to_writebuf(int port, word32 val, double dcycs);
word32 scc_read_data(int port, double dcycs);
void scc_write_data(int port, word32 val, double dcycs);


/* scc_socket_driver.c */
void scc_socket_init(int port);
void scc_socket_maybe_open_incoming(int port, double dcycs);
void scc_socket_open_outgoing(int port, double dcycs);
void scc_socket_make_nonblock(int port, double dcycs);
void scc_socket_change_params(int port);
void scc_socket_close(int port, int full_close, double dcycs);
void scc_accept_socket(int port, double dcycs);
void scc_socket_telnet_reqs(int port, double dcycs);
void scc_socket_fill_readbuf(int port, int space_left, double dcycs);
void scc_socket_recvd_char(int port, int c, double dcycs);
void scc_socket_empty_writebuf(int port, double dcycs);
void scc_socket_modem_write(int port, int c, double dcycs);
void scc_socket_do_cmd_str(int port, double dcycs);
void scc_socket_send_modem_code(int port, int code, double dcycs);
void scc_socket_modem_hangup(int port, double dcycs);
void scc_socket_modem_connect(int port, double dcycs);
void scc_socket_modem_do_ring(int port, double dcycs);
void scc_socket_do_answer(int port, double dcycs);


/* scc_windriver.c */


/* scc_macdriver.c */


/* iwm.c */
void iwm_init_drive(Disk *dsk, int smartport, int drive, int disk_525);
void disk_set_num_tracks(Disk *dsk, int num_tracks);
void iwm_init(void);
void iwm_reset(void);
void draw_iwm_status(int line, char *buf);
void iwm_flush_disk_to_unix(Disk *dsk);
void iwm_vbl_update(int doit_3_persec);
void iwm_show_stats(void);
void iwm_touch_switches(int loc, double dcycs);
void iwm_move_to_track(Disk *dsk, int new_track);
void iwm525_phase_change(int drive, int phase);
int iwm_read_status35(double dcycs);
void iwm_do_action35(double dcycs);
int iwm_read_c0ec(double dcycs);
int read_iwm(int loc, double dcycs);
void write_iwm(int loc, int val, double dcycs);
int iwm_read_enable2(double dcycs);
int iwm_read_enable2_handshake(double dcycs);
void iwm_write_enable2(int val, double dcycs);
void iwm_fastemul_start_write(Disk *dsk, double dcycs_passed, double dcycs);
word32 iwm_read_data(Disk *dsk, int fast_disk_emul, double dcycs);
void iwm_write_data(Disk *dsk, word32 val, int fast_disk_emul, double dcycs);
void sector_to_partial_nib(byte *in, byte *nib_ptr);
int disk_unnib_4x4(Disk *dsk);
int iwm_denib_track525(Disk *dsk, Trk *trk, int qtr_track, byte *outbuf);
int iwm_denib_track35(Disk *dsk, Trk *trk, int qtr_track, byte *outbuf);
int disk_track_to_unix(Disk *dsk, int qtr_track, byte *outbuf);
void show_hex_data(byte *buf, int count);
void disk_check_nibblization(Disk *dsk, int qtr_track, byte *buf, int size);
void disk_unix_to_nib(Disk *dsk, int qtr_track, int unix_pos, int unix_len, int nib_len);
void iwm_nibblize_track_nib525(Disk *dsk, Trk *trk, byte *track_buf, int qtr_track);
void iwm_nibblize_track_525(Disk *dsk, Trk *trk, byte *track_buf, int qtr_track);
void iwm_nibblize_track_35(Disk *dsk, Trk *trk, byte *track_buf, int qtr_track);
void disk_4x4_nib_out(Disk *dsk, word32 val);
void disk_nib_out(Disk *dsk, byte val, int size);
void disk_nib_out_raw(Disk *dsk, byte val, int size, double dcycs);
void disk_nib_end_track(Disk *dsk);
Disk *iwm_get_dsk_from_slot_drive(int slot, int drive);
void iwm_eject_named_disk(int slot, int drive, const char *name, const char *partition_name);
void iwm_eject_disk_by_num(int slot, int drive);
void iwm_eject_disk(Disk *dsk);
void iwm_show_track(int slot_drive, int track);
void iwm_show_a_track(Disk *dsk, Trk *trk);


/* joystick_driver.c */


/* moremem.c */
void fixup_brks(void);
void fixup_hires_on(void);
void fixup_bank0_2000_4000(void);
void fixup_bank0_0400_0800(void);
void fixup_any_bank_any_page(int start_page, int num_pages, byte *mem0rd, byte *mem0wr);
void fixup_intcx(void);
void fixup_wrdefram(int new_wrdefram);
void fixup_st80col(double dcycs);
void fixup_altzp(void);
void fixup_page2(double dcycs);
void fixup_ramrd(void);
void fixup_ramwrt(void);
void fixup_lcbank2(void);
void fixup_rdrom(void);
void set_statereg(double dcycs, int val);
void fixup_shadow_txt1(void);
void fixup_shadow_txt2(void);
void fixup_shadow_hires1(void);
void fixup_shadow_hires2(void);
void fixup_shadow_shr(void);
void fixup_shadow_iolc(void);
void update_shadow_reg(int val);
void fixup_shadow_all_banks(void);
void setup_pageinfo(void);
void show_bankptrs_bank0rdwr(void);
void show_bankptrs(int bnk);
void show_addr(byte *ptr);
int io_read(word32 loc, double *cyc_ptr);
void io_write(word32 loc, int val, double *cyc_ptr);
word32 get_lines_since_vbl(double dcycs);
int in_vblank(double dcycs);
int read_vid_counters(int loc, double dcycs);


/* paddles.c */
void paddle_fixup_joystick_type(void);
void paddle_trigger(double dcycs);
void paddle_trigger_mouse(double dcycs);
void paddle_trigger_keypad(double dcycs);
void paddle_update_trigger_dcycs(double dcycs);
int read_paddles(double dcycs, int paddle);
void paddle_update_buttons(void);


/* mockingboard.c */
void mock_ay8913_reset(int pair_num, double dcycs);
void mockingboard_reset(double dcycs);
void mock_show_pair(int pair_num, double dcycs, const char *str);
void mock_update_timers(int doit, double dcycs);
void mockingboard_event(double dcycs);
word32 mockingboard_read(word32 loc, double dcycs);
void mockingboard_write(word32 loc, word32 val, double dcycs);
word32 mock_6522_read(int pair_num, word32 loc, double dcycs);
void mock_6522_write(int pair_num, word32 loc, word32 val, double dcycs);
word32 mock_6522_new_ifr(word32 ifr, word32 ier);
word32 mock_ay8913_read(int pair_num, double dcycs);
void mock_ay8913_reg_read(int pair_num, double dcycs);
void mock_ay8913_reg_write(int pair_num, double dcycs);
void mock_ay8913_control_update(int pair_num, word32 new_val, word32 prev_val, double dcycs);
void mockingboard_show(int got_num, word32 disable_mask);


/* sim65816.c */
int sim_get_force_depth(void);
int sim_get_use_shmem(void);
void sim_set_use_shmem(int use_shmem);
word32 toolbox_debug_4byte(word32 addr);
void toolbox_debug_c(word32 xreg, word32 stack, double *cyc_ptr);
void show_toolbox_log(void);
word32 get_memory_io(word32 loc, double *cyc_ptr);
void set_memory_io(word32 loc, int val, double *cyc_ptr);
void show_regs_act(Engine_reg *eptr);
void show_regs(void);
void my_exit(int ret);
void do_reset(void);
void check_engine_asm_defines(void);
byte *memalloc_align(int size, int skip_amt, void **alloc_ptr);
void memory_ptr_init(void);
int parse_argv(int argc, char **argv, int slashes_to_find);
int kegs_init(int mdepth);
void load_roms_init_memory(void);
void kegs_expand_path(char *out_ptr, const char *in_ptr, int maxlen);
void setup_kegs_file(char *outname, int maxlen, int ok_if_missing, int can_create_file, const char **name_ptr);
void initialize_events(void);
void check_for_one_event_type(int type);
void add_event_entry(double dcycs, int type);
double remove_event_entry(int type);
void add_event_stop(double dcycs);
void add_event_doc(double dcycs, int osc);
void add_event_scc(double dcycs, int type);
void add_event_vbl(void);
void add_event_vid_upd(int line);
void add_event_mockingboard(double dcycs);
double remove_event_doc(int osc);
double remove_event_scc(int type);
void remove_event_mockingboard(void);
void show_all_events(void);
void show_pmhz(void);
void setup_zip_speeds(void);
int run_16ms(void);
int run_a2_one_vbl(void);
void add_irq(word32 irq_mask);
void remove_irq(word32 irq_mask);
void take_irq(int is_it_brk);
void show_dtime_array(void);
void update_60hz(double dcycs, double dtime_now);
void do_vbl_int(void);
void do_scan_int(double dcycs, int line);
void check_scan_line_int(double dcycs, int cur_video_line);
void check_for_new_scan_int(double dcycs);
void init_reg(void);
void handle_action(word32 ret);
void do_break(word32 ret);
void do_cop(word32 ret);
void do_wdm(word32 arg);
void do_wai(void);
void do_stp(void);
void size_fail(int val, word32 v1, word32 v2);
int fatal_printf(const char *fmt, ...);
int kegs_vprintf(const char *fmt, va_list ap);
void must_write(int fd, char *bufptr, int len);
void clear_fatal_logs(void);
char *kegs_malloc_str(char *in_str);


/* smartport.c */
void smartport_error(void);
void smartport_log(word32 start_addr, int cmd, int rts_addr, int cmd_list);
void do_c70d(word32 arg0);
void do_c70a(word32 arg0);
int do_read_c7(int unit_num, word32 buf, int blk);
int do_write_c7(int unit_num, word32 buf, int blk);
int do_format_c7(int unit_num);
void do_c700(word32 ret);


/* sound.c */
void doc_log_rout(char *msg, int osc, double dsamps, int etc);
void show_doc_log(void);
void sound_init(void);
void sound_set_audio_rate(int rate);
void sound_reset(double dcycs);
void sound_shutdown(void);
void sound_update(double dcycs, double dtime);
void open_sound_file(void);
void close_sound_file(void);
void check_for_range(word32 *addr, int num_samps, int offset);
void send_sound_to_file(word32 *addr, int shm_pos, int num_samps);
void show_c030_state(void);
void show_c030_samps(int *outptr, int num);
void sound_play(double dsamps);
void sound_mock_envelope(int pair, int *env_ptr, int num_samps, int *vol_ptr);
void sound_mock_noise(int pair, byte *noise_ptr, int num_samps);
void sound_mock_play(int pair, int channel, int *outptr, int *env_ptr, byte *noise_ptr, int *vol_ptr, int num_samps);
void doc_handle_event(int osc, double dcycs);
void doc_sound_end(int osc, int can_repeat, double eff_dsamps, double dsamps);
void add_sound_irq(int osc);
void remove_sound_irq(int osc, int must);
void start_sound(int osc, double eff_dsamps, double dsamps);
void wave_end_estimate(int osc, double eff_dsamps, double dsamps);
void remove_sound_event(int osc);
void doc_write_ctl_reg(int osc, int val, double dsamps);
void doc_recalc_sound_parms(int osc, double eff_dcycs, double dsamps);
int doc_read_c030(double dcycs);
int doc_read_c03c(double dcycs);
int doc_read_c03d(double dcycs);
void doc_write_c03c(int val, double dcycs);
void doc_write_c03d(int val, double dcycs);
void doc_show_ensoniq_state(int osc);


/* sound_driver.c */
void snddrv_init(void);
void sound_child_fork(int size);
void parent_sound_get_sample_rate(int read_fd);
void snddrv_shutdown(void);
void snddrv_send_sound(int real_samps, int size);
void child_sound_playit(word32 tmp);
void reliable_buf_write(word32 *shm_addr, int pos, int size);
void reliable_zero_write(int amt);
int child_send_samples(byte *ptr, int size);
void child_sound_loop(int read_fd, int write_fd, word32 *shm_addr);


/* unshk.c */
word32 unshk_get_long4(byte *bptr);
word32 unshk_get_word2(byte *bptr);
word32 unshk_calc_crc(byte *bptr, int size, word32 start_crc);
int unshk_unrle(byte *cptr, int len, word32 rle_delim, byte *ucptr);
void unshk_lzw_clear(Lzw_state *lzw_ptr, word32 thread_format);
byte *unshk_unlzw(byte *cptr, Lzw_state *lzw_ptr, byte *ucptr, word32 uclen, word32 thread_format);
void unshk_data(Disk *dsk, byte *cptr, word32 compr_size, byte *ucptr, word32 uncompr_size, word32 thread_format, byte *base_cptr);
void unshk_parse_header(Disk *dsk, byte *cptr, int compr_size, byte *base_cptr);
void unshk(Disk *dsk, const char *name_str);


/* undeflate.c */
void show_bits(unsigned *llptr, int nl);
void show_huftb(unsigned *tabptr, int bits);
void undeflate_init_len_dist_tab(word32 *tabptr, word64 drepeats, word32 start);
void undeflate_init_bit_rev_tab(word32 *tabptr, int num);
word32 undeflate_bit_reverse(word32 val, word32 bits);
word32 undeflate_calc_crc32(byte *bptr, word32 len);
byte *undeflate_ensure_dest_len(Disk *dsk, byte *ucptr, word32 len);
void undeflate_add_tab_code(word32 *tabptr, word32 tabsz_lg2, word32 code, word32 entry);
word32 *undeflate_init_fixed_tabs(void);
void undeflate_check_bit_reverse(void);
word32 *undeflate_build_huff_tab(word32 *tabptr, word32 *entry_ptr, word32 len_size, word32 *bl_count_ptr, int max_bits);
word32 *undeflate_dynamic_table(byte *cptr, word32 *bit_pos_ptr, byte *cptr_base);
byte *undeflate_block(Disk *dsk, byte *cptr, word32 *bit_pos_ptr, byte *cptr_base, byte *cptr_end);
byte *undeflate_gzip_header(Disk *dsk, byte *cptr, int compr_size);
void undeflate_gzip(Disk *dsk, const char *name_str);


/* video.c */
void video_set_red_mask(word32 red_mask);
void video_set_green_mask(word32 green_mask);
void video_set_blue_mask(word32 blue_mask);
void video_set_alpha_mask(word32 alpha_mask);
void video_set_mask_and_shift(word32 x_mask, word32 *mask_ptr, int *shift_left_ptr, int *shift_right_ptr);
void video_set_palette(void);
void video_set_redraw_skip_amt(int amt);
Kimage *video_get_kimage(int win_id);
char *video_get_status_ptr(int line);
void video_set_x_refresh_needed(Kimage *kimage_ptr, int do_refresh);
int video_get_active(Kimage *kimage_ptr);
void video_set_active(Kimage *kimage_ptr, int active);
void video_init(int mdepth);
void video_init_kimage(Kimage *kimage_ptr, int width, int height);
void show_a2_line_stuff(void);
void video_reset(void);
void video_update(void);
int video_all_stat_to_line_stat(int line, int new_all_stat);
void change_display_mode(double dcycs);
void video_update_all_stat_through_line(int line);
void change_border_color(double dcycs, int val);
void update_border_info(void);
void update_border_line(int st_line_offset, int end_line_offset, int color);
void video_border_pixel_write(Kimage *kimage_ptr, int starty, int num_lines, int color, int st_off, int end_off);
void redraw_changed_text(int start_offset, int start_line, int reparse, word32 *in_wptr, int altcharset, word32 bg_pixel, word32 fg_pixel, int pixels_per_line, int dbl);
void redraw_changed_string(byte *bptr, int start_line, word32 ch_mask, word32 *in_wptr, word32 bg_pixel, word32 fg_pixel, int pixels_per_line, int dbl);
void redraw_changed_gr(int start_offset, int start_line, int reparse, word32 *in_wptr, int pixels_per_line, int dbl);
void video_hgr_line_segment(byte *slow_mem_ptr, word32 *wptr, int x1, int monochrome, int dbl, int pixels_per_line, int st_line);
void redraw_changed_hgr(int start_offset, int start_line, int reparse, word32 *in_wptr, int pixels_per_line, int monochrome, int dbl);
int video_rebuild_super_hires_palette(word32 scan_info, int line, int reparse);
word32 redraw_changed_super_hires_oneline(word32 *in_wptr, int pixels_per_line, int y, int scan, word32 ch_mask);
void redraw_changed_super_hires(int start_offset, int start_line, int reparse, word32 *wptr, int pixels_per_line);
void video_update_event_line(int line);
void video_update_through_line(int line);
void video_refresh_line(int line, int must_reparse);
void prepare_a2_font(void);
void video_add_rect(Kimage *kimage_ptr, int x, int y, int width, int height);
void video_add_a2_rect(int start_line, int end_line, int left_pix, int right_pix);
void video_form_change_rects(void);
int video_get_a2_width(Kimage *kimage_ptr);
int video_get_a2_height(Kimage *kimage_ptr);
int video_out_query(Kimage *kimage_ptr);
void video_out_done(Kimage *kimage_ptr);
int video_out_data(void *vptr, Kimage *kimage_ptr, int out_width_act, Change_rect *rectptr, int pos);
int video_out_data_intscaled(void *vptr, Kimage *kimage_ptr, int out_width_act, Change_rect *rectptr);
int video_out_data_scaled(void *vptr, Kimage *kimage_ptr, int out_width_act, Change_rect *rectptr);
word32 video_scale_calc_frac(int pos, int out_max, word32 frac_inc, word32 frac_inc_inv);
void video_update_scale(Kimage *kimage_ptr, int out_width, int out_height);
int video_scale_mouse_x(Kimage *kimage_ptr, int raw_x, int x_width);
int video_scale_mouse_y(Kimage *kimage_ptr, int raw_y, int y_height);
int video_unscale_mouse_x(Kimage *kimage_ptr, int a2_x, int x_width);
int video_unscale_mouse_y(Kimage *kimage_ptr, int a2_y, int y_height);
void video_update_color_raw(int col_num, int a2_color);
void video_update_status_line(int line, const char *string);
void video_show_debug_info(void);
word32 float_bus(double dcycs);

