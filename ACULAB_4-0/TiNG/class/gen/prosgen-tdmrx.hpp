private:
	class Create {
		mutable int used_;
		SM_TDMRX_CREATE_PARMS tdmrxp_;
		tSMTDMrxId *tdmrxP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&tdmrxp_, 0, sizeof(tdmrxp_));
			tdmrxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_tdmrx_create(&tdmrxp_));
			used_ = 1;
			if (tdmrxP_) *tdmrxP_ = tdmrxp_.tdmrx;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmrx_create not called\n");
				abort();
			};
		};
		Create &tdmrx(tSMTDMrxId *v) {
			tdmrxP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			tdmrxp_.module = v;
			return *this;
		};
		Create &stream(tSM_INT v) {
			tdmrxp_.stream = v;
			return *this;
		};
		Create &timeslot(tSM_INT v) {
			tdmrxp_.timeslot = v;
			return *this;
		};
		Create &type(enum kSMTimeslotType v) {
			tdmrxp_.type = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().tdmrx(&tdmrx_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMTDMrxId tdmrx_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMTDMrxId tdmrx) : used_(0) {
			tdmrx_ = tdmrx;
		};
		operator Error() {
			Error e(Prosody::sm_tdmrx_destroy(tdmrx_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmrx_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(tdmrx_);
		
		tdmrx_ = 0;
		return temp;
	}
private:
	class GetDatafeed {
		mutable int used_;
		SM_TDMRX_DATAFEED_PARMS datafeedp_;
		tSMDatafeedId *datafeedP_;
	public:
		GetDatafeed(const GetDatafeed &old) {
			*this = old;
			old.used_ = 1;
		};
		GetDatafeed() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
			datafeedP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_tdmrx_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~GetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_tdmrx_get_datafeed not called\n");
				abort();
			};
		};
		GetDatafeed &tdmrx(tSMTDMrxId v) {
			datafeedp_.tdmrx = v;
			return *this;
		};
		GetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	GetDatafeed get_datafeed() {
		return GetDatafeed().tdmrx(tdmrx_);
	}
