#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/connector.h>

#include "xt_pknock.h"

#define GROUP 1

static struct sockaddr_nl src_addr, dest_addr;
static int sock_fd;

static unsigned char *buf;

static struct xt_pknock_nl_msg *nlmsg;

int main(void)
{
	socklen_t addrlen;
	int status;
	int group = GROUP;

	int buf_size;

	const char *ip;
	char ipbuf[48];

	sock_fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

	if (sock_fd == -1) {
		perror("socket()");
		return 1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	src_addr.nl_groups = group;

	status = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

	if (status == -1) {
		close(sock_fd);
		perror("bind()");
		return 1;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;
	dest_addr.nl_groups = group;

	buf_size = sizeof(struct xt_pknock_nl_msg) + sizeof(struct cn_msg) + sizeof(struct nlmsghdr);
	buf = malloc(buf_size);

	if (!buf) {
		perror("malloc()");
		return 1;
	}

	addrlen = sizeof(dest_addr);

	while(1) {

		memset(buf, 0, buf_size);

		status = recvfrom(sock_fd, buf, buf_size, 0, (struct sockaddr *)&dest_addr, &addrlen);

		if (status <= 0) {
			perror("recvfrom()");
			return 1;
		}

	nlmsg = (struct xt_pknock_nl_msg *) (buf + sizeof(struct cn_msg) + sizeof(struct nlmsghdr));

		ip = inet_ntop(AF_INET, &nlmsg->peer_ip, ipbuf, sizeof(ipbuf));
		printf("rule_name: %s - ip %s\n", nlmsg->rule_name, ip);

	}

	close(sock_fd);

	free(buf);

	return 0;
}
