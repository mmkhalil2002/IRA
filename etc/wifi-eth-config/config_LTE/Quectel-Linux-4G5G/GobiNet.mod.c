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
	{ 0x70e440, "module_layout" },
	{ 0x28a5d5ae, "cdev_del" },
	{ 0xe1fbc38f, "kmalloc_caches" },
	{ 0x2d6fcc06, "__kmalloc" },
	{ 0x2a9ce9a2, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0x12585e08, "_raw_spin_unlock" },
	{ 0x60b63428, "usbnet_resume" },
	{ 0xd4ddb82a, "param_ops_int" },
	{ 0x62e0d83d, "usbnet_probe" },
	{ 0xc891136a, "__wake_up_sync" },
	{ 0x5ce04931, "netif_carrier_on" },
	{ 0x3dd2114a, "usbnet_disconnect" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xca5a7528, "down_interruptible" },
	{ 0xab2868b5, "netif_carrier_off" },
	{ 0xca0814e, "usb_kill_urb" },
	{ 0xddecd509, "device_destroy" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xeb39f272, "__dev_kfree_skb_any" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0xc34ab941, "usb_autopm_get_interface" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x8488320e, "kthread_create_on_node" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0x754f348a, "complete_all" },
	{ 0x71eb10ed, "__netdev_alloc_skb" },
	{ 0xd718b80e, "netif_rx" },
	{ 0x5bbe49f4, "__init_waitqueue_head" },
	{ 0x5f754e5a, "memset" },
	{ 0x39a12ca7, "_raw_spin_unlock_irqrestore" },
	{ 0x4c02fffe, "usb_deregister" },
	{ 0xc5850110, "printk" },
	{ 0x7fa59411, "usb_set_interface" },
	{ 0x6aa0b9b0, "free_netdev" },
	{ 0x4e00bc92, "register_netdev" },
	{ 0x174f82da, "usb_control_msg" },
	{ 0x8893685c, "skb_push" },
	{ 0x1e6d26a8, "strstr" },
	{ 0x7c9873b0, "usbnet_start_xmit" },
	{ 0x9e915354, "device_create" },
	{ 0x80b33b49, "usbnet_suspend" },
	{ 0xa9e6cbbe, "skb_pull" },
	{ 0x7a68cbd2, "_dev_err" },
	{ 0x249e95b1, "cdev_add" },
	{ 0xcd318b53, "netif_device_attach" },
	{ 0xff742542, "_dev_info" },
	{ 0x15956d79, "usb_submit_urb" },
	{ 0xd9ce8f0c, "strnlen" },
	{ 0xc16e6d25, "usbnet_tx_timeout" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xb14c5254, "usb_clear_halt" },
	{ 0x5ba7212a, "usbnet_skb_return" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xc104368b, "wait_for_completion_interruptible_timeout" },
	{ 0xc1d068c2, "eth_type_trans" },
	{ 0x77974b14, "wake_up_process" },
	{ 0x5bfb48d5, "pskb_expand_head" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x7398040a, "kmem_cache_alloc_trace" },
	{ 0xdb9ca3c5, "_raw_spin_lock" },
	{ 0x5f849a69, "_raw_spin_lock_irqsave" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x581cde4e, "up" },
	{ 0xe97eeeb5, "usb_register_driver" },
	{ 0x27789b6d, "class_destroy" },
	{ 0x5f030671, "unregister_netdev" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0xc358aaf8, "snprintf" },
	{ 0xc0a572c4, "dev_queue_xmit" },
	{ 0x49ebacbd, "_clear_bit" },
	{ 0xe1cb08d9, "skb_put" },
	{ 0xc5a227e7, "param_ops_uint" },
	{ 0x9fbc1528, "__class_create" },
	{ 0x7b248e58, "usb_free_urb" },
	{ 0x8b5927a0, "down_timeout" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x17c78c57, "usb_autopm_put_interface" },
	{ 0x18a2d854, "usb_alloc_urb" },
	{ 0xc37efa05, "alloc_etherdev_mqs" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v05C6p9003d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9215d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0125d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0121d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0306d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0435d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0296d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0191d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0195d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0512d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0620d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2C7Cp0800d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "0C16CA9369F2ADA32A5B530");
