$TiNGTARGET = $ENV{"TiNGTARGET"};
$serialno = "../diag/serialno/gen-$TiNGTARGET/serialno";
$conffile = "/usr/kernel/drv/ACU_.conf";
	# This script handles the necessary changes when a card is
	# replaced with another identical one or when a card is
	# moved from one slot to another (which is the same as
	# replacing a card with itself).

print STDERR "Aculab cards: check for a replaced card\n";
	# run script, catching any errors
my $ret = eval { &main() };
	# that's it - now handle any errors
if ($@) {
		# an error!
	print STDERR "\t$@";
	$ret = 1;
}
print STDERR "Aculab card check finished\n";
exit $ret;


	# Find all the cards currently in the machine. This involves a
	# recursive search of /devices, so this function is called with
	# the parameter specifying where to start. It returns a hash
	# which maps card path to serial number where path is the device
	# name prefix (under /devices).
sub curcards {
	my $dir = $_[0];	# where to start searching
	my @subdirs = ();	# subdirectories to search
	my %curcards = ();	# cards found so far
	local(*D);
	opendir(D, $dir) || die "Cannot open directory: $dir: $!\n";
 DEVICE:
	for (;;) {
		my $file = readdir(D);
		last unless defined($file);
		next if $file eq '.' || $file eq '..';
		my $path = "$dir/$file";
		if (-d $path) {
				# subdirectory - save for later
			push(@subdirs, $path)
		} elsif ($file =~ /:TiNG$/) {
			local(*S);
				# check if device is really there -
				# it may merely be a ghost of a
				# removed card
			if (!open(S, $path)) {
				my $err = $!;
				if ($err eq 'No such device or address') {
						# not really there
					next DEVICE;
				}
					# unexpected error
				die "Cannot open device: $path: $err\n";
			}
				# card is really present - read serial number
			open(S, "$serialno -c $path|")
				|| die "Cannot run $serialno -c $path: $!\n";
			my $s = <S>;	# first line only
			close(S) || die "Error closing pipe from $serialno -c $path: $!\n";
			if ($s =~ /.\n/m) {	# got something
				chop($s);
				$path =~ s/:TiNG$//;
				$curcards{$path} = $s;
			} else {
				die "No output from '$serialno -c $path'\n";
			}
		}
	}
	closedir(D);
		# now scan the subdirectories found. We defer
		# scanning them to avoid having many directories open at
		# the same time. This way we only have at most one
		# directory open at a time.
	my $d;
	for $d (@subdirs) {
		my %more = &curcards($d);
		@curcards{keys(%more)} = values(%more);
	}
	return %curcards;
}

	# Find all the cards which used to be in the machine.
	# This simply means looking for /dev/aculab/Prosody_*.
	# The value returned is a hash mapping the device name prefix
	# (under /devices) to an array: [serialno, callno, switchno]
sub oldcards {
	my %oldcard = ();
	local(*D);
	opendir(D, "/dev/aculab")
		|| die "Cannot open directory /dev/aculab: $!\n";
	for (;;) {
		my $file = readdir(D);
		last unless defined($file);
		if ($file =~ /^Prosody_(.*)/) {
			my $ser = $1;
			my $dev = readlink("/dev/aculab/$file")
				|| die "Cannot read link /dev/aculab/$file: $!\n";
			$dev =~ s/:TiNG//;
			$dev =~ s#^\.\./\.\./##;	# make absolute
			$oldcard{$dev}[0] = $ser;
		} elsif ($file =~ /^ACUc([0-9]+)/) {
			my $call = $1;
			my $dev = readlink("/dev/aculab/$file")
				|| die "Cannot read link /dev/aculab/$file: $!\n";
			$dev =~ s/:ACUc//;
			$dev =~ s#^\.\./\.\./##;	# make absolute
			$oldcard{$dev}[1] = $call;
		} elsif ($file =~ /^ACUs([0-9]+)/) {
			my $switch = $1;
			my $dev = readlink("/dev/aculab/$file")
				|| die "Cannot read link /dev/aculab/$file: $!\n";
			$dev =~ s/:ACUs//;
			$dev =~ s#^\.\./\.\./##;	# make absolute
			$oldcard{$dev}[2] = $switch;
		}
	}
		# now check what we got
	my $dev;
	my $val;
	while (($dev, $val) = each(%oldcard)) {
		defined($val->[0])
			|| die "Bad config: no Prosody device file for $dev\n";
		defined($val->[1])
			|| die "Bad config: no ACUc device file for $dev\n";
		defined($val->[2])
			|| die "Bad config: no ACUs device file for $dev\n";
	}
	return %oldcard;
}

# Fix driver configuration file when a card is replaced.
# replace "card_$old" with "card_$new" in /usr/kernel/drv/ACU_.conf
sub changeconf {
	my ($old, $new) = @_;
	local(*CONF);
	open(CONF, $conffile)
		|| die "Cannot open driver conf file: $conffile: $!\n";
		# maybe do s/(.*card_)$old(.*)/$1$new$2/ on each line?
		# This just reads it all in as one lump, so it
		# changes comments also.
	local($/) = undef;
	my $conf = <CONF>;
	close(CONF);
	$conf =~ s/card_$old/card_$new/g;
		# now save the .conf file somwhere
	$ver = '0';
	$ver++ while -e "$conffile.$ver";
	rename($conffile, "$conffile.$ver")
		|| die "Cannot rename old driver conf file ($conffile) as $conffile.$ver: $!\n";
		# old one saved - replace the file
	open(CONF, ">$conffile")
		|| die "Cannot create driver conf file: $conffile: $!\n";
	print CONF $conf
		|| die "Cannot write to driver conf file: $conffile: $!\n";
	close(CONF);
}

# replace a symbolic link
sub changedevlink {
	my ($d_old, $d_new, $dev) = @_;
	unlink($d_old) || die "Cannot remove old device file: $d_old: $!\n";
	symlink($dev, $d_new) || die "Cannot symlink $d_new to $dev: $!\n";
}

# Fix device special files in /dev/aculab when a card is replaced
# note that the links are made relative ("/devices..." -> "../../devices...")
# because if they are accessed over NFS we don't want them to refer
# to local files e.g. "/net/foo/dev/aculab/Prosody_123456" would
# refer to "/devices/..." if it were an absolute symbolic link, but
# to "/net/foo/dev/aculab/../../devices..." if it is relative. The
# relative version simplifies to "/net/foo/devices..." which is the
# correct place (not that you can use it since you can't use device
# files over NFS - but it's better than getting the wrong file).
sub changedev {
	my ($oldserial, $olddev, $newserial, $newdev, $call, $switch) = @_;
	&changedevlink("/dev/aculab/Prosody_$oldserial",
		"/dev/aculab/Prosody_$newserial",
		"../..$newdev:TiNG");
	&changedevlink("/dev/aculab/ACUc$call",
		"/dev/aculab/ACUc$call",
		"../..$newdev:ACUc");
	&changedevlink("/dev/aculab/ACUs$switch",
		"/dev/aculab/ACUs$switch",
		"../..$newdev:ACUs");
}

# Unload driver and reload it.
# This makes it use the new config file if it has changed.
sub reload_driver {
	local(*P);
	open(P, "modinfo|") || die "Cannot run 'modinfo': $!\n";
	while (<P>) {
		if (/^([0-9]+) .*Aculab Prosody subframe driver/) {
			my $modid = $1;
			system("modunload -i $modid");
			close(P);
			return 0;
		}
	}
	close(P);
	print STDERR "Warning: cannot find loaded driver\n";
	return 1;
}

# Replace configuration of one card with another (which may be the
# same card in a different slot).
sub swapcard {
	my ($oldserial, $olddev, $newserial, $newdev, $call, $switch) = @_;
	&changeconf($oldserial, $newserial);
	&changedev($oldserial, $olddev, $newserial, $newdev, $call, $switch);
	&reload_driver() && die "Reboot required to make driver see changes\n";
}

	# Find the cards which have changed and perform appropriate fix.
sub main {
	my %curcards = &curcards("/devices");
	my %oldcards = &oldcards();
		# ignore any cards which have not changed
	my $dev;
	for $dev (keys(%oldcards)) {
		if ($oldcards{$dev}[0] eq $curcards{$dev}) {
			delete($oldcards{$dev});
			delete($curcards{$dev});
		}
	}
		# here 'oldcards' describes cards which are no longer
		# in the system and 'curcards' describes new cards which
		# have appeared
	my @oldkeys = keys(%oldcards);
	my @curkeys = keys(%curcards);
	if (!@oldkeys && !@curkeys) {	# no change
		print STDERR "\tNo change\n";
		return 0;
	}
		# check if the change is appropriate
	if (@curkeys > @oldkeys) {
		print STDERR "\tToo many new cards have appeared\n";
	} elsif (@curkeys < @oldkeys) {
		print STDERR "\tToo many cards have been removed\n";
	} elsif (@curkeys != 1) {
		print STDERR "\tMore than one card has been replaced\n";
	} else {
			# yes, it's ok. Fix the system configuration.
		my $olddev = $oldkeys[0];
		my $curdev = $curkeys[0];
		my ($oldserial, $call, $switch) = @{$oldcards{$olddev}};
		my $curserial = $curcards{$curdev};
		&swapcard($oldserial, $olddev, $curserial, $curdev, $call, $switch);
		print STDERR "\treplaced card $oldserial with $curserial\n";
		return 0;
	}
		# for the various failures - list the changes detected
	for $dev (keys(%curcards)) {
		print STDERR "\tNew card: $curcards{$_}\n";
	}
	for $dev (keys(%oldcards)) {
		print STDERR "\tRemoved card: ", $oldcards{$dev}[0],
			"  call:", $oldcards{$dev}[1],
			"  switch:", $oldcards{$dev}[2], "\n";
	}
	return 1;
}
