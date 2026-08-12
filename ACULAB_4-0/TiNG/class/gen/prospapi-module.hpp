private:
	class AddOutputFreq {
		mutable int used_;
		SM_OUTPUT_FREQ_PARMS freqp_;
		tSM_INT *idP_;
	public:
		AddOutputFreq(const AddOutputFreq &old) {
			*this = old;
			old.used_ = 1;
		};
		AddOutputFreq() : used_(0) {
			memset(&freqp_, 0, sizeof(freqp_));
			idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_add_output_freq(&freqp_));
			used_ = 1;
			if (idP_) *idP_ = freqp_.id;
			return e;
		};
		~AddOutputFreq() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_output_freq not called\n");
				abort();
			};
		};
		AddOutputFreq &module(tSMModuleId v) {
			freqp_.module = v;
			return *this;
		};
		AddOutputFreq &freq(double v) {
			freqp_.freq = v;
			return *this;
		};
		AddOutputFreq &amplitude(double v) {
			freqp_.amplitude = v;
			return *this;
		};
		AddOutputFreq &id(tSM_INT *v) {
			idP_ = v;
			return *this;
		};
	};
public:
	AddOutputFreq add_output_freq() {
		return AddOutputFreq().module(module_);
	}
private:
	class AddOutputTone {
		mutable int used_;
		SM_OUTPUT_TONE_PARMS tonep_;
		tSM_INT *idP_;
	public:
		AddOutputTone(const AddOutputTone &old) {
			*this = old;
			old.used_ = 1;
		};
		AddOutputTone() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
			idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_add_output_tone(&tonep_));
			used_ = 1;
			if (idP_) *idP_ = tonep_.id;
			return e;
		};
		~AddOutputTone() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_output_tone not called\n");
				abort();
			};
		};
		AddOutputTone &module(tSMModuleId v) {
			tonep_.module = v;
			return *this;
		};
		AddOutputTone &component1_id(tSM_INT v) {
			tonep_.component1_id = v;
			return *this;
		};
		AddOutputTone &component2_id(tSM_INT v) {
			tonep_.component2_id = v;
			return *this;
		};
		AddOutputTone &id(tSM_INT *v) {
			idP_ = v;
			return *this;
		};
	};
public:
	AddOutputTone add_output_tone() {
		return AddOutputTone().module(module_);
	}
private:
	class AddInputFreqCoeffs {
		mutable int used_;
		SM_INPUT_FREQ_COEFFS_PARMS freqcoeffp_;
		tSM_INT *idP_;
	public:
		AddInputFreqCoeffs(const AddInputFreqCoeffs &old) {
			*this = old;
			old.used_ = 1;
		};
		AddInputFreqCoeffs() : used_(0) {
			memset(&freqcoeffp_, 0, sizeof(freqcoeffp_));
			idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_add_input_freq_coeffs(&freqcoeffp_));
			used_ = 1;
			if (idP_) *idP_ = freqcoeffp_.id;
			return e;
		};
		~AddInputFreqCoeffs() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_input_freq_coeffs not called\n");
				abort();
			};
		};
		AddInputFreqCoeffs &module(tSMModuleId v) {
			freqcoeffp_.module = v;
			return *this;
		};
		AddInputFreqCoeffs &upper_limit(double v) {
			freqcoeffp_.upper_limit = v;
			return *this;
		};
		AddInputFreqCoeffs &lower_limit(double v) {
			freqcoeffp_.lower_limit = v;
			return *this;
		};
		AddInputFreqCoeffs &id(tSM_INT *v) {
			idP_ = v;
			return *this;
		};
	};
public:
	AddInputFreqCoeffs add_input_freq_coeffs() {
		return AddInputFreqCoeffs().module(module_);
	}
private:
	class AddInputToneSet {
		mutable int used_;
		SM_INPUT_TONE_SET_PARMS tonesetp_;
		tSM_INT *idP_;
	public:
		AddInputToneSet(const AddInputToneSet &old) {
			*this = old;
			old.used_ = 1;
		};
		AddInputToneSet() : used_(0) {
			memset(&tonesetp_, 0, sizeof(tonesetp_));
			idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_add_input_tone_set(&tonesetp_));
			used_ = 1;
			if (idP_) *idP_ = tonesetp_.id;
			return e;
		};
		~AddInputToneSet() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_input_tone_set not called\n");
				abort();
			};
		};
		AddInputToneSet &module(tSMModuleId v) {
			tonesetp_.module = v;
			return *this;
		};
		AddInputToneSet &band1_first_freq_coeffs_id(tSM_INT v) {
			tonesetp_.band1_first_freq_coeffs_id = v;
			return *this;
		};
		AddInputToneSet &band1_freq_count(tSM_INT v) {
			tonesetp_.band1_freq_count = v;
			return *this;
		};
		AddInputToneSet &band2_first_freq_coeffs_id(tSM_INT v) {
			tonesetp_.band2_first_freq_coeffs_id = v;
			return *this;
		};
		AddInputToneSet &band2_freq_count(tSM_INT v) {
			tonesetp_.band2_freq_count = v;
			return *this;
		};
		AddInputToneSet &req_third_peak(double v) {
			tonesetp_.req_third_peak = v;
			return *this;
		};
		AddInputToneSet &req_signal_to_noise_ratio(double v) {
			tonesetp_.req_signal_to_noise_ratio = v;
			return *this;
		};
		AddInputToneSet &req_minimum_power(double v) {
			tonesetp_.req_minimum_power = v;
			return *this;
		};
		AddInputToneSet &req_twist_for_dual_tone(double v) {
			tonesetp_.req_twist_for_dual_tone = v;
			return *this;
		};
		AddInputToneSet &id(tSM_INT *v) {
			idP_ = v;
			return *this;
		};
	};
public:
	AddInputToneSet add_input_tone_set() {
		return AddInputToneSet().module(module_);
	}
private:
	class AdjustInputToneSet {
		mutable int used_;
		SM_ADJUST_TONE_SET_PARMS tonesetp_;
	public:
		AdjustInputToneSet(const AdjustInputToneSet &old) {
			*this = old;
			old.used_ = 1;
		};
		AdjustInputToneSet() : used_(0) {
			memset(&tonesetp_, 0, sizeof(tonesetp_));
		};
		operator Error() {
			Error e(Prosody::sm_adjust_input_tone_set(&tonesetp_));
			used_ = 1;
			return e;
		};
		~AdjustInputToneSet() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_adjust_input_tone_set not called\n");
				abort();
			};
		};
		AdjustInputToneSet &module(tSMModuleId v) {
			tonesetp_.module = v;
			return *this;
		};
		AdjustInputToneSet &tone_set_id(tSM_INT v) {
			tonesetp_.tone_set_id = v;
			return *this;
		};
		AdjustInputToneSet &parameter_id(enum kAdjustToneSet v) {
			tonesetp_.parameter_id = v;
			return *this;
		};
		AdjustInputToneSet &parameter_value_fp_value(double v) {
			tonesetp_.parameter_value.fp_value = v;
			return *this;
		};
		AdjustInputToneSet &parameter_value_int_value(tSM_INT v) {
			tonesetp_.parameter_value.int_value = v;
			return *this;
		};
	};
public:
	AdjustInputToneSet adjust_input_tone_set() {
		return AdjustInputToneSet().module(module_);
	}
private:
	class ResetInputCptones {
		mutable int used_;
		SM_RESET_INPUT_CPTONES_PARMS resetp_;
	public:
		ResetInputCptones(const ResetInputCptones &old) {
			*this = old;
			old.used_ = 1;
		};
		ResetInputCptones() : used_(0) {
			memset(&resetp_, 0, sizeof(resetp_));
		};
		operator Error() {
			Error e(Prosody::sm_reset_input_cptones(&resetp_));
			used_ = 1;
			return e;
		};
		~ResetInputCptones() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_reset_input_cptones not called\n");
				abort();
			};
		};
		ResetInputCptones &module(tSMModuleId v) {
			resetp_.module = v;
			return *this;
		};
		ResetInputCptones &tone_set_id(tSM_INT v) {
			resetp_.tone_set_id = v;
			return *this;
		};
	};
public:
	ResetInputCptones reset_input_cptones() {
		return ResetInputCptones().module(module_);
	}
private:
	class AddInputCptone {
		mutable int used_;
		SM_INPUT_CPTONE_PARMS cptonep_;
	public:
		AddInputCptone(const AddInputCptone &old) {
			*this = old;
			old.used_ = 1;
		};
		AddInputCptone() : used_(0) {
			memset(&cptonep_, 0, sizeof(cptonep_));
		};
		operator Error() {
			Error e(Prosody::sm_add_input_cptone(&cptonep_));
			used_ = 1;
			return e;
		};
		~AddInputCptone() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_input_cptone not called\n");
				abort();
			};
		};
		AddInputCptone &module(tSMModuleId v) {
			cptonep_.module = v;
			return *this;
		};
		AddInputCptone &state_count(tSM_INT v) {
			cptonep_.state_count = v;
			return *this;
		};
		AddInputCptone &id(tSM_INT v) {
			cptonep_.id = v;
			return *this;
		};
	};
public:
	AddInputCptone add_input_cptone() {
		return AddInputCptone().module(module_);
	}
private:
	class ResetInputVocabs {
		mutable int used_;
		SM_RESET_INPUT_VOCABS_PARMS resetp_;
	public:
		ResetInputVocabs(const ResetInputVocabs &old) {
			*this = old;
			old.used_ = 1;
		};
		ResetInputVocabs() : used_(0) {
			memset(&resetp_, 0, sizeof(resetp_));
		};
		operator Error() {
			Error e(Prosody::sm_reset_input_vocabs(&resetp_));
			used_ = 1;
			return e;
		};
		~ResetInputVocabs() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_reset_input_vocabs not called\n");
				abort();
			};
		};
		ResetInputVocabs &module(tSMModuleId v) {
			resetp_.module = v;
			return *this;
		};
	};
public:
	ResetInputVocabs reset_input_vocabs() {
		return ResetInputVocabs().module(module_);
	}
private:
	class AddInputVocab {
		mutable int used_;
		SM_INPUT_VOCAB_PARMS vocabp_;
		tSM_UT32 *item_idP_;
	public:
		AddInputVocab(const AddInputVocab &old) {
			*this = old;
			old.used_ = 1;
		};
		AddInputVocab() : used_(0) {
			memset(&vocabp_, 0, sizeof(vocabp_));
			item_idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_add_input_vocab(&vocabp_));
			used_ = 1;
			if (item_idP_) *item_idP_ = vocabp_.item_id;
			return e;
		};
		~AddInputVocab() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_add_input_vocab not called\n");
				abort();
			};
		};
		AddInputVocab &module(tSMModuleId v) {
			vocabp_.module = v;
			return *this;
		};
		AddInputVocab &filename(char* v) {
			vocabp_.filename = v;
			return *this;
		};
		AddInputVocab &item_id(tSM_UT32 *v) {
			item_idP_ = v;
			return *this;
		};
	};
public:
	AddInputVocab add_input_vocab() {
		return AddInputVocab().module(module_);
	}
private:
	class AdjustCatsigParams {
		mutable int used_;
		SM_ADJUST_CATSIG_MODULE_PARMS catsigp_;
	public:
		AdjustCatsigParams(const AdjustCatsigParams &old) {
			*this = old;
			old.used_ = 1;
		};
		AdjustCatsigParams() : used_(0) {
			memset(&catsigp_, 0, sizeof(catsigp_));
		};
		operator Error() {
			Error e(Prosody::sm_adjust_catsig_module_params(&catsigp_));
			used_ = 1;
			return e;
		};
		~AdjustCatsigParams() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_adjust_catsig_module_params not called\n");
				abort();
			};
		};
		AdjustCatsigParams &module(tSMModuleId v) {
			catsigp_.module = v;
			return *this;
		};
		AdjustCatsigParams &catsig_alg_id(tSM_INT v) {
			catsigp_.catsig_alg_id = v;
			return *this;
		};
		AdjustCatsigParams &parameter_id(enum kSMBESPCatSigParam v) {
			catsigp_.parameter_id = v;
			return *this;
		};
		AdjustCatsigParams &parameter_value_fp_value(double v) {
			catsigp_.parameter_value.fp_value = v;
			return *this;
		};
		AdjustCatsigParams &parameter_value_int_value(tSM_INT v) {
			catsigp_.parameter_value.int_value = v;
			return *this;
		};
	};
public:
	AdjustCatsigParams adjust_catsig_params() {
		return AdjustCatsigParams().module(module_);
	}
