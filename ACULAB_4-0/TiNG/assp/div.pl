srand(1);
$n = 557754;
$d = 2404;
$s = 7;
for (;;) {
	my @r = &div($n, $d, $s);
	my $rf = &fscale($n / $d, $s);
	my $r = &fscale($r[0], $r[1]);
	my $e = ($r - $rf) / $rf;
	print "n=$n, d=$d, s=$s, /:$rf div:$r, e=$e\n" if $e > 0.0001 || $e < -0.0001;
	last;
	my $v = rand(3);
	if ($v < 1) {
		$n = int(rand(65536 * 256));
	} elsif ($v < 2) {
		$d = int(rand(65536 * 256));
	} else {
		$s = int(rand(32)) - 16;
	}
}

sub showdiv {
	my ($n, $d, $s) = @_;
	my @r = &div($n, $d, $s);
	print "$n << $s / $d = $r[0] << $r[1]\n";
}


sub fscale {
	my ($v, $s) = @_;
	while ($s > 0) {
		$v *= 2.0;
		$s--;
	}
	while ($s < 0) {
		$v /= 2.0;
		$s++;
	}
	return $v;
}

sub div {
	my ($n, $d, $s) = @_;
	$d <<= 1;
	$n <<= 1;
	my $q = 0;
	 print "$q\t$n\t$s\t$d\n";
	while ($n >= $d) {
		$n >>= 1;
		$s++;
		 print "$q\t$n\t$s\t$d\n";
	}
	for (; $n;) {
		 print "$q\t$n\t$s\t$d\n";
		last if $q >= (1 << 30);
		$n <<= 1;
		$q <<= 1;
		$s--;
		if ($n >= $d) {
			$n -= $d;
			$q++;
		}
	}
	return ($q, $s);
}
