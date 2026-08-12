private:
	class Create {
		mutable int used_;
		SM_TDMTX_CREATE_PARMS tdmtxp_;
		tSMTDMtxId *tdmtxP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&tdmtxp_, 0, sizeof(tdmtxp_));
			tdmtxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_tdmtx_create(&tdmtxp_));
			used_ = 1;
			if (tdmtxP_) *tdmtxP_ = tdmtxp_.tdmtx;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmtx_create not called\n");
				abort();
			};
		};
		Create &tdmtx(tSMTDMtxId *v) {
			tdmtxP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			tdmtxp_.module = v;
			return *this;
		};
		Create &stream(tSM_INT v) {
			tdmtxp_.stream = v;
			return *this;
		};
		Create &timeslot(tSM_INT v) {
			tdmtxp_.timeslot = v;
			return *this;
		};
		Create &type(enum kSMTimeslotType v) {
			tdmtxp_.type = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().tdmtx(&tdmtx_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMTDMtxId tdmtx_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMTDMtxId tdmtx) : used_(0) {
			tdmtx_ = tdmtx;
		};
		operator Error() {
			Error e(Prosody::sm_tdmtx_destroy(tdmtx_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmtx_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(tdmtx_);
		
		tdmtx_ = 0;
		return temp;
	}
private:
	class DatafeedConnect {
		mutable int used_;
		SM_TDMTX_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		DatafeedConnect(const DatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_tdmtx_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~DatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmtx_datafeed_connect not called\n");
				abort();
			};
		};
		DatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
		DatafeedConnect &tdmtx(tSMTDMtxId v) {
			datafeedp_.tdmtx = v;
			return *this;
		};
	};
public:
	DatafeedConnect datafeed_connect() {
		return DatafeedConnect().tdmtx(tdmtx_);
	}
