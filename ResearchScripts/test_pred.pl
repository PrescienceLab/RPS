#!/usr/bin/env perl
#
# test_pred.pl scenariofile

if ($#ARGV!=0) {
    print STDERR "test_pred.pl scenariofile\n";
    exit;
}

open(S,$ARGV[0]);
while (<S>) { 
    chomp;
    if (/^\s*PRED\s+(.*)$/) {
	$PREDLIST = $1;
    }   
    if (/^\s*SPIN\s+(.*)$/) {
	$SPINLIST = $1;
    }    
    if (/^\s*TESTPRED\s+(.*)$/) {
	$TESTPRED = $1;
    }    
    if (/^\s*NUMTASKS\s+(.*)$/) {
	$NUMTASKS = $1;
    }    
    if (/^\s*TASKLEN\s+(.*)$/) {
	$TASKLEN = $1;
    }   
    if (/^\s*INTLEN\s+(.*)$/) {
	$INTLEN = $1;
    }
    if (/^\s*HOST\s+(.*)$/) {
	push @hosts, $1;
    }
}
close(S);

open(P,$PREDLIST);
while (<P>) { 
    chomp;
    if (/^\s*\#.*$/ || /^\s*$/) { 
	next;
    }
    @fields = split;
    push @predhosts, $fields[0];
    push @predports, $fields[7];
}
close(P);
open(P,$SPINLIST);
while (<P>) { 
    chomp;
    if (/^\s*\#.*$/ || /^\s*$/) { 
	next;
    }
    @fields = split;
    push @spinhosts, $fields[0];
    push @spinports, $fields[3];
}
close(P);

# sanity check - each host must have both a prediction pipeline 
# and a spinserver 

$firstfail=0;
pred: foreach $predhost (@predhosts) { 
    foreach $spinhost (@spinhosts) { 
	if ($spinhost eq $predhost) { 
	    next pred;
	}
    }
    print STDERR "$predhost in $PREDLIST has no entry in $SPINLIST\n";
    $firstfail=1;
}

$secondfail=0;
spin: foreach $spinhost (@spinhosts) { 
    foreach $predhost (@predhosts) { 
	if ($spinhost eq $predhost) { 
	    next spin;
	}
    }
    print STDERR "$spinhost in $SPINLIST has no entry in $PREDLIST\n";
    $secondfail=1;
}

if ($firstfail || $secondfail) { 
    exit;
}

#sanity check - each host must have entries in both lists
for ($i=0;$i<=$#hosts;$i++) {
    $found=0;
    for ($j=0;$j<=$#predhosts;$j++) { 
	if ($predhosts[$j] eq $hosts[$i]) { 
	    $pbports[$i]=$predports[$j];
	    $found=1;
	    last;
	}
    }
    if (!$found) { 
	print STDERR "Can't find $host in $PREDLIST\n";
	exit;
    }
    $found=0;
    for ($j=0;$j<=$#spinhosts;$j++) { 
	if ($spinhosts[$j] eq $hosts[$i]) { 
	    $ssports[$i]=$spinports[$j];
	    $found=1;
	    last;
	}
    }
    if (!$found) { 
	print STDERR "Can't find $host in $SPINLIST\n";
	exit;
    }
}

$CMD = "$TESTPRED $NUMTASKS $INTLEN $TASKLEN";

if (0) {
for ($i=0;$i<=$#predhosts;$i++) {
    $CMD.=" source:tcp:$predhosts[$i]:$predports[$i]";
    for ($j=0;$j<=$#spinhosts;$j++) { 
	if ($spinhosts[$j] eq $predhosts[$i]) {
	    $CMD.=" source:tcp:$spinhosts[$j]:$spinports[$j]";
	}
    }
}
}

for ($i=0;$i<=$#hosts;$i++) {
    $CMD.=" source:tcp:$hosts[$i]:$pbports[$i] source:tcp:$hosts[$i]:$ssports[$i]";
}


print STDERR "$CMD\n";
system $CMD;
print STDERR "done\n";	    
    
