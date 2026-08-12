private:
	class VidmprxCreate {
		mutable int used_;
		SM_VIDMPRX_CREATE_PARMS vidmprxp_;
		tSMVidMPrxId *vidmprxP_;
	public:
		VidmprxCreate(const VidmprxCreate &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxCreate() : used_(0) {
			memset(&vidmprxp_, 0, sizeof(vidmprxp_));
			vidmprxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_create(&vidmprxp_));
			used_ = 1;
			if (vidmprxP_) *vidmprxP_ = vidmprxp_.vidmprx;
			return e;
		};
		~VidmprxCreate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_create not called\n");
				abort();
			};
		};
		VidmprxCreate &vidmprx(tSMVidMPrxId *v) {
			vidmprxP_ = v;
			return *this;
		};
		VidmprxCreate &module(tSMModuleId v) {
			vidmprxp_.module = v;
			return *this;
		};
		VidmprxCreate &type(enum kSMVidMPrxType v) {
			vidmprxp_.type = v;
			return *this;
		};
		VidmprxCreate &address(in_addr v) {
			vidmprxp_.address = v;
			return *this;
		};
		VidmprxCreate &ipv6_address(in6_addr v) {
			vidmprxp_.ipv6_address = v;
			return *this;
		};
	};
public:
	VidmprxCreate vidmprx_create() {
		return VidmprxCreate().module(module_);
	}
private:
	class VidmptxCreate {
		mutable int used_;
		SM_VIDMPTX_CREATE_PARMS vidmptxp_;
		tSMVidMPtxId *vidmptxP_;
	public:
		VidmptxCreate(const VidmptxCreate &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxCreate() : used_(0) {
			memset(&vidmptxp_, 0, sizeof(vidmptxp_));
			vidmptxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_create(&vidmptxp_));
			used_ = 1;
			if (vidmptxP_) *vidmptxP_ = vidmptxp_.vidmptx;
			return e;
		};
		~VidmptxCreate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_create not called\n");
				abort();
			};
		};
		VidmptxCreate &vidmptx(tSMVidMPtxId *v) {
			vidmptxP_ = v;
			return *this;
		};
		VidmptxCreate &module(tSMModuleId v) {
			vidmptxp_.module = v;
			return *this;
		};
	};
public:
	VidmptxCreate vidmptx_create() {
		return VidmptxCreate().module(module_);
	}
private:
	class VmptxCreateCsrcList {
		mutable int used_;
		SM_VMPTX_CREATE_CSRC_LIST_PARMS csrcp_;
		tSMVMPTxCSRCListId *csrc_listP_;
	public:
		VmptxCreateCsrcList(const VmptxCreateCsrcList &old) {
			*this = old;
			old.used_ = 1;
		};
		VmptxCreateCsrcList() : used_(0) {
			memset(&csrcp_, 0, sizeof(csrcp_));
			csrc_listP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_create_csrc_list(&csrcp_));
			used_ = 1;
			if (csrc_listP_) *csrc_listP_ = csrcp_.csrc_list;
			return e;
		};
		~VmptxCreateCsrcList() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_create_csrc_list not called\n");
				abort();
			};
		};
		VmptxCreateCsrcList &csrc_list(tSMVMPTxCSRCListId *v) {
			csrc_listP_ = v;
			return *this;
		};
		VmptxCreateCsrcList &module(tSMModuleId v) {
			csrcp_.module = v;
			return *this;
		};
	};
public:
	VmptxCreateCsrcList vmptx_create_csrc_list() {
		return VmptxCreateCsrcList().module(module_);
	}
private:
	class VmptxCreateToneset {
		mutable int used_;
		SM_VMPTX_CREATE_TONESET_PARMS tonesetp_;
		tSMVMPTxToneSetId *tone_set_idP_;
	public:
		VmptxCreateToneset(const VmptxCreateToneset &old) {
			*this = old;
			old.used_ = 1;
		};
		VmptxCreateToneset() : used_(0) {
			memset(&tonesetp_, 0, sizeof(tonesetp_));
			tone_set_idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_create_toneset(&tonesetp_));
			used_ = 1;
			if (tone_set_idP_) *tone_set_idP_ = tonesetp_.tone_set_id;
			return e;
		};
		~VmptxCreateToneset() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_create_toneset not called\n");
				abort();
			};
		};
		VmptxCreateToneset &toneset(tSMVMPTxToneSet* v) {
			tonesetp_.toneset = v;
			return *this;
		};
		VmptxCreateToneset &module(tSMModuleId v) {
			tonesetp_.module = v;
			return *this;
		};
		VmptxCreateToneset &tone_set_id(tSMVMPTxToneSetId *v) {
			tone_set_idP_ = v;
			return *this;
		};
	};
public:
	VmptxCreateToneset vmptx_create_toneset() {
		return VmptxCreateToneset().module(module_);
	}
