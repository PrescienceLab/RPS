package RPS;
require Exporter;

@ISA=qw(Exporter);
@EXPORT= qw(RPSBanner);

sub RPSBanner {
return 
"\nRPS: Resource Prediction System Toolkit\n".
"---------------------------------------\n\n".
"Copyright (c) 1999-2003 by Peter A. Dinda\n".
"Use subject to license (\$RPS_DIR/LICENSE)\n\n".
"http://www.cs.northwestern.edu/~RPS\n".
"rps-help\@cs.northwestern.edu\n\n";
}
