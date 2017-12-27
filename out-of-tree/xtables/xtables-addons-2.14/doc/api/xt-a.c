match:

	/* true/false */
	bool
	(*match)(
		const struct sk_buff *skb,
		struct xt_action_param *,
	);

	/* error code */
	int
	(*checkentry)(
		const struct xt_mtchk_param *,
	);

	void
	(*destroy)(
		const struct xt_mtdtor_param *,
	);

target:

	/* verdict */
	unsigned int
	(*target)(
		struct sk_buff **pskb,
		const struct xt_action_param *,
	);

	/* error code */
	int
	(*checkentry)(
		const struct xt_tgchk_param *,
	);

	void
	(*destroy)(
		const struct xt_tgdtor_param *,
	);
