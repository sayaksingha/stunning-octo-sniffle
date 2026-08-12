@states = (
	'CLOSED', 'LISTEN', 'WAIT_COOKIE', 'ESTABLISHED', 'FIN_WAIT', 'CLOSE_WAIT', 'LAST_ACK', 'RST_WAIT', 'TIME_WAIT'
);
%abbr = (
	'CLOSED' => 'x',
	'LISTEN' => 'l',
	'WAIT_COOKIE' => 'w',
	'ESTABLISHED' => 'e',
	'FIN_WAIT' => 'f',
	'CLOSE_WAIT' => 'c',
	'LAST_ACK' => 'a',
	'RST_WAIT' => 'r',
	'TIME_WAIT' => 't',
);

@strans = (
	[ 'CLOSED', 'connect()', 'WAIT_COOKIE', ],
	[ 'WAIT_COOKIE', 'got COOKIE', 'ESTABLISHED', ],
	[ 'CLOSED', 'got SYN+COOKIE', 'ESTABLISHED', ],
	[ 'TIME_WAIT', 'got SYN+COOKIE', 'ESTABLISHED', ],
	[ 'ESTABLISHED', 'process FIN', 'CLOSE_WAIT', ],
	[ 'ESTABLISHED', 'send EOF', 'FIN_WAIT', ],
	[ 'CLOSE_WAIT', 'send EOF', 'LAST_ACK', ],
	[ 'FIN_WAIT', 'process FIN', 'LAST_ACK', ],
	[ 'LAST_ACK', 'all ACKED', 'TIME_WAIT', ],
	#[ 'TIME_WAIT', 'connect()', 'WAIT_COOKIE', ],
	[ 'TIME_WAIT', 'timeout', 'CLOSED', ],
	[ 'RST_WAIT', 'got RST', 'TIME_WAIT', ],
);
print "digraph D {\n";
for $x (@states) {
	for $y (@states) {
		print "s_$abbr{$x}$abbr{$y} [label=\"$x/$y\" ];\n";
	}
}
&showstates('CLOSED', 'CLOSED');
print "}\n";
#&showtrans();

sub nextstates {
	my ($st) = @_;
	my @r = ();
	my $t;
	for $t (@strans) {
		if ($st eq $t->[0]) {
			push(@r, [$t->[1], $t->[2]]);
		}
	}
	if ($st ne 'CLOSED') {
	#	push(@r, ['abort()', 'RST_WAIT']);
	#	push(@r, ['got RST', 'TIME_WAIT']);
	}
	return @r;
}

sub showstates {
	my ($x, $y) = @_;
	($x, $y) = ($y, $x) if $x lt $y;
	my $comb = "$x:$y";
	my $comba = $abbr{$x} . $abbr{$y};
	#return if $x lt $y;
	return if defined($g_had{$comba});
	$g_had{$comba} = 1;
	my $n;
	for $n (&nextstates($x)) {
		next if $n->[0] eq 'connect()';
		my $nx = $n->[1];
		my $ny = $y;
		($nx, $ny) = ($ny, $nx) if $nx lt $ny;
		my $toa = $abbr{$nx} . $abbr{$ny};
		print "s_$comba -> s_$toa [label=\"$n->[0]:\" ];\n";
		$g_trans{$x}{$y}{$nx}{$ny} = 1;
		$g_trans{$comba}{$toa} = 1;
		&showstates($nx, $ny);
	}
	for $n (&nextstates($y)) {
		my $nx = $x;
		my $ny = $n->[1];
		($nx, $ny) = ($ny, $nx) if $nx lt $ny;
		my $toa = $abbr{$nx} . $abbr{$ny};
		print "s_$comba -> s_$toa [label=\":$n->[0]\" ];\n";
		$g_trans{$x}{$y}{$nx}{$ny} = 1;
		$g_trans{$comba}{$toa} = 1;
		&showstates($nx, $ny);
	}
}

sub showtrans {
	print <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-type" content="text/html;charset=UTF-8">
<title>States</title>
</head>
<body>
<h1>States</h1>
<p>
<table border>
<tr><td>
EOF
	my $x;
	for $x (@states) {
		my $xa = $abbr{$x};
		my $y;
		for $y (@states) {
			next if $x lt $y;
			print "<th>$xa$abbr{$y}\n";
		}
	}
	my $x1;
	for $x1 (@states) {
		my $x1a = $abbr{$x1};
		my $y1;
		for $y1 (@states) {
			next if $x1 lt $y1;
			my $from = $x1a . $abbr{$y1};
			print "<tr><th>$from\n";
			my $x2;
			for $x2 (@states) {
			my $x2a = $abbr{$x2};
				my $y2;
				for $y2 (@states) {
					next if $x2 lt $y2;
					my $to = $x2a . $abbr{$y2};
					print "<td>";
					if ($from eq $to) {
						print "";
					} elsif ($g_trans{$from}{$to}) {
						print "*";
					} else {
						print ".";
					}
				}
			}
			print "\n";
		}
	}
}
