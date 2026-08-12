private:
	class ResetModule {
		mutable int used_;
		tSMModuleId module_;
	public:
		ResetModule(const ResetModule &old) {
			*this = old;
			old.used_ = 1;
		};
		ResetModule(tSMModuleId module) : used_(0) {
			module_ = module;
		};
		operator Error() {
			Error e(Prosody::sm_reset_module(module_));
			used_ = 1;
			return e;
		};
		~ResetModule() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_reset_module not called\n");
				abort();
			};
		};
	};
public:
	ResetModule reset_module() {
		return ResetModule(module_);
	}
private:
	class GetInfo {
		mutable int used_;
		SM_MODULE_INFO_PARMS moduleinfop_;
		tSM_INT *firmware_runningP_;
		tSM_INT *firmware_idP_;
		tSM_INT *average_loadingP_;
		tSM_INT *max_loadingP_;
		tSM_INT *module_slotP_;
		tSM_INT *min_streamP_;
		tSM_INT *stream_countP_;
	public:
		GetInfo(const GetInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		GetInfo() : used_(0) {
			memset(&moduleinfop_, 0, sizeof(moduleinfop_));
			firmware_runningP_ = 0;
			firmware_idP_ = 0;
			average_loadingP_ = 0;
			max_loadingP_ = 0;
			module_slotP_ = 0;
			min_streamP_ = 0;
			stream_countP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_module_info(&moduleinfop_));
			used_ = 1;
			if (firmware_runningP_) *firmware_runningP_ = moduleinfop_.firmware_running;
			if (firmware_idP_) *firmware_idP_ = moduleinfop_.firmware_id;
			if (average_loadingP_) *average_loadingP_ = moduleinfop_.average_loading;
			if (max_loadingP_) *max_loadingP_ = moduleinfop_.max_loading;
			if (module_slotP_) *module_slotP_ = moduleinfop_.module_slot;
			if (min_streamP_) *min_streamP_ = moduleinfop_.min_stream;
			if (stream_countP_) *stream_countP_ = moduleinfop_.stream_count;
			return e;
		};
		~GetInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_module_info not called\n");
				abort();
			};
		};
		GetInfo &module(tSMModuleId v) {
			moduleinfop_.module = v;
			return *this;
		};
		GetInfo &firmware_running(tSM_INT *v) {
			firmware_runningP_ = v;
			return *this;
		};
		GetInfo &firmware_id(tSM_INT *v) {
			firmware_idP_ = v;
			return *this;
		};
		GetInfo &average_loading(tSM_INT *v) {
			average_loadingP_ = v;
			return *this;
		};
		GetInfo &max_loading(tSM_INT *v) {
			max_loadingP_ = v;
			return *this;
		};
		GetInfo &module_slot(tSM_INT *v) {
			module_slotP_ = v;
			return *this;
		};
		GetInfo &min_stream(tSM_INT *v) {
			min_streamP_ = v;
			return *this;
		};
		GetInfo &stream_count(tSM_INT *v) {
			stream_countP_ = v;
			return *this;
		};
	};
public:
	GetInfo get_info() {
		return GetInfo().module(module_);
	}
private:
	class GetStreamInfo {
		mutable int used_;
		SM_STREAM_INFO_PARMS streaminfop_;
		tSM_INT *serial_port_ixP_;
		tSM_INT *timeslot_countP_;
	public:
		GetStreamInfo(const GetStreamInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		GetStreamInfo() : used_(0) {
			memset(&streaminfop_, 0, sizeof(streaminfop_));
			serial_port_ixP_ = 0;
			timeslot_countP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_stream_info(&streaminfop_));
			used_ = 1;
			if (serial_port_ixP_) *serial_port_ixP_ = streaminfop_.serial_port_ix;
			if (timeslot_countP_) *timeslot_countP_ = streaminfop_.timeslot_count;
			return e;
		};
		~GetStreamInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_stream_info not called\n");
				abort();
			};
		};
		GetStreamInfo &module(tSMModuleId v) {
			streaminfop_.module = v;
			return *this;
		};
		GetStreamInfo &stream(tSM_INT v) {
			streaminfop_.stream = v;
			return *this;
		};
		GetStreamInfo &serial_port_ix(tSM_INT *v) {
			serial_port_ixP_ = v;
			return *this;
		};
		GetStreamInfo &timeslot_count(tSM_INT *v) {
			timeslot_countP_ = v;
			return *this;
		};
	};
public:
	GetStreamInfo get_stream_info() {
		return GetStreamInfo().module(module_);
	}
private:
	class GetFirmwareInfo {
		mutable int used_;
		SM_FWINFO_PARMS fp_;
		tSM_UT32 *positionP_;
		tSM_UT32 *tasktypecodeP_;
		tSM_UT32 *versionP_;
	public:
		GetFirmwareInfo(const GetFirmwareInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		GetFirmwareInfo() : used_(0) {
			memset(&fp_, 0, sizeof(fp_));
			positionP_ = 0;
			tasktypecodeP_ = 0;
			versionP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_firmware_info(&fp_));
			used_ = 1;
			if (positionP_) *positionP_ = fp_.position;
			if (tasktypecodeP_) *tasktypecodeP_ = fp_.tasktypecode;
			if (versionP_) *versionP_ = fp_.version;
			return e;
		};
		~GetFirmwareInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_firmware_info not called\n");
				abort();
			};
		};
		GetFirmwareInfo &module(tSMModuleId v) {
			fp_.module = v;
			return *this;
		};
		GetFirmwareInfo &position(tSM_UT32 *v) {
			positionP_ = v;
			return *this;
		};
		GetFirmwareInfo &position(tSM_UT32 v) {
			fp_.position = v;
			return *this;
		};
		GetFirmwareInfo &tasktypecode(tSM_UT32 *v) {
			tasktypecodeP_ = v;
			return *this;
		};
		GetFirmwareInfo &version(tSM_UT32 *v) {
			versionP_ = v;
			return *this;
		};
	};
public:
	GetFirmwareInfo get_firmware_info() {
		return GetFirmwareInfo().module(module_);
	}
private:
	class ConfigSwitching {
		mutable int used_;
		SM_CONFIG_MODULE_SW_PARMS configp_;
	public:
		ConfigSwitching(const ConfigSwitching &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigSwitching() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_config_module_switching(&configp_));
			used_ = 1;
			return e;
		};
		~ConfigSwitching() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_config_module_switching not called\n");
				abort();
			};
		};
		ConfigSwitching &module(tSMModuleId v) {
			configp_.module = v;
			return *this;
		};
		ConfigSwitching &auto_assign_out_ts(tSM_INT v) {
			configp_.auto_assign_out_ts = v;
			return *this;
		};
		ConfigSwitching &auto_assign_in_ts(tSM_INT v) {
			configp_.auto_assign_in_ts = v;
			return *this;
		};
		ConfigSwitching &out_st0(tSM_INT v) {
			configp_.out_st0 = v;
			return *this;
		};
		ConfigSwitching &out_st1(tSM_INT v) {
			configp_.out_st1 = v;
			return *this;
		};
		ConfigSwitching &out_base_ts(tSM_INT v) {
			configp_.out_base_ts = v;
			return *this;
		};
		ConfigSwitching &in_type(enum kSMTimeslotType v) {
			configp_.in_type = v;
			return *this;
		};
		ConfigSwitching &in_st0(tSM_INT v) {
			configp_.in_st0 = v;
			return *this;
		};
		ConfigSwitching &in_st1(tSM_INT v) {
			configp_.in_st1 = v;
			return *this;
		};
		ConfigSwitching &in_base_ts(tSM_INT v) {
			configp_.in_base_ts = v;
			return *this;
		};
		ConfigSwitching &out_type(enum kSMTimeslotType v) {
			configp_.out_type = v;
			return *this;
		};
	};
public:
	ConfigSwitching config_switching() {
		return ConfigSwitching().module(module_);
	}
private:
	class OpenModule {
		mutable int used_;
		SM_OPEN_MODULE_PARMS module_parmsp_;
		tSMModuleId *module_idP_;
	public:
		OpenModule(const OpenModule &old) {
			*this = old;
			old.used_ = 1;
		};
		OpenModule() : used_(0) {
			memset(&module_parmsp_, 0, sizeof(module_parmsp_));
			module_idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_open_module(&module_parmsp_));
			used_ = 1;
			if (module_idP_) *module_idP_ = module_parmsp_.module_id;
			return e;
		};
		~OpenModule() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_open_module not called\n");
				abort();
			};
		};
		OpenModule &card_id(tSMCardId v) {
			module_parmsp_.card_id = v;
			return *this;
		};
		OpenModule &module_ix(tSM_UT32 v) {
			module_parmsp_.module_ix = v;
			return *this;
		};
		OpenModule &module_id(tSMModuleId *v) {
			module_idP_ = v;
			return *this;
		};
	};
public:
	OpenModule open_module() {
		OpenModule temp;
		temp.module_id(&module_);
		module_ = 0;
		return temp;
	}
private:
	class SecModule {
		mutable int used_;
		SM_SEC_MODULE_PARMS module_parmsp_;
	public:
		SecModule(const SecModule &old) {
			*this = old;
			old.used_ = 1;
		};
		SecModule() : used_(0) {
			memset(&module_parmsp_, 0, sizeof(module_parmsp_));
		};
		operator Error() {
			Error e(Prosody::sm_sec_module(&module_parmsp_));
			used_ = 1;
			return e;
		};
		~SecModule() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_sec_module not called\n");
				abort();
			};
		};
		SecModule &module_id(tSMModuleId v) {
			module_parmsp_.module_id = v;
			return *this;
		};
		SecModule &exclude_data_to_module(tSM_INT v) {
			module_parmsp_.exclude_data_to_module = v;
			return *this;
		};
		SecModule &exclude_data_from_module(tSM_INT v) {
			module_parmsp_.exclude_data_from_module = v;
			return *this;
		};
		SecModule &key_id(tSM_INT v) {
			module_parmsp_.key_id = v;
			return *this;
		};
		SecModule &context(char* v) {
			module_parmsp_.context = v;
			return *this;
		};
		SecModule &encdec(tSMSecCallback v) {
			module_parmsp_.encdec = v;
			return *this;
		};
	};
public:
	SecModule sec_module() {
		return SecModule().module_id(module_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_MODULE_EVENT_PARMS eventp_;
		tSMEventId *eventP_;
	public:
		GetEvent(const GetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		GetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
			eventP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_module_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_module_get_event not called\n");
				abort();
			};
		};
		GetEvent &module_id(tSMModuleId v) {
			eventp_.module_id = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().module_id(module_);
	}
private:
	class Status {
		mutable int used_;
		SM_MODULE_STATUS_PARMS statusp_;
		enum kSMModuleStatus *statusP_;
	public:
		Status(const Status &old) {
			*this = old;
			old.used_ = 1;
		};
		Status() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_module_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_module_status not called\n");
				abort();
			};
		};
		Status &module_id(tSMModuleId v) {
			statusp_.module_id = v;
			return *this;
		};
		Status &status(enum kSMModuleStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().module_id(module_);
	}
private:
	class ShutdownModule {
		mutable int used_;
		SM_SHUTDOWN_MODULE_PARMS shutdownp_;
	public:
		ShutdownModule(const ShutdownModule &old) {
			*this = old;
			old.used_ = 1;
		};
		ShutdownModule() : used_(0) {
			memset(&shutdownp_, 0, sizeof(shutdownp_));
		};
		operator Error() {
			Error e(Prosody::sm_shutdown_module(&shutdownp_));
			used_ = 1;
			return e;
		};
		~ShutdownModule() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_shutdown_module not called\n");
				abort();
			};
		};
		ShutdownModule &module_id(tSMModuleId v) {
			shutdownp_.module_id = v;
			return *this;
		};
	};
public:
	ShutdownModule shutdown_module() {
		return ShutdownModule().module_id(module_);
	}
private:
	class CloseModule {
		mutable int used_;
		SM_CLOSE_MODULE_PARMS closep_;
	public:
		CloseModule(const CloseModule &old) {
			*this = old;
			old.used_ = 1;
		};
		CloseModule() : used_(0) {
			memset(&closep_, 0, sizeof(closep_));
		};
		operator Error() {
			Error e(Prosody::sm_close_module(&closep_));
			used_ = 1;
			return e;
		};
		~CloseModule() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_close_module not called\n");
				abort();
			};
		};
		CloseModule &module_id(tSMModuleId v) {
			closep_.module_id = v;
			return *this;
		};
	};
public:
	CloseModule close_module() {
		CloseModule temp;
		temp.module_id(module_);
		module_ = 0;
		return temp;
	}
private:
	class GetCardId {
		mutable int used_;
		tSMModuleId mod_id_;
	public:
		GetCardId(const GetCardId &old) {
			*this = old;
			old.used_ = 1;
		};
		GetCardId(tSMModuleId mod_id) : used_(0) {
			mod_id_ = mod_id;
		};
		operator Error() {
			Error e(Prosody::sm_get_module_card_id(mod_id_));
			used_ = 1;
			return e;
		};
		~GetCardId() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_module_card_id not called\n");
				abort();
			};
		};
	};
public:
	GetCardId get_card_id() {
		return GetCardId(module_);
	}
private:
	class UdpCollectorCreate {
		mutable int used_;
		SM_UDP_COLLECTOR_CREATE_PARMS dp_;
		tSMCollectorId *collectorP_;
	public:
		UdpCollectorCreate(const UdpCollectorCreate &old) {
			*this = old;
			old.used_ = 1;
		};
		UdpCollectorCreate() : used_(0) {
			memset(&dp_, 0, sizeof(dp_));
			collectorP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_udp_collector_create(&dp_));
			used_ = 1;
			if (collectorP_) *collectorP_ = dp_.collector;
			return e;
		};
		~UdpCollectorCreate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_udp_collector_create not called\n");
				abort();
			};
		};
		UdpCollectorCreate &collector(tSMCollectorId *v) {
			collectorP_ = v;
			return *this;
		};
		UdpCollectorCreate &module(tSMModuleId v) {
			dp_.module = v;
			return *this;
		};
		UdpCollectorCreate &type(enum kSMCollectorType v) {
			dp_.type = v;
			return *this;
		};
		UdpCollectorCreate &address(in_addr v) {
			dp_.address = v;
			return *this;
		};
		UdpCollectorCreate &ipv6_address(in6_addr v) {
			dp_.ipv6_address = v;
			return *this;
		};
	};
public:
	UdpCollectorCreate udp_collector_create() {
		return UdpCollectorCreate().module(module_);
	}
private:
	class UdpDispatcherCreate {
		mutable int used_;
		SM_UDP_DISPATCHER_CREATE_PARMS dp_;
		tSMDispatcherId *dispatcherP_;
	public:
		UdpDispatcherCreate(const UdpDispatcherCreate &old) {
			*this = old;
			old.used_ = 1;
		};
		UdpDispatcherCreate() : used_(0) {
			memset(&dp_, 0, sizeof(dp_));
			dispatcherP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_udp_dispatcher_create(&dp_));
			used_ = 1;
			if (dispatcherP_) *dispatcherP_ = dp_.dispatcher;
			return e;
		};
		~UdpDispatcherCreate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_udp_dispatcher_create not called\n");
				abort();
			};
		};
		UdpDispatcherCreate &dispatcher(tSMDispatcherId *v) {
			dispatcherP_ = v;
			return *this;
		};
		UdpDispatcherCreate &module(tSMModuleId v) {
			dp_.module = v;
			return *this;
		};
		UdpDispatcherCreate &type(enum kSMDispatcherType v) {
			dp_.type = v;
			return *this;
		};
		UdpDispatcherCreate &destination(SOCKADDR_IN v) {
			dp_.destination = v;
			return *this;
		};
		UdpDispatcherCreate &source(SOCKADDR_IN v) {
			dp_.source = v;
			return *this;
		};
		UdpDispatcherCreate &TOS(int v) {
			dp_.TOS = v;
			return *this;
		};
		UdpDispatcherCreate &destination_ipv6(SOCKADDR_IN6 v) {
			dp_.destination_ipv6 = v;
			return *this;
		};
		UdpDispatcherCreate &source_ipv6(SOCKADDR_IN6 v) {
			dp_.source_ipv6 = v;
			return *this;
		};
	};
public:
	UdpDispatcherCreate udp_dispatcher_create() {
		return UdpDispatcherCreate().module(module_);
	}
