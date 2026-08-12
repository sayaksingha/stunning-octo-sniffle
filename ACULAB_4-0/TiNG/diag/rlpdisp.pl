#!/usr/local/bin/perl
# V.110 RLP
# bits are msb first in input
die "Usage: rlpdisp [file]\n" unless $#ARGV <= 0;
if ($#ARGV == -1) {
	*FILE = *STDIN;
} else {
	$file = shift;
	open(FILE, $file) || die "Cannot open $file: $!\n";
}
binmode(FILE);
for ($i=0; $i < 256; $i++) {
	my $byte = $i;
	my $n;
	for ($n=0; $n < 8; $n++, $byte <<= 1) {
		$expbyte[$i] .= $byte & 128 ? "1" : "0";
	}
}
	# create array of the positions of data bits
@dpos = ();
&addrange(9, 7);
&addrange(17, 7);
&addrange(25, 7);
&addrange(33, 7);
&addrange(44, 4);
&addrange(49, 7);
&addrange(57, 7);
&addrange(65, 7);
&addrange(73, 7);
&addrange(89, 7);
&addrange(97, 7);
&addrange(105, 7);
&addrange(113, 7);
&addrange(124, 4);
&addrange(129, 7);
&addrange(137, 7);
&addrange(145, 7);
&addrange(153, 7);
&addrange(169, 7);
&addrange(177, 7);
&addrange(185, 7);
&addrange(193, 7);
&addrange(204, 4);
&addrange(209, 7);
&addrange(217, 7);
&addrange(225, 7);
&addrange(233, 7);
&addrange(249, 7);
&addrange(257, 7);
&addrange(265, 7);
&addrange(273, 7);
&addrange(284, 4);
&addrange(289, 7);
&addrange(297, 7);
&addrange(305, 7);
&addrange(313, 7);
$s = '';
$this = 'xxx';
$copies = 0;
while (!$eof) {
	if (read(FILE, $_, 40)) {	# read a chunk in
		while (s/(.)//) {
			$s .= $expbyte[ord($1)];
		}
	} else {
		$eof = 1;		# no more: deal with whatever is left
	}
	while (substr($s, 0, 320) eq $this) {
		$s = substr($s, 320);
		$copies++;
	}
	while ($s =~ /000000001.......1.......1.......1.......1.00....1.......1.......1.......1.......000000001.......1.......1.......1.......1.01....1.......1.......1.......1.......000000001.......1.......1.......1.......1.10....1.......1.......1.......1.......000000001.......1.......1.......1.......1.11....1.......1.......1.......1......./) {
		$skipbits = $`;
		$this = $&;
		$s = $';
		$l = length($skipbits);
		$skip = "[$l bits]" if $l;
		$skip .= $skipbits if $l < 100;
		print " copies=$copies" if $copies > 1;
		print "\nskipbits $skip" if $skipbits;
		$copies = 1;
		&printframe();
	}
}
print " copies=$copies" if $copies > 1;
print " junkbits ", length($s), "\n" if $s;

sub printframe {
	my $nbits = 0;
	my $nbytes = 0;
	my $byte = 0;
	my $crc = 0xffffff00;
	my $pos;
	print "\n";
	for $pos (@dpos) {
		if (substr($this, $pos, 1) eq '1') {
			$byte |= 1 << $nbits;
			$crc ^= 0x80000000;
		}
		if ($crc & 0x80000000) {
			$crc ^= 0xbba1b500 >> 1;
		}
		$crc <<= 1;
		if (++$nbits == 8) {
			printf("%02x", $byte);
			$byte = 0;
			$nbits = 0;
			print " " if (++$nbytes & 3) == 2 || $nbytes == 27;
		}
	}
	print $crc == 0x6d893000 ? "+ " : " -";
	print " ", substr($this, 41, 1), substr($this, 121, 1), substr($this, 201, 1), substr($this, 281, 1);
}

sub addrange {
	my ($pos, $len) = @_;
	for (; $len; $len--) {
		push(@dpos, $pos++);
	}
}
