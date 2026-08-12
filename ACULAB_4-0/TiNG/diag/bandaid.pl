$fquant = 8000 / 256 / 4;
die "Usage: bandaid lower-limit upper-limit\n" unless @ARGV == 2;
my $lwr = shift;
my $upr = shift;
die "Lower frequency is not below upper frequency!\n" if $lwr >= $upr;
print "Note: actual detection range starts at 250 Hz\n" if $lwr < 250;
my @band_lwr = &fbounds($lwr);
my @band_upr = &fbounds($upr);
print "Reject below $band_lwr[0]\n";
print "Uncertain from $band_lwr[0] to $band_lwr[1]\n";
print "Accept from $band_lwr[1] to $band_upr[0]\n";
print "Uncertain from $band_upr[0] to $band_upr[1]\n";
print "Reject above $band_upr[1]\n";
print "Note: actual detection range finishes at 3406.25 Hz\n" if $upr > 3406.25;

sub fbounds {
	my $f = $_[0];
	my $lwr = int($f / $fquant / 2 - 0.5) * $fquant * 2 + $fquant;
	my $upr = $lwr + $fquant * 2;
	return ($lwr, $upr);
}
