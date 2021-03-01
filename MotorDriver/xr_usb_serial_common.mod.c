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
	{ 0xe7cc3b88, "module_layout" },
	{ 0x2920af5f, "usb_deregister" },
	{ 0xc5850110, "printk" },
	{ 0x25270b7e, "put_tty_driver" },
	{ 0x7e95e504, "tty_unregister_driver" },
	{ 0xa25b516c, "usb_register_driver" },
	{ 0xb8f5588b, "tty_register_driver" },
	{ 0x8a140e9f, "tty_set_operations" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x71a0f97d, "__tty_alloc_driver" },
	{ 0xaed736a9, "tty_port_register_device" },
	{ 0xba4a7b8a, "usb_get_intf" },
	{ 0x6cf4af, "usb_driver_claim_interface" },
	{ 0x45e226be, "_dev_info" },
	{ 0x2d6fcc06, "__kmalloc" },
	{ 0x6ca95036, "device_create_file" },
	{ 0xbbbca01e, "_dev_warn" },
	{ 0xcd140bcd, "usb_alloc_urb" },
	{ 0x1d59b75d, "usb_alloc_coherent" },
	{ 0x7c13ee85, "tty_port_init" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0xefd12756, "usb_ifnum_to_if" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0x5f754e5a, "memset" },
	{ 0xc6cbbc89, "capable" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0xa9a5f9a6, "kmem_cache_alloc_trace" },
	{ 0xcda51ca2, "kmalloc_caches" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0x71c90087, "memcmp" },
	{ 0x409873e3, "tty_termios_baud_rate" },
	{ 0x870fd6f6, "tty_port_open" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x91a79a2, "usb_autopm_put_interface" },
	{ 0x51679d7b, "usb_autopm_get_interface" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xdb9ca3c5, "_raw_spin_lock" },
	{ 0xd697e69a, "trace_hardirqs_on" },
	{ 0x2da81bff, "_raw_spin_lock_irq" },
	{ 0x8489f41d, "tty_standard_install" },
	{ 0xe2b5e146, "refcount_inc_not_zero_checked" },
	{ 0xac735808, "tty_port_close" },
	{ 0xa7714062, "usb_autopm_get_interface_async" },
	{ 0x3bd51bfb, "tty_port_hangup" },
	{ 0x34dfce65, "tty_port_tty_wakeup" },
	{ 0x37a0cba, "kfree" },
	{ 0x20a3f147, "usb_put_intf" },
	{ 0x55f3e465, "tty_flip_buffer_push" },
	{ 0x9c67a9d7, "tty_insert_flip_string_fixed_flag" },
	{ 0xb2d48a2e, "queue_work_on" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x527ac8f3, "tty_port_tty_hangup" },
	{ 0x6ebe366f, "ktime_get_mono_fast_ns" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x124c40a6, "tty_port_put" },
	{ 0x6fe86e64, "usb_driver_release_interface" },
	{ 0xb3bc7e99, "usb_free_urb" },
	{ 0x34a242b2, "tty_unregister_device" },
	{ 0xb2ab6238, "tty_kref_put" },
	{ 0x350c5b71, "tty_vhangup" },
	{ 0xb6806c3f, "tty_port_tty_get" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0xcff8b182, "device_remove_file" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0xb765cdd3, "usb_free_coherent" },
	{ 0x4205ad24, "cancel_work_sync" },
	{ 0xc6968c7, "usb_kill_urb" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0x2a3aa678, "_test_and_clear_bit" },
	{ 0xb4ee6bd3, "usb_autopm_put_interface_async" },
	{ 0xf4b72d54, "_dev_err" },
	{ 0xd48c98ef, "usb_submit_urb" },
	{ 0x9d669763, "memcpy" },
	{ 0xc9e55696, "usb_control_msg" },
	{ 0x39a12ca7, "_raw_spin_unlock_irqrestore" },
	{ 0x5f849a69, "_raw_spin_lock_irqsave" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v04E2p1410d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1411d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1412d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1414d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1420d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1421d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1422d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1424d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1400d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1401d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1402d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E2p1403d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2890p0213d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "E61C727E66DCC716B13DD8F");
