$| = 1;
@ARGV == 1 || die "Usage: mkclass.pl out-prefix\n";
$g_outpfx = shift(@ARGV);
$g_BOGUSSRC = 'Unknown';

	# maps name of field to our type
%g_ntypes = (
	'card' => 'card',
	'card_id' => 'card',
	'channel' => 'channel',
	'mod_id' => 'module',
	'module' => 'module',
	'module_id' => 'module',
	'path' => 'path',
	'rtcphand' => 'rtcphand',
	'tdmrx' => 'tdmrx',
	'tdmtx' => 'tdmtx',
	'avfplay' => 'avfplay',
	'avfrec' => 'avfrec',
	'vmprx' => 'vmprx',
	'vmptx' => 'vmptx',
);

	# maps type of field to our type
%g_ttypes = (
	'tSMCardId' => 'card',
	'tSMChannelId' => 'channel',
	'tSMModuleId' => 'module',
	'tSMPathId' => 'path',
	'tSMRTCPHandId' => 'rtcphand',
	'tSMTDMrxId' => 'tdmrx',
	'tSMTDMtxId' => 'tdmtx',
	'tSMVMPrxId' => 'vmprx',
	'tSMVMPtxId' => 'vmptx',
	'tSMAVFplayId' => 'avfplay',
	'tSMAVFrecId' => 'avfrec',
);

use IO::File;

sub dumpval {
	my ($v, $lvl) = @_;
	$lvl++;
	if (!ref($v)) {
		if (!defined($v)) {
			print "undef";
		} else {
			$v =~ s/["\\]/\\&/g;
			print '"', $v, '"';
		}
	} elsif (ref($v) eq 'REF') {
		print "&";
		&dumpval($$v, $lvl+1);
	} elsif (ref($v) eq 'SCALAR') {
		print "&";
		&dumpval($$v, $lvl+1);
	} elsif (ref($v) eq 'ARRAY') {
		my $x;
		print "(\n";
		for $x (@{$v}) {
			print "\t" x $lvl;
			&dumpval($x, $lvl);
			print ",\n";
		}
		print "\t" x ($lvl - 1);
		print ")";
	} elsif (ref($v) eq 'HASH') {
		my $x;
		print "{\n";
		for $x (sort(keys(%{$v}))) {
			print "\t" x $lvl;
			&dumpval($x, $lvl+1);
			print " => ";
			&dumpval($v->{$x}, $lvl+1);
			print ",\n";
		}
		print "\t" x ($lvl - 1);
		print "}";
	} elsif (ref($v) eq 'CODE') {
		print "sub $x";
	} elsif (ref($v) eq 'GLOB') {
		print "*$x";
	}
}

sub flatten {
	my ($p, $parblk) = @_;
	my @r = ();
	my $x;
	for $x (@{$p}) {
		if ($x->{type} eq 'struct') {
			my $y;
			my %had;
			for $y (@{&flatten($x->{fields})}) {
				my $xn = $x->{name};
				if ($xn =~ /\*/) {
					if (!$had{$xn}) {
						my %xp = %{$x};
						$had{$xn} = 1;
						$xp{type} = $parblk . '::' . $xp{tagname};
						$xp{dir} = 'in';
						push(@r, \%xp);
					}
				} else {
					$y->{name} = $xn . '.' . $y->{name};
					push(@r, $y);
				}
			}
		} else {
			push(@r, $x);
		}
	}
	return \@r;
}

sub readinput {
	my @enums = ();
	my %fn;
	# fields in fn
	# 
	# name	the name of the function (from apifn)
	# params	the parameter structure
	# class	the type (class) to which this function belongs
	# ctor	this function is a ctor, and this field is the type from which
	#	objects are built
	my @ver = ();
	my @pstack;
	my @pname;
	my $linno = 0;
	while (<STDIN>) {
		$linno++;
		chomp;
		next if /^\t/ || /^\s*#/;
		my @s = split;
		next unless @s;
		if ($s[0] eq 'version') {	# code/-/+
			if ($s[1] =~ /^[-+]$/) {
				die "Line $linno: Confused: no version, but got $_\n" if !@ver;
				shift(@ver);
			} else {
				unshift(@ver, $s[1]);
			}
		} elsif (@ver && $ver[0] !~ /TiNG/ && $ver[0] !~ /Prel/ && $ver[0] !~ /deprecated/) {
			# skip it - non TiNG
		} elsif ($s[0] eq 'apifn') {		# funcname
			die "Line $linno: Oops: got $_ but still in previous function!\n" if %fn;
			$fn{name} = $s[1];
			$fn{dtor} = 1 if $s[2] eq '-dtor';
		} elsif ($s[0] eq 'params') {	# struct-tag paramname
			shift(@s);
			push(@pname, \@s);
			my @p;
			unshift(@pstack, \@p);
		} elsif ($s[0] eq 'field') {	# type name direction
			my %e;
			$e{type} = $s[1];
			$e{name} = $s[2];
			$e{dir} = $s[3];
			shift(@s);
			push(@{$pstack[0]}, \%e);
			&checktype(\%fn, \%e);
		} elsif ($s[0] eq 'unstruct') {	# name display-fn
			my $pr = shift(@pstack);
			my %e;
			$e{fields} = $pr;
			if (@s > 1) {
				$e{type} = 'struct';
				$e{name} = $s[1];
				$e{dir} = 'X';
				$e{tagname} = shift(@pstack);
			} else {
				my $ps = pop(@pname);
				$e{type} = 'struct';
				$e{tagname} = $ps->[0];
				$e{dir} = 'in';
				$e{name} = $ps->[1];
			}
			push(@{$pstack[0]}, \%e);
		} elsif ($s[0] eq 'enumend') {	# fieldname direction
			my %e;
			$e{type} = "enum " . pop(@enums);
			$e{name} = $s[1];
			$e{dir} = $s[2];
			shift(@s);
			push(@{$pstack[0]}, \%e);
		} elsif ($s[0] eq 'enumref') {	# enum-tag fieldname direection
			my %e;
			$e{type} = "enum " . $s[1];
			$e{name} = $s[2];
			$e{dir} = $s[3];
			shift(@s);
			push(@{$pstack[0]}, \%e);
		} elsif ($s[0] eq 'enumstart') {	# enum-tag
			push(@enums, $s[1]);
		} elsif ($s[0] eq 'param') {	# type name
			my %e;
			$e{type} = $s[1];
			$e{name} = $s[2];
			$e{dir} = 'in';
			push(@{$pstack[0]}, \%e);
			&checktype(\%fn, \%e);
		} elsif ($s[0] eq 'struct') {	# struct-tag
			my @p;
			unshift(@pstack, $s[1]);
			unshift(@pstack, \@p);
		} elsif ($s[0] eq 'union') {	#
			my @p;
			unshift(@pstack, $s[1]);
			unshift(@pstack, \@p);
		} elsif ($s[0] eq 'error') {	# error summary
		} elsif ($s[0] eq 'explain') {	#
			die "Line $linno: 'params' not closed in $fn{name}\n" if @pname;
			my $pr = shift(@pstack);
			$fn{params} = $pr;
			&onefn(\%fn);
			%fn = ();
		} elsif ($s[0] eq 'fnref') {	# funcname
		} elsif ($s[0] eq 'modref') {	# modulename
		} elsif ($s[0] eq 'ref') {	# name
		} elsif ($s[0] eq 'retval') {	#
		} elsif ($s[0] eq 'secname') {	#
		} elsif ($s[0] eq 'const') {	# name value
		} elsif ($s[0] eq 'enum') {	# name value
		} elsif ($s[0] eq '') {
		} else {
			print STDERR "K='$s[0]', Ignoring: $_\n";
		}
	}
}

# check if this field (or param) tells us what object the current
# function operates on. If it does, and it's an 'out' field, then this
# function must be a constructor for the object. Such a constructor must
# construct only one type of object and must do so by taking another type
# of object as input.
sub checktype {
	my ($fn, $e) = @_;
	my $entype = $g_ntypes{$e->{name}};	# obj from field name
	return unless defined($entype);
	{
	 my $ettype = $g_ttypes{$e->{type}};	# obj from field type
	 return unless defined($ettype) && $ettype eq $entype;
	}
		# we have suitable field name and type
		# and they match (e.g. 'tSMChannelId' & 'channel')
	my $class = $fn->{class};
	if (defined($class)) {
		my $dir = $e->{dir};
		if ($dir eq 'out') {	# ctor for this type
			if (defined($fn->{ctor})) {
				die "Oops: $fn->{name}: both $class and $entype are 'out'\n";
			} else {
				$fn->{ctor} = $class;
				$fn->{class} = $entype;
			}
		} elsif (!defined($fn->{ctor})) {
					# not a ctor: first named in function
					# name is the object
					# e.g. sm_vmprx_set_rtcphand belongs
					# to vmprx, not rtcphand
				my $pold = index($fn->{name}, $class);
				my $pnew = index($fn->{name}, $entype);
				if ($pold < 0 || $pold > $pnew) {
					$fn->{class} = $entype;
				}
		} elsif ($fn->{ctor} ne $g_BOGUSSRC) {
			die "Oops: $fn->{name} ctor for $class uses both $fn->{ctor} and $entype\n";
		} else {
			$fn->{ctor} = $entype;
		}
	} else {
		$fn->{class} = $entype;
			# if dir is out, we don't know the correct value
			# for 'ctor', since we haven't seen the field yet.
			# Just put anything in here to remember that it
			# must be a ctor. If we never see another field
			# from which this class can be constructed, then
			# that is an error, so we use a fake type so that
			# it will be spotted more easily
		$fn->{ctor} = $g_BOGUSSRC if $e->{dir} eq 'out';
	}
}

sub fn_to_class {
	my $f = $_[0];
	my @f = split(/_/, $f);
	for $f (@f) {
		substr($f, 0, 1) =~ tr/a-z/A-Z/;
	}
	return join('', @f);
}

sub onefn {
	my $fn = $_[0];
	my $class = $fn->{class};
	my $par = $fn->{params};
	my $fname = $fn->{name};
	if (!defined($class)) {
		print STDERR "Other: $fname\n";
		$class = 'other';
	}
	my $mname = $fname;
	$mname =~ s/^sm_//;
	$mname =~ s/^smdc_//;
	$mname =~ s/${class}_//;
	my $cname = &fn_to_class($mname);
	my $fh = $files{$class};
	if (!defined($fh)) {
		$files{$class} = new IO::File;
		$files{$class}->open(">$g_outpfx$class.hpp") || die "Cannot create file: $g_outpfx$class.hpp: $!\n";
		$fh = $files{$class};
		binmode $fh;
	}
	my $docfh = $docfiles{$class};
	if (!defined($docfh)) {
		$docfiles{$class} = new IO::File;
		$docfiles{$class}->open(">$g_outpfx$class.html") || die "Cannot create file: $g_outpfx$class.html: $!\n";
		$docfh = $docfiles{$class};
		binmode $docfh;
		print $docfh <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
<link rel=stylesheet type="text/css" href="api.css">
<title>Prosody with classes - methods provided for a $class</title>
</head>
<body>
<h1>Prosody with classes - methods provided for a $class</h1>
<p>
<table border>
<tr><th>method<th>Prosody C API function
EOF
	}
	print $fh "private:\n";
	print $fh "\tclass $cname {\n";
	print $fh "\t\tmutable int used_;\n";
	my $memset = '';
	my $is_ctor = $fn->{ctor};
	my $is_dtor = $fn->{dtor};
	die "$fn->{name} is ctor, but no source type is known\n" if $is_ctor eq $g_BOGUSSRC;
	my $init_list = '';
	my @param_list = ();
	my $ctor_init = '';
	my @ctor_args = ();
	my $ret = '';
	my $parpass = '';
	my @plist = ();
	for $x (@{$par}) {
		if ($x->{type} eq 'struct') {
			my $p = $x->{tagname};
			$p =~ tr/a-z/A-Z/;
			my $n = $x->{name} . '_';
				# declare field for param block
			print $fh "\t\t$p $n;\n";
				# add to memset list
			$memset .= "\t\t\tmemset(&$n, 0, sizeof($n));\n";
				# add to API parameters
			push(@plist, '&' . $n);
			$p =~ tr/a-z/A-Z/;
			for $y (@{&flatten($x->{fields}, $p)}) {	# out and inout - init
				my $name = $y->{name};
				my $type = $y->{type};
				my $dir = $y->{dir};
				my $isself = defined($g_ttypes{$type})
					&& $g_ttypes{$type} eq $class
					&& defined($g_ntypes{$name})
					&& $g_ntypes{$name} eq $class;
				if ($name =~ s/\*//) {
					die "Has pointer to output: $fname -> $name\n" unless $dir eq 'in';
					$type .= '*';
				}
				my $isarray = 0;
				if ($name =~ s/\[.*\]//) {
					if ($dir eq 'out' || $type ne 'char') {
						print STDERR "array: $name\n";
						next;	# FIXME: handle arrays!
					}
					$isarray = 1;
				}
				my $qname = $name;
				$qname =~ s/\./_/g;
				if ($dir ne 'in') {
						# declare pointer to the value
					print $fh "\t\t$type *${qname}P_;\n";
						# init the pointer
					$ctor_init .= "\t\t\t${qname}P_ = 0;\n";
						# store the output value
					if ($isarray) {
						$ret .=  "\t\t\tif (${qname}P_) memcpy(${qname}P_, $n.$name, sizeof($n.$name));\n";
					} else {
						$ret .=  "\t\t\tif (${qname}P_) *${qname}P_ = $n.$name;\n";
					}
						# member to set the pointer
					$parpass .= <<EOF;
		$cname &$qname($type *v) {
			${qname}P_ = v;
			return *this;
		};
EOF
					if ($isself) {
						$init_list .= ".$name(&${class}_)";
					}
				}
				if ($dir ne 'out') {
					# member to set the input value
					if ($isarray) {
						$parpass .= <<EOF;
		$cname &$qname($type *v) {
			strncpy($n.$name, v, sizeof($n.$name));
			return *this;
		};
EOF
					} else {
						$parpass .= <<EOF;
		$cname &$qname($type v) {
			$n.$name = v;
			return *this;
		};
EOF
					}
					if ($isself) {
						$init_list .= ".$name(${class}_)";
					}
				}
			}
		} else {
			my $n = $x->{name};
			my $type = $x->{type};
			my $isself = defined($g_ttypes{$type})
				&& $g_ttypes{$type} eq $class;
				# declare field for value
			print $fh "\t\t$type ${n}_;\n";
				# add to ctor args
			push(@ctor_args, $type . " " . $n);
				# copy into field
			$ctor_init .= "\t\t\t${n}_ = $n;\n";
				# pass to API function
			push(@plist, $n . '_');
			if ($isself) {
				push(@param_list, "${class}_");
			}
		}
	}
	print $fh "\tpublic:\n";
	print $fh "\t\t$cname(const $cname &old) {\n";
	print $fh "\t\t\t*this = old;\n";
	print $fh "\t\t\told.used_ = 1;\n";
	print $fh "\t\t};\n";
	print $fh "\t\t$cname(", join(', ', @ctor_args), ") : used_(0) {\n";
	print $fh $memset;
	print $fh $ctor_init;
	print $fh "\t\t};\n";
	print $fh "\t\toperator Error() {\n";
	print $fh "\t\t\tError e(Prosody::$fname(", join(', ', @plist), "));\n";
	#print $fh "\t\t\t${is_ctor}_ = $is_ctor;\n" if defined($is_ctor);
	print $fh "\t\t\tused_ = 1;\n";
	print $fh $ret;
	print $fh "\t\t\treturn e;\n";
	print $fh "\t\t};\n";
	print $fh "\t\t~$cname() {\n";
	print $fh "\t\t\tif (!used_) {\n";
	print $fh "\t\t\t	fprintf(stderr, \"Prosody function $fname not called\\n\");\n";
	print $fh "\t\t\t	abort();\n";
	print $fh "\t\t\t};\n";
	print $fh "\t\t};\n";
	print $fh $parpass;
	print $fh "\t};\n";
	my $pl = join(', ', @param_list);
	if ($is_dtor) {
		$pl = "($pl)" if $pl ne '';
		$init_list = "temp$init_list;" if $init_list;
		print $fh <<EOF;
public:
	$cname $mname() {
		$cname temp$pl;
		$init_list
		${class}_ = 0;
		return temp;
	}
EOF
	} else {
		print $fh <<EOF;
public:
	$cname $mname() {
		return $cname($pl)$init_list;
	}
EOF
	}
	print $docfh <<EOF;
	<tr><td>$mname<td><a href="../../pubdoc/gen/apifn-$fname.html">$fname</a>
EOF
}

&readinput();
for $docfh (values(%docfiles)) {
	print $docfh <<EOF;
</table>
<p>
These methods are part of <a href="../pwc.html">Prosody with classes</a>.
EOF
}
