static bool mac_parse(const char *addr, unsigned char *dest, uint8_t *mask)
{
	unsigned int i = 0, value;
	char *end;

	for (i = 0; i < ETH_ALEN; ++i) {
		value = strtoul(addr, &end, 16);
		if (addr == end || value > 0xFF)
			return false;
		if (i == ETH_ALEN - 1) {
			if (*end != '\0' && *end != '/')
				return false;
		} else if (*end != ':') {
			return false;
		}
		dest[i] = value;
		addr = end + 1;
	}

	*mask = 48;
	if (*end == '/') {
		if (!xtables_strtoui(end + 1, &end, &value, 0, 48))
			return false;
		if (*end != '\0')
			return false;
	}

	return true;
}
