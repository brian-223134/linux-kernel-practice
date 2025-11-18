#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x31d7ffc5, "jbd3_inode_cache" },
	{ 0xe24f5060, "jbd3_journal_forget" },
	{ 0x7142c11e, "jbd3_journal_stop" },
	{ 0x7a7a032c, "jbd3_journal_force_commit" },
	{ 0xeb5b9ec7, "jbd3_journal_dirty_metadata" },
	{ 0xf9d8149e, "jbd3_journal_check_available_features" },
	{ 0x21756af8, "jbd3_journal_force_commit_nested" },
	{ 0xc3ec1c94, "jbd3_journal_init_jbd_inode" },
	{ 0x8cc82cf6, "jbd3_journal_destroy" },
	{ 0xf21f3d39, "jbd3_journal_revoke" },
	{ 0x4122a94d, "jbd3_journal_extend" },
	{ 0x6f86588b, "jbd3_complete_transaction" },
	{ 0xa438a280, "jbd3_journal_load" },
	{ 0xc975e596, "jbd3_trans_will_send_data_barrier" },
	{ 0x5b9b0ccc, "jbd3_journal_clear_features" },
	{ 0xb1c35d38, "jbd3_journal_restart" },
	{ 0xf67cd4e6, "jbd3_journal_release_jbd_inode" },
	{ 0x14ed7825, "jbd3_journal_get_write_access" },
	{ 0x9993abd, "jbd3_journal_lock_updates" },
	{ 0xfc5df39d, "jbd3_journal_clear_err" },
	{ 0x25b89123, "jbd3_journal_init_dev" },
	{ 0x37c1d2d6, "jbd3_journal_begin_ordered_truncate" },
	{ 0x11125db5, "jbd3_journal_flush" },
	{ 0xffc34517, "jbd3_journal_invalidatepage" },
	{ 0xc24ab43d, "jbd3_journal_init_inode" },
	{ 0x4f5cba65, "jbd3__journal_start" },
	{ 0xdea19381, "jbd3_journal_errno" },
	{ 0x6ad3c36a, "jbd3_journal_update_sb_errno" },
	{ 0xf0b136d, "jbd3_journal_inode_ranged_write" },
	{ 0xa1de11a6, "jbd3_journal_abort" },
	{ 0x8e078b69, "jbd3_journal_inode_ranged_wait" },
	{ 0xb17986a8, "jbd3_log_wait_commit" },
	{ 0xc0e16814, "jbd3_journal_wipe" },
	{ 0xe429053c, "jbd3_journal_try_to_free_buffers" },
	{ 0x4eb2b01c, "jbd3_journal_set_features" },
	{ 0x6b2870b3, "jbd3_journal_free_reserved" },
	{ 0xf3d26fd0, "jbd3_journal_start_commit" },
	{ 0x28dbebb8, "jbd3_journal_start_reserved" },
	{ 0x75bf2b63, "jbd3_journal_unlock_updates" },
	{ 0xbf4b4611, "jbd3_journal_get_create_access" },
	{ 0x26bb67b9, "jbd3_journal_blocks_per_page" },
	{ 0x4b1dfa31, "jbd3_transaction_committed" },
};

MODULE_INFO(depends, "jbd3");


MODULE_INFO(srcversion, "6C1DFCD75567F2140676A88");
