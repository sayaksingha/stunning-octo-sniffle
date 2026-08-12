private:
	class GetDriverInfo {
		mutable int used_;
		SM_DRIVER_INFO_PARMS drvinfop_;
		tSM_INT *majorP_;
		tSM_INT *minorP_;
		tSM_INT *stepP_;
		tSM_INT *customP_;
		tSM_INT *qualityP_;
		tSM_INT *buildno0P_;
		tSM_INT *buildno1P_;
	public:
		GetDriverInfo(const GetDriverInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		GetDriverInfo() : used_(0) {
			memset(&drvinfop_, 0, sizeof(drvinfop_));
			majorP_ = 0;
			minorP_ = 0;
			stepP_ = 0;
			customP_ = 0;
			qualityP_ = 0;
			buildno0P_ = 0;
			buildno1P_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_driver_info(&drvinfop_));
			used_ = 1;
			if (majorP_) *majorP_ = drvinfop_.major;
			if (minorP_) *minorP_ = drvinfop_.minor;
			if (stepP_) *stepP_ = drvinfop_.step;
			if (customP_) *customP_ = drvinfop_.custom;
			if (qualityP_) *qualityP_ = drvinfop_.quality;
			if (buildno0P_) *buildno0P_ = drvinfop_.buildno0;
			if (buildno1P_) *buildno1P_ = drvinfop_.buildno1;
			return e;
		};
		~GetDriverInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_driver_info not called\n");
				abort();
			};
		};
		GetDriverInfo &major(tSM_INT *v) {
			majorP_ = v;
			return *this;
		};
		GetDriverInfo &minor(tSM_INT *v) {
			minorP_ = v;
			return *this;
		};
		GetDriverInfo &step(tSM_INT *v) {
			stepP_ = v;
			return *this;
		};
		GetDriverInfo &custom(tSM_INT *v) {
			customP_ = v;
			return *this;
		};
		GetDriverInfo &quality(tSM_INT *v) {
			qualityP_ = v;
			return *this;
		};
		GetDriverInfo &buildno0(tSM_INT *v) {
			buildno0P_ = v;
			return *this;
		};
		GetDriverInfo &buildno1(tSM_INT *v) {
			buildno1P_ = v;
			return *this;
		};
	};
public:
	GetDriverInfo get_driver_info() {
		return GetDriverInfo();
	}
private:
	class SmdEvCreate {
		mutable int used_;
		tSMEventId* eventId_;
		tSMChannelId channelId_;
		int eventType_;
		int eventChannelBinding_;
	public:
		SmdEvCreate(const SmdEvCreate &old) {
			*this = old;
			old.used_ = 1;
		};
		SmdEvCreate(tSMEventId* eventId, tSMChannelId channelId, int eventType, int eventChannelBinding) : used_(0) {
			eventId_ = eventId;
			channelId_ = channelId;
			eventType_ = eventType;
			eventChannelBinding_ = eventChannelBinding;
		};
		operator Error() {
			Error e(Prosody::smd_ev_create(eventId_, channelId_, eventType_, eventChannelBinding_));
			used_ = 1;
			return e;
		};
		~SmdEvCreate() {
			if (!used_) {
				fprintf(stderr, "Prosody function smd_ev_create not called\n");
				abort();
			};
		};
	};
public:
	SmdEvCreate smd_ev_create() {
		return SmdEvCreate();
	}
private:
	class SmdEvWait {
		mutable int used_;
		tSMEventId eventId_;
	public:
		SmdEvWait(const SmdEvWait &old) {
			*this = old;
			old.used_ = 1;
		};
		SmdEvWait(tSMEventId eventId) : used_(0) {
			eventId_ = eventId;
		};
		operator Error() {
			Error e(Prosody::smd_ev_wait(eventId_));
			used_ = 1;
			return e;
		};
		~SmdEvWait() {
			if (!used_) {
				fprintf(stderr, "Prosody function smd_ev_wait not called\n");
				abort();
			};
		};
	};
public:
	SmdEvWait smd_ev_wait() {
		return SmdEvWait();
	}
private:
	class SmdEvFree {
		mutable int used_;
		tSMEventId eventId_;
	public:
		SmdEvFree(const SmdEvFree &old) {
			*this = old;
			old.used_ = 1;
		};
		SmdEvFree(tSMEventId eventId) : used_(0) {
			eventId_ = eventId;
		};
		operator Error() {
			Error e(Prosody::smd_ev_free(eventId_));
			used_ = 1;
			return e;
		};
		~SmdEvFree() {
			if (!used_) {
				fprintf(stderr, "Prosody function smd_ev_free not called\n");
				abort();
			};
		};
	};
public:
	SmdEvFree smd_ev_free() {
		SmdEvFree temp;
		
		other_ = 0;
		return temp;
	}
private:
	class CollectorGetEvent {
		mutable int used_;
		SM_COLLECTOR_EVENT_PARMS eventp_;
		tSMEventId *eventP_;
	public:
		CollectorGetEvent(const CollectorGetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		CollectorGetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
			eventP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_collector_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~CollectorGetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_collector_get_event not called\n");
				abort();
			};
		};
		CollectorGetEvent &collector(tSMCollectorId v) {
			eventp_.collector = v;
			return *this;
		};
		CollectorGetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	CollectorGetEvent collector_get_event() {
		return CollectorGetEvent();
	}
private:
	class CollectorStatus {
		mutable int used_;
		SM_COLLECTOR_STATUS_PARMS statusp_;
		enum kSMCollectorStatus *statusP_;
		in_addr *u_ports_addressP_;
		int *u_ports_portP_;
		in_addr *u_peer_addressP_;
		int *u_peer_portP_;
		in6_addr *u_ports_ipv6_addressP_;
		int *u_ports_ipv6_portP_;
		in6_addr *u_peer_ipv6_addressP_;
		int *u_peer_ipv6_portP_;
	public:
		CollectorStatus(const CollectorStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		CollectorStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_ports_addressP_ = 0;
			u_ports_portP_ = 0;
			u_peer_addressP_ = 0;
			u_peer_portP_ = 0;
			u_ports_ipv6_addressP_ = 0;
			u_ports_ipv6_portP_ = 0;
			u_peer_ipv6_addressP_ = 0;
			u_peer_ipv6_portP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_collector_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_ports_addressP_) *u_ports_addressP_ = statusp_.u.ports.address;
			if (u_ports_portP_) *u_ports_portP_ = statusp_.u.ports.port;
			if (u_peer_addressP_) *u_peer_addressP_ = statusp_.u.peer.address;
			if (u_peer_portP_) *u_peer_portP_ = statusp_.u.peer.port;
			if (u_ports_ipv6_addressP_) *u_ports_ipv6_addressP_ = statusp_.u.ports_ipv6.address;
			if (u_ports_ipv6_portP_) *u_ports_ipv6_portP_ = statusp_.u.ports_ipv6.port;
			if (u_peer_ipv6_addressP_) *u_peer_ipv6_addressP_ = statusp_.u.peer_ipv6.address;
			if (u_peer_ipv6_portP_) *u_peer_ipv6_portP_ = statusp_.u.peer_ipv6.port;
			return e;
		};
		~CollectorStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_collector_status not called\n");
				abort();
			};
		};
		CollectorStatus &collector(tSMCollectorId v) {
			statusp_.collector = v;
			return *this;
		};
		CollectorStatus &status(enum kSMCollectorStatus *v) {
			statusP_ = v;
			return *this;
		};
		CollectorStatus &u_ports_address(in_addr *v) {
			u_ports_addressP_ = v;
			return *this;
		};
		CollectorStatus &u_ports_port(int *v) {
			u_ports_portP_ = v;
			return *this;
		};
		CollectorStatus &u_peer_address(in_addr *v) {
			u_peer_addressP_ = v;
			return *this;
		};
		CollectorStatus &u_peer_port(int *v) {
			u_peer_portP_ = v;
			return *this;
		};
		CollectorStatus &u_ports_ipv6_address(in6_addr *v) {
			u_ports_ipv6_addressP_ = v;
			return *this;
		};
		CollectorStatus &u_ports_ipv6_port(int *v) {
			u_ports_ipv6_portP_ = v;
			return *this;
		};
		CollectorStatus &u_peer_ipv6_address(in6_addr *v) {
			u_peer_ipv6_addressP_ = v;
			return *this;
		};
		CollectorStatus &u_peer_ipv6_port(int *v) {
			u_peer_ipv6_portP_ = v;
			return *this;
		};
	};
public:
	CollectorStatus collector_status() {
		return CollectorStatus();
	}
private:
	class CollectorDestroy {
		mutable int used_;
		tSMCollectorId collector_;
	public:
		CollectorDestroy(const CollectorDestroy &old) {
			*this = old;
			old.used_ = 1;
		};
		CollectorDestroy(tSMCollectorId collector) : used_(0) {
			collector_ = collector;
		};
		operator Error() {
			Error e(Prosody::sm_collector_destroy(collector_));
			used_ = 1;
			return e;
		};
		~CollectorDestroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_collector_destroy not called\n");
				abort();
			};
		};
	};
public:
	CollectorDestroy collector_destroy() {
		CollectorDestroy temp;
		
		other_ = 0;
		return temp;
	}
private:
	class DispatcherDestroy {
		mutable int used_;
		tSMDispatcherId dispatcher_;
	public:
		DispatcherDestroy(const DispatcherDestroy &old) {
			*this = old;
			old.used_ = 1;
		};
		DispatcherDestroy(tSMDispatcherId dispatcher) : used_(0) {
			dispatcher_ = dispatcher;
		};
		operator Error() {
			Error e(Prosody::sm_dispatcher_destroy(dispatcher_));
			used_ = 1;
			return e;
		};
		~DispatcherDestroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_dispatcher_destroy not called\n");
				abort();
			};
		};
	};
public:
	DispatcherDestroy dispatcher_destroy() {
		DispatcherDestroy temp;
		
		other_ = 0;
		return temp;
	}
private:
	class ErrorName {
		mutable int used_;
		SM_ERROR_NAME_PARMS errp_;
	public:
		ErrorName(const ErrorName &old) {
			*this = old;
			old.used_ = 1;
		};
		ErrorName() : used_(0) {
			memset(&errp_, 0, sizeof(errp_));
		};
		operator Error() {
			Error e(Prosody::sm_error_name(&errp_));
			used_ = 1;
			return e;
		};
		~ErrorName() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_error_name not called\n");
				abort();
			};
		};
		ErrorName &code(tSM_INT v) {
			errp_.code = v;
			return *this;
		};
	};
public:
	ErrorName error_name() {
		return ErrorName();
	}
