#!/usr/bin/perl -w
#Peter Dinda
#Jason Skicewicz
#Luka Spoljaric

#Libraries
use Getopt::Long;
use Time::HiRes qw(usleep gettimeofday);
#End of Libraries

#Arguments passed by user
$Getopt::Long::ignorecase = 0;
%optcl = ('name=s'   , \@names,
	  'rate=f'   , \$rate,
	  'max=i'    , \$max,
          'period=i' , \$period,
	  'timestamp', \$timestamp,
	  'help', \$help,
	  'h', \$help);
GetOptions(%optcl);

if ($help || !(@names)) { 
    print STDERR "usage: proc_ns.pl [--rate=Hz] [--period=usec] [--max=number_of_samples] [--timestamp] --name=name_list\n";
    exit;
}



if (!($rate ))
{
    if ($period){
	$rate = (1.0/($period))*1e6;
    }	# usleep($period_usec);

    else {
	$rate = 1;
    }
}

if (!($max ))
{
    $max = 9^999999999999;
}

if (!($period ))
{
    if ($rate){
	$period = (1.0/($rate))*1e6;
    }
    else{
    $period = 9^999999999999;
    }
}

if ($timestamp)
{$t = gettimeofday(); print  "$t\n";}
for ($i = 0; $i < $max; $i++){
    #$t = gettimeofday(); print  "$t\n";
    mapProc(@names);
    usleep($period);
}

sub mapProc
{#@names
     @mynames = @_;
     
#temporary varible for the current number
     $result = "\nERROR! Could not find requested namespace!\n";
#all temp results will be stored here
     my @results;
     my $counter = 0; #will count the numner of results

#We split are namespace into separate fields so as to map the /proc
     foreach $name (@mynames)
     {
	 $name =~ s/^\/(.*)/$1/; # elliminate initial /
	 ($primary, $secondary, $terciary, $cetvrti,$peti) = split /\//, $name;
	 if (!($primary)) {die "ERROR!!Could not parse your namespace!";}
	 if (!($secondary)) {$secondary = 0;}
	 if (!($terciary)) {$terciary = 0;}
	 if (!($cetvrti)) {$cetvrti = 0;}
	 if (!($peti)) {$peti = 0;}
	
	 $directory = '/proc/';
	 $filePath = 'nothing';
	 
#We now map the /proc
	 if ($primary eq "process")
	 {
	     unless (($secondary =~ /\d+/) || ($secondary eq "self"))
	     { die "ERROR!!Process ID is self or numeric ONLY!";}
	     $directory = $directory . $secondary;
	     #add trailing slash if there isn't any
	     $directory .= '/' unless ( substr($directory, -1, 1) eq '/' );
	     #We have now set up our directory path
	     
	     if ($terciary eq "num_mem_areas")
	     {
		 $filePath = $directory . 'maps';
		 $result = print_num_areas ($filePath);
	     }
	     
	     elsif ($terciary eq "mem_area")
	     {
		 $filePath = $directory . 'maps';
		 
		 unless ($cetvrti =~ /\d+/)
		 { die "ERROR!!Area number is numeric only";}
		 
		 if ($peti eq "address")
		 {
		     $result = print_mem_address($filePath, $cetvrti);
		 }
	     }
	     
	     elsif ($terciary eq "stat")
	     {
		 $filePath = $directory . 'stat';
		 
		 if ($cetvrti eq "pid")
		 {
		     $result = process_file ($filePath,0);
		 }
		 
		 elsif ($cetvrti eq "ppid")
		 {
		     $result = process_file ($filePath,3);
		 }
		 
		 elsif ($cetvrti eq "pgrp")
		 {
		     $result = process_file ($filePath,4);
		 }
		 
		 elsif ($cetvrti eq "session")
		 {
		     $result = process_file ($filePath,5);
		 }
		 
		 elsif ($cetvrti eq "tty")
		 {
		     $result = process_file ($filePath,6);
		 }
		 
		 elsif ($cetvrti eq "tpgid")
		 {
		     $result = process_file ($filePath,7);
		 }
		 
		 elsif ($cetvrti eq "flags")
		 {
		     $result = process_file ($filePath,8);
		 }
		 
		 elsif ($cetvrti eq "minflt")
		 {
		     $result = process_file ($filePath,9);
		 }
		 
		 elsif ($cetvrti eq "cminflt")
		 {
		     $result = process_file ($filePath,10);
		 }
		 
		 elsif ($cetvrti eq "majflt")
		 {
		     $result = process_file ($filePath,11);
		 }
		 
		 elsif ($cetvrti eq "cmajflt")
		 {
		     $result = process_file ($filePath,12);
		 }
		 
		 elsif ($cetvrti eq "utime")
		 {
		     $result = process_file ($filePath,13);
		 }
		 
		 elsif ($cetvrti eq "stime")
		 {
		     $result = process_file ($filePath,14);
		 }
		 
		 elsif ($cetvrti eq "cutime")
		 {
		     $result = process_file ($filePath,15);
		 }
		 
		 elsif ($cetvrti eq "cstime")
		 {
		     $result = process_file ($filePath,16);
		 }
		 
		 elsif ($cetvrti eq "counter")
		 {
		     $result = process_file ($filePath,17);
		 }
		 
		 elsif ($cetvrti eq "priority")
		 {
		     $result = process_file ($filePath,18);
		 }
		 
		 elsif ($cetvrti eq "timeout")
		 {
		     $result = process_file ($filePath,19);
		 }
		 
		 elsif ($cetvrti eq "itrealvalue")
		 {
		     $result = process_file ($filePath,20);
		 }
		 
		 elsif ($cetvrti eq "starttime")
		 {
		     $result = process_file ($filePath,21);
		 }
		 
		 elsif ($cetvrti eq "vsize")
		 {
		     $result = process_file ($filePath,22);
		 }
		 
		 elsif ($cetvrti eq "rss")
		 {
		     $result = process_file ($filePath,23);
		 }
		 
		 elsif ($cetvrti eq "rlim")
		 {
		     $result = process_file ($filePath,24);
		 }
		 
		 elsif ($cetvrti eq "startcode")
		 {
		     $result = process_file ($filePath,25);
		 }
		 
		 elsif ($cetvrti eq "endcode")
		 {
		     $result = process_file ($filePath,26);
		 }
		 
		 elsif ($cetvrti eq "startstack")
		 {
		     $result = process_file ($filePath,27);
		 }
		 
		 elsif ($cetvrti eq "kstkesp")
		 {
		     $result = process_file ($filePath,28);
		 }
		 
		 elsif ($cetvrti eq "kstkeip")
		 {
		     $result = process_file ($filePath,29);
		 }
		 
		 elsif ($cetvrti eq "signal")
		 {
		     $result = process_file ($filePath,30);
		 }
		 
		 elsif ($cetvrti eq "blocked")
		 {
		     $result = process_file ($filePath,31);
		 }
		 
		 elsif ($cetvrti eq "sigignore")
		 {
		     $result = process_file ($filePath,32);
		 }
		 
		 elsif ($cetvrti eq "sigcatch")
		 {
		     $result = process_file ($filePath,33);
		 }
		 
		 elsif ($cetvrti eq "wchan")
		 {
		     $result = process_file ($filePath,34);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($terciary eq "statm")
	     {
		 $filePath = $directory . $terciary;
		 unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 8))
		 {die "\nERROR! STATM values are numeric, 1-7 ONLY!\n";}
		 $offset = $cetvrti - 1; #for the first/zeroth entry
		 $result = process_file ($filePath, $offset);
	     }
	     elsif ($terciary eq "status")#does not work
	     {
		 $filePath = $directory . $terciary;
		 $initial_offset = 1;
		 $searchWord = $cetvrti . ':';
		 $line_offset = calculate_offset ($filePath, $searchWord);
		 if (!($cetvrti))#not all values are multivalued
		 {$cetvrti = 1;}
		 $additional_offset = $cetvrti - 1;
		 $total_offset = $initial_offset + $line_offset + $additional_offset;
		 $result = process_file ($filePath, $offset);
	     }


	 }
	 
	 elsif ($primary eq "cpu")
	 {
	     #in this case we use cpuinfo ASCII file
	     #since there can be more than one processor
	     #we'll use a below- defined function, which
	     #will look at every line for the word processor
	     #and search untill it finds a dsired processor
	     #then extract what we want
	     unless ($secondary =~ /\d+/)#this tells us the processor # (from 1 on)
	     { die "ERROR!!Processor ID is numeric ONLY!";}
	     $filePath = $directory . 'cpuinfo';
	     $offsetVar = 'processor';#this is what the function will look for
		 
		 #the following if statements merely convert "_" to space
		 if ($terciary eq "family")
		 {
		     $terciary = 'cpu family';
		 }
	     elsif (($terciary eq "MHz") || ($terciary eq "MHZ") || ($terciary eq "mhz"))
	     {
		 $terciary = 'cpu MHz';
	     }
	     elsif ($terciary eq "cache_size")
	     {
		 $terciary = 'cache size';
	     }
	     elsif ($terciary eq "cpuid_level")
	     {
		 $terciary = 'cpuid level';
	     }
	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     #send the data to the function
	     $result = process_offset_file ($filePath, $secondary, $terciary, $offsetVar);
	 }
	 
	 elsif ($primary eq "time")
	 {
	     $filePath = $directory . 'driver/' . 'rtc';
	     if ($secondary eq "time")
	     {
		 $secondary = 'rtc_time';
	     }
	     elsif ($secondary eq "date")
	     {
		 $secondary = 'rtc_date';
	     }
	     elsif ($secondary eq "epoch")
	     {
		 $secondary = 'rtc_epoch';
	     }
	     elsif ($secondary eq "freq")
	     {
		 $secondary = 'periodic_freq';
	     }
	     elsif ($secondary eq "alarm")
	     {
		 $secondary = 'alarm';
	     }
	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     $result = process_normal_file ($filePath, $secondary); 
	 }
	 
	 elsif ($primary eq "drive")
	 {
	     if ($secondary eq "ide")
	     {
		 $filePath = $directory . 'ide/';
		 
		 if ($terciary eq "floppy")
		 {
		     if ($cetvrti eq "version")
		     {
			 $filePath = $filePath . 'drivers';
			 $cetvrti = 'ide-floppy version';
			 $result =  process_normal_file ($filePath, $cetvrti);
		     }
		     else
		     {
			 $filePath = $filePath . 'hdd/';
			 if ($cetvrti eq "capacity")
			 {
			     $filePath = $filePath . 'capacity';
			     $result = process_single_line_file ($filePath);
			 }
			 elsif ($cetvrti eq "geometry")
			 {
			     $filePath = $filePath . 'geometry';
			     $result = process_normal_file ($filePath, $peti);
			 }
			 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		     }
		     
		 }
		 
		 elsif ($terciary eq "disk")
		 {
		     if ($cetvrti eq "version")
		     {
			 $filePath = $filePath . 'drivers';
			 $cetvrti = 'ide-disk version';
			 $result = process_normal_file ($filePath, $cetvrti);
		     }
		     else
		     {
			 $filePath = $filePath . 'hda/';
			 if (($cetvrti eq "cache") || ($cetvrti eq "capacity"))
			 {
			     $filePath = $filePath . $cetvrti;
			     $result = process_single_line_file ($filePath, $peti);
			 }
			 elsif ($cetvrti eq "geometry")
			 {
			     $filePath = $filePath . 'geometry';
			     $result = process_normal_file ($filePath, $peti);
			 }
			 else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		     }
		 }
	     }
	     elsif ($secondary eq "scsi")#doesn't work
	     {
		 $filePath = $directory . 'scsi/';
		 
		 if ($terciary eq "aic7xxx")
		 { 
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0))
		     {die "\nSCSI drives are numeric, non_zero only\n";}
		     $filePath = $filePath . 'aic7xxx/';
		     $terciary--; #to adjust for the first/zeroth element
		     $filePath = $filePath . $cetvrti;
		     
		     if ($peti eq "driver_version")
		     {
			 $result =  process_normal_file ($filePath, 'Adaptec AIC7xxx driver version');
		     }
		     elsif ($peti eq "ultra_enable_flags")
		     {
			 $result =  process_normal_file ($filePath, 'Ultra Enable Flags');
		     }
		 }
	     }
	       
	     
	 }
	 
	 elsif ($primary eq "ports")
	 {
	     $filePath = $directory . 'ioports';
	     unless ($secondary =~ /\d+/)
	     {die "\n!!ERROR!! Port # is numeric value ONLY!!\n";}
		 $result = print_mem_address($filePath, $secondary);
	 }
	 
	 elsif ($primary eq "kernel")
	 {
	     if ($secondary eq "symbol")
	     {
	$filePath = $directory . 'ksyms';
	unless ($terciary =~ /\d+/)
	{die "\n!!ERROR!! Port # is numeric value ONLY!!\n";}
	    $result = print_mem_address($filePath, $terciary);
    }
	 }
	 
	 elsif ($primary eq "loadavg")
	 {
	     unless ($secondary =~ /\d+/)
	     {die "\n!!ERROR!! Port # is numeric value ONLY!!\n";}
		 $filePath = $directory . 'loadavg';
	     $result = process_file ($filePath, ($secondary - 1));
	 }
	 
	 elsif ($primary eq "mem")
	 {
	     $filePath = $directory . 'meminfo';
	     
	     if ($secondary eq "physical")
	     {
		 if ($terciary eq "total")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 7);
		     }
		     else
		     {
			 $result = process_normal_file ($filePath, 'MemTotal');
		     }
		 }
		 elsif ($terciary eq "used")
		 {
		     unless ($cetvrti eq "bytes")
		     {die "\nWARNING: Currently only able to output used mem in bytes.\n";}
		     $result = process_file ($filePath, 8);
		 }
		 elsif ($terciary eq "free")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 9);
		     }
		     else
		     {
			 $result = process_normal_file ($filePath, 'MemFree');
		     }
		 }
		 elsif ($terciary eq "shared")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 10);
		     }
		     else
		     {
			 $result = process_normal_file ($filePath, 'MemShared');
		     }
		 }
		 elsif ($terciary eq "buffers")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 11);
		     }
		     else 
		     {
			 $result = process_normal_file ($filePath, 'Buffers');
		     }
		 }
		 elsif ($terciary eq "cached")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 12);
		     }
		     else 
		     {
			 $result = process_normal_file ($filePath, 'Cached');
		     }
		 }
		 elsif ($terciary eq "active")
		 {
		     if ($cetvrti eq "bytes")
		     {die "\nWARNING: Currently only able to output active physical mem in kB.\n";}
		     $result = process_normal_file($filePath, 'Active');
		 }
		 elsif ($terciary eq "inactive")
		 {
		     if ($cetvrti eq "dirty")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output inactive dirty mem in kB.\n";}
			 $result = process_normal_file($filePath, 'Inact_dirty');
		     }
		     elsif ($cetvrti eq "clean")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output inactive clean mem in kB.\n";}
			 $result = process_normal_file($filePath, 'Inact_clean');
		     }
		     elsif ($cetvrti eq "target")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output inactive target mem in kB.\n";}
			 $result = process_normal_file($filePath, 'Inact_target');
		     }
		     else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 }
		 elsif ($terciary eq "high")
		 {
		     if ($cetvrti eq "total")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output total high mem in kB.\n";}
			 $result = process_normal_file($filePath, 'HighTotal');
		     }
		     elsif ($cetvrti eq "free")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output free high mem in kB.\n";}
			 $result = process_normal_file($filePath, 'HighFree');
		     }
		     else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 }
		 elsif ($terciary eq "low")
		 {
		     if ($cetvrti eq "total")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output total low mem in kB.\n";}
			 $result = process_normal_file($filePath, 'LowTotal');
		     }
		     elsif ($cetvrti eq "free")
		     {
			 if ($peti eq "bytes")
			 {die "\nWARNING: Currently only able to output free low mem in kB.\n";}
			 $result = process_normal_file($filePath, 'LowFree');
		     }
		     else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 }
		 else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "swap")
	     {
		 if ($terciary eq "total")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 14);
		     }
		     else
		     {
			 $result = process_normal_file ($filePath, 'SwapTotal');
		     }
		 }
		 elsif ($terciary eq "used")
		 {
		     unless ($cetvrti eq "bytes")
		     {die "\nWARNING: Currently only able to output used mem in bytes.\n";}
		     $result = process_file ($filePath, 15);
		 }
		 elsif ($terciary eq "free")
		 {
		     if ($cetvrti eq "bytes")
		     {
			 $result = process_file ($filePath, 16);
		     }
		     else 
		     {
			 $result = process_normal_file ($filePath, 'SwapFree');
		     }
		 }
		 elsif ($terciary eq "cached")
		 {
		     if ($cetvrti eq "bytes")
		     {die "\nWARNING: Currently only able to output cached swap mem in kB.\n";}
		     $result = process_normal_file ($filePath, 'SwapCached');
		 }
		 elsif ($terciary eq "no_pages")
		 {
		     $result = process_normal_file ($filePath, 'NrSwapPages');
		 }
		 else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	 }
	 
	 elsif ($primary eq "net")
	 {
	     $filePath = $directory . 'net/';
	     if ($secondary eq "arp")
	     {
		 $filePath = $filePath . 'arp';
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nERROR! ARP entries are numeric and non-zero only!\n";}
		 $terciary = $terciary - 1; #to adjust for the zeroth/first entry
		 $initial_offset = 9;
		 if ($cetvrti eq "IP_address")
		 {
		     $initial_offset = $initial_offset + 6 * $terciary;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "hw_type")
		 {
		     $initial_offset = $initial_offset + 6 * $terciary + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "flags")
		 {
		     $initial_offset = $initial_offset + 6 * $terciary + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "hw_address")
		 {
		     $initial_offset = $initial_offset + 6 * $terciary + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 else { print "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "dev")
	     {
		 $filePath = $filePath . 'dev';
		 unless ((($terciary eq "lo") || ($terciary eq "eth0")) && ($cetvrti eq "receive") || ($cetvrti eq "transmit")) 
		 {die "\nERROR! DEV entries wrongly specified!\n";}
		 my $multiplier = 0;  #for line accessing, lo ->0; eth0->1
		 my $added_offset = 0; #for receive/send column accessing
		 if ($terciary eq "eth0")
		 {$multiplier = 1;}
		 if ($cetvrti eq "transmit")
		 {$added_offset = 8;}
		 $initial_offset = 20;
		 if ($peti eq "bytes")
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($peti eq "packets")
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 1;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($peti eq "errs")
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 2;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($peti eq "drop")
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 3;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($peti eq "fifo")
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 4;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "frame") && ($cetvrti eq "receive"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 5;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "colls") && ($cetvrti eq "transmit"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 5;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "compressed") && ($cetvrti eq "receive"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 6;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "carrier") && ($cetvrti eq "transmit"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 6;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "compressed") && ($cetvrti eq "transmit"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 7;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "multicast") && ($cetvrti eq "receive"))
		 {
		     $initial_offset = $initial_offset + 16 * $multiplier + $added_offset + 7;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "route")
	     {
		 $filePath = $filePath . 'route';
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nERROR! ROUTE entries are numeric and non-zero only!\n";}
		 $terciary = $terciary - 1; #to adjust for the zeroth/first entry
		 $initial_offset = 12; #to skip through descriptors
		 if ($cetvrti eq "destination")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "gateway")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "flags")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "refcnt")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "use")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 4;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "metric")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 5;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "mask")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 6;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "mtu")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 7;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "window")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 8;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "irtt")
		 {
		     $initial_offset = $initial_offset + 11 * $terciary + 9;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "snmp")
	     {
		 $filePath = $filePath . 'snmp';
		 $initial_offset = 21; #skip beggining descriptors 
		 $jump_offset = 0; #skip through numers and descriptors
		 
		 if ($terciary eq "icmp")
		 {
		     $jump_offset = 47; #skip through ip data
		 }
		 
		 if (($terciary eq "icmp") && ($cetvrti eq "out"))
		 {
		     $jump_offset = $jump_offset + 13;#jump through the 'in' date
			 #and save us a some programming
		     }
		 
		 elsif ($terciary eq "tcp")
		 {
		     $jump_offset = 90; #skip through ip and icmp data
		 }
		 
		 elsif ($terciary eq "udp")
		 {
		     $jump_offset = 110;
		 }
		 
		 if (($cetvrti eq "forwarding") && ($terciary eq "ip"))
		 {
		     $result = process_file ($filePath, $initial_offset);
		 }
		 
		 elsif (($cetvrti eq "default_ttl") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_receives") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_hdr_errors") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_addr_errors") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 4;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "forw_datagrams") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 5;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_unknown_protos") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 6;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_discards") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 7;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_delivers") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 8;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_requests") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 9;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_discards") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 10;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_no_routes") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 11;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "reasm_timeout") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 12;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "reasm_reqds") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 13;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "reasm_oks") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 14;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "reasm_fails") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 15;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "frag_oks") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 16;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "frag_fails") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 17;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "frag_creates") && ($terciary eq "ip"))
		 {
		     $initial_offset = $initial_offset + 18;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 
		 
		 elsif (($peti eq "msgs") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "errors") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "dest_unreachs") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "time_excds") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "parm_probs") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 4;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "src_quenchs") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 5;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "redirects") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 6;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "echos") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 7;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "echo_reps") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 8;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "timestamps") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 9;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "timestamp_reps") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 10;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "addr_masks") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 11;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($peti eq "addr_mask_reps") && ($terciary eq "icmp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 12;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 
		 elsif (($cetvrti eq "rto_algorithm") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "rto_min") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "rto_max") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "max_conn") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "active_opens") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 4;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "passive_opens") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 5;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "attempt_fails") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 6;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "estab_resets") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 7;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "curr_estab") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 8;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_segs") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 9;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_segs") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 10;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "retrans_segs") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 11;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_errs") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 12;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_rsts") && ($terciary eq "tcp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 13;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 
		 elsif (($cetvrti eq "in_datagrams") && ($terciary eq "udp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "no_ports") && ($terciary eq "udp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "in_errors") && ($terciary eq "udp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif (($cetvrti eq "out_datagrams") && ($terciary eq "udp"))
		 {
		     $initial_offset = $initial_offset + $jump_offset + 3;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "sockstat")
	     {
		 $filePath = $filePath . 'sockstat';
		 $initial_offset = 2; #skip through descriptors
		 
		 #we don't anything for ../sockets/used, offset
		 #already callibrated, used only for else statement below
		 if ($terciary eq "sockets"){}
		 elsif ($terciary eq "tcp")
		 {$initial_offset = $initial_offset + 2;}
		 elsif ($terciary eq "udp")
		 {$initial_offset = $initial_offset + 13;}
		 elsif ($terciary eq "raw")
		 {$initial_offset = $initial_offset + 16;}
		 elsif ($terciary eq "frag")
		 {$initial_offset = $initial_offset + 19;}
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 
		 if ($cetvrti eq "used"){}
		 elsif ($cetvrti eq "inuse")
		 {$initial_offset = $initial_offset + 1;}
		 elsif ((($cetvrti eq "orphan") && ($terciary eq "tcp")) || (($cetvrti eq "memory") && ($terciary eq "frag")))
		 {$initial_offset = $initial_offset + 3;}
		 elsif (($cetvrti eq "tw") && ($terciary eq "tcp"))
		 {$initial_offset = $initial_offset + 5;}
		 elsif (($cetvrti eq "alloc") && ($terciary eq "tcp"))
		 {$initial_offset = $initial_offset + 7;}
		 elsif (($cetvrti eq "mem") && ($terciary eq "tcp"))
		 {$initial_offset = $initial_offset + 9;}
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}

		 $result = process_file ($filePath, $initial_offset);
	     }
	     
	     elsif ($secondary eq "softnet_stat")
	     {
		 unless ($terciary =~ /\d+/)
		 {die "\nERROE! SOFTNET_STAT entries are numeric only!!\n";}
		 $filePath = $filePath . 'softnet_stat';
		 $result = process_file ($filePath, ($terciary - 1)); #adjust for zeroth/first
		 #entry
	     }
	     
	     elsif (($secondary eq "tcp") || ($secondary eq "udp"))
	     {
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nERROR! Invalid entry for TCP SLOT!\n";}
		 if ($secondary eq "tcp")
		 {$filePath = $filePath . 'tcp';}
		 elsif ($secondary eq "udp")
		 {$filePath = $filePath . 'udp';}
		 $terciary--; #to adjust for the zeroth/fist value
		 $initial_offset = 13;
		 $slot_offset = 17;
		 
		 if ($cetvrti eq "local_address")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "rem_address")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 1;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "st")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 2;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "tx_queue")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 3;
		     $result = process_file_extraction_first ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "rx_queue")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 3;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "tr")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 4;
		     $result = process_file_extraction_first ($filePath, $initial_offset);
      		 }
		 elsif ($cetvrti eq "tm_when")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 4;
		     $result = process_file_extraction ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "retrnsmt")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 5;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "uid")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 6;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "timeout")
		 {
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 7;
		     $result = process_file ($filePath, $initial_offset);
		 }
		 elsif ($cetvrti eq "inode")
		 {
		     unless (($peti =~ /\d+/) && ($peti != 0) && ((($secondary eq "tcp") && ($peti < 9)) || (($secondary eq "udp") && ($peti < 4))))
		     {die "\nPlease speicfy which INODE value you want! Please note values are numeric 1-8 for TCP and 1-3 for UDP. First number is first, not zeroth.\n";}
		     $initial_offset = $initial_offset + $slot_offset * $terciary + 8 + $peti - 1;#to account for the zeroth element
		     $result = process_file ($filePath, $initial_offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "unix")
	     {
		 $filePath = $filePath . 'unix';
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nNUMs in UNIX are numeric and non-zero only!\n";}
		 $initial_offset = 8;
		 $line_offset = 8;
		 $variable_offset = 0;

		 if ($cetvrti eq "num")
		 {$variable_offset = 0;}
		 
		 if ($cetvrti eq "ref_count")
		 {$variable_offset = 1;}
		 
		 elsif ($cetvrti eq "protocol")
		 {$variable_offset = 2;}
		 
		 elsif ($cetvrti eq "flags")
		 {$variable_offset = 3;}
		 
		 elsif ($cetvrti eq "type")
		 {$variable_offset = 4;}

		 elsif ($cetvrti eq "st")
		 {$variable_offset = 5;}
		 
		 elsif ($cetvrti eq "inode")
		 {$variable_offset = 6;}

		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}

		 $total_offset = $initial_offset + $line_offset * ($terciary - 1) + $variable_offset;

		 $result = process_file_extraction_first($filePath, $total_offset);
	     }
	 }
	 elsif ($primary eq "partitions")
	 {
	     unless (($secondary =~ /\d+/) && ($secondary != 0))
	     {die "\nPARTITION # is numeric ONLY!\n";}
	     
	     $filePath = $directory . 'partitions';
	     $initial_offset = 15;
	     $variable_offset = 0;
  
	     if ($terciary eq "major")
	     {$variable_offset = 0;}

	     elsif ($terciary eq "minor")
	     {$variable_offset = 1;}
		 
	     elsif ($terciary eq "no_blocks")
	     {$variable_offset = 2;}
		 
	     elsif ($terciary eq "rio")
	     {$variable_offset = 4;}

	     elsif ($terciary eq "rmerge")
	     {$variable_offset = 5;}
	     
	     elsif ($terciary eq "rsect")
	     {$variable_offset = 6;}

	     elsif ($terciary eq "ruse")
	     {$variable_offset = 7;}

	     elsif ($terciary eq "wio")
	     {$variable_offset = 8;}

	     elsif ($terciary eq "wmerge")
	     {$variable_offset = 9;}

	     elsif ($terciary eq "wsect")
	     {$variable_offset = 10;}

	     elsif ($terciary eq "wuse")
	     {$variable_offset = 11;}

	     elsif ($terciary eq "running")
	     {$variable_offset = 12;}

	     elsif ($terciary eq "use")
	     {$variable_offset = 13;}

	     elsif ($terciary eq "aveq")
	     {$variable_offset = 14;}

	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     
	     $total_offset = $initial_offset + ($secondary - 1) * $initial_offset + $variable_offset;

	     $result = process_file ($filePath, $total_offset);
	 }
	 elsif ($primary eq "slabinfo")
	 {
	     unless (($terciary =~ /\d+/) && ($terciary != 0) && ($terciary < 7))
	     {die "\nERROR! Please specify which counter in ", $secondary, " you want to collect with a non-zero, less-than-seven numeric character!\n";}
	     
	     $filePath = $directory . 'slabinfo';
	     $initial_offset = 5;
	     $line_offset = 7;
	     $additional_offset = 0;

	     if ($secondary eq "kmem_cache")
	     {}
	     elsif ($secondary eq "ip_fib_hash")
	     {$additional_offset = $line_offset;}
	     elsif ($secondary eq "urb_priv")
	     {$additional_offset = 2 * $line_offset;}
	     elsif ($secondary eq "journal_head")
	     {$additional_offset = 3 * $line_offset;}
	     elsif ($secondary eq "revoke_table")
	     {$additional_offset = 4 * $line_offset;}
	     elsif ($secondary eq "revoke_record")
	     {$additional_offset = 5 * $line_offset;}
	     elsif ($secondary eq "clip_arp_cache")
	     {$additional_offset = 6 * $line_offset;}
	     elsif ($secondary eq "ip_mrt_cache")
	     {$additional_offset = 7 * $line_offset;}
	     elsif ($secondary eq "tcp_tw_bucket")
	     {$additional_offset = 8 * $line_offset;}
	     elsif ($secondary eq "tcp_bind_bucket")
	     {$additional_offset = 9 * $line_offset;}
	     elsif ($secondary eq "tcp_open_request")
	     {$additional_offset = 10 * $line_offset;}
	     elsif ($secondary eq "inet_peer_cache")
	     {$additional_offset = 11 * $line_offset;}
	     elsif ($secondary eq "ip_dst_cache")
	     {$additional_offset = 12 * $line_offset;}
	     elsif ($secondary eq "arp_cache")
	     {$additional_offset = 13 * $line_offset;}
	     elsif ($secondary eq "blkdev_requests")
	     {$additional_offset = 14 * $line_offset;}
	     elsif ($secondary eq "dnotify_cache")
	     {$additional_offset = 15 * $line_offset;}
	     elsif ($secondary eq "file_lock_cache")
	     {$additional_offset = 16 * $line_offset;}
	     elsif ($secondary eq "fasync_cache")
	     {$additional_offset = 17 * $line_offset;}
	     elsif ($secondary eq "uid_cache")
	     {$additional_offset = 18 * $line_offset;}
	     elsif ($secondary eq "skbuff_head_cache")
	     {$additional_offset = 19 * $line_offset;}
	     elsif ($secondary eq "sock")
	     {$additional_offset = 20 * $line_offset;}
	     elsif ($secondary eq "sigqueue")
	     {$additional_offset = 21 * $line_offset;}
	     elsif ($secondary eq "cdev_cache")
	     {$additional_offset = 22 * $line_offset;}
	     elsif ($secondary eq "bdev_cache")
	     {$additional_offset = 23 * $line_offset;}
	     elsif ($secondary eq "mnt_cache")
	     {$additional_offset = 24 * $line_offset;}
	     elsif ($secondary eq "inode_cache")
	     {$additional_offset = 25 * $line_offset;}
	     elsif ($secondary eq "dentry_cache")
	     {$additional_offset = 26 * $line_offset;}
	     elsif ($secondary eq "dquot")
	     {$additional_offset = 27 * $line_offset;}
	     elsif ($secondary eq "filp")
	     {$additional_offset = 28 * $line_offset;}
	     elsif ($secondary eq "names_cache")
	     {$additional_offset = 29 * $line_offset;}
	     elsif ($secondary eq "buffer_head")
	     {$additional_offset = 30 * $line_offset;}
	     elsif ($secondary eq "mm_struct")
	     {$additional_offset = 31 * $line_offset;}
	     elsif ($secondary eq "vm_area_struct")
	     {$additional_offset = 32 * $line_offset;}
	     elsif ($secondary eq "fs_cache")
	     {$additional_offset = 33 * $line_offset;}
	     elsif ($secondary eq "files_cache")
	     {$additional_offset = 34 * $line_offset;}
	     elsif ($secondary eq "signal_act")
	     {$additional_offset = 35 * $line_offset;}
	     elsif ($secondary eq "size_131072_dma")
	     {$additional_offset = 36 * $line_offset;}
	     elsif ($secondary eq "size_131072")
	     {$additional_offset = 37 * $line_offset;}
	     elsif ($secondary eq "size_65536_dma")
	     {$additional_offset = 38 * $line_offset;}
	     elsif ($secondary eq "size_65536")
	     {$additional_offset = 39 * $line_offset;}
	     elsif ($secondary eq "size_32768_dma")
	     {$additional_offset = 40 * $line_offset;}
	     elsif ($secondary eq "size_32768")
	     {$additional_offset = 41 * $line_offset;}
	     elsif ($secondary eq "size_16384_dma")
	     {$additional_offset = 42 * $line_offset;}
	     elsif ($secondary eq "size_16384")
	     {$additional_offset = 43 * $line_offset;}
	     elsif ($secondary eq "size_8192_dma")
	     {$additional_offset = 44 * $line_offset;}
	     elsif ($secondary eq "size_8192")
	     {$additional_offset = 45 * $line_offset;}
	     elsif ($secondary eq "size_4096_dma")
	     {$additional_offset = 46 * $line_offset;}
	     elsif ($secondary eq "size_4096")
	     {$additional_offset = 47 * $line_offset;}
	     elsif ($secondary eq "size_2048_dma")
	     {$additional_offset = 48 * $line_offset;}
	     elsif ($secondary eq "size_2048")
	     {$additional_offset = 49 * $line_offset;}
	     elsif ($secondary eq "size_1024_dma")
	     {$additional_offset = 50 * $line_offset;}
	     elsif ($secondary eq "size_1024")
	     {$additional_offset = 51 * $line_offset;}
	     elsif ($secondary eq "size_512_dma")
	     {$additional_offset = 52 * $line_offset;}
	     elsif ($secondary eq "size_512")
	     {$additional_offset = 53 * $line_offset;}
	     elsif ($secondary eq "size_256_dma")
	     {$additional_offset = 54 * $line_offset;}
	     elsif ($secondary eq "size_256")
	     {$additional_offset = 55 * $line_offset;}
	     elsif ($secondary eq "size_128_dma")
	     {$additional_offset = 56 * $line_offset;}
	     elsif ($secondary eq "size_128")
	     {$additional_offset = 57 * $line_offset;}
	     elsif ($secondary eq "size_64_dma")
	     {$additional_offset = 58 * $line_offset;}
	     elsif ($secondary eq "size_64")
	     {$additional_offset = 59 * $line_offset;}
	     elsif ($secondary eq "size_32_dma")
	     {$additional_offset = 60 * $line_offset;}
	     elsif ($secondary eq "size_32")
	     {$additional_offset = 61 * $line_offset;}

	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     
	     $total_offset = $initial_offset + $additional_offset + $terciary - 1;
	     $result = process_file ($filePath, $total_offset);
	 }
	 elsif ($primary eq "stat")
	 {
	     $filePath = $directory . 'stat';
	     $initial_offset = 1;
	     $additional_offset = 0;
	     if ($secondary eq "cpu")
	     {
		 unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! CPU values are numeric, 1-4 only!\n";}
		 if ($terciary eq "whole")
		 {
		     $result = process_file ($filePath, ($initial_offset + $cetvrti - 1));
		 }
		 elsif ($terciary =~ /\d+/)
		 {
		     $total_offset = $initial_offset + ($terciary - 1) * 5 + $cetvrti - 1; #this calculates an ofset for a particular cpu
		     $result = process_file ($filePath, $total_offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}

	     }
	     #now we need to know how many cpus there are, so we know how many words to skip
	     else
	     {
		 #we know that 'page' follows the last cpu's values
		 $jump_offset = calculate_offset ($filePath, 'page');
		 
		 if ($secondary eq "page")
		 {
		     unless (($terciary =~ /\d+/) && ($terciary != 0) && ($terciary < 3))
		     {die "\nERROR! PAGE values are numeric, 1-2 only!\n";}
		     $total_offset = $initial_offset + $jump_offset + $terciary - 1;
		     $result = process_file ($filePath, $total_offset);
		 }
		 elsif ($secondary eq "swap")
		 {
		     unless (($terciary =~ /\d+/) && ($terciary != 0) && ($terciary < 3))
		     {die "\nERROR! SWAP values are numeric, 1-2 only!\n";}
		     $total_offset = $initial_offset + 3 + $jump_offset + $terciary - 1;
		     $result = process_file ($filePath, $total_offset);
		 }
		 elsif ($secondary eq "intr")
		 {
		     unless (($terciary =~ /\d+/) && ($terciary != 0) && ($terciary < 226))
		     {die "\nERROR! INTR values are numeric, 1-225 only!\n";}
		     $total_offset = $initial_offset + 6 + $jump_offset + $terciary - 1;
		     $result = process_file ($filePath, $total_offset);
		 }
		 else
		 {
		     $result = process_normal_file ($filePath, $secondary);
		 }
	     }
	 }

	 elsif ($primary eq "swaps")
	 {
	     unless (($secondary =~ /\d+/) && ($secondary != 0))
	     {die "\nERROR! SWAPS values are numeric only!\n";}
	     $filePath = $directory . 'swaps';
	     $initial_offset = 7;
	     $line_offset = 5;
	     $additional_offset = 0;
	     
	     if ($terciary eq "size")
	     {}
	     elsif ($terciary eq "used")
	     {$additional_offset = 1;}
	     elsif ($terciary eq "priority")
	     {$additional_offset = 2;}
	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     $total_offset = $initial_offset + ($secondary - 1) * $line_offset + $additional_offset;
	     $result = process_file ($filePath, $total_offset);
	 }
	 
	 elsif ($primary eq "sys")
	 {
	     $filePath = $directory . 'sys/';
	     
	     if ($secondary eq "abi")
	     {
		 $filePath = $filePath . 'abi/' . $terciary;
		 $result = process_single_line_file ($filePath);
	     }
	     elsif ($secondary eq "dev")
	     {
		 $filePath = $filePath . 'dev/';
		 if ($terciary eq "raid")
		 {
		     $filePath = $filePath . 'raid/' . $cetvrti;
		     $result = process_single_line_file ($filePath);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     elsif ($secondary eq "fs")
	     {
		 $filePath = $filePath . 'fs/';
		 
		 if ($terciary eq "dentry_state")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! DENTRY_STATE values are numeric, 1-3 ONLY!\n";} 
		     $filePath = $filePath . 'dentry-state';
		     $offset = $cetvrti - 1; #for the first/zeroth entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "dir_notify_enable")
		 {
		     $filePath = $filePath . 'dir-notify-enable';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "dquot_nr")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 3))
		     {die "\nERROR! DQUOT_NR values are numeric, 1-2 ONLY!\n";} 
		     $filePath = $filePath . 'dquot-nr';
		     $offset = $cetvrti - 1; #for the first/zeroth entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "file_max")
		 {
		     $filePath = $filePath . 'file-max';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "file_nr")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! FILE_NR values are numeric, 1-3 ONLY!\n";} 
		     $filePath = $filePath . 'file-nr';
		     $offset = $cetvrti - 1; #for the first/zeroth entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "inode_nr")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 3))
		     {die "\nERROR! INODE_NR values are numeric, 1-2 ONLY!\n";} 
		     $filePath = $filePath . 'inode-nr';
		     $offset = $cetvrti - 1; #for the first/zeroth entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "inode_state")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 8))
		     {die "\nERROR! INODE_STATE values are numeric, 1-7 ONLY!\n";} 
		     $filePath = $filePath . 'inode-state';
		     $offset = $cetvrti - 1; #for the first/zeroth entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "jbd_oom_retry")
		 {
		     $filePath = $filePath . 'jbd-oom-retry';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "lease_break_time")
		 {
		     $filePath = $filePath . 'lease-break-time';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "leases_enable")
		 {
		     $filePath = $filePath . 'leases-enable';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "overflowgid")
		 {
		     $filePath = $filePath . 'overflowgid';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "overflowuid")
		 {
		     $filePath = $filePath . 'overflowuid';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "super_max")
		 {
		     $filePath = $filePath . 'super-max';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "super_nr")
		 {
		     $filePath = $filePath . 'super-nr';
		     $result = process_single_line_file ($filePath);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     
	     elsif ($secondary eq "kernel")
	     {
		 $filePath = $filePath . 'kernel/';
		 
		 if ($terciary eq "acct")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! ACCT values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . 'acct';
		     $offset = $cetvrti - 1; #for the zeroth/firsr entry
		     $result = process_file ($filePath, $offset)
		     }
		 elsif ($terciary eq "core_uses_pid")
		 {
		     $filePath = $filePath . 'core_uses_pid';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "ctrl_alt_del")
		 {
		     $filePath = $filePath . 'ctrl-alt-del';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "msgmax")
		 {
		     $filePath = $filePath . 'msgmax';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "msgmnb")
		 {
		     $filePath = $filePath . 'msgmnb';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "msgmni")
		 {
		     $filePath = $filePath . 'msgmni';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "osrelease")
		 {
		     $filePath = $filePath . 'osrelease';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "overflowgid")
		 {
		     $filePath = $filePath . 'overflowgid';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "overflowuid")
		 {
		     $filePath = $filePath . 'overflowuid';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "panic")
		 {
		     $filePath = $filePath . 'panic';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "printk")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 5))
		     {die "\nERROR! PRINTK values are numeric, 1-4 ONLY!\n";}
		     $filePath = $filePath . 'printk';
		     $offset = $cetvrti - 1; #for the zeroth/firsr entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "prof_pid")
		 {
		     $filePath = $filePath . 'prof_pid';
		     $result = process_file ($filePath, 0);
		 }
		 elsif ($terciary eq "random")
		 {
		     $filePath = $filePath . 'random/';
		     
		     if ($cetvrti eq "boot_id")
		     {
			 $filePath = $filePath . 'boot_id';
			 $result = process_file ($filePath, 0);
		     }
		     elsif ($cetvrti eq "entropy_avail")
		     {
			 $filePath = $filePath . 'entropy_avail';
			 $result = process_file ($filePath, 0);
		     }
		     elsif ($cetvrti eq "read_wakeup_threshold")
		     {
			 $filePath = $filePath . 'read_wakeup_threshold';
			 $result = process_file ($filePath, 0);
		     }
		     elsif ($cetvrti eq "uuid")
		     {
			 $filePath = $filePath . 'uuid';
			 $result = process_file ($filePath, 0);
		     }
		     elsif ($cetvrti eq "write_wakeup_threshold")
		     {
			 $filePath = $filePath . 'write_wakeup_threshold';
			 $result = process_file ($filePath, 0);
		     }
		     else
		     {die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		     
		 }
		 elsif ($terciary eq "real_root_dev")
		 {
		     $filePath = $filePath . 'real-root-dev';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "rtsig_max")
		 {
		     $filePath = $filePath . 'rtsig-max';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "rtsig_nr")
		 {
		     $filePath = $filePath . 'rtsig-nr';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "sem")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 5))
		     {die "\nERROR! SEM values are numeric, 1-4 ONLY!\n";}
		     $filePath = $filePath . 'sem';
		     $offset = $cetvrti - 1; #for the zeroth/firsr entry
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "shmall")
		 {
		     $filePath = $filePath . 'shmall';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "shmmax")
		 {
		     $filePath = $filePath . 'shmmax';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "shmmni")
		 {
		     $filePath = $filePath . 'shmmni';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "sysrq")
		 {
		     $filePath = $filePath . 'sysrq';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "threads_max")
		 {
		     $filePath = $filePath . 'threads-max';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "version")
		 {
		     $filePath = $filePath . 'version';
		     
		     if ($cetvrti eq "day")
		     {
			 $result = process_file ($filePath, 3);
		     }
		     elsif ($cetvrti eq "time")
		     {
			 $result = process_file ($filePath, 4);
		     }
		     elsif ($cetvrti eq "year")
		     {
			 $result = process_file ($filePath, 6);
		     }
		     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     
	     }
	     elsif ($secondary eq "net")
	     {
		 $filePath = $filePath . 'net/';
		 
		 if ($terciary eq "appletalk")
		 {
		     $filePath = $filePath . 'appletalk/';
		     
		     if ($cetvrti eq "aarp_expiry_time")
		     {
			 $filePath = $filePath . 'aarp-expiry-time';
			 $result = process_single_line_file ($filePath);
		     }
		     elsif ($cetvrti eq "aarp_resolve_time")
		     {
			 $filePath = $filePath . 'aarp-resolve-time';
			 $result = process_single_line_file ($filePath);
		     }
		     elsif ($cetvrti eq "aarp_retransmit_limit")
		     {
			 $filePath = $filePath . 'aarp-retransmit-limit';
			 $result = process_single_line_file ($filePath);
		     }
		     elsif ($cetvrti eq "aarp_tick_time")
		     {
			 $filePath = $filePath . 'aarp-tick-time';
			 $result = process_single_line_file ($filePath);
		     }
		     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 }
		 elsif ($terciary eq "core")
		 {
		     $filePath = $filePath . 'core/' . $cetvrti;
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "ipv4")
		 {
		     $filePath = $filePath . 'ipv4/';
		     
		     if ($cetvrti eq "route")
		     {
			 $filePath = $filePath . 'route' . $peti;
			 $result = process_single_line_file ($filePath);
		     } 
		     elsif ($cetvrti eq "tcp_mem")
		     {
			 unless (($peti =~ /\d+/) && ($peti != 0) && ($peti < 4))
			 {die "\nERROR! TCP_MEM value are numeric, 1-3 ONLY!\n";}
			 $filePath = $filePath . 'tcp_mem';
			 $offset = $peti - 1; #for the first/zeroth element
			 $result = process_file ($filePath, $offset);
		     } 
		     elsif ($cetvrti eq "tcp_rmem")
		     {
			 unless (($peti =~ /\d+/) && ($peti != 0) && ($peti < 4))
			 {die "\nERROR! TCP_RMEM value are numeric, 1-3 ONLY!\n";}
			 $filePath = $filePath . 'tcp_rmem';
			 $offset = $peti - 1; #for the first/zeroth element
			 $result = process_file ($filePath, $offset);
		     }
		     elsif ($cetvrti eq "tcp_wmem")
		     {
			 unless (($peti =~ /\d+/) && ($peti != 0) && ($peti < 4))
			 {die "\nERROR! TCP_WMEM value are numeric, 1-3 ONLY!\n";}
			 $filePath = $filePath . 'tcp_wmem';
			 $offset = $peti - 1; #for the first/zeroth element
			 $result = process_file ($filePath, $offset);
		     }
		     else
		     {
			 $filePath = $filePath . $cetvrti;
			 $result = process_single_line_file ($filePath);
		     }
		 }
		 elsif ($terciary eq "ipx")
		 {
		     $filePath = $filePath . 'ipx/';
		     unless ($cetvrti eq "pprop_broadcasting")
		     {die "\nWARNING! Can only display values for PPROP_BROADCASTING\n";}
		     $filePath = $filePath . 'ipx_pprop_broadcasting';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "token_ring")
		 {
		     $filePath = $filePath . 'token-ring/';
		     unless ($cetvrti eq "rif_timeout")
		     {die "\nWARNING! Can only display values for RIF_TIMEOUT\n";}
		     $filePath = $filePath . $cetvrti;
		     $result = process_single_line_file ($filePath);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		     
	     }
	     elsif ($secondary eq "vm")
	     {
		 $filePath = $filePath . 'vm/';
		 
		 if ($terciary eq "bdflush")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 10))
		     {die "\nERROR! BDFLUSH values are numeric, 1-10 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "buffermem")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! BUFFERMEM values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "freepages")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! FREEPAGES values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "kswapd")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! KSWAPD values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "max_map_count")
		 {
		     $filePath = $filePath . $terciary;
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "max_readahead")
		 {
		     $filePath = $filePath . 'max-readahead';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "min_readahead")
		 {
		     $filePath = $filePath . 'min-readahead';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "overcommit_memory")
		 {
		     $filePath = $filePath . $terciary;
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "pagecache")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 4))
		     {die "\nERROR! PAGECACHE values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 elsif ($terciary eq "page_cluster")
		 {
		     $filePath = $filePath . 'page-cluster';
		     $result = process_single_line_file ($filePath);
		 }
		 elsif ($terciary eq "pagetable_cache")
		 {
		     unless (($cetvrti =~ /\d+/) && ($cetvrti != 0) && ($cetvrti < 3))
		     {die "\nERROR! PAGETABLE_CACHE values are numeric, 1-3 ONLY!\n";}
		     $filePath = $filePath . $terciary;
		     $offset = $cetvrti - 1; #for the first/zeroth element
		     $result = process_file ($filePath, $offset);
		 }
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	     }
	     
	 }
	 
	 elsif ($primary eq "sysvipc")
	 {
	     $filePath = $directory . 'sysvipc/';
	     
	     if ($secondary eq "sem")
	     {
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nERROR! SEM value are numeric, non-zero ONLY!\n";}
		 $filePath = $filePath . 'sem';
		 $initial_offset = 10;
		 $additional_offset = 0;
		 
		 if ($cetvrti eq "key")
		 {}
		 elsif ($cetvrti eq "semid")
		 {$additional_offset = 1;}
		 elsif ($cetvrti eq "perms")
		 {$additional_offset = 2;}
		 elsif ($cetvrti eq "nsems")
		 {$additional_offset = 3;}
		 elsif ($cetvrti eq "uid")
		 {$additional_offset = 4;}
		 elsif ($cetvrti eq "gid")
		 {$additional_offset = 5;}
		 elsif ($cetvrti eq "cuid")
		 {$additional_offset = 6;}
		 elsif ($cetvrti eq "cgid")
		 {$additional_offset = 7;}
		 elsif ($cetvrti eq "otime")
		 {$additional_offset = 8;}
		 elsif ($cetvrti eq "ctime")
		 {$additional_offset = 9;}
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 
		 $total_offset = $initial_offset + ($terciary - 1) * $initial_offset + $additional_offset;
		 $result = process_file ($filePath, $total_offset);
	     }
	     if ($secondary eq "shm")
	     {
		 unless (($terciary =~ /\d+/) && ($terciary != 0))
		 {die "\nERROR! SHM value are numeric, non-zero ONLY!\n";}
		 $filePath = $filePath . 'shm';
		 $initial_offset = 14;
		 $additional_offset = 0;
		 
		 if ($cetvrti eq "key")
		 {}
		 elsif ($cetvrti eq "shmid")
		 {$additional_offset = 1;}
		 elsif ($cetvrti eq "perms")
		 {$additional_offset = 2;}
		 elsif ($cetvrti eq "size")
		 {$additional_offset = 3;}
		 elsif ($cetvrti eq "cpid")
		 {$additional_offset = 4;}
		 elsif ($cetvrti eq "lpid")
		 {$additional_offset = 5;}
		 elsif ($cetvrti eq "nattch")
		 {$additional_offset = 6;}
		 elsif ($cetvrti eq "uid")
		 {$additional_offset = 7;}
		 elsif ($cetvrti eq "gid")
		 {$additional_offset = 8;}
		 elsif ($cetvrti eq "cuid")
		 {$additional_offset = 9;}
		 elsif ($cetvrti eq "cgid")
		 {$additional_offset = 10;}
		 elsif ($cetvrti eq "atime")
		 {$additional_offset = 11;}
		  elsif ($cetvrti eq "dtime")
		 {$additional_offset = 12;}
		 elsif ($cetvrti eq "ctime")
		 {$additional_offset = 13;}
		 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
		 
		 $total_offset = $initial_offset + ($terciary - 1) * $initial_offset + $additional_offset;
		 $result = process_file ($filePath, $total_offset);
	     }
	     
	 }
	 
	 elsif ($primary eq "tty")
	 {
	     $filePath = $directory . 'tty/';
	     if ($secondary eq "driver")
	     {
		 $filePath = $filePath . 'driver/';
		 unless ($terciary eq "serial")
		 {die "\nWARNING! Currently only able to process SERIAL counters!\n";}
		 $filePath = $filePath . $terciary;
		 $tempKey1 = $cetvrti - 1;#for the first/zeroth element
		 $searchWord = $tempKey1 . ':';#to get the index, eg 3
		 $offset1 = calculate_offset ($filePath, $searchWord);
		 $searchWord = $peti . ':';
		 $offset2 = calculate_offset_jump ($filePath, $searchWord, $offset1);
		 $result = find_extract_word ($filePath, $searchWord, $offset2);
	     }
	 }
	 
	 elsif ($primary eq "uptime")
	 {
	     $filePath = $directory . $primary;
	     if ($secondary eq "system")
	     {
		 $result = process_file ($filePath, 0);
	     }
	     elsif ($secondary eq "process")
	     {
		 $result = process_file ($filePath, 1);
	     }
	     else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	 }
	 
	 elsif ($primary eq "version")
	 {
	     $filePath = $directory . 'version';
	     
	     if ($secondary eq "version")
	     {$result = process_file ($filePath, 2);}
	     elsif ($secondary eq "gcc_version")
	     {$result = process_file ($filePath, 6);}
	     elsif ($secondary eq "day")
	     {$result = process_file ($filePath, 16);}
	     elsif ($secondary eq "time")
	     {$result = process_file ($filePath, 17);}
	     elsif ($secondary eq "year")
	     {$result = process_file ($filePath, 19);}
	 }
	 else { die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";}
	 $results[$counter] = $result;
	 $counter++;
     }
     for $current_result (@results)
     {
	 print $current_result, "\t";
     }
     print "\n";
}    

sub process_file {  # ($filePath, $offset)
    my $fullname = $_[0];
    my $offset = $_[1];
    my $count = 0;
    open(TEXTFILE, "$fullname") or die "Can't open $fullname: $!\n";
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($count eq $offset)
	    {
		#print "\n", $fullname; 
		#print "\n", $word. "\n";
		#print "\n", $count, ": ja sam peder!!\n";
		return $word;
	    }
	    else { $count++;next;}
	}
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub print_num_areas { #($filePath);
    my $fullname = $_[0];
    my $count = 0;
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
    while($line = <TEXTFILE>){
	$count++;
    }
    #print "\n", $count, "\n";
    return $count;
  END:
}

sub print_mem_address { #$filePath, $cetvrti
    my $fullname = $_[0];
    my $offset = $_[1];
    my $count = 0;
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
    while($line = <TEXTFILE>)
    {
	$count++;#first memory address is the 1st onw
	    if ($offset == $count)
	    {
		$line =~ /((\w+)(\-*)(\w*))/;#store the address in $1
		    #print "\n", $1, "\n";
		return $1;
	    }
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
  END:
}

sub process_offset_file { #$filePath, $secondary, $terciary, $offsetVar

#This function goes through all the lines in a $fullname specified
#text file looking for the $offsetVar variable with a certain
#$file_offset
#Once it passes the offset variable with the desired $file_offset
#it will look for a line containing a $searchKey and output the
#part of the line that corresponds to the respective number

#This function is here primarily due to multiple processors
#Example: user wants to now bogomips of the 3rd processor
#The function will first look to pass the "processor: 2 "line in cpuinfo
#and then look for a line containing 'bogomips' and print out the respective
#number

    my $fullname = $_[0];#Complete filepath
    my $file_offset = $_[1];#F.e looking for attribute of the 3rd processor
                            #then file_offse is 3
    my $searchKey = $_[2];#what are we looking for in a file
	                  #might be same name with tool as in /proc
	                  #otherwise defined in the caller procedure
    my $offsetVar = $_[3];#the variable whose offset we are looing for
	                  #in the above example, it would be 'processor'
    my $passedOffset = 1; #FALSE, offset not passed yet
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
    while($line = <TEXTFILE>){
	
	if ($line =~ /$offsetVar/)#look for the variable
	{
	    $line =~ /(\d)/; #capture the offset varable's  digit
	    #print "\n", $1, "\n";
	    #print "\n", $searchKey, "\n";
	    if ($1 == ($file_offset - 1))#computers count from 0, humans dont
		                         #there's no zeroth processor in english
	    {
		$passedOffset = 0;#TRUE
		next;
	    }
	}
	
	if (($line =~ /$searchKey/) && ($passedOffset == 0))
	{
	    $line =~ /((\d+)(\.*)(\d*))/;#capture the entire composite into $1
	    #print "\n", $1, "\n";
	    return $1;
	}
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub process_normal_file {
    my $fullname = $_[0];
    my $searchKey = $_[1];
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
        while($line = <TEXTFILE>){
	
	    if ($line =~ /$searchKey/)
	    {
		$line =~ /((\d+)(\.*)(\-*)(\/*)(\:*)(\d*)(\.*)(\-*)(\/*)(\:*)(\d*))/;#capture the entire composite into $1
		    #print "\n", $1, "\n";
		return $1;
	    }
	}
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub process_single_line_file { #($filePath);
    my $fullname = $_[0];
     open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
        while($line = <TEXTFILE>){
	    $line =~ /((\d+)(\.*)(\-*)(\:*)(\d*)(\.*)(\-*)(\:*)(\d*))/;#capture the entire composite into $1
		    #print "\n", $1, "\n";
	    return $1;
	}
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}
	    
sub process_file_extraction {  # ($filePath, $offset)
#this subroutine extracts the number to the right of a sentinel
    my $fullname = $_[0];
    my $offset = $_[1];
    my $count = 0;
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($count eq $offset)
	    {
		#print "\n", $fullname;
		$word =~ /((\d+)(\.*)(\-*)(\:*)(\d*)(\.*)(\-*)(\:*)(\d*))$/;#capture the entire composite into $1
		    $1 =~ /(\d+):(\d+)/; #dev has bad word and
		                         #num separation so we 
		                         #have to get rid of 
		                         #any unneccessary :s
		#print "\n", $2, "\n";
		return $2;
	    }
	    else { $count++;next;}
	}
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub process_file_extraction_first {  # ($filePath, $offset)
#assumes there will digits separated by separators
#gets the first one
    my $fullname = $_[0];
    my $offset = $_[1];
    my $count = 0;
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
  START:
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($count eq $offset)
	    {
		#print "\n", $fullname;
		$word =~ /((\w+)(\.*)(\-*)(\:*)(\w*)(\.*)(\-*)(\:*)(\w*))$/;#capture the entire composite into $1
		if (!($2))
		{goto START;} #couldn't fetch the number, so I'll go next
		        #no error msg since it would corrupt our
		        #numbers only output
		#print "\n", $word, "\n", $2, "\n";
		return $2;
	    }
	    else { $count++;next;}
	}
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub process_unequal_file {
    my $fullname = $_[0];
    my $lineOffset = $_[1];
    my $variableOffset = $_[2];
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
        while($line = <TEXTFILE>){
	    $count++;
	    if ($count == $lineOffset)
	    {
		$line =~ /((\d+)(\d+)(\d+)(\d+)(\d+)(\d+)(\d*))/;#capture the entire composite into $1
		    #print "\n", $1, "\n";
		return $3;
	    }
	}
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub process_search_file {
    my $fullname = $_[0];
    my $searchKey = $_[1];
    my $offset = $_[2];
    my $count = 0;
    open(TEXTFILE, "$fullname") or print "Can't open $fullname: $!\n";
        while($line = <TEXTFILE>){
	
	    if ($line =~ /$searchKey/)
	    {
		foreach $word ( split /\w/, $line)
		{
		    if ($offset == $count)
		    {return $word;}
		    else {$count++;}
		}
	    }
	}
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub calculate_offset {
    my $fullname = $_[0];
    my $searchKey = $_[1];
    my $count = 0;
    open(TEXTFILE, "$fullname") or die "Can't open $fullname: $!\n";
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($word =~ /$searchKey/)
	    {return $count;}
	    else
	    {$count++;}
	}
	
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub calculate_offset_jump {
    my $fullname = $_[0];
    my $searchKey = $_[1];
    my $offset = $_[2];
    my $count = 0;
    open(TEXTFILE, "$fullname") or die "Can't open $fullname: $!\n";
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($count == $offset)
	    {
		if ($word =~ /$searchKey/)
		{return $count;}	      
	    }
	    else
	    {$count++;}
	}
	
    }
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}

sub find_extract_word { 
    my $fullname = $_[0];
    my $searchKey = $_[1];
    my $starting_offset = $_[2];
    my $count = 0;
    my $offset_passed = 1;
    open(TEXTFILE, "$fullname") or die "Can't open $fullname: $!\n";
    while(<TEXTFILE>){
	foreach $word( split )
	{
	    if ($count eq $starting_offset)
	    {
		$offset_passed = 0;
	    }
	    if ($offset_passed == 0)
	    {
		$word = /^((:)(\w+))$/;
		print "\n", $word, "\n", $1, "\n", $3, "\n";
		return $2;
	    }
	    else { $count++;next;}
	}
    }
    
    die "\nERROR! Could not find requested namespace in /proc. Either your number has not been mapped yet or it is erraneous!\n";
}
