#!/usr/bin/perl
# generate a division table of four bit precision estimates
use POSIX;
for ($i=0; $i < 16; $i++) {
	my $v = (1 + $i/16) * (1 << 30);
	my $r = 1/$v;
	my ($m, $e) =  &toFLOAT($r);
	printf("\t{ 0x%08x, $e, },\t// $r = 1/$v\n", $m);
}

sub toFLOAT {
	my ($v) = @_;
	my ($m, $e) = POSIX::frexp($v);
	$m *= 1 << 30;
	return (int($m), $e - 30);
}
