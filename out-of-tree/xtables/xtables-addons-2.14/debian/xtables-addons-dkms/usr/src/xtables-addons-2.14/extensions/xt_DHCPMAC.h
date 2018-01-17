#ifndef _LINUX_NETFILTER_XT_DHCPMAC_H
#define _LINUX_NETFILTER_XT_DHCPMAC_H 1

#define DH_MAC_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define DH_MAC_HEX(z) z[0], z[1], z[2], z[3], z[4], z[5]

struct dhcpmac_info {
	unsigned char addr[ETH_ALEN];
	uint8_t mask, invert;
};

#endif /* _LINUX_NETFILTER_XT_DHCPMAC_H */
