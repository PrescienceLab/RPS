package predci_dataaccess;
require Exporter;
require FileHandle;
require dataaccess;
@ISA = qw(Exporter);
@EXPORT = qw(GetPredCITestcasesWhere);





#
# testref points to a hash of arrays
# @{$testref->{"tnom"}} are the tnom values for the testcases
# @{$testref->{"tlb"}} are the corresponding tlb values, etc.
#
sub GetTestcasesWhere {
    my ($database, $table, $where) = @_;
    my %testcases;
    my $i;
    my $sql = "select tnom, tlb, texp, tub, tact, (texp-tact)/texp as relerr, (tnom-usrtime)/tnom as replayerr from $table where $where";
    
    open(MYSQL, "mysql --user root --batch --silent --execute=\"$sql\" $database | ");
    $i=0;
    while (<MYSQL>) {
	chomp;
	@vals = split;
	$testcases{"tnom"}[$i] = $vals[0];
	$testcases{"tlb"}[$i] = $vals[1];
	$testcases{"texp"}[$i] = $vals[2];
	$testcases{"tub"}[$i] = $vals[3];
	$testcases{"tact"}[$i] = $vals[4];
	$testcases{"relerr"}[$i] = $vals[5];
	$testcases{"replayerr"}[$i] = $vals[6];
	$i++;
    }
    close(MYSQL);
    return %testcases;
}

