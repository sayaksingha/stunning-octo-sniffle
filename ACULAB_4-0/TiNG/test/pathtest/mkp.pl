#!/usr/bin/perl
# generate a C++ class which represents the whole configuration
use strict;
@ARGV == 2 || die "Usage: mkcfg.pl hpp cpp\n";
open(HPP, ">$ARGV[0]") || die "Cannot create file: $ARGV[0]: $!\n";
open(CPP, ">$ARGV[1]") || die "Cannot create file: $ARGV[0]: $!\n";
&makecfg();

sub makecfg {
	my %rxcodec = (
		alaw => {
			payload_type => 'payload',
			plc_mode => 'plcmode',
		},
		amrnb => {
			payload_type => 'int',
			aligned => 'int',
			gsmefr => 'int',
			plc_mode => 'plcmode',
		},
		comfort_noise => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		evrc => {
			payload_type => 'int',
			post_filter => 'int',
			rtp_mode => 'evrcrtpmode',
			plc_mode => 'plcmode',
		},
		g723_1 => {
			payload_type => 'int',
			post_filter => 'int',
			plc_mode => 'plcmode',
		},
		g726 => {
			payload_type => 'int',
			bits => 'int',
			plc_mode => 'plcmode',
		},
		g728 => {
			payload_type => 'int',
			rate => 'int',
			post_filter => 'int',
			plc_mode => 'plcmode',
		},
		g729ab => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		g729i => {
			payload_type => 'int',
			g729_mode => 'g729mode',
		},
		gsmefr => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		gsmfr => {
			payload_type => 'int',
			variant => 'gsmfr_variant',
			plc_mode => 'plcmode',
		},
		ilbc => {
			payload_type => 'int',
			frame_len => 'int',
			enhancer => 'int',
			plc_mode => 'plcmode',
		},
		isac => {
			payload_type => 'int',
		},
		l16 => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		l8 => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		melpe => {
			payload_type => 'int',
		},
		mulaw => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		rfc2833 => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
		rfc4040 => {
			payload_type => 'int',
		},
		smv => {
			payload_type => 'int',
			plc_mode => 'plcmode',
		},
	);
	my %txcodec = (
		alaw => {
			payload_type => 'payload',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		amrnb => {
			payload_type => 'int',
			aligned => 'int',
			gsmefr => 'int',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
		comfort_noise => {
			payload_type => 'int',
		},
		evrc => {
			payload_type => 'int',
			high_pass_filter => 'int',
			noise_suppression_filter => 'int',
			rtp_mode => 'evrcrtpmode',
			max_rate => 'evrcrate',
			min_rate => 'evrcrate',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
		g723_1 => {
			payload_type => 'int',
			high_pass_filter => 'int',
			rate => 'int',
			silence_compression => 'int',
			frames_per_packet => 'int',
		},
		g726 => {
			payload_type => 'int',
			bits => 'int',
			VADMode => 'vadmode',
			packetlen => 'int',
		},
		g728 => {
			payload_type => 'int',
			rate => 'int',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		g729ab => {
			payload_type => 'int',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		g729i => {
			payload_type => 'int',
			g729_mode => 'g729mode',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		gsmefr => {
			payload_type => 'int',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
		gsmfr => {
			payload_type => 'int',
			variant => 'gsmfr_variant',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
		ilbc => {
			payload_type => 'int',
			frame_len => 'int',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
		isac => {
			payload_type => 'int',
			partner => 'vmprx',
			channel_associated_mode => 'int',
		},
		l16 => {
			payload_type => 'int',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		l8 => {
			payload_type => 'int',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		melpe => {
			payload_type => 'int',
			ptime => 'int',
		},
		mulaw => {
			payload_type => 'int',
			VADMode => 'vadmode',
			ptime => 'int',
		},
		rfc2833 => {
			payload_type => 'int',
		},
		rfc4040 => {
			payload_type => 'int',
			packetlen => 'int',
		},
		smv => {
			payload_type => 'int',
			VADMode => 'vadmode',
			frames_per_packet => 'int',
		},
	);
	my %cfg = (
		card => 'card',
		mod => 'mod',
		path => {
			agc => {
				agc => "int",
				volume => "int",
			},
			ec => {
				enable => 'int',
				ref => "source",
				non_linear => 'int',
				use_agc => 'int',
				fix_agc => 'int',
				span => 'int',
				delay => 'int',
			},
			mix => {
				enable => 'int',
				mixin => "source",
				volume => "int",
			},
			pitchshift => {
				by => 'float',
			},
			resample => {
				uprate => 'int',
				downrate => 'int',
			},
			},
		play => {
			agc => "int",
			file => "string",
			format => "format",
			once => "once",
			rate => "int",
			speed => "int",
			threshold => "int",
			volume => "int",
			outrate => 'int',
			},
		rec => {
			agc => "int",
			file => "string",
			format => "format",
			gain => "int",
			rate => "int",
			threshold => "int",
			volume => "int",
			},
		rtcp => {
			bw => "int",
			file => "string",
			reports => "int",
			rxbw => "int",
			sdes => 'svec',
			txbw => "int",
			},
		rtprx => {
			local => 'addr',
			regen => 'int',
			rtcp => 'zint',
			detect => 'int',
			codec => 'rxcodec',
			rate => 'int',
			dataloss => 'int',
			maxjitter => 'int',
			initjitter => 'int',
			unhandled => 'int',
			},
		rtptx => {
			codec => 'txcodec',
			dest => 'addr',
			destport => 'port',
			isac_ptime => 'int',
			isac_rate => 'int',
			rate => 'int',
			rtcp => 'zint',
			rtcpdest => 'addr',
			rtcpdestport => 'port',
			rtcpsrc => 'addr',
			rtcpsrcport => 'port',
			src => 'addr',
			srcport => 'port',
			tosrtcp => 'int',
			tosrtp => 'int',
			},
		tdmrx => {
			ts => "timeslot",
			},
		tdmtx => {
			ts => "timeslot",
			},
	);
	print HPP &gencfgclass('CFGRXCODEC', \%rxcodec, ''), ";\n";
	print HPP &gencfgclass('CFGTXCODEC', \%txcodec, ''), ";\n";
	print HPP "\n";
	print HPP &gencfgclass('CFG', \%cfg, ''), ";\n";
	&makeconfigfn('rx', \%rxcodec);
	&makeconfigfn('tx', \%txcodec);
}

# generate a class definition derived from CFGVAL (i.e. "class FOO : class CFGVAL { ... }")
# 'name' is its name (i.e. 'CFGfoo' for 'class CFGfoo'), 'fields' is a
# ref to a class hash which is a mapping from field name to field type
# with a field type being a text string for predefined types (such as 'int')
# or another class hash to describe an inner class, and 'pfx' is the prefix required
# when referring to this class from global scope (e.g. 'BAR::')
# function bodies are written to CPP, but the declaration is returned.
sub gencfgclass {
	my ($name, $fields, $pfx) = @_;
	my $data = '';	# data part of class
	my $names = '';	# field names for help text
	my $f;
	for $f (sort keys %{$fields}) {
		my $field = $fields->{$f};
		if (ref($field)) {	# another class
			my $uc = $f;
			$uc =~ tr/a-z/A-Z/;
			$data .= &gencfgclass("CFG$uc", $field, $pfx . $name . '::');
		} else {		# simple type
			$field =~ tr/a-z/A-Z/;
			$data .= "CFG$field";
		}
		$data .= " $f;\n";	# name of field
		$names .= $f . ' ';
	}
	chop($names);
		# generate the body of the parsing function
	my $parse = "char *s;\nif (!*cmd) return cmd;\n";
	for $f (sort keys %{$fields}) {	# find the field being configured and forward to its parse()
		$parse .= "if ((s = wprefix(cmd, \"$f\"))) return $f.parse(cfg, s);\n";
	}
		# failed to find a field to configure - check for help
	$parse .= <<EOF;
if (*cmd == '?') {
	std::cerr << "Item names: $names" << std::endl;
	return 0;
}
EOF
		# nothing matched - show that we made no progress
	$parse .= "return cmd;\n";
		# wrap and indent the body of parse()
	print CPP "char *$pfx${name}::parse(CFG *cfg, char *cmd) {\n" . &indent($parse) . "}\n\n";
		# now generate the put function
	my $show = '';
	for $f (sort keys %{$fields}) {
		$show .= "<< \" $f\" << $f\n";
	}
	$show = "return os\n" . &indent($show) . ";\n";
	print CPP "std::ostream &$pfx${name}::put(std::ostream &os) {\n"
		. &indent($show) . "}\n\n";
		# all the parts are ready, compose and return the whole class
	return <<EOF
class $name : public CFGVAL {
public:
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os);
EOF
		. &indent($data)
		. "}";
}

# given a string containing any number of lines, return the same text
# with each line indented by one more tab
sub indent {
	my ($c) = @_;
	return '' if $c eq '';
	$c =~ s/\n/\n\t/g;
	chop($c);
	return "\t" . $c;
}

sub makecfgenum {
	my ($name, $vals) = @_;
	my $exp = '';
	my $names = '';
	my $parse = '';
	my $show = '';
	my $e;
	for $e (@{$vals}) {
		$names .= "v_$e,\n";
		$exp .= " '$e'";
		$parse .= qq!else if ((s = prefix(cmd, "$e"))) val = v_$e;\n!;
		$show .= qq!case v_$e: return os << " $e";\n!
	}
	$names = &indent(&indent($names));
	$parse = &indent(&indent($parse));
	$show = &indent(&indent($show));
	chop($names);
	chop($parse);
	chop($show);
	return <<EOF;
class CFGENUM_$name : public CFGVAL {
	enum {
$names
	} val;
public:
	CFGENUM_$name() { }
	char *parse(CFG *cfg, char *cmd) {
		char *s;
		if (0);	// to make remaining cases look the same
$parse
		else {
			std::cerr << "Expected one of$exp; got '"
				<< cmd << "'\\n";
			return 0;
		}
		return s;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
$show
		}
		return os << " (?err)";
	}
};
EOF
}

sub makeconfigfn {
	my ($dir, $fields) = @_;
	my $ucdir = $dir;
	$ucdir =~ tr/a-z/A-Z/;
	my $v = '';
	my $codec;
	for $codec (sort(keys(%{$fields}))) {
		my @check = ();
		my $cmd = "\tProsody::Error pe = v$dir->config_codec_$codec()";
		my $field;
		for $field (sort(keys(%{$fields->{$codec}}))) {
			$cmd .= "\n\t\t\t.$field(newcfg->$codec.$field)";
			push(@check, "oldcfg->$codec.$field != newcfg->$codec.$field");
		}
		$cmd .= ";\n";
		$cmd .= <<EOF;
	if (pe) {
		std::cerr << "sm_rtp${dir}_config_codec_$codec() failed: " << errcode_sm(pe) << "\\n";
		return 1;
	}
EOF
		$v .= " if (" . join(' || ', @check) . ") {\n";
		$v .= $cmd;
		$v .= " }\n";
	}
	print HPP "int reconfig_${dir}codec(CFG${ucdir}CODEC *oldcfg, CFG${ucdir}CODEC *newcfg, Prosody::Vmp$dir *v$dir);\n";
	print CPP <<EOF;
int reconfig_${dir}codec(CFG${ucdir}CODEC *oldcfg, CFG${ucdir}CODEC *newcfg, Prosody::Vmp$dir *v$dir)
{
$v return 0;
}

EOF
}
