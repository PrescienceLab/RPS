#!/usr/bin/perl -w
#Peter Dinda
#Jason Skicewicz
#Luka Spoljaric

#Libraries
use Getopt::Long;
use Time::HiRes qw(usleep gettimeofday);
use FileHandle;
#End of Libraries

STDOUT->autoflush(1);
STDERR->autoflush(1);

#Arguments passed by user
$Getopt::Long::ignorecase = 0;

$showtimestamp=0;
$width=0;

%optcl = ('rate=f'   , \$rate,
	  'max=i'    , \$loopcnt,
          'period=i' , \$period,
          'h', \$help,
          'help', \$help,
	  'ignorepids',\$ignorepids,
	  'timestamp',\$showtimestamp,
	  'width=i',\$width);

GetOptions(%optcl);

if ($help) {
    print "usage: proc_spit.pl [--rate=Hz] [--max=numsamples] [--period=usec] [--ignorepids] [--timestamp] [--width=numcols] [--help]\n";
    exit;
}

if (!($rate )){
    if ($period){
	$rate = (1.0/($period))*1e6;
    }	# usleep($period_usec);

    else {
	$rate = 1;
    }
}

if (!($loopcnt )){
    $loopcnt = 9^999999999999;
}

if (!($period )){
    if ($rate){
	$period = (1.0/($rate))*1e6;
    }
    else{
    $period = 9^999999999999;
    }
}

for ($i = 0; $i < $loopcnt; $i++){
    $t = gettimeofday(); 
    print "$t\t" if $showtimestamp;
    $col=0;
    take_snapshot(); 
    if ($width>0) {
	for ($j=$col;$j<$width;$j++) {
	    print "\t0";
	}
    }
    print "\n";
    usleep($period);
}

sub take_snapshot
{

    # Directory in which to start
    my @dirs = ('/proc');
    my $count = 0;
    
    for $root (@dirs) {
	chdir($root);
	$root = `pwd`;
	chomp($root);
	# Add trailing slash if missing
	$root .= '/' unless ( substr($root, -1, 1) eq '/' );
	next if (!(-d $root));
	process_dir($root);
    }
}

sub process_dir 
{  # ($dirname, $level)
    my $dirname = $_[0];
    my $fullname;
    my $entry;

    if ( !opendir(DIR, $dirname) ) 
    {
	#we don't really want to print this- it'd mess our file up
	#so just return
	#print "!!Can't open directory $dirname: $!\n";
	return;
    }

    my @entries = sort readdir(DIR);
    closedir(DIR);

    foreach $entry (@entries) {
	next if ($ignorepids && $dirname eq '/proc/' && $entry=~/^\d+$/);
	next if ( $entry eq '.'  ||  $entry eq '..' ); #skip through the dots
	$fullname = $dirname . $entry; #our current pathname
	next if (!(-r $fullname)); #if u can't read the file, skip it
	if (-T $fullname)
	{#if it's a textfile
	     open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
	     while(<TEXTFILE>){
		foreach $word( split )
		{
		    if ($word =~ /^\d+\.*\d*$/)
		    {#is the word a number (of any sort)?
			 if ($width==0 || $col<$width) {
			     my $num=$word+0;
			     print "\t", $num;
			     $col++;
			 } else {
			     return;
			 }
		     }
		}  
	    }
	 }  
	if ( -d $entry ) 
	{#if it's a directory, recurse it
	     process_dir(($fullname . '/'));
	 }
	
	
	
    }
  END:
}
