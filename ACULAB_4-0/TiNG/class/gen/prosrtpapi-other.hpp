private:
	class VidmprxConfigCodecRfc3984 {
		mutable int used_;
		SM_VIDMPRX_CODEC_RFC3984_PARMS codecp_;
	public:
		VidmprxConfigCodecRfc3984(const VidmprxConfigCodecRfc3984 &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxConfigCodecRfc3984() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_config_codec_rfc3984(&codecp_));
			used_ = 1;
			return e;
		};
		~VidmprxConfigCodecRfc3984() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_config_codec_rfc3984 not called\n");
				abort();
			};
		};
		VidmprxConfigCodecRfc3984 &vidmprx(tSMVidMPrxId v) {
			codecp_.vidmprx = v;
			return *this;
		};
		VidmprxConfigCodecRfc3984 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		VidmprxConfigCodecRfc3984 &pic_size(enum kSMRFC3984PicSize v) {
			codecp_.pic_size = v;
			return *this;
		};
	};
public:
	VidmprxConfigCodecRfc3984 vidmprx_config_codec_rfc3984() {
		return VidmprxConfigCodecRfc3984();
	}
private:
	class VidmprxConfigCodecRfc4629 {
		mutable int used_;
		SM_VIDMPRX_CODEC_RFC4629_PARMS codecp_;
	public:
		VidmprxConfigCodecRfc4629(const VidmprxConfigCodecRfc4629 &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxConfigCodecRfc4629() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_config_codec_rfc4629(&codecp_));
			used_ = 1;
			return e;
		};
		~VidmprxConfigCodecRfc4629() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_config_codec_rfc4629 not called\n");
				abort();
			};
		};
		VidmprxConfigCodecRfc4629 &vidmprx(tSMVidMPrxId v) {
			codecp_.vidmprx = v;
			return *this;
		};
		VidmprxConfigCodecRfc4629 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		VidmprxConfigCodecRfc4629 &h263_profile(tSM_INT v) {
			codecp_.h263_profile = v;
			return *this;
		};
		VidmprxConfigCodecRfc4629 &h263_level(tSM_INT v) {
			codecp_.h263_level = v;
			return *this;
		};
	};
public:
	VidmprxConfigCodecRfc4629 vidmprx_config_codec_rfc4629() {
		return VidmprxConfigCodecRfc4629();
	}
private:
	class VidmprxConfigDataloss {
		mutable int used_;
		SM_VIDMPRX_DATALOSS_PARMS datalossp_;
	public:
		VidmprxConfigDataloss(const VidmprxConfigDataloss &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxConfigDataloss() : used_(0) {
			memset(&datalossp_, 0, sizeof(datalossp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_config_dataloss(&datalossp_));
			used_ = 1;
			return e;
		};
		~VidmprxConfigDataloss() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_config_dataloss not called\n");
				abort();
			};
		};
		VidmprxConfigDataloss &vidmprx(tSMVidMPrxId v) {
			datalossp_.vidmprx = v;
			return *this;
		};
		VidmprxConfigDataloss &loss_threshold(tSM_INT v) {
			datalossp_.loss_threshold = v;
			return *this;
		};
	};
public:
	VidmprxConfigDataloss vidmprx_config_dataloss() {
		return VidmprxConfigDataloss();
	}
private:
	class VidmprxConfigJitter {
		mutable int used_;
		SM_VIDMPRX_JITTER_PARMS jitterp_;
	public:
		VidmprxConfigJitter(const VidmprxConfigJitter &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxConfigJitter() : used_(0) {
			memset(&jitterp_, 0, sizeof(jitterp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_config_jitter(&jitterp_));
			used_ = 1;
			return e;
		};
		~VidmprxConfigJitter() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_config_jitter not called\n");
				abort();
			};
		};
		VidmprxConfigJitter &vidmprx(tSMVidMPrxId v) {
			jitterp_.vidmprx = v;
			return *this;
		};
		VidmprxConfigJitter &max_delay_ms(tSM_UT32 v) {
			jitterp_.max_delay_ms = v;
			return *this;
		};
		VidmprxConfigJitter &initial_delay_ms(tSM_UT32 v) {
			jitterp_.initial_delay_ms = v;
			return *this;
		};
	};
public:
	VidmprxConfigJitter vidmprx_config_jitter() {
		return VidmprxConfigJitter();
	}
private:
	class VidmprxConfigUnhandledPayloadReporting {
		mutable int used_;
		SM_VIDMPRX_UNHANDLED_PAYLOAD_REPORTING_PARMS pp_;
	public:
		VidmprxConfigUnhandledPayloadReporting(const VidmprxConfigUnhandledPayloadReporting &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxConfigUnhandledPayloadReporting() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_config_unhandled_payload_reporting(&pp_));
			used_ = 1;
			return e;
		};
		~VidmprxConfigUnhandledPayloadReporting() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_config_unhandled_payload_reporting not called\n");
				abort();
			};
		};
		VidmprxConfigUnhandledPayloadReporting &vidmprx(tSMVidMPrxId v) {
			pp_.vidmprx = v;
			return *this;
		};
		VidmprxConfigUnhandledPayloadReporting &delay(tSM_INT v) {
			pp_.delay = v;
			return *this;
		};
	};
public:
	VidmprxConfigUnhandledPayloadReporting vidmprx_config_unhandled_payload_reporting() {
		return VidmprxConfigUnhandledPayloadReporting();
	}
private:
	class VidmprxDestroy {
		mutable int used_;
		tSMVidMPrxId vidmprx_;
	public:
		VidmprxDestroy(const VidmprxDestroy &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxDestroy(tSMVidMPrxId vidmprx) : used_(0) {
			vidmprx_ = vidmprx;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_destroy(vidmprx_));
			used_ = 1;
			return e;
		};
		~VidmprxDestroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_destroy not called\n");
				abort();
			};
		};
	};
public:
	VidmprxDestroy vidmprx_destroy() {
		VidmprxDestroy temp;
		
		other_ = 0;
		return temp;
	}
private:
	class VidmprxGetDatafeed {
		mutable int used_;
		SM_VIDMPRX_DATAFEED_PARMS datafeedp_;
		tSMDatafeedId *datafeedP_;
	public:
		VidmprxGetDatafeed(const VidmprxGetDatafeed &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxGetDatafeed() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
			datafeedP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~VidmprxGetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_get_datafeed not called\n");
				abort();
			};
		};
		VidmprxGetDatafeed &vidmprx(tSMVidMPrxId v) {
			datafeedp_.vidmprx = v;
			return *this;
		};
		VidmprxGetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	VidmprxGetDatafeed vidmprx_get_datafeed() {
		return VidmprxGetDatafeed();
	}
private:
	class VidmprxGetEvent {
		mutable int used_;
		SM_VIDMPRX_EVENT_PARMS eventp_;
		tSMEventId *eventP_;
	public:
		VidmprxGetEvent(const VidmprxGetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxGetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
			eventP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~VidmprxGetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_get_event not called\n");
				abort();
			};
		};
		VidmprxGetEvent &vidmprx(tSMVidMPrxId v) {
			eventp_.vidmprx = v;
			return *this;
		};
		VidmprxGetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	VidmprxGetEvent vidmprx_get_event() {
		return VidmprxGetEvent();
	}
private:
	class VidmprxGetPorts {
		mutable int used_;
		SM_VIDMPRX_PORT_PARMS portp_;
		int *RTP_portP_;
		int *RTCP_portP_;
		in_addr *addressP_;
		in6_addr *ipv6_addressP_;
	public:
		VidmprxGetPorts(const VidmprxGetPorts &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxGetPorts() : used_(0) {
			memset(&portp_, 0, sizeof(portp_));
			RTP_portP_ = 0;
			RTCP_portP_ = 0;
			addressP_ = 0;
			ipv6_addressP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_get_ports(&portp_));
			used_ = 1;
			if (RTP_portP_) *RTP_portP_ = portp_.RTP_port;
			if (RTCP_portP_) *RTCP_portP_ = portp_.RTCP_port;
			if (addressP_) *addressP_ = portp_.address;
			if (ipv6_addressP_) *ipv6_addressP_ = portp_.ipv6_address;
			return e;
		};
		~VidmprxGetPorts() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_get_ports not called\n");
				abort();
			};
		};
		VidmprxGetPorts &vidmprx(tSMVidMPrxId v) {
			portp_.vidmprx = v;
			return *this;
		};
		VidmprxGetPorts &RTP_port(int *v) {
			RTP_portP_ = v;
			return *this;
		};
		VidmprxGetPorts &RTCP_port(int *v) {
			RTCP_portP_ = v;
			return *this;
		};
		VidmprxGetPorts &address(in_addr *v) {
			addressP_ = v;
			return *this;
		};
		VidmprxGetPorts &ipv6_address(in6_addr *v) {
			ipv6_addressP_ = v;
			return *this;
		};
		VidmprxGetPorts &nowait(tSM_UT32 v) {
			portp_.nowait = v;
			return *this;
		};
	};
public:
	VidmprxGetPorts vidmprx_get_ports() {
		return VidmprxGetPorts();
	}
private:
	class VidmprxStatus {
		mutable int used_;
		SM_VIDMPRX_STATUS_PARMS statusp_;
		enum kSMVidMPrxStatus *statusP_;
		int *u_ports_RTP_PortP_;
		int *u_ports_RTCP_PortP_;
		in_addr *u_ports_addressP_;
		tSM_INT *u_run_no_dataP_;
		in_addr *u_ssrc_addressP_;
		int *u_ssrc_portP_;
		int *u_ssrc_ssrcP_;
		int *u_payload_typeP_;
		int *u_internal_error_num_errorsP_;
		int *u_ports_ipv6_RTP_PortP_;
		int *u_ports_ipv6_RTCP_PortP_;
		in6_addr *u_ports_ipv6_addressP_;
		in6_addr *u_ssrc_ipv6_addressP_;
		int *u_ssrc_ipv6_portP_;
		int *u_ssrc_ipv6_ssrcP_;
		int *u_bind_request_recv_portP_;
		enum kSMAddressType *u_bind_request_address_typeP_;
		in_addr *u_bind_request_address_ipv4P_;
		in6_addr *u_bind_request_address_ipv6P_;
		int *u_bind_request_portP_;
		tSM_UT32 *u_bind_request_priorityP_;
		int *u_bind_request_use_candidateP_;
		int *u_bind_request_remote_is_controllingP_;
		int *u_bind_response_recv_portP_;
		enum kSMVidMPrxSTUNResult *u_bind_response_resultP_;
		tSM_INT *u_bind_response_error_codeP_;
		enum kSMAddressType *u_bind_response_address_typeP_;
		in_addr *u_bind_response_address_ipv4P_;
		in6_addr *u_bind_response_address_ipv6P_;
		int *u_bind_response_portP_;
		int *u_bind_response_local_was_controllingP_;
	public:
		VidmprxStatus(const VidmprxStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_ports_RTP_PortP_ = 0;
			u_ports_RTCP_PortP_ = 0;
			u_ports_addressP_ = 0;
			u_run_no_dataP_ = 0;
			u_ssrc_addressP_ = 0;
			u_ssrc_portP_ = 0;
			u_ssrc_ssrcP_ = 0;
			u_payload_typeP_ = 0;
			u_internal_error_num_errorsP_ = 0;
			u_ports_ipv6_RTP_PortP_ = 0;
			u_ports_ipv6_RTCP_PortP_ = 0;
			u_ports_ipv6_addressP_ = 0;
			u_ssrc_ipv6_addressP_ = 0;
			u_ssrc_ipv6_portP_ = 0;
			u_ssrc_ipv6_ssrcP_ = 0;
			u_bind_request_recv_portP_ = 0;
			u_bind_request_address_typeP_ = 0;
			u_bind_request_address_ipv4P_ = 0;
			u_bind_request_address_ipv6P_ = 0;
			u_bind_request_portP_ = 0;
			u_bind_request_priorityP_ = 0;
			u_bind_request_use_candidateP_ = 0;
			u_bind_request_remote_is_controllingP_ = 0;
			u_bind_response_recv_portP_ = 0;
			u_bind_response_resultP_ = 0;
			u_bind_response_error_codeP_ = 0;
			u_bind_response_address_typeP_ = 0;
			u_bind_response_address_ipv4P_ = 0;
			u_bind_response_address_ipv6P_ = 0;
			u_bind_response_portP_ = 0;
			u_bind_response_local_was_controllingP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_ports_RTP_PortP_) *u_ports_RTP_PortP_ = statusp_.u.ports.RTP_Port;
			if (u_ports_RTCP_PortP_) *u_ports_RTCP_PortP_ = statusp_.u.ports.RTCP_Port;
			if (u_ports_addressP_) *u_ports_addressP_ = statusp_.u.ports.address;
			if (u_run_no_dataP_) *u_run_no_dataP_ = statusp_.u.run.no_data;
			if (u_ssrc_addressP_) *u_ssrc_addressP_ = statusp_.u.ssrc.address;
			if (u_ssrc_portP_) *u_ssrc_portP_ = statusp_.u.ssrc.port;
			if (u_ssrc_ssrcP_) *u_ssrc_ssrcP_ = statusp_.u.ssrc.ssrc;
			if (u_payload_typeP_) *u_payload_typeP_ = statusp_.u.payload.type;
			if (u_internal_error_num_errorsP_) *u_internal_error_num_errorsP_ = statusp_.u.internal_error.num_errors;
			if (u_ports_ipv6_RTP_PortP_) *u_ports_ipv6_RTP_PortP_ = statusp_.u.ports_ipv6.RTP_Port;
			if (u_ports_ipv6_RTCP_PortP_) *u_ports_ipv6_RTCP_PortP_ = statusp_.u.ports_ipv6.RTCP_Port;
			if (u_ports_ipv6_addressP_) *u_ports_ipv6_addressP_ = statusp_.u.ports_ipv6.address;
			if (u_ssrc_ipv6_addressP_) *u_ssrc_ipv6_addressP_ = statusp_.u.ssrc_ipv6.address;
			if (u_ssrc_ipv6_portP_) *u_ssrc_ipv6_portP_ = statusp_.u.ssrc_ipv6.port;
			if (u_ssrc_ipv6_ssrcP_) *u_ssrc_ipv6_ssrcP_ = statusp_.u.ssrc_ipv6.ssrc;
			if (u_bind_request_recv_portP_) *u_bind_request_recv_portP_ = statusp_.u.bind_request.recv_port;
			if (u_bind_request_address_typeP_) *u_bind_request_address_typeP_ = statusp_.u.bind_request.address_type;
			if (u_bind_request_address_ipv4P_) *u_bind_request_address_ipv4P_ = statusp_.u.bind_request.address.ipv4;
			if (u_bind_request_address_ipv6P_) *u_bind_request_address_ipv6P_ = statusp_.u.bind_request.address.ipv6;
			if (u_bind_request_portP_) *u_bind_request_portP_ = statusp_.u.bind_request.port;
			if (u_bind_request_priorityP_) *u_bind_request_priorityP_ = statusp_.u.bind_request.priority;
			if (u_bind_request_use_candidateP_) *u_bind_request_use_candidateP_ = statusp_.u.bind_request.use_candidate;
			if (u_bind_request_remote_is_controllingP_) *u_bind_request_remote_is_controllingP_ = statusp_.u.bind_request.remote_is_controlling;
			if (u_bind_response_recv_portP_) *u_bind_response_recv_portP_ = statusp_.u.bind_response.recv_port;
			if (u_bind_response_resultP_) *u_bind_response_resultP_ = statusp_.u.bind_response.result;
			if (u_bind_response_error_codeP_) *u_bind_response_error_codeP_ = statusp_.u.bind_response.error_code;
			if (u_bind_response_address_typeP_) *u_bind_response_address_typeP_ = statusp_.u.bind_response.address_type;
			if (u_bind_response_address_ipv4P_) *u_bind_response_address_ipv4P_ = statusp_.u.bind_response.address.ipv4;
			if (u_bind_response_address_ipv6P_) *u_bind_response_address_ipv6P_ = statusp_.u.bind_response.address.ipv6;
			if (u_bind_response_portP_) *u_bind_response_portP_ = statusp_.u.bind_response.port;
			if (u_bind_response_local_was_controllingP_) *u_bind_response_local_was_controllingP_ = statusp_.u.bind_response.local_was_controlling;
			return e;
		};
		~VidmprxStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_status not called\n");
				abort();
			};
		};
		VidmprxStatus &vidmprx(tSMVidMPrxId v) {
			statusp_.vidmprx = v;
			return *this;
		};
		VidmprxStatus &status(enum kSMVidMPrxStatus *v) {
			statusP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_RTP_Port(int *v) {
			u_ports_RTP_PortP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_RTCP_Port(int *v) {
			u_ports_RTCP_PortP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_address(in_addr *v) {
			u_ports_addressP_ = v;
			return *this;
		};
		VidmprxStatus &u_run_no_data(tSM_INT *v) {
			u_run_no_dataP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_address(in_addr *v) {
			u_ssrc_addressP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_port(int *v) {
			u_ssrc_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_ssrc(int *v) {
			u_ssrc_ssrcP_ = v;
			return *this;
		};
		VidmprxStatus &u_payload_type(int *v) {
			u_payload_typeP_ = v;
			return *this;
		};
		VidmprxStatus &u_internal_error_num_errors(int *v) {
			u_internal_error_num_errorsP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_ipv6_RTP_Port(int *v) {
			u_ports_ipv6_RTP_PortP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_ipv6_RTCP_Port(int *v) {
			u_ports_ipv6_RTCP_PortP_ = v;
			return *this;
		};
		VidmprxStatus &u_ports_ipv6_address(in6_addr *v) {
			u_ports_ipv6_addressP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_ipv6_address(in6_addr *v) {
			u_ssrc_ipv6_addressP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_ipv6_port(int *v) {
			u_ssrc_ipv6_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_ssrc_ipv6_ssrc(int *v) {
			u_ssrc_ipv6_ssrcP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_recv_port(int *v) {
			u_bind_request_recv_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_address_type(enum kSMAddressType *v) {
			u_bind_request_address_typeP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_address_ipv4(in_addr *v) {
			u_bind_request_address_ipv4P_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_address_ipv6(in6_addr *v) {
			u_bind_request_address_ipv6P_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_port(int *v) {
			u_bind_request_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_priority(tSM_UT32 *v) {
			u_bind_request_priorityP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_use_candidate(int *v) {
			u_bind_request_use_candidateP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_request_remote_is_controlling(int *v) {
			u_bind_request_remote_is_controllingP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_recv_port(int *v) {
			u_bind_response_recv_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_result(enum kSMVidMPrxSTUNResult *v) {
			u_bind_response_resultP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_error_code(tSM_INT *v) {
			u_bind_response_error_codeP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_address_type(enum kSMAddressType *v) {
			u_bind_response_address_typeP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_address_ipv4(in_addr *v) {
			u_bind_response_address_ipv4P_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_address_ipv6(in6_addr *v) {
			u_bind_response_address_ipv6P_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_port(int *v) {
			u_bind_response_portP_ = v;
			return *this;
		};
		VidmprxStatus &u_bind_response_local_was_controlling(int *v) {
			u_bind_response_local_was_controllingP_ = v;
			return *this;
		};
	};
public:
	VidmprxStatus vidmprx_status() {
		return VidmprxStatus();
	}
private:
	class VidmprxStop {
		mutable int used_;
		SM_VIDMPRX_STOP_PARMS stopp_;
	public:
		VidmprxStop(const VidmprxStop &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxStop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~VidmprxStop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_stop not called\n");
				abort();
			};
		};
		VidmprxStop &vidmprx(tSMVidMPrxId v) {
			stopp_.vidmprx = v;
			return *this;
		};
	};
public:
	VidmprxStop vidmprx_stop() {
		return VidmprxStop();
	}
private:
	class VidmptxConfig {
		mutable int used_;
		SM_VIDMPTX_CONFIG_PARMS configp_;
	public:
		VidmptxConfig(const VidmptxConfig &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxConfig() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_config(&configp_));
			used_ = 1;
			return e;
		};
		~VidmptxConfig() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_config not called\n");
				abort();
			};
		};
		VidmptxConfig &vidmptx(tSMVidMPtxId v) {
			configp_.vidmptx = v;
			return *this;
		};
		VidmptxConfig &destination_rtp(SOCKADDR_IN v) {
			configp_.destination_rtp = v;
			return *this;
		};
		VidmptxConfig &source_rtp(SOCKADDR_IN v) {
			configp_.source_rtp = v;
			return *this;
		};
		VidmptxConfig &TOS_RTP(int v) {
			configp_.TOS_RTP = v;
			return *this;
		};
		VidmptxConfig &destination_rtcp(SOCKADDR_IN v) {
			configp_.destination_rtcp = v;
			return *this;
		};
		VidmptxConfig &source_rtcp(SOCKADDR_IN v) {
			configp_.source_rtcp = v;
			return *this;
		};
		VidmptxConfig &TOS_RTCP(int v) {
			configp_.TOS_RTCP = v;
			return *this;
		};
	};
public:
	VidmptxConfig vidmptx_config() {
		return VidmptxConfig();
	}
private:
	class VidmptxConfigIpv6 {
		mutable int used_;
		SM_VIDMPTX_CONFIG_IPV6_PARMS configp_;
	public:
		VidmptxConfigIpv6(const VidmptxConfigIpv6 &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxConfigIpv6() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_config_ipv6(&configp_));
			used_ = 1;
			return e;
		};
		~VidmptxConfigIpv6() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_config_ipv6 not called\n");
				abort();
			};
		};
		VidmptxConfigIpv6 &vidmptx(tSMVidMPtxId v) {
			configp_.vidmptx = v;
			return *this;
		};
		VidmptxConfigIpv6 &destination_rtp(SOCKADDR_IN6 v) {
			configp_.destination_rtp = v;
			return *this;
		};
		VidmptxConfigIpv6 &source_rtp(SOCKADDR_IN6 v) {
			configp_.source_rtp = v;
			return *this;
		};
		VidmptxConfigIpv6 &destination_rtcp(SOCKADDR_IN6 v) {
			configp_.destination_rtcp = v;
			return *this;
		};
		VidmptxConfigIpv6 &source_rtcp(SOCKADDR_IN6 v) {
			configp_.source_rtcp = v;
			return *this;
		};
	};
public:
	VidmptxConfigIpv6 vidmptx_config_ipv6() {
		return VidmptxConfigIpv6();
	}
private:
	class VidmptxConfigCodecRfc3984 {
		mutable int used_;
		SM_VIDMPTX_CODEC_RFC3984_PARMS codecp_;
	public:
		VidmptxConfigCodecRfc3984(const VidmptxConfigCodecRfc3984 &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxConfigCodecRfc3984() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_config_codec_rfc3984(&codecp_));
			used_ = 1;
			return e;
		};
		~VidmptxConfigCodecRfc3984() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_config_codec_rfc3984 not called\n");
				abort();
			};
		};
		VidmptxConfigCodecRfc3984 &vidmptx(tSMVidMPtxId v) {
			codecp_.vidmptx = v;
			return *this;
		};
		VidmptxConfigCodecRfc3984 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		VidmptxConfigCodecRfc3984 &deaggregate(tSM_INT v) {
			codecp_.deaggregate = v;
			return *this;
		};
	};
public:
	VidmptxConfigCodecRfc3984 vidmptx_config_codec_rfc3984() {
		return VidmptxConfigCodecRfc3984();
	}
private:
	class VidmptxConfigCodecRfc4629 {
		mutable int used_;
		SM_VIDMPTX_CODEC_RFC4629_PARMS codecp_;
	public:
		VidmptxConfigCodecRfc4629(const VidmptxConfigCodecRfc4629 &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxConfigCodecRfc4629() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_config_codec_rfc4629(&codecp_));
			used_ = 1;
			return e;
		};
		~VidmptxConfigCodecRfc4629() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_config_codec_rfc4629 not called\n");
				abort();
			};
		};
		VidmptxConfigCodecRfc4629 &vidmptx(tSMVidMPtxId v) {
			codecp_.vidmptx = v;
			return *this;
		};
		VidmptxConfigCodecRfc4629 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		VidmptxConfigCodecRfc4629 &rem_ext_pic_hdr(tSM_INT v) {
			codecp_.rem_ext_pic_hdr = v;
			return *this;
		};
	};
public:
	VidmptxConfigCodecRfc4629 vidmptx_config_codec_rfc4629() {
		return VidmptxConfigCodecRfc4629();
	}
private:
	class VidmptxDatafeedConnect {
		mutable int used_;
		SM_VIDMPTX_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		VidmptxDatafeedConnect(const VidmptxDatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxDatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~VidmptxDatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_datafeed_connect not called\n");
				abort();
			};
		};
		VidmptxDatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
		VidmptxDatafeedConnect &vidmptx(tSMVidMPtxId v) {
			datafeedp_.vidmptx = v;
			return *this;
		};
	};
public:
	VidmptxDatafeedConnect vidmptx_datafeed_connect() {
		return VidmptxDatafeedConnect();
	}
private:
	class VidmptxDestroy {
		mutable int used_;
		tSMVidMPtxId vidmptx_;
	public:
		VidmptxDestroy(const VidmptxDestroy &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxDestroy(tSMVidMPtxId vidmptx) : used_(0) {
			vidmptx_ = vidmptx;
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_destroy(vidmptx_));
			used_ = 1;
			return e;
		};
		~VidmptxDestroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_destroy not called\n");
				abort();
			};
		};
	};
public:
	VidmptxDestroy vidmptx_destroy() {
		VidmptxDestroy temp;
		
		other_ = 0;
		return temp;
	}
private:
	class VidmptxGetEvent {
		mutable int used_;
		SM_VIDMPTX_EVENT_PARMS eventp_;
		tSMEventId *eventP_;
	public:
		VidmptxGetEvent(const VidmptxGetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxGetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
			eventP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~VidmptxGetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_get_event not called\n");
				abort();
			};
		};
		VidmptxGetEvent &vidmptx(tSMVidMPtxId v) {
			eventp_.vidmptx = v;
			return *this;
		};
		VidmptxGetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	VidmptxGetEvent vidmptx_get_event() {
		return VidmptxGetEvent();
	}
private:
	class VidmptxStatus {
		mutable int used_;
		SM_VIDMPTX_STATUS_PARMS statusp_;
		enum kSMVidMPtxStatus *statusP_;
		int *u_ssrc_ssrcP_;
		int *u_internal_error_num_errorsP_;
	public:
		VidmptxStatus(const VidmptxStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_ssrc_ssrcP_ = 0;
			u_internal_error_num_errorsP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_ssrc_ssrcP_) *u_ssrc_ssrcP_ = statusp_.u.ssrc.ssrc;
			if (u_internal_error_num_errorsP_) *u_internal_error_num_errorsP_ = statusp_.u.internal_error.num_errors;
			return e;
		};
		~VidmptxStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_status not called\n");
				abort();
			};
		};
		VidmptxStatus &vidmptx(tSMVidMPtxId v) {
			statusp_.vidmptx = v;
			return *this;
		};
		VidmptxStatus &status(enum kSMVidMPtxStatus *v) {
			statusP_ = v;
			return *this;
		};
		VidmptxStatus &u_ssrc_ssrc(int *v) {
			u_ssrc_ssrcP_ = v;
			return *this;
		};
		VidmptxStatus &u_internal_error_num_errors(int *v) {
			u_internal_error_num_errorsP_ = v;
			return *this;
		};
	};
public:
	VidmptxStatus vidmptx_status() {
		return VidmptxStatus();
	}
private:
	class VidmptxStop {
		mutable int used_;
		SM_VIDMPTX_STOP_PARMS stopp_;
	public:
		VidmptxStop(const VidmptxStop &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmptxStop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmptx_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~VidmptxStop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmptx_stop not called\n");
				abort();
			};
		};
		VidmptxStop &vidmptx(tSMVidMPtxId v) {
			stopp_.vidmptx = v;
			return *this;
		};
	};
public:
	VidmptxStop vidmptx_stop() {
		return VidmptxStop();
	}
private:
	class VmptxCsrcListSet {
		mutable int used_;
		SM_VMPTX_CSRC_LIST_SET_PARMS csrcp_;
	public:
		VmptxCsrcListSet(const VmptxCsrcListSet &old) {
			*this = old;
			old.used_ = 1;
		};
		VmptxCsrcListSet() : used_(0) {
			memset(&csrcp_, 0, sizeof(csrcp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_csrc_list_set(&csrcp_));
			used_ = 1;
			return e;
		};
		~VmptxCsrcListSet() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_csrc_list_set not called\n");
				abort();
			};
		};
		VmptxCsrcListSet &csrc_list(tSMVMPTxCSRCListId v) {
			csrcp_.csrc_list = v;
			return *this;
		};
		VmptxCsrcListSet &num_csrc(int v) {
			csrcp_.num_csrc = v;
			return *this;
		};
		VmptxCsrcListSet &csrc(int* v) {
			csrcp_.csrc = v;
			return *this;
		};
	};
public:
	VmptxCsrcListSet vmptx_csrc_list_set() {
		return VmptxCsrcListSet();
	}
private:
	class VmptxDestroyCsrcList {
		mutable int used_;
		SM_VMPTX_DESTROY_CSRC_LIST_PARMS csrcp_;
	public:
		VmptxDestroyCsrcList(const VmptxDestroyCsrcList &old) {
			*this = old;
			old.used_ = 1;
		};
		VmptxDestroyCsrcList() : used_(0) {
			memset(&csrcp_, 0, sizeof(csrcp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_destroy_csrc_list(&csrcp_));
			used_ = 1;
			return e;
		};
		~VmptxDestroyCsrcList() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_destroy_csrc_list not called\n");
				abort();
			};
		};
		VmptxDestroyCsrcList &csrc_list(tSMVMPTxCSRCListId v) {
			csrcp_.csrc_list = v;
			return *this;
		};
	};
public:
	VmptxDestroyCsrcList vmptx_destroy_csrc_list() {
		return VmptxDestroyCsrcList();
	}
private:
	class VmptxDestroyToneset {
		mutable int used_;
		SM_VMPTX_DESTROY_TONESET_PARMS tonep_;
	public:
		VmptxDestroyToneset(const VmptxDestroyToneset &old) {
			*this = old;
			old.used_ = 1;
		};
		VmptxDestroyToneset() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_destroy_toneset(&tonep_));
			used_ = 1;
			return e;
		};
		~VmptxDestroyToneset() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_destroy_toneset not called\n");
				abort();
			};
		};
		VmptxDestroyToneset &tone_set_id(tSMVMPTxToneSetId v) {
			tonep_.tone_set_id = v;
			return *this;
		};
	};
public:
	VmptxDestroyToneset vmptx_destroy_toneset() {
		return VmptxDestroyToneset();
	}
private:
	class VidmprxIceStunConfig {
		mutable int used_;
		SM_VIDMPRX_ICE_STUN_CONFIG_PARMS configp_;
	public:
		VidmprxIceStunConfig(const VidmprxIceStunConfig &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxIceStunConfig() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_ice_stun_config(&configp_));
			used_ = 1;
			return e;
		};
		~VidmprxIceStunConfig() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_ice_stun_config not called\n");
				abort();
			};
		};
		VidmprxIceStunConfig &vidmprx(tSMVidMPrxId v) {
			configp_.vidmprx = v;
			return *this;
		};
		VidmprxIceStunConfig &local_role(enum kSMICERole v) {
			configp_.local_role = v;
			return *this;
		};
		VidmprxIceStunConfig &local_ice_ufrag(char* v) {
			configp_.local_ice_ufrag = v;
			return *this;
		};
		VidmprxIceStunConfig &local_ice_ufrag_length(tSM_UT32 v) {
			configp_.local_ice_ufrag_length = v;
			return *this;
		};
		VidmprxIceStunConfig &local_ice_pwd(char* v) {
			configp_.local_ice_pwd = v;
			return *this;
		};
		VidmprxIceStunConfig &local_ice_pwd_length(tSM_UT32 v) {
			configp_.local_ice_pwd_length = v;
			return *this;
		};
		VidmprxIceStunConfig &role_tiebreaker(tSM_UT64 v) {
			configp_.role_tiebreaker = v;
			return *this;
		};
	};
public:
	VidmprxIceStunConfig vidmprx_ice_stun_config() {
		return VidmprxIceStunConfig();
	}
private:
	class VidmprxIceStunChangeRole {
		mutable int used_;
		SM_VIDMPRX_ICE_STUN_CHANGE_ROLE_PARMS rolep_;
	public:
		VidmprxIceStunChangeRole(const VidmprxIceStunChangeRole &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxIceStunChangeRole() : used_(0) {
			memset(&rolep_, 0, sizeof(rolep_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_ice_stun_change_role(&rolep_));
			used_ = 1;
			return e;
		};
		~VidmprxIceStunChangeRole() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_ice_stun_change_role not called\n");
				abort();
			};
		};
		VidmprxIceStunChangeRole &vidmprx(tSMVidMPrxId v) {
			rolep_.vidmprx = v;
			return *this;
		};
		VidmprxIceStunChangeRole &local_role(enum kSMICERole v) {
			rolep_.local_role = v;
			return *this;
		};
	};
public:
	VidmprxIceStunChangeRole vidmprx_ice_stun_change_role() {
		return VidmprxIceStunChangeRole();
	}
private:
	class VidmprxIceStunRemoteCredentials {
		mutable int used_;
		SM_VIDMPRX_ICE_STUN_REMOTE_CREDENTIALS_PARMS credentialsp_;
	public:
		VidmprxIceStunRemoteCredentials(const VidmprxIceStunRemoteCredentials &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxIceStunRemoteCredentials() : used_(0) {
			memset(&credentialsp_, 0, sizeof(credentialsp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_ice_stun_remote_credentials(&credentialsp_));
			used_ = 1;
			return e;
		};
		~VidmprxIceStunRemoteCredentials() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_ice_stun_remote_credentials not called\n");
				abort();
			};
		};
		VidmprxIceStunRemoteCredentials &vidmprx(tSMVidMPrxId v) {
			credentialsp_.vidmprx = v;
			return *this;
		};
		VidmprxIceStunRemoteCredentials &remote_ice_ufrag(char* v) {
			credentialsp_.remote_ice_ufrag = v;
			return *this;
		};
		VidmprxIceStunRemoteCredentials &remote_ice_ufrag_length(tSM_UT32 v) {
			credentialsp_.remote_ice_ufrag_length = v;
			return *this;
		};
		VidmprxIceStunRemoteCredentials &remote_ice_pwd(char* v) {
			credentialsp_.remote_ice_pwd = v;
			return *this;
		};
		VidmprxIceStunRemoteCredentials &remote_ice_pwd_length(tSM_UT32 v) {
			credentialsp_.remote_ice_pwd_length = v;
			return *this;
		};
	};
public:
	VidmprxIceStunRemoteCredentials vidmprx_ice_stun_remote_credentials() {
		return VidmprxIceStunRemoteCredentials();
	}
private:
	class VidmprxIceStunSendBindRequest {
		mutable int used_;
		SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS requestp_;
	public:
		VidmprxIceStunSendBindRequest(const VidmprxIceStunSendBindRequest &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxIceStunSendBindRequest() : used_(0) {
			memset(&requestp_, 0, sizeof(requestp_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_ice_stun_send_bind_request(&requestp_));
			used_ = 1;
			return e;
		};
		~VidmprxIceStunSendBindRequest() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_ice_stun_send_bind_request not called\n");
				abort();
			};
		};
		VidmprxIceStunSendBindRequest &vidmprx(tSMVidMPrxId v) {
			requestp_.vidmprx = v;
			return *this;
		};
		VidmprxIceStunSendBindRequest &use_RTCP_port(int v) {
			requestp_.use_RTCP_port = v;
			return *this;
		};
		VidmprxIceStunSendBindRequest &transaction_id(char *v) {
			strncpy(requestp_.transaction_id, v, sizeof(requestp_.transaction_id));
			return *this;
		};
		VidmprxIceStunSendBindRequest &destination_ipv4(SOCKADDR_IN v) {
			requestp_.destination.ipv4 = v;
			return *this;
		};
		VidmprxIceStunSendBindRequest &destination_ipv6(SOCKADDR_IN6 v) {
			requestp_.destination.ipv6 = v;
			return *this;
		};
		VidmprxIceStunSendBindRequest &priority(tSM_UT32 v) {
			requestp_.priority = v;
			return *this;
		};
		VidmprxIceStunSendBindRequest &use_candidate(tSM_INT v) {
			requestp_.use_candidate = v;
			return *this;
		};
	};
public:
	VidmprxIceStunSendBindRequest vidmprx_ice_stun_send_bind_request() {
		return VidmprxIceStunSendBindRequest();
	}
