package dataaccess;
require Exporter;
require FileHandle;
@ISA = qw(Exporter);
@EXPORT = qw(GetCountWhere GetStatsOfColumnWhere GetColumnWhere);

sub GetStatsOfColumnWhere {
    my ($database, $table, $where, $column) = @_;
    my $sql = "select count(*), sum($column), sum(($column)*($column)) from $table where $where";
    open(MYSQL, "mysql --user root --batch --silent --execute=\"$sql\" $database | ");
    $line=<MYSQL>;
    chomp($line);
    my ($count, $sum, $sum2) = split(/\s+/,$line);
    close(MYSQL);
    return ($count,$sum,$sum2);
}
	
sub GetColumnWhere {
    my $line;
    my ($database, $table, $where, $column) = @_;
#    print "col='$column' tab='$table' where='$where'\n";
    my $sql = "select $column from $table where $where";
    open(MYSQL, "mysql --user root --batch --silent --execute=\"$sql\" $database | ");
    my @col;
    while ($line=<MYSQL>) {
	chomp($line);
	push @col, $line;
    }
    close(MYSQL);
    return @col;
}

sub GetCountWhere {
    my ($database, $table, $where) = @_;

    my $sql = "select count(*) from $table where $where";
    
    open(MYSQL, "mysql --user root --batch --silent --execute=\"$sql\" $database | ");
    my $line = <MYSQL>;
    close(MYSQL);
    chomp ($line);
    return $line;
}
