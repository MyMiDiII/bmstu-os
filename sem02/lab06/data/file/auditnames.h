struct audit_names {
	struct list_head list;

	struct filename	*name;
	int			    name_len;
	bool			hidden;

	unsigned long	ino;
	dev_t			dev;
	umode_t			mode;
	kuid_t			uid;
	kgid_t			gid;
	dev_t			rdev;
	u32			    osid;
	struct audit_cap_data	fcap;
	unsigned int		    fcap_ver;
	unsigned char		    type;
	bool			        should_free;
};
