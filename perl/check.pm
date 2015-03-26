package check;
use strict;
use warnings;
use FindBin qw($Bin);
use lib "$Bin/../lib";

use constant isdebug=>1;

use parent 'CheckIndex';

sub new
{
    my($type) = shift @_;
    my %params = @_;
    my $self = {};
    bless($self, $type);
    die "verifyConf param is required" if !defined $params{verifyConf};
    $self->{indexInfo} = {
        index=> {
            tokenNum => 134
        }
    }
    $self->{filter} = "\e\\\\";
    return $self;
}
sub getLogName
{
    return "~/log/".__PACKAGE__;
}
my $LOG;
sub run
{
    my $self = shift @_;
    if(!defined $LOG){
        my $file = $self->getLogName();
        open( $LOG, ">>$file") or die "open log file $file failed $!";
    }
    print $LOG $msg;
    autoflush $LOG 1;
    ++ $|;

    my $cmd = "$cmdPrefix -f $fileName";
    open(FN, "$cmd|") or ( $self->logMsg("run $cmd failed $!") and return -1);
    while(<FN>){
        chomp;
        s/$self->{filter}//g;
        s/$self->{filter}//g;
        if( $_ =~ /^s\c*docnum[=\s:,]/ ){

        }
    }
    close FN;
    for my $field( keys %{$self->{index}})
    {
        next if $field eq 'docnum';
    }
    return 0;
}
