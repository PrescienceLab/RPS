#!/usr/bin/env perl
use Getopt::Std;
getopts('s:f:rh');
$scale = 1;
$format = "letter";
$rotate = 0;
if(defined $opt_h){
    print "\nUsage: eps2ps [ -s Scale ] [ -f Format ] [ -r ] infile [ outfile ]\n\n";
    print "DESCRIPTION:\n";
    print "eps2ps converts Encapsulated Postscript (*.eps) files to\n";
    print "Postscript (.ps) format.\n"; 
    print "eps2ps is not guaranteed to work with all *.eps files on earth!!!\n\n";
    print "-s defines the scale factor. It's limited to the paper's extents.\n";
    print "-f defines the paper format (letter, A0-A4). Default is letter.\n";
    print "-r rotates the plot by 90 deg.\n\n";
    exit 1;
}
if(defined $opt_s){
#    print "-s $opt_s\n";
    $scale = $opt_s;
}
if(defined $opt_f){
#    print "-f $opt_f\n";
    $format = $opt_f;
}
if(defined $opt_r){
#    print "-r \n";
    $rotate = 1;
}
$format =~ s|a|A|;

# Say Hello:

print "This is Andy's EPS to PS Utility, Ver. 0.01\n";

# Open Input- and Output-Files
 
unless ($#ARGV > -1)               # Identify arguments
{
    print "Error: No Input File specified!\n";
    exit 1;
}

$infile = $ARGV[0];                  # Try to open Input
unless(open(IN,"<".$infile)){
    print "Input-File $infile is empty, I will try $infile".".eps!\n";
    $infile = $infile.".eps";          # forgotten .eps ending ?
    unless(open(IN,"<".$infile)){
        print "Error: Input-File $infile is empty, too!\n";
        exit 1;
    }
}         

@intext = <IN>;
close(IN);
print "Input-File:  $infile \n";
if($intext[0] =~ m|^%!PS-Adobe.*EPSF.*|){
    print "Wow! Input-File is really an EPS File!\n";
}
else{
    print "Error: Input-File is not an EPS File!\n";
    exit 1;
}

if($#ARGV > 0){$outfile = $ARGV[1];}
else{$outfile = $infile.".ps";}
unless(open(OUT,">".$outfile)){
    print "Error: Cannot open Output-File!\n";
    exit 1;
}
print "Output-File: $outfile \n";


# Initialize Postscript-File

print OUT "%!PS-Adobe-2.0\n";
print OUT "%%Title: PS-File including EPS-File $infile\n";
print OUT "%%Creator: Andy's eps2ps Utility, Ver. 0.00\n";
print OUT "%%For: $ENV{USER}"."@"."$ENV{HOST}\n";
print OUT "%%Pages: 1\n";
print OUT "%%Orientation: Portrait\n";
print OUT "%%BoundingBox: (atend)\n";
print OUT "%%DocumentFonts: (atend)\n";
print OUT "%%BeginSetup\n";
print OUT "%%IncludeFeature: *PageSize $format\n";
print OUT "%%EndSetup\n";
print OUT "%%EndComments\n";
print OUT "% Input-File: $ENV{PWD}/$infile\n";
print OUT "\n";


print OUT "%%EndProlog\n";
print OUT "\n";
print OUT "%%Page: 1 1\n";
print OUT "\n";
print OUT "gsave\n";

# Start with real paintings
if ($format eq "letter"){
    $paperwidth = 8.5*72;
    $paperheight = 11*72;
}
elsif ($format eq "A3"){
    $paperwidth = 841.889763779;        #a3 in pts
    $paperheight = 2 * 595.275590551;   # height must be greater equal width!!!
}
elsif ($format eq "A2"){
    $paperwidth = 2 * 595.275590551;    #a2 in pts
    $paperheight = 2 * 841.889763779;   # height must be greater equal width!!!
}
elsif ($format eq "A1"){
    $paperwidth = 2 * 841.889763779;    #a1 in pts
    $paperheight = 4 * 595.275590551;   # height must be greater equal width!!!
}
elsif ($format eq "A0"){
    $paperwidth = 4 * 595.275590551;    #a0 in pts
    $paperheight = 4 * 841.889763779;   # height must be greater equal width!!!
}
else{
    $paperwidth = 595.275590551;        #a4 in pts
    $paperheight = 841.889763779;       # height must be greater equal width!!!
}

# Where's the BoundingBox?? What Fonts are needed?

$Fonts = "";
$bbundefined=1;
foreach(@intext){
    if($_ =~ m|^%%BoundingBox:\s(-?\d+(\.\d*)?)\s(-?\d+(\.\d*)?)\s(-?\d+(\.\d*)?)\s(-?\d+(\.\d*)?)|&&$bbundefined){
       $bbx = $1;
       $bby = $3;
       $bburx = $5;
       $bbury = $7;
       $bbw = $bburx - $bbx;
       $bbh = $bbury - $bby;
       $bbundefined=0;
       print "I found this BoundingBox: $bbx $bby $bburx $bbury\n";
#       print "BBw= $bbw, BBh = $bbh\n";
   }
   elsif($_ =~ m|^%%DocumentFonts:([\w\s]+)$|){
       $Fonts = join(' ',$Fonts,$1);
       $Fonts =~ s/\(atend\)//g;
       $Fonts =~ s/\s\s+/ /g;
       print "I found these DocumentFonts:$Fonts\n";

   } 
}

#find out, if we have to rotate this stuff
if($bbh > $bbw){
    $maxdim = $bbh;
    $mindim = $bbw;
}
else{
    $maxdim = $bbw;
    $mindim = $bbh;
}

if($maxdim * $scale > $paperheight){
    $scale = $paperheight / $maxdim * 0.95;
    print "to fit on page $format Scale is reduced to $scale!\n";
}
if($mindim *$scale > $paperwidth){
    $scale = $paperwidth / $mindim * 0.95;
    print "to fit on page $format Scale is reduced to $scale!\n";
}
if($rotate == 0 && $bbw * $scale > $paperwidth){
    $rotate = 1;
    print "to fit on page $format figure will be rotated by 90 degrees!\n";
}    
elsif($rotate == 1 && $bbh * $scale > $paperwidth){
    $rotate = 0;
    print "to fit on page $format figure will not be rotated!\n";
}    
    

if($rotate == 0){
    $xorig = $paperwidth/2-$bbw*$scale/2-$bbx*$scale;
    $yorig = $paperheight/2-$bbh*$scale/2-$bby*$scale;
    print OUT "$xorig $yorig translate\n";
    $BBoxX = $xorig + $bbx * $scale;
    $BBoxY = $yorig + $bby * $scale;
    $BBoxW = $bbw * $scale;
    $BBoxH = $bbh * $scale;
    print "Scale is $scale, picture is not rotated\n";
}
else{
    $xorig = $paperheight/2-$bbw*$scale/2-$bbx*$scale;
    $yorig = $paperwidth/2-$bbh*$scale/2-$bby*$scale;
    print OUT "$paperwidth 0 translate\n";
    print OUT "90 rotate\n";
    print OUT "$xorig $yorig translate\n";
    $BBoxX = $paperwidth/2-$bbh*$scale/2;
    $BBoxY = $paperheight/2-$bbw*$scale/2;
    $BBoxW = $bbh*$scale;
    $BBoxH = $bbw*$scale;
    print "Scale is $scale, picture is rotated by 90 degrees\n";
}    
print OUT "$scale $scale scale\n";
print OUT "save\n";
print OUT "/showpage {} def\n";
print OUT "\n%\n% Imported EPS-File starts here!\n%\n";
print OUT "%%BeginDocument: $ENV{PWD}/$infile\n%\n"; 

foreach(@intext){
    print OUT "$_";
}

print OUT "\n%\n% Imported EPS-File ends here!\n%\n";
print OUT "%%EndDocument\n%\n"; 
print OUT "restore\n";
print OUT "grestore\n";




print OUT "showpage\n";

print OUT "%%Trailer\n";
$BBoxurX = $BBoxW+$BBoxX;
$BBoxurY = $BBoxH+$BBoxY;
print OUT "%%BoundingBox: $BBoxX $BBoxY $BBoxurX $BBoxurY\n";
print OUT "%%DocumentFonts:$Fonts\n";

close(OUT);
print "\nConversion succeeded!\n\n";




