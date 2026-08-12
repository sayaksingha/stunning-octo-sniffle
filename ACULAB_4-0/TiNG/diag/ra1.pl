@rem = '-*- Perl -*-';
@rem = '
@echo off
perl h:\CharlesB\bin\%0.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
';

# V.110 RA1 stage (extract bits from timeslots)
# bits are msb first
die "Usage: ra1 [file]\n" unless $#ARGV <= 0;
if ($#ARGV == -1) {
	*FILE = *STDIN;
} else {
	$file = shift;
	open(FILE, $file) || die "Cannot open $file: $!\n";
}
binmode(FILE);
%e123 = (
	('000', '000=?'),
	('001', '001=d30-12k'),
	('010', '010=d12-1200'),
	('011', '011=d48-4800'),
	('100', '100=d6-600'),
	('101', '101=d36-3600'),
	('110', '110=d24-2400'),
	('111', '111=?')
);
$s = '';
$this = undef;
$copies = 0;
for ($i=0; $i < 256; $i++) {
	my $byte = $i;
	my $s;
	for ($n=0; $n < 8; $n++, $byte <<= 1) {
		$s .= $byte & 128 ? "1" : "0";
	}
	$bin[$i] = $s;
}
$showskiplen = 100;
# in this loop we have:
#	a) skiplen == 0 && skipbits == ''
#		when receiving idle
#	b) skiplen == 0 && skipbits ne ''
#		after receiving a small number of non-idle bits
#	c) skiplen != 0
#		after receiving many non-idle bits
mainloop:
for (;;) {
	for (;;) {
		$_ = <FILE>;
		last mainloop unless $_;
		if (/@/) {
			print $_;
			next;
		}
		chop;
		if (length($s)) {
			$s .= $_;
		} elsif (/0/) {
			$s = $_;
		} else {
			if ($skiplen) {
				$skiplen += length($_);
			} elsif ($skipbits ne '') {
				$skipbits .= $_;
				if (length($skipbits) > $showskiplen) {
					$skiplen = length($skipbits);
					$skipbits = '';
				}
			} else {
				$idlelen += length($_);
			}
			$this = undef;
		}
		last if length($s) >= 80;
	}
	# assert 80 <= length($s) <= 88
	if (defined($this) && substr($s, 0, 80) eq $this && 0) {
		$s = substr($s, 80);
		$copies++;
	} elsif ($s =~ /000000001.......1.......1.......1.......1.......1.......1.......1.......1......./) {
		$skipbits .= $`;
		$this = $&;
		$s = $';
		&flushbits();
		&printframe($this);
	}
	if (length($s) >= 80) {
		$skipbits .= substr($s, 0, -80);
		$s = substr($s, -80);
		if (!$skiplen && $skipbits =~ s/^(1*)//) {
			$idlelen += length($1);
		}
		if ($skiplen || length($skipbits) > $showskiplen) {
			$skiplen += length($skipbits);
			$skipbits = '';
		}
	}
	# assert 0 <= length($s) <= 80
}
$skipbits .= $s;
&flushbits();
print "\n";

sub printframe {
	my $this = $_[0];
	print "\n";
	print substr($this, 9, 6), " ";
	print substr($this, 17, 6), " ";
	print substr($this, 25, 6), " ";
	print substr($this, 33, 6), " (";
	print substr($this, 41, 7), ") ";
	print substr($this, 49, 6), " ";
	print substr($this, 57, 6), " ";
	print substr($this, 65, 6), " ";
	print substr($this, 73, 6), "\n";
	$s1 = substr($this, 15, 1);
	$x1 = substr($this, 23, 1);
	$s3 = substr($this, 31, 1);
	$s4 = substr($this, 39, 1);
	$e13 = substr($this, 41, 3);
	$e47 = substr($this, 44, 4);
	$s6 = substr($this, 55, 1);
	$x2 = substr($this, 63, 1);
	$s8 = substr($this, 71, 1);
	$s9 = substr($this, 79, 1);
	if ($s1 eq $s3 && $s1 eq $s6 && $s1 eq $s8
		&& $s1 eq $s4 && $s1 eq $s9) {
		print " s=$s1" if $s1 ne $prevs;
		$prevs = $s1;
		$prevsa = $presb = '';
		$prevs1 = $prevs3 = $prevs4 = '';
		$prevs6 = $prevs8 = $prevs9 = '';
	} else {
		if ($s1 eq $s3 && $s1 eq $s6 && $s1 eq $s8) {
			print " sa=$s1" if $s1 ne $prevsa;
			$prevsa = $s1;
			$prevs = '';
			$prevs1 = $prevs3 = $prevs6 = $prev8 = '';
		} else {
			$prevs = $prevsa = '';
			print " s1=$s1" if $s1 ne $prevs1;
			print " s3=$s3" if $s3 ne $prevs3;
			print " s6=$s6" if $s6 ne $prevs6;
			print " s8=$s8" if $s8 ne $prevs8;
			$prevs1 = $s1;
			$prevs3 = $s3;
			$prevs6 = $s6;
			$prevs8 = $s8;
		}
		if ($s4 eq $s9) {
			print " sb=$s4" if $s4 ne $prevsb;
			$prevs = '';
			$prevsb = $s4;
			$prevs4 = $prevs9 = '';
		} else {
			print " s4=$s4" if $s4 ne $prevs4;
			print " s9=$s9" if $s9 ne $prevs9;
			$prevs = $prevsb = '';
			$prevs4 = $s4;
			$prevs9 = $s9;
		}
	}	
	if ($x1 eq $x2) {
		print " x=$x1" if $x1 ne $prevx;
		$prevx = $x1;
		$prevx1 = $prevx2 = '';
	} else {
		print " x1=$x1" if $x1 ne $prevx1;
		print " x2=$x2" if $x2 ne $prevx2;
		$prevx = '';
		$prevx1 = $x1;
		$prevx2 = $x2;
	}
	print " e123=$e123{$e13}" if $e13 ne $preve13;
	$preve13 = $e13;
	print " e4567=$e47" if $e47 ne $preve47;
	$preve47 = $e47;
}

sub flushbits {
	print " copies=$copies" if $copies > 1;
	$copies = 1;
	if (!$skiplen && $skipbits =~ s/^(1*)//) {
		$idlelen += length($1);
	}
	if ($idlelen < 8) {
		if ($skiplen) {
			$skiplen += $idlelen;
		} else {
			$skipbits = ('1' x $idlelen) . $skipbits;
		}
		$idlelen = 0;
	}
	if ($skiplen || length($skipbits) > $showskiplen) {
		$skiplen += length($skipbits);
		$skipbits = '';
	}
	my $skip = '';
	$skip = "idle($idlelen)" if $idlelen;
	my $l = $skiplen + length($skipbits);
	$skip .= "bits($l)" if $l;
	print "\n skip $skip $skipbits" if $skip ne '';
	$idlelen = 0;
	$skiplen = 0;
	$skipbits = '';
}


__END__
:endofperl
